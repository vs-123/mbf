#include "mbf.h"

#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tokeniser_t.h"

const char *
mbf_preprocess (const char *program)
{
   char *expanded_bf = malloc (sizeof (char) * 256);

   tokeniser_t tokeniser = {
      .program = program, .prog_idx = 0,
      // .tokens not initialised -- inited by =mbf_tokenise=
   };

   mbf_tokenise (&tokeniser);

   assert (0 && "tokenised, now what?");

   return expanded_bf;
}
