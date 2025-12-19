#ifndef STR_H
#define STR_H

typedef struct
{
   char *elems;
   unsigned int size;
   unsigned int capacity;
} string_t;

string_t new_string (unsigned int capacity);
void string_push (string_t *this, char);
void *string_at (string_t *this, unsigned int index);
void string_free(string_t*);

#endif /* STR_H */
