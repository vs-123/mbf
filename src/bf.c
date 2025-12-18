#include "bf.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAPE_SIZE 30000

void
mbf_exec_bf (const char *bf)
{
   unsigned char tape[TAPE_SIZE] = { 0 };
   unsigned char *ptr            = tape;

   for (unsigned int i = 0; bf[i] != '\0'; i++)
      {
         switch (bf[i])
            {
            case '>':
               ptr++;
               break;

            case '<':
               ptr--;
               break;

            case '+':
               (*ptr)++;
               break;

            case '-':
               (*ptr)--;
               break;

            case '.':
               putchar (*ptr);
               break;

            case ',':
               *ptr = getchar ();
               break;

            case '[':
               if (*ptr == 0)
                  {
                     unsigned int loop = 1;
                     while (loop > 0)
                        {
                           i++;
                           if (bf[i] == '[')
                              loop++;
                           if (bf[i] == ']')
                              loop--;
                        }
                  }
               break;

            case ']':
               if (*ptr != 0)
                  {
                     unsigned int loop = 1;
                     while (loop > 0)
                        {
                           i--;
                           if (bf[i] == '[')
                              loop--;
                           if (bf[i] == ']')
                              loop++;
                        }
                  }
               break;

            default:
               break;
            }
      }
}
