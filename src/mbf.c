#include "mbf.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

const char *
mbf_expand_macros (const char *program)
{
   char *expanded_bf = malloc (sizeof (char) * 256);

   char current_char = '\1';
   int prog_idx      = 0;

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
               printf ("found weird char `%c`\n", current_char);
	       assert(0);
            }
      }

   return expanded_bf;
}
