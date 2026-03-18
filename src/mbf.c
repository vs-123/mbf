#include "mbf.h"

#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macromiser.h"
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

void
mbf_expand_number_prefixes (token_vector_t *tokens)
{
   assert (tokens->count > 0 && "need at least 1 token");

   /* WE DON'T CARE ABOUT ANYTHING ELSE, */
   /* EXCEPT FOR NUMBERS AND THE TOKEN THAT */
   /* FOLLOWS IT. */

   token_vector_t expanded_tokens = { 0 };

   unsigned int i = 0;
   while (i < tokens->count)
      {
         token_t curr_tok = tokens->elems[i];

         if (curr_tok.type == TOKEN_NUMBER)
            {

               token_t next_tok   = tokens->elems[i + 1];
               unsigned int times = curr_tok.value.num;

               if (next_tok.type != TOKEN_PLUS && next_tok.type != TOKEN_MINUS
                   && next_tok.type != TOKEN_LEFT
                   && next_tok.type != TOKEN_RIGHT)
                  {
                     cry (&next_tok,
                          "EXPECTED ONE OF + - < >, INSTEAD GOT %s",
                          tok_to_str (next_tok.type));
                  }

               i++;
               curr_tok = tokens->elems[i];

               /* curr_tok is now either one of + - < > */
               for (unsigned int i = 0; i < times; i++)
                  {
                     DAPPEND (expanded_tokens, curr_tok);
                  }
            }
         else
            {
               DAPPEND (expanded_tokens, curr_tok);
            }

         i++;
      }

   token_vector_t old = *tokens;
   *tokens            = expanded_tokens;
   free (old.elems);
}

dstr_t
mbf_preprocess (const char *program)
{
   tokeniser_t tokeniser = {
      .program  = program,
      .prog_idx = 0,
      .tokens   = { 0 },
   };

   mbf_tokenise (&tokeniser);
   mbf_expand_number_prefixes (&tokeniser.tokens);

   /* FOR ACTUAL MACROS, WE WILL USE A TWO-PHASE APPROACH: */
   /*    1. COLLECT */
   /*    2. EXPAND */

   unsigned int expansion_stack[1024];
   unsigned int expansion_depth = 0;

   /* WHAT'S THE NOUN FOR SOMETHING THAT COLLECTS MACROS? */
   /*   I'LL GO WITH `MACROMISER`, SOUNDS GOOD ENOUGH */

   macromiser_t macromiser = new_macromiser (tokeniser.tokens);

   macromiser_collect_macros (&macromiser);

   /* KEEP EXPANDING UNTIL NO MACROS ARE LEFT, BUT LET'S NOT RUN IT FOREVER */
   /* AT MOST WE'LL HAVE 32 EXPANSION-PASSES */
   for (int pass = 0; pass < 32; pass++)
      {
         bool something_has_NOT_expanded
             = !macromiser_expand_macros (&macromiser,
                                          expansion_stack,
                                          &expansion_depth);
         if (something_has_NOT_expanded)
            {
               break;
            }
      }

   dstr_t bf_str = tokens_to_bf_str (macromiser.tokens);

   macromiser_free (&macromiser);

   return bf_str;
}
