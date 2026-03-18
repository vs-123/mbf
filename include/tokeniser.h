#ifndef TOKENISER_H
#define TOKENISER_H

#include "dstr.h"

typedef struct
{
   unsigned int line;
   unsigned int col;
} line_col_t;

#define TOKEN_TYPE_LIST                                                       \
   /* MBF */                                                                  \
   X (TOKEN_IDENT)                                                            \
   X (TOKEN_NUMBER)                                                           \
                                                                              \
   X (TOKEN_LCURLY)                                                           \
   X (TOKEN_RCURLY)                                                           \
                                                                              \
   X (TOKEN_SEMICOLON)                                                        \
                                                                              \
   /* CLASSIC BF */                                                           \
   X (TOKEN_PLUS)                                                             \
   X (TOKEN_MINUS)                                                            \
                                                                              \
   X (TOKEN_LEFT)                                                             \
   X (TOKEN_RIGHT)                                                            \
                                                                              \
   X (TOKEN_LLOOP)                                                            \
   X (TOKEN_RLOOP)                                                            \
                                                                              \
   X (TOKEN_DOT)                                                              \
   X (TOKEN_COMMA)                                                            \
                                                                              \
   /* MISC. */                                                                \
   X (TOKEN_EOF)

typedef enum
{
#define X(name) name,
   TOKEN_TYPE_LIST
#undef X
} token_type_t;

typedef struct
{
   token_type_t type;
   union
   {
      unsigned int num;
      const char *chars;
   } value;
   line_col_t lc; /* line & col */
} token_t;

void token_free (token_t *);

void token_free_wrapper(void *p);

typedef struct {
   token_t *elems;
   size_t count;
   size_t capacity;
} token_vector_t;

typedef struct
{
   const char *program;
   unsigned int prog_idx;
   token_vector_t tokens;
} tokeniser_t;

char *tok_to_str (token_type_t tok);
void print_tokens (token_vector_t tokens);
dstr_t tokens_to_bf_str (token_vector_t tokens);

void mbf_tokenise (tokeniser_t *tokeniser);

void tokeniser_free (tokeniser_t *);

#endif /* TOKENISER_H */
