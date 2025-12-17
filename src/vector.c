#include "vector.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

vector_t
new_vector (unsigned int capacity, unsigned int elem_size)
{
   assert ((capacity > 0) && "capacity must be greater than zero.");
   assert ((elem_size > 0) && "element size must be greater than zero.");
   vector_t vec = {
      .elems     = malloc (elem_size * capacity),
      .elem_size = elem_size,
      .size      = 0,
      .capacity  = capacity,
   };

   return vec;
}

void
vector_push_elem (vector_t *this, void *elem)
{
   // case 1: size >= capacity
   if (this->size >= this->capacity)
      {
         this->capacity *= 2;
         unsigned int elem_size = this->elem_size;
         void *new_elems        = malloc (elem_size * this->capacity);
         memmove (new_elems, this->elems, this->size * this->elem_size);
         free (this->elems);
         this->elems = new_elems;
      }

   // case 2: size < capacity
   void *dest = (char *)this->elems + this->size * this->elem_size;
   memmove (dest, elem, this->elem_size);
   this->size++;
}

void *
vector_at (vector_t *this, unsigned int index)
{
   return (char *)this->elems + index * this->elem_size;
}

void
vector_free (vector_t *v)
{
   free (v->elems);
   v->elems = NULL;
   v->size = v->capacity = 0;
}
