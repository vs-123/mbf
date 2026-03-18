#include "macromiser.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "tokeniser.h"
#include "util.h"

#define bool int
#define true 1
#define false 0

IMPLEMENT_VECTOR (token_t, token_vector_t)
IMPLEMENT_VECTOR (macro_t, macro_vector_t)

static void
cry (const token_t *token, const char *format, ...)
{
   line_col_t lc = token->lc;

   fprintf (stderr, "[ERROR %u:%u] ", lc.line, lc.col);

   va_list args;

   va_start (args, format);
   vfprintf (stderr, format, args);
   va_end (args);

   fprintf (stderr, "\n");

   exit (1);
}

static void
free_token_heap_fields_vector (token_vector_t *v)
{
   for (unsigned int i = 0; i < v->size; i++)
      {
         token_t *t = (token_t *)token_vector_t_at (v, i);
         if (t->type == Token_Ident && t->value.chars)
            {
               free ((void *)t->value.chars);
               t->value.chars = NULL;
            }
      }
}

static void
replace_tokens (token_vector_t *dst, token_vector_t *src)
{
   token_vector_t old = *dst;
   *dst               = *src;
   token_vector_t_free (&old);
}

/* impl */

macro_t
new_macro (const char *macro_name, token_vector_t body)
{
   macro_t m = {
      .hash = str_hash (macro_name),
      .body = body,
   };
   return m;
}

macromiser_t
new_macromiser (token_vector_t tokens)
{
   macromiser_t m = { 0 };

   m.macros = macro_vector_t_new (4);
   m.tokens = tokens;

   return m;
}

void
macromiser_free (macromiser_t *m)
{
   for (unsigned int i = 0; i < m->macros.size; i++)
      {
         macro_t *mm = (macro_t *)macro_vector_t_at (&m->macros, i);
         free_token_heap_fields_vector (&mm->body);
         token_vector_t_free (&mm->body);
      }
   macro_vector_t_free (&m->macros);

   free_token_heap_fields_vector (&m->tokens);
   token_vector_t_free (&m->tokens);
}

void
macromiser_collect_macros (macromiser_t *m)
{
   unsigned int idx          = 0;
   unsigned int prog_size    = m->tokens.size;
   token_vector_t new_tokens = token_vector_t_new (32);

   while (idx < prog_size)
      {
         token_t curr_tok = *token_vector_t_at (&m->tokens, idx);
         if (curr_tok.type == Token_EOF)
            {
               break;
            }

         token_t next_tok = *token_vector_t_at (&m->tokens, idx + 1);
         if (curr_tok.type == Token_Ident)
            {

               if (next_tok.type == Token_LCurly)
                  {
                     token_t *orig_ident
                         = (token_t *)token_vector_t_at (&m->tokens, idx);
                     const char *macro_name    = curr_tok.value.chars;
                     token_vector_t macro_body = token_vector_t_new (16);
                     idx += 2;
                     while (idx < prog_size)
                        {
                           token_t t = *token_vector_t_at (&m->tokens, idx);
                           if (t.type == Token_RCurly)
                              {
                                 idx++;
                                 break;
                              }
                           if (t.type == Token_EOF)
                              {
                                 cry (&t, "expected '}', found EOF.");
                              }
                           token_vector_t_push (&macro_body, t);
                           idx++;
                        }
                     if (curr_tok.type == Token_EOF)
                        {
                           cry (&curr_tok,
                                "expected ending curly brace }, "
                                "found end of file instead.");
                        }

                     macro_t macro = new_macro (macro_name, macro_body);
                     macro_vector_t_push (&m->macros, macro);
                     if (orig_ident->value.chars)
                        {
                           free ((void *)orig_ident->value.chars);
                           orig_ident->value.chars = NULL;
                        }
                     continue;
                  }
            }

         /* at this point, curr_tok is either Token_RCurly or something else. */
         token_vector_t_push (&new_tokens, curr_tok);
         idx++;
      }

   /*   m->tokens = new_tokens; */
   replace_tokens (&m->tokens, &new_tokens);
}

static bool
is_in_expansion_stack (const unsigned int *stack,
                       unsigned int depth,
                       unsigned int h)
{
   for (unsigned int i = 0; i < depth; i++)
      {
         if (stack[i] == h)
            {
               return true;
            }
      }
   return false;
}

static void
expand_tokens_into (token_vector_t *expanded_tokens,
                    token_vector_t *tokens,
                    macro_vector_t *macros,
                    unsigned int *expansion_stack,
                    unsigned int *expansion_depth)
{
   unsigned int i       = 0;
   const unsigned int n = tokens->size;

   while (i < n)
      {
         token_t curr = *token_vector_t_at (tokens, i);
         if (curr.type == Token_EOF)
            {
               break;
            }

         token_t next = (i + 1 < n)
                            ? *token_vector_t_at (tokens, i + 1)
                            : (token_t){ .type = Token_EOF, .lc = curr.lc };

         if (curr.type == Token_Ident && next.type == Token_Semicolon)
            {
               const char *name = curr.value.chars;
               unsigned int h   = str_hash (name);

               /* detect infinite-expansion cycles */
               if (is_in_expansion_stack (expansion_stack, *expansion_depth, h))
                  {
                     cry (&curr,
                          "macro `%s` eventually expands to itself, leading "
                          "to an infinite expansion.",
                          name);
                  }

               /* find macro and expand its body recursively */
               bool found = false;
               for (unsigned int m = 0; m < macros->size; m++)
                  {
                     macro_t mm = *macro_vector_t_at (macros, m);
                     if (mm.hash == h)
                        {
                           found = true;

                           if (*expansion_depth >= 1024)
                              {
                                 cry (&curr, "macro expansion depth exceeded");
                              }
                           expansion_stack[(*expansion_depth)++] = h;

                           /* recursively expand the macro body INTO
                            * =expanded_tokens= */
                           expand_tokens_into (expanded_tokens,
                                               &mm.body,
                                               macros,
                                               expansion_stack,
                                               expansion_depth);

                           (*expansion_depth)--;

                           i += 2; /* skip ident and ';' */
                           goto continue_loop;
                        }
                  }

               if (!found)
                  {
                     cry (&curr, "bad macro '%s' was called.", name);
                  }
            }

         /* default: copy through */
         token_vector_t_push (expanded_tokens, curr);
         i++;

      continue_loop:;
      }
}

/* expand inline in a single pass. no recursion. */
/* returns true if any macro was expanded in this pass. */
bool
macromiser_expand_macros (macromiser_t *m,
                          unsigned int *expansion_stack,
                          unsigned int *expansion_depth)
{
   token_vector_t expanded_tokens = token_vector_t_new (32);
   unsigned int before            = m->tokens.size;

   expand_tokens_into (&expanded_tokens,
                       &m->tokens,
                       &m->macros,
                       expansion_stack,
                       expansion_depth);

   replace_tokens (&m->tokens, &expanded_tokens);

   /* consider "something was expanded" if size differs */
   return m->tokens.size != before;
}
