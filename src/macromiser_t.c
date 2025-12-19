#include "macromiser_t.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "tokeniser_t.h"
#include "util.h"

#define bool int
#define true 1
#define false 0

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
free_token_heap_fields_vector (vector_t *v)
{
   for (unsigned int i = 0; i < v->size; i++)
      {
         token_t *t = (token_t *)vector_at (v, i);
         if (t->type == Token_Ident && t->value.chars)
            {
               free ((void *)t->value.chars);
               t->value.chars = NULL;
            }
      }
}

static void
replace_tokens (vector_t *dst, vector_t *src)
{
   vector_t old = *dst;
   *dst         = *src;
   vector_free (&old);
}

/* impl */

macro_t
new_macro (const char *macro_name, vector_t body)
{
   macro_t m = {
      .hash = str_hash (macro_name),
      .body = body,
   };
   return m;
}

macromiser_t
new_macromiser (vector_t tokens)
{
   macromiser_t m = { 0 };

   m.macros = new_vector (4, sizeof (macro_t));
   m.tokens = tokens;

   return m;
}

void
macromiser_free (macromiser_t *m)
{
   for (unsigned int i = 0; i < m->macros.size; i++)
      {
         macro_t *mm = (macro_t *)vector_at (&m->macros, i);
         free_token_heap_fields_vector (&mm->body);
         vector_free (&mm->body);
      }
   vector_free (&m->macros);

   free_token_heap_fields_vector (&m->tokens);
   vector_free (&m->tokens);
}

void
macromiser_collect_macros (macromiser_t *m)
{
   unsigned int idx       = 0;
   unsigned int prog_size = m->tokens.size;
   vector_t new_tokens    = new_vector (32, sizeof (token_t));

   while (idx < prog_size)
      {
         token_t curr_tok = *(token_t *)vector_at (&m->tokens, idx);
         if (curr_tok.type == Token_EOF)
            {
               break;
            }

         token_t next_tok = *(token_t *)vector_at (&m->tokens, idx + 1);

         if (curr_tok.type == Token_Ident)
            {

               if (next_tok.type == Token_LCurly)
                  {
                     token_t *orig_ident
                         = (token_t *)vector_at (&m->tokens, idx);
                     const char *macro_name = curr_tok.value.chars;
                     vector_t macro_body = new_vector (16, sizeof (token_t));
                     idx += 2;
                     while (idx < prog_size)
                        {
                           token_t t = *(token_t *)vector_at (&m->tokens, idx);
                           if (t.type == Token_RCurly)
                              {
                                 idx++;
                                 break;
                              }
                           if (t.type == Token_EOF)
                              {
                                 cry (&t, "expected '}', found EOF.");
                              }
                           vector_push_elem (&macro_body, &t);
                           idx++;
                        }
                     if (curr_tok.type == Token_EOF)
                        {
                           cry (&curr_tok, "expected ending curly brace }, "
                                           "found end of file instead.");
                        }

                     macro_t macro = new_macro (macro_name, macro_body);
                     vector_push_elem (&m->macros, &macro);
                     if (orig_ident->value.chars)
                        {
                           free ((void *)orig_ident->value.chars);
                           orig_ident->value.chars = NULL;
                        }
                     continue;
                  }
            }

         /* at this point, curr_tok is either Token_RCurly or something else. */
         vector_push_elem (&new_tokens, &curr_tok);
         idx++;
      }

   /*   m->tokens = new_tokens; */
   replace_tokens (&m->tokens, &new_tokens);
}

static bool
is_in_expansion_stack (const unsigned int *stack, unsigned int depth,
                       unsigned int h)
{
   for (unsigned int i = 0; i < depth; i++)
      {
         if (stack[i] == h)
            return true;
      }
   return false;
}

static void
expand_tokens_into (vector_t *expanded_tokens, vector_t *tokens,
                    vector_t *macros, unsigned int *expansion_stack,
                    unsigned int *expansion_depth)
{
   unsigned int i       = 0;
   const unsigned int n = tokens->size;

   while (i < n)
      {
         token_t curr = *(token_t *)vector_at (tokens, i);
         if (curr.type == Token_EOF)
            {
               break;
            }

         token_t next = (i + 1 < n)
                            ? *(token_t *)vector_at (tokens, i + 1)
                            : (token_t){ .type = Token_EOF, .lc = curr.lc };

         if (curr.type == Token_Ident && next.type == Token_Semicolon)
            {
               const char *name = curr.value.chars;
               unsigned int h   = str_hash (name);

               /* detect infinite-expansion cycles */
               if (is_in_expansion_stack (expansion_stack, *expansion_depth,
                                          h))
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
                     macro_t mm = *(macro_t *)vector_at (macros, m);
                     if (mm.hash == h)
                        {
                           found = true;

                           if (*expansion_depth >= 1024)
                              {
                                 cry (&curr, "macro expansion depth exceeded");
                              }
                           expansion_stack[(*expansion_depth)++] = h;

                           /* recursively expand the macro body INTO =expanded_tokens= */
                           expand_tokens_into (expanded_tokens, &mm.body,
                                               macros, expansion_stack,
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
         vector_push_elem (expanded_tokens, &curr);
         i++;

      continue_loop:;
      }
}

/* expand inline in a single pass. no recursion. */
/* returns true if any macro was expanded in this pass. */
bool
macromiser_expand_macros (macromiser_t *m, unsigned int *expansion_stack,
                          unsigned int *expansion_depth)
{
   vector_t expanded_tokens = new_vector (32, sizeof (token_t));
   unsigned int before      = m->tokens.size;

   expand_tokens_into (&expanded_tokens, &m->tokens, &m->macros,
                       expansion_stack, expansion_depth);

   replace_tokens (&m->tokens, &expanded_tokens);

   /* consider "something was expanded" if size differs */
   return m->tokens.size != before;   
}
