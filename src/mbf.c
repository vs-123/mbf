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

/* [PARAMS] */
/*    o   tokens -- vector_t of token_t's */
void
mbf_expand_number_prefixes (vector_t *tokens)
{
   assert (tokens->size > 0 && "need at least 1 token");
   assert (tokens->elem_size == sizeof (token_t)
           && "doesn't smell like a token_t...");

   /* we don't care about anything else, */
   /* except for numbers and the token that */
   /* follows it. */

   vector_t expanded_tokens = new_vector (tokens->size, sizeof (token_t));

   unsigned int i = 0;
   while (i < tokens->size)
      {
         token_t curr_tok = *(token_t *)vector_at (tokens, i);

         if (curr_tok.type == Token_Number)
            {

               token_t next_tok   = *(token_t *)vector_at (tokens, i + 1);
               unsigned int times = curr_tok.value.num;

               if (next_tok.type != Token_Plus && next_tok.type != Token_Minus
                   && next_tok.type != Token_Left
                   && next_tok.type != Token_Right)
                  {
                     cry (&next_tok, "expected one of + - < >, instead got %s",
                          tok_to_str (next_tok.type));
                  }

               i++;
               curr_tok = *(token_t *)vector_at (tokens, i);

               /* curr_tok is now either one of + - < > */
               for (unsigned int i = 0; i < times; i++)
                  {
                     vector_push_elem (&expanded_tokens, &curr_tok);
                  }
            }
         else
            {
               vector_push_elem (&expanded_tokens, &curr_tok);
            }

         i++;
      }

   vector_t old = *tokens;
   *tokens      = expanded_tokens;
   vector_free (&old);
}

dstr_t
mbf_preprocess (const char *program)
{
   tokeniser_t tokeniser = {
      .program = program, .prog_idx = 0,
      /* .tokens not initialised -- inited by =mbf_tokenise= */
   };

   mbf_tokenise (&tokeniser);
   mbf_expand_number_prefixes (&tokeniser.tokens);

   /* for actual macros, we will use a two-phase approach: */
   /*    1. collect */
   /*    2. expand */

   unsigned int expansion_stack[1024];
   unsigned int expansion_depth = 0;

   /* what's the noun for something that collects macros? */
   /*   i'll go with `macromiser`, sounds good enough */

   macromiser_t macromiser = new_macromiser (tokeniser.tokens);

   macromiser_collect_macros (&macromiser);

   /* keep expanding until no macros are left, but let's not run it forever */
   /* at most we'll have 32 expansion-passes */
   for (int pass = 0; pass < 32; pass++)
      {
         bool something_has_NOT_expanded = !macromiser_expand_macros (
             &macromiser, expansion_stack, &expansion_depth);
         if (something_has_NOT_expanded)
            {
               break;
            }
      }

   dstr_t bf_str = tokens_to_bf_str (macromiser.tokens);

   macromiser_free (&macromiser);

   return bf_str;
}
