#include "macromiser.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "tokeniser.h"
#include "util.h"

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
   for (unsigned int i = 0; i < v->count; i++)
      {
         token_t *t = &v->elems[i];
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
   free (old.elems);
   /* TODO token_vector_t_free (&old); */
}

/* impl */

void
macromiser_collect_macros (macromiser_t *m)
{
   unsigned int idx          = 0;
   unsigned int prog_size    = m->tokens.count;
   token_vector_t new_tokens = { 0 };

   while (idx < prog_size)
      {
         token_t curr_tok = m->tokens.elems[idx];
         if (curr_tok.type == Token_EOF)
            {
               break;
            }

         token_t next_tok = m->tokens.elems[idx + 1];
         if (curr_tok.type == Token_Ident)
            {

               if (next_tok.type == Token_LCurly)
                  {
                     token_t *orig_ident       = &m->tokens.elems[idx];
                     const char *macro_name    = curr_tok.value.chars;
                     token_vector_t macro_body = { 0 };
                     idx += 2;
                     while (idx < prog_size)
                        {
                           token_t t = m->tokens.elems[idx];
                           if (t.type == Token_RCurly)
                              {
                                 idx++;
                                 break;
                              }
                           if (t.type == Token_EOF)
                              {
                                 cry (&t, "expected '}', found EOF.");
                              }
                           DAPPEND (macro_body, t);
                           idx++;
                        }
                     if (curr_tok.type == Token_EOF)
                        {
                           cry (&curr_tok,
                                "expected ending curly brace }, "
                                "found end of file instead.");
                        }

                     macro_t macro = new_macro (macro_name, macro_body);
                     DAPPEND (m->macros, macro);
                     if (orig_ident->value.chars)
                        {
                           free ((void *)orig_ident->value.chars);
                           orig_ident->value.chars = NULL;
                        }
                     continue;
                  }
            }

         /* at this point, curr_tok is either Token_RCurly or something else. */
         DAPPEND (new_tokens, curr_tok);
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
   const unsigned int n = tokens->count;

   while (i < n)
      {
         token_t curr = tokens->elems[i];
         if (curr.type == Token_EOF)
            {
               break;
            }

         token_t next = (i + 1 < n)
                            ? tokens->elems[i + 1]
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
               for (unsigned int m = 0; m < macros->count; m++)
                  {
                     macro_t mm = macros->elems[m];
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
         DAPPEND ((*expanded_tokens), curr);
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
   token_vector_t expanded_tokens = { 0 };
   unsigned int before            = m->tokens.count;

   expand_tokens_into (&expanded_tokens,
                       &m->tokens,
                       &m->macros,
                       expansion_stack,
                       expansion_depth);

   replace_tokens (&m->tokens, &expanded_tokens);

   /* consider "something was expanded" if size differs */
   return m->tokens.count != before;
}

/****************/
/*  PUBLIC API  */
/****************/

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

   m.tokens = tokens;

   return m;
}

void
macromiser_free (macromiser_t *m)
{
   for (unsigned int i = 0; i < m->macros.count; i++)
      {
         macro_t *mm = &m->macros.elems[i];
         free_token_heap_fields_vector (&mm->body);
         free (mm->body.elems);
      }

   free_token_heap_fields_vector (&m->tokens);
   free (m->macros.elems);
   free (m->tokens.elems);
}
