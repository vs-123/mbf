#include <stdio.h>
#include <stdlib.h>

#include "mbf.h"
#include "tokeniser_t.h"
#include "vector.h"

int
main (void)
{
   const char *program = "###################\n"
                         "# hello_world.mbf #\n"
                         "###################\n"
                         "\n"
                         "# hello macro\n"
                         "hello {\n"
                         "> 8+ [< 9+ > -] < .\n"
                         "> 4+ [< 7+ > -] < +.\n"
                         "7+ . .\n"
                         "3+ .\n"
                         ">> 6+ [< 7+ >-] < ++.\n"
                         "12- .\n"
                         "> 6+ [< 9+ >-] < + .\n"
                         "<.\n"
                         "3+ .\n"
                         "6- .\n"
                         "8- .\n"
                         ">>> 4+ [< 8+ >-] <+ .\n"
                         "}\n"
                         "\n"
                         "# call hello\n"
                         "hello ;\n";

   //   vector_t v = new_vector (32, sizeof (token_t));
   //   token_t *t1 = malloc(sizeof(token_t));
   //   vector_push_elem(&v, (void*));
   //
   //   return 0;

   const char *mbf_expanded = mbf_preprocess (program);
   printf ("%s\n", mbf_expanded);

   //   free ((void *)mbf_expanded);
   // mbf_exec_bf (mbf_expanded);
}
