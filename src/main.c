#include <stdio.h>

#include "mbf.h"

int
main (void)
{
   const char *program = "###################\n"
                         "# hello_world.mbf #\n"
                         "###################\n"
                         "\n"
                         "; hello macro\n"
                         "hello {\n"
                         "> 8+ [< 9+ > -] < .\n"
                         "> 4+ [< 7+ > -] > +.\n"
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
                         "; call hello\n"
                         "hello\n";

   const char *mbf_expanded = mbf_expand_macros (program);
   printf ("%s\n", mbf_expanded);
   // mbf_exec_bf (mbf_expanded);
}
