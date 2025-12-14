#include <stdio.h>

#include "mbf.h"

int
main (void)
{
   const char *program = "###################"
                         "# hello_world.mbf #"
                         "###################"
                         ""
                         "; hello macro"
                         "hello {"
                         "> 8+ [< 9+ > -] < ."
                         "> 4+ [< 7+ > -] > +."
                         "7+ . ."
                         "3+ ."
                         ">> 6+ [< 7+ >-] < ++."
                         "12- ."
                         "> 6+ [< 9+ >-] < + ."
                         "<."
                         "3+ ."
                         "6- ."
                         "8- ."
                         ">>> 4+ [< 8+ >-] <+ ."
                         "}"
                         ""
                         "; call hello"
                         "hello";

   const char *mbf_expanded = mbf_expand_macros (program);
   // mbf_exec_bf (mbf_expanded);
}
