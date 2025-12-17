#ifndef MACROMISER_T_H
#define MACROMISER_T_H

#include "tokeniser_t.h"
#include "vector.h"

#define bool int
#define true 1
#define false 0

typedef struct
{
   // instead of storing the macro name,
   // we'll store its hash
   unsigned int hash;
   vector_t body; // vector_t of token_t's
} macro_t;

macro_t new_macro (const char *macro_name, vector_t body);

typedef struct
{
   vector_t tokens; // vector_t of tokens_t's
   vector_t macros; // vector_t of macro_t's
} macromiser_t;

macromiser_t new_macromiser (vector_t tokens);

// collects the macros and removes macro definitions from =tokens=
void macromiser_collect_macros (macromiser_t *);

// expands macros into =tokens=
bool macromiser_expand_macros (macromiser_t *);

void macromiser_free(macromiser_t*);

#endif // MACROMISER_T_H
