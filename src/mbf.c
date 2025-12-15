#include "mbf.h"

#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tokeniser_t.h"

#include "str.h"
#include "vector.h"

// tbh compilers yelling errors
// feels like it's crying
// and yup this throws an error
void
cry (const tokeniser_t *tokeniser, const char *format, ...)
{
   const unsigned int idx = tokeniser->prog_idx;
   const char *prog       = tokeniser->program;
   unsigned int line      = 1;
   unsigned int col       = 1;

   char c = '\1';

   for (unsigned int i = 0; i < idx; i++)
      {
         c = prog[i];
         if (c == '\n')
            {
               line++;
               col = 0;
            }
         col++;
      }

   fprintf (stderr, "[ERROR %u:%u] ", line, col);

   va_list args;

   va_start (args, format);
   vfprintf (stderr, format, args);
   va_end (args);

   fprintf (stderr, "\n");

   exit (1);
}

void
tokenise_ident (tokeniser_t *tokeniser)
{
   // we're at an isalpha character right now
   // eat up isalnum characters until we hit a non-isalnum char
   // and then push it as a token to tokeniser->tokens

   // starting with 32 capacity
   string_t eaten_alnum = new_string (32);
   char curr_char       = '\1';

   while ((isalnum ((curr_char = tokeniser->program[tokeniser->prog_idx++]))
           || curr_char == '_'))
      {
         string_push (&eaten_alnum, curr_char);
      }

   token_t *token = malloc (sizeof (token_t));
   token->type    = Token_Ident;
   token->c_val   = eaten_alnum.elems;

   vector_push_elem (&tokeniser->tokens, (void *)token);
}

void
tokenise_num (tokeniser_t *tokeniser)
{
   // we're at an isdigit character right now
   // eat up isalnum characters until we hit a non-isdigit char
   // and then push it as a token to tokeniser->tokens

   // no malloc; it's a number
   unsigned int eaten_num = 0;
   char curr_char         = '\1';

   while (isdigit ((curr_char = tokeniser->program[tokeniser->prog_idx++])))
      {
         eaten_num *= 10;
         eaten_num += (curr_char - '0');
      }

   token_t *token = malloc (sizeof (token_t));
   token->type    = Token_Number;
   token->n_val   = eaten_num;

   vector_push_elem (&tokeniser->tokens, (void *)token);
}

void
mbf_tokenise (tokeniser_t *tokeniser)
{
   tokeniser->tokens = new_vector (32, sizeof (token_t));
   char current_char = '\1';

   unsigned int prog_size = strlen(tokeniser->program);

   while (tokeniser->prog_idx < prog_size)
      {
	 current_char = tokeniser->program[tokeniser->prog_idx];
         switch (current_char)
            {
            // comments
            case '#':
            case ';':
               while (current_char != '\n' && current_char != '\0')
                  {
                     current_char = tokeniser->program[++tokeniser->prog_idx];
                  }
               break;

            // skip whitespaces, tabs and newlines
            case '\t':
            case '\n':
            case '\r':
            case ' ':
               break;

            case '{':
               {
                  token_t *token = malloc (sizeof (token_t));
                  token->type    = Token_LCurly;
                  vector_push_elem (&tokeniser->tokens, (void *)token);
               }
               break;

            case '}':
               {
                  token_t *token = malloc (sizeof (token_t));
                  token->type    = Token_RCurly;
                  vector_push_elem (&tokeniser->tokens, (void *)token);
               }
               break;

            case '+':
               {
                  token_t *token = malloc (sizeof (token_t));
                  token->type    = Token_Plus;
                  vector_push_elem (&tokeniser->tokens, (void *)token);
               }
               break;

            case '-':
               {
                  token_t *token = malloc (sizeof (token_t));
                  token->type    = Token_Minus;
                  vector_push_elem (&tokeniser->tokens, (void *)token);
               }
               break;

            case '<':
               {
                  token_t *token = malloc (sizeof (token_t));
                  token->type    = Token_Left;
                  vector_push_elem (&tokeniser->tokens, (void *)token);
               }
               break;

            case '>':
               {
                  token_t *token = malloc (sizeof (token_t));
                  token->type    = Token_Right;
                  vector_push_elem (&tokeniser->tokens, (void *)token);
               }
               break;

            case '[':
               {
                  token_t *token = malloc (sizeof (token_t));
                  token->type    = Token_LLoop;
                  vector_push_elem (&tokeniser->tokens, (void *)token);
               }
               break;

            case ']':
               {
                  token_t *token = malloc (sizeof (token_t));
                  token->type    = Token_RLoop;
                  vector_push_elem (&tokeniser->tokens, (void *)token);
               }
               break;

            case '.':
               {
                  token_t *token = malloc (sizeof (token_t));
                  token->type    = Token_Dot;
                  vector_push_elem (&tokeniser->tokens, (void *)token);
               }
               break;

            case '\0':
               {
                  token_t *token = malloc (sizeof (token_t));
                  token->type    = Token_EOF;
                  vector_push_elem (&tokeniser->tokens, (void *)token);
               }
               break;

            default:
               if (isalpha (current_char))
                  {
                     // cry(tokeniser, "before curr char : %c", current_char);

                     tokenise_ident (tokeniser);
                     // cry(tokeniser, "after curr char : %c", current_char);
                     // const token_t *ident = vector_at (&tokeniser->tokens,
                     // 0); cry (tokeniser, "found ident: %s", ident->c_val);
                  }
               else if (isdigit (current_char))
                  {
                     tokenise_num (tokeniser);
                  }
               else
                  {
                     cry (tokeniser, "found weird char `%c` (code: %d)\n",
                          current_char, (int)current_char);
                  }
            }
         tokeniser->prog_idx++;
      }
}

// mbf.h impl

const char *
mbf_preprocess (const char *program)
{
   char *expanded_bf = malloc (sizeof (char) * 256);

   tokeniser_t tokeniser = {
      .program = program, .prog_idx = 0,
      // .tokens not initialised -- inited by =mbf_tokenise=
   };

   mbf_tokenise (&tokeniser);

   assert (0 && "tokenised, now what?");

   return expanded_bf;
}
