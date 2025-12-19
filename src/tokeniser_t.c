#include "tokeniser_t.h"

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "str.h"

line_col_t
get_line_col (const tokeniser_t *tokeniser)
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

   line_col_t lc = {
      .line = line,
      .col  = col,
   };

   return lc;
}

/* tbh compilers yelling errors */
/* feels like it's crying */
/* and yup this throws an error */
static void
cry (const tokeniser_t *tokeniser, const char *format, ...)
{
   line_col_t lc = get_line_col (tokeniser);

   fprintf (stderr, "[ERROR %u:%u] ", lc.line, lc.col);

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
   /* we're at an isalpha character right now */
   /* eat up isalnum characters until we hit a non-isalnum char */
   /* and then push it as a token to tokeniser->tokens */

   /* starting with 32 capacity */
   string_t eaten_alnum = new_string (32);
   char curr_char       = '\1';
   line_col_t lc        = get_line_col (tokeniser);

   while ((isalnum ((curr_char = tokeniser->program[tokeniser->prog_idx++]))
           || curr_char == '_'))
      {
         string_push (&eaten_alnum, curr_char);
      }

   tokeniser->prog_idx--;

   size_t len = eaten_alnum.size; /* number of chars pushed */
   char *name = malloc (len + 1);
   memcpy (name, eaten_alnum.elems, len);
   name[len] = '\0';

   token_t token = {
      .type        = Token_Ident,
      .value.chars = name,
      .lc          = lc,
   };

   string_free (&eaten_alnum);

   vector_push_elem (&tokeniser->tokens, &token);
}

void
tokenise_num (tokeniser_t *tokeniser)
{
   /* we're at an isdigit character right now */
   /* eat up isalnum characters until we hit a non-isdigit char */
   /* and then push it as a token to tokeniser->tokens */

   /* no malloc; it's a number */
   unsigned int eaten_num = 0;
   char curr_char         = '\1';
   line_col_t lc          = get_line_col (tokeniser);

   while (isdigit ((curr_char = tokeniser->program[tokeniser->prog_idx++])))
      {
         eaten_num *= 10;
         eaten_num += (curr_char - '0');
      }

   tokeniser->prog_idx--;

   token_t token = {
      .type      = Token_Number,
      .value.num = eaten_num,
      .lc        = lc,
   };

   vector_push_elem (&tokeniser->tokens, &token);
}

char *
tok_to_str (token_type_t tok_type)
{
   switch (tok_type)
      {
#define X(t)                                                                  \
   case t:                                                                    \
      return #t;
         TOKEN_TYPE_LIST
#undef X
      }
}

string_t
tokens_to_bf_str (vector_t tokens)
{
   token_t curr_tok = { 0 };
   string_t bf      = new_string (64);
   for (unsigned int i = 0; i < tokens.size; i++)
      {
         curr_tok  = *(token_t *)vector_at (&tokens, i);
         char bf_c = ' ';
         switch (curr_tok.type)
            {
               /* Classic BF */
            case Token_Plus:
               {
                  bf_c = '+';
               }
               break;
            case Token_Minus:
               {
                  bf_c = '-';
               }
               break;

            case Token_Left:
               {
                  bf_c = '<';
               }
               break;
            case Token_Right:
               {
                  bf_c = '>';
               }
               break;

            case Token_LLoop:
               {
                  bf_c = '[';
               }
               break;
            case Token_RLoop:
               {
                  bf_c = ']';
               }
               break;

            case Token_Dot:
               {
                  bf_c = '.';
               }
               break;

            case Token_Comma:
               {
                  bf_c = ',';
               }
               break;

            default:
               /* anything here will be an invalid token */
               printf ("[WARNING %u:%u] unexpected token %s, skipping...\n",
                       curr_tok.lc.line, curr_tok.lc.col,
                       tok_to_str (curr_tok.type));
               continue;
            }
         string_push (&bf, bf_c);
      }

   return bf;
}

void
print_tokens (vector_t tokens)
{
   puts ("Printing Tokens");
   puts ("===============");
   for (unsigned int i = 0; i < tokens.size; i++)
      {
         token_t *curr_tok = (token_t *)vector_at (&tokens, i);
         token_type_t tt_t = curr_tok->type;
         const char *tt    = tok_to_str (tt_t);

         printf ("i: %d; ", i);
         printf ("type: %s; ", tt);

         if (tt_t == Token_Number)
            {
               printf ("value.num: %d;", curr_tok->value.num);
            }
         else if (tt_t == Token_Ident)
            {
               printf ("value.chars: %s;", curr_tok->value.chars);
            }

         printf ("\n");
      }
}

void
mbf_tokenise (tokeniser_t *tokeniser)
{
   tokeniser->tokens = new_vector (32, sizeof (token_t));
   char current_char = '\1';

   while (current_char != '\0')
      {
         current_char = tokeniser->program[tokeniser->prog_idx];
         switch (current_char)
            {
            /* comments */
            case '#':
               while (current_char != '\n' && current_char != '\0')
                  {
                     current_char = tokeniser->program[++tokeniser->prog_idx];
                  }
               break;

            /* skip whitespaces, tabs and newlines */
            case '\t':
            case '\n':
            case '\r':
            case ' ':
               break;

            case '{':
               {
                  token_t tok = {
                     .type = Token_LCurly,
                     .lc   = get_line_col (tokeniser),
                  };
                  vector_push_elem (&tokeniser->tokens, &tok);
               }
               break;

            case '}':
               {
                  token_t tok = {
                     .type = Token_RCurly,
                     .lc   = get_line_col (tokeniser),
                  };
                  vector_push_elem (&tokeniser->tokens, &tok);
               }
               break;

            case '+':
               {
                  token_t tok = {
                     .type = Token_Plus,
                     .lc   = get_line_col (tokeniser),
                  };
                  vector_push_elem (&tokeniser->tokens, &tok);
               }
               break;

            case '-':
               {
                  token_t tok = {
                     .type = Token_Minus,
                     .lc   = get_line_col (tokeniser),
                  };

                  vector_push_elem (&tokeniser->tokens, &tok);
               }
               break;

            case '<':
               {
                  token_t tok = {
                     .type = Token_Left,
                     .lc   = get_line_col (tokeniser),
                  };

                  vector_push_elem (&tokeniser->tokens, &tok);
               }
               break;

            case '>':
               {
                  token_t tok = {
                     .type = Token_Right,
                     .lc   = get_line_col (tokeniser),
                  };

                  vector_push_elem (&tokeniser->tokens, &tok);
               }
               break;

            case '[':
               {
                  token_t tok = {
                     .type = Token_LLoop,
                     .lc   = get_line_col (tokeniser),
                  };

                  vector_push_elem (&tokeniser->tokens, &tok);
               }
               break;

            case ']':
               {
                  token_t tok = {
                     .type = Token_RLoop,
                     .lc   = get_line_col (tokeniser),
                  };

                  vector_push_elem (&tokeniser->tokens, &tok);
               }
               break;

            case '.':
               {
                  token_t tok = {
                     .type = Token_Dot,
                     .lc   = get_line_col (tokeniser),
                  };

                  vector_push_elem (&tokeniser->tokens, &tok);
               }
               break;

            case ',':
               {
                  token_t tok = {
                     .type = Token_Comma,
                     .lc   = get_line_col (tokeniser),
                  };

                  vector_push_elem (&tokeniser->tokens, &tok);
               }
               break;

            case ';':
               {
                  token_t tok = {
                     .type = Token_Semicolon,
                     .lc   = get_line_col (tokeniser),
                  };

                  vector_push_elem (&tokeniser->tokens, &tok);
               }
               break;

            case '\0':
               {
                  token_t tok = {
                     .type = Token_EOF,
                     .lc   = get_line_col (tokeniser),
                  };

                  vector_push_elem (&tokeniser->tokens, &tok);
               }
               break;

            default:
               if (isalpha (current_char))
                  {
                     tokenise_ident (tokeniser);
                     continue;
                  }
               else if (isdigit (current_char))
                  {
                     tokenise_num (tokeniser);
                     continue;
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

void
tokeniser_free (tokeniser_t *tokeniser)
{

   for (unsigned int i = 0; i < tokeniser->tokens.size; i++)
      {
         token_t *tok = (token_t *)vector_at (&tokeniser->tokens, i);
         if (tok->type == Token_Ident && tok->value.chars)
            {
               free ((void *)tok->value.chars);
               tok->value.chars = NULL;
            }
      }
   vector_free (&tokeniser->tokens);
   tokeniser->prog_idx = 0;
}
