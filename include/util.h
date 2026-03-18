#ifndef UTIL_H
#define UTIL_H

#define DAPPEND(vector, i)                                                     \
   do                                                                          \
      {                                                                        \
         if (vector.count >= vector.capacity)                                  \
            {                                                                  \
               vector.capacity                                                 \
                   = (vector.capacity) ? vector.capacity * 2 : 256;            \
               vector.elems                                                    \
                   = realloc (vector.elems,                                    \
                              vector.capacity * sizeof (vector.elems[0]));     \
            }                                                                  \
         vector.elems[vector.count++] = i;                                     \
      }                                                                        \
   while (0);

unsigned int str_hash (const char *str);

#endif /* UTIL_H */
