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

// impl

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
macromiser_collect_macros (macromiser_t *m)
{
   unsigned int idx       = 0;
   unsigned int prog_size = m->tokens.size;
   vector_t new_tokens    = new_vector (32, sizeof (token_t));

   while (idx < prog_size)
      {
         token_t *curr_tok = (token_t *)vector_at (&m->tokens, idx);
         if (curr_tok->type == Token_EOF)
            {
               break;
            }

         token_t *next_tok  = (token_t *)vector_at (&m->tokens, idx + 1);
         bool is_macro_defn = false;

         if (curr_tok->type == Token_Ident)
            {

               if (next_tok->type == Token_LCurly)
                  {
                     is_macro_defn          = true;
                     const char *macro_name = curr_tok->c_val;
                     vector_t macro_body = new_vector (16, sizeof (token_t));
                     idx++;
                     idx++;
                     curr_tok = (token_t *)vector_at (&m->tokens, idx);
                     while (curr_tok->type != Token_RCurly
                            && curr_tok->type != Token_EOF)
                        {
                           idx++;
                           curr_tok = (token_t *)vector_at (&m->tokens, idx);
                           vector_push_elem (&macro_body, (void *)curr_tok);
                        }
                     if (curr_tok->type == Token_EOF)
                        {
                           cry (curr_tok, "expected ending curly brace }, "
                                          "found end of file instead.");
                        }
                     macro_t *macro = malloc (sizeof (macro_t));
                     *macro         = new_macro (macro_name, macro_body);
                     vector_push_elem (&m->macros, (void *)macro);
                  }
            }
         if (!is_macro_defn)
            {
               vector_push_elem (&new_tokens, curr_tok);
            }
         idx++;
      }

   m->tokens = new_tokens;
}
