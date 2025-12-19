#ifndef TOKENISER_T_H
#define TOKENISER_T_H

#include "str.h"
#include "vector.h"

typedef struct
{
   unsigned int line;
   unsigned int col;
} line_col_t;

#define TOKEN_TYPE_LIST                                                       \
   /* MBF */                                                                  \
   X (Token_Ident)                                                            \
   X (Token_Number)                                                           \
                                                                              \
   X (Token_LCurly)                                                           \
   X (Token_RCurly)                                                           \
                                                                              \
   X (Token_Semicolon)                                                        \
                                                                              \
   /* Classic BF */                                                           \
   X (Token_Plus)                                                             \
   X (Token_Minus)                                                            \
                                                                              \
   X (Token_Left)                                                             \
   X (Token_Right)                                                            \
                                                                              \
   X (Token_LLoop)                                                            \
   X (Token_RLoop)                                                            \
                                                                              \
   X (Token_Dot)                                                              \
   X (Token_Comma)                                                            \
                                                                              \
   /* Misc. */                                                                \
   X (Token_EOF)

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

void token_free_wrapper (void *p);

typedef struct
{
   const char *program;
   unsigned int prog_idx;
   vector_t tokens; /* vector_t of token_t's */
} tokeniser_t;

char *tok_to_str (token_type_t tok);
void print_tokens (vector_t tokens);
string_t tokens_to_bf_str (vector_t tokens);

void mbf_tokenise (tokeniser_t *tokeniser);

void tokeniser_free (tokeniser_t *);

#endif /* TOKENISER_T_H */
