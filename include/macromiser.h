#ifndef MACROMISER_H
#define MACROMISER_H

#include <stdbool.h>

#include "tokeniser.h"

typedef struct
{
   /* instead of storing the macro name, */
   /* we'll store its hash */
   unsigned int hash;
   token_vector_t body;
} macro_t;

typedef struct
{
   macro_t *elems;
   size_t count;
   size_t capacity;
} macro_vector_t;

macro_t new_macro (const char *macro_name, token_vector_t body);

typedef struct
{
   token_vector_t tokens;
   macro_vector_t macros;
} macromiser_t;

macromiser_t new_macromiser (token_vector_t tokens);

/* collects the macros and removes macro definitions from =tokens= */
void macromiser_collect_macros (macromiser_t *);

/* expands macros into =tokens= */
bool macromiser_expand_macros (macromiser_t *m,
                               unsigned int *expansion_stack,
                               unsigned int *expansion_depth);

void macromiser_free (macromiser_t *);

#endif /* MACROMISER_H */
