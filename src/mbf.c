#include "mbf.h"

#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macromiser_t.h"
#include "tokeniser_t.h"

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

// [PARAMS]
//    o   tokens -- vector_t of token_t's
void
mbf_expand_number_prefixes (vector_t *tokens)
{
   assert (tokens->size > 0 && "need at least 1 token");
   assert (tokens->elem_size == sizeof (token_t)
           && "doesn't smell like a token_t...");

   // we don't care about anything else,
   // except for numbers and the token that
   // follows it.

   vector_t expanded_tokens = new_vector (tokens->size, tokens->elem_size);

   unsigned int i = 0;
   while (i < tokens->size)
      {
         token_t *curr_tok = (token_t *)vector_at (tokens, i);

         switch (curr_tok->type)
            {
            case Token_Number:
               {
                  unsigned int times = curr_tok->n_val;

                  token_t *next_tok = (token_t *)vector_at (tokens, i + 1);
                  if (next_tok->type != Token_Plus
                      && next_tok->type != Token_Minus)
                     {
                        printf ("times %d\n", times);
                        cry (next_tok, "expected either + or -, got a %s",
                             tok_to_str (next_tok->type));
                     }

                  i++;
                  curr_tok = (token_t *)vector_at (tokens, i);

                  // curr_tok is now either + or -

                  for (unsigned int i = 0; i < times; i++)
                     {
                        vector_push_elem (&expanded_tokens, curr_tok);
                     }
               }
               break;

            default:
               vector_push_elem (&expanded_tokens, curr_tok);
               break;
            }
         i++;
      }

   memmove (tokens, &expanded_tokens,
            expanded_tokens.size * expanded_tokens.elem_size);
}

const char *
mbf_preprocess (const char *program)
{
   char *expanded_bf = malloc (sizeof (char) * 256);

   tokeniser_t tokeniser = {
      .program = program, .prog_idx = 0,
      // .tokens not initialised -- inited by =mbf_tokenise=
   };

   mbf_tokenise (&tokeniser);

   mbf_expand_number_prefixes (&tokeniser.tokens);

   // print_tokens(tokeniser.tokens);

   // for actual macros, we will use a two-phase approach -- first collect,
   // then expand

   // what's the noun for something that collects macros?
   //   i'll go with `macromiser`, sounds good enough

   macromiser_t macromiser = new_macromiser (tokeniser.tokens);

   macromiser_collect_macros (&macromiser);

   printf ("collected %d macros\n", macromiser.macros.size);
   assert (0 && "collected macros, now what?");
   // macromiser_expand_macros (&macromiser);

   return expanded_bf;
}
