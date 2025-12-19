#include "str.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

string_t
new_string (unsigned int capacity)
{
   assert ((capacity > 0) && "capacity must be greater than zero.");
   string_t vec = {
      .elems     = malloc (sizeof(char) * capacity),
      .size      = 0,
      .capacity  = capacity,
   };

   return vec;
}

void
string_push (string_t *this, char elem)
{
   /* case 1: size >= capacity */
   if (this->size >= this->capacity)
      {
         this->capacity *= 2;
         unsigned int elem_size = sizeof(char);
         void *new_elems        = malloc (elem_size * this->capacity);
         memmove (new_elems, this->elems, this->size * sizeof(char));
         free (this->elems);
         this->elems = new_elems;
      }

   /* case 2: size < capacity */
   void *dest = (char *)this->elems + this->size * sizeof(char);
   memmove (dest, &elem, sizeof(char));
   this->size++;
}

void *
string_at (string_t *this, unsigned int index)
{
   return (char *)this->elems + index * sizeof(char);
}

void
string_free (string_t *str)
{
   free (str->elems);
   str->elems = NULL;
   str->capacity = 0;
   str->size = 0;
}
