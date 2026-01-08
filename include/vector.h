#ifndef VECTOR_H
#define VECTOR_H

#include <assert.h>
#include <stdlib.h>

#define DECLARE_VECTOR(type, name)                                             \
   typedef struct                                                              \
   {                                                                           \
      type *elems;                                                             \
      unsigned int size;                                                       \
      unsigned int capacity;                                                   \
   } name;                                                                     \
   name name##_new(unsigned int capacity);                                     \
   void name##_push(name *v, type elem);                                       \
   type *name##_at(name *v, unsigned int index);                               \
   void name##_free(name *v);

#define IMPLEMENT_VECTOR(type, name)                                           \
   name name##_new(unsigned int capacity)                                      \
   {                                                                           \
      assert((capacity > 0) && "capacity must be > 0");                        \
      name v;                                                                  \
      v.elems    = malloc(sizeof(type) * capacity);                            \
      v.size     = 0;                                                          \
      v.capacity = capacity;                                                   \
      return v;                                                                \
   }                                                                           \
   void name##_push(name *v, type elem)                                        \
   {                                                                           \
      if (v->size >= v->capacity) {                                            \
         v->capacity *= 2;                                                     \
         v->elems = realloc(v->elems, sizeof(type) * v->capacity);             \
      }                                                                        \
      v->elems[v->size++] = elem;                                              \
   }                                                                           \
   type *name##_at(name *v, unsigned int index)                                \
   {                                                                           \
      assert(index < v->size && "Index out of bounds");                        \
      return &v->elems[index];                                                 \
   }                                                                           \
   void name##_free(name *v)                                                   \
   {                                                                           \
      free(v->elems);                                                          \
      v->elems = NULL;                                                         \
      v->size = v->capacity = 0;                                               \
   }

#endif /* VECTOR_H */
