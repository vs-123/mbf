#include "mbf.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "vector.h"
#include "str.h"

typedef enum
{
   Token_Ident,
   Token_Number,

   Token_Left,
   Token_Right,
   Token_Dot,
} token_type_t;

typedef struct
{
   token_type_t type;
   union
   {
      int n_val;
      const char *c_val;
   };
} token_t;

const token_t *
mbf_tokenise (const char *program, unsigned int prog_idx)
{
   char current_char = '\1';
   vector_t tokens = new_vector(32, sizeof(tokens));

   while ((current_char = program[prog_idx++]) != '\0')
      {
         switch (current_char)
            {
            // comments
            case '#':
            case ';':
               while (current_char != '\n' && current_char != '\0')
                  {
                     current_char = program[prog_idx++];
                  }
               break;

            // skip whitespaces, tabs and newlines
            case '\t':
            case '\n':
            case '\r':
            case ' ':
               break;

            default:
               if (isascii (current_char))
                  {
		     assert(0 && "identifiers not implemented");
                  }
               printf ("found weird char `%c`\n", current_char);
            }
      }
}

// mbf.h impl

const char *
mbf_preprocess (const char *program)
{
   char *expanded_bf = malloc (sizeof (char) * 256);
   const token_t *tokens = mbf_tokenise (program, 0);


   assert(0 && "not implemented.");

   return expanded_bf;
}
