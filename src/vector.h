#ifndef VECTOR_H
#define VECTOR_H

typedef struct
{
   void *elems;
   unsigned int elem_size;
   unsigned int size;
   unsigned int capacity;
} vector_t;

vector_t new_vector (unsigned int capacity, unsigned int elem_size);

void vector_push_elem (vector_t *this, void *elem);

void *vector_at (vector_t *this, unsigned int index);

void vector_free (vector_t *v);


#endif // VECTOR_H
