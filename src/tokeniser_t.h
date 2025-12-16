#ifndef TOKENISER_T_H
#define TOKENISER_T_H

#include "str.h"
#include "vector.h"

typedef struct
{
   unsigned int line;
   unsigned int col;
} line_col_t;

typedef enum
{
   // MBF
   Token_Ident,
   Token_Number,

   Token_LCurly,
   Token_RCurly,

   Token_Semicolon,

   // Classic BF
   Token_Plus,
   Token_Minus,

   Token_Left,
   Token_Right,

   Token_LLoop,
   Token_RLoop,

   Token_Dot,

   // Misc.
   Token_EOF,
} token_type_t;

typedef struct
{
   token_type_t type;
   union
   {
      unsigned int n_val;
      const char *c_val;
   };
   line_col_t lc; // line & col
} token_t;

typedef struct
{
   const char *program;
   unsigned int prog_idx;
   vector_t tokens; // vector_t of token_t's
} tokeniser_t;

char *tok_to_str (token_type_t tok);
void print_tokens (vector_t tokens);
string_t tokens_to_bf_str (vector_t tokens);

void mbf_tokenise (tokeniser_t *tokeniser);

#endif // TOKENISER_T_H
