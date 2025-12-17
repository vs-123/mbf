#include <stdio.h>
#include <stdlib.h>

#include "bf.h"
#include "mbf.h"
#include "tokeniser_t.h"
#include "vector.h"

string_t
test (void)
{
   string_t s = new_string (32);
   return s;
}

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

  string_t mbf_expanded = mbf_preprocess (program);

  mbf_exec_bf (mbf_expanded.elems);
  string_free(&mbf_expanded);
}
