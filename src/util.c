#include "util.h"

unsigned int
str_hash (const char *str)
{
   unsigned long hash = 5381;
   int c;

   while ((c = *str++))
      {
         hash = ((hash << 5) + hash) + c;
      }

   return (unsigned int)hash;
}
