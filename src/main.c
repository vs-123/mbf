#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bf.h"
#include "mbf.h"

#define bool int
#define true 1
#define false 0

typedef struct
{
   const char *program_file;
   bool can_expand;
   bool should_save_expansion_to_file;
   const char *output_file;
} mbf_opts_t;

void
print_help (char *mbf_name)
{
   printf ("~=~=~=~=~=~=~=~=~\n"
           "|| MBF -- HELP ||\n"
           "~=~=~=~=~=~=~=~=~\n"
           "\n"
           "~=~=~=~=~=\n"
           "|| DESC ||\n"
           "~=~=~=~=~=\n"
           "   A compiler/interpreter for MBF (Macro BF).\n"
           "\n"
           "~=~=~=~=~=~\n"
           "|| USAGE ||\n"
           "~=~=~=~=~=~\n"
           "   %% %s [OPTION] <input_file>\n"
           "\n"
           "~=~=~=~=~=~=~\n"
           "|| OPTIONS ||\n"
           "~=~=~=~=~=~=~\n"
           "   o   -h/--help                  --- print help\n"
           "   o   -E/--expand <output_file>  --- expand & write to file; do "
           "not interpret.\n"
           "   o   -ne/--no-expand            --- do not expand; interpret "
           "as-is\n"
           "   o   -bf                        --- interpret as BF\n"
           "\n"
           "~=~=~=~=~=~=~\n"
           "|| EXAMPLE ||\n"
           "~=~=~=~=~=~=~\n"
           "   %% %s ./hello_world.mbf\n"
           "   %% %s -E output.bf ./hello_world.mbf\n"
           "   %% %s -bf ./output.bf\n"
           "\n"
           "~=~=~=~=~=~=~\n"
           "|| LICENSE ||\n"
           "~=~=~=~=~=~=~\n"
           "   This program is licensed under AGPLv3-or-later. No warranty.\n"
           "   See `LICENSE` file for full terms.\n",
           mbf_name, mbf_name, mbf_name, mbf_name);
}

void
parse_args (unsigned int argc, char **argv, mbf_opts_t *mbf_opts)
{
   mbf_opts->should_save_expansion_to_file = false;
   mbf_opts->can_expand                    = true;
   bool got_program                        = false;

   if (argc < 2)
      {
         print_help (argv[0]);
         exit (1);
      }
   for (unsigned int i = 1; i < argc; i++)
      {
         const char *arg = argv[i];

         /* CLI OPTIONS */
         if (strcmp (arg, "-h") == 0 || strcmp (arg, "--help") == 0)
            {
               print_help (argv[0]);
               exit (0);
            }
         else if (strcmp (arg, "-E") == 0 || strcmp (arg, "--expand") == 0)
            {
               if (i + 1 >= argc)
                  {
                     fprintf (stderr,
                              "[ERROR] expected output file after %s.\n", arg);
                     exit (0);
                  }
               i++;
               mbf_opts->can_expand                    = true;
               mbf_opts->should_save_expansion_to_file = true;
               mbf_opts->output_file                   = argv[i];
            }
         else if (strcmp (arg, "-ne") == 0 || strcmp (arg, "-bf") == 0
                  || strcmp (arg, "--no-expand") == 0)
            {
               mbf_opts->can_expand = false;
            }
         else if (strncmp (arg, "-", 1) == 0)
            {
               fprintf (stderr,
                        "[ERROR] unknown option '%s'. Use --help flag.\n",
                        arg);
               exit (1);
            }
         else
            {
               mbf_opts->program_file = arg;
               got_program            = true;
            }
      }
   if (!got_program)
      {
         fprintf (stderr, "[ERROR] mbf program file was not provided.\n");
         exit (1);
      }
}

int
main (int argc, char **argv)
{
   mbf_opts_t mbf_opts = { 0 };
   parse_args (argc, argv, &mbf_opts);

   FILE *prog_file = fopen (mbf_opts.program_file, "r");

   if (!prog_file)
      {
         fclose (prog_file);
         fprintf (stderr, "[ERROR] not able to open '%s'.\n",
                  mbf_opts.program_file);
         return 1;
      }

   string_t program = new_string (128);

   char ch;
   while ((ch = fgetc (prog_file)) != EOF)
      {
         string_push (&program, ch);
      }

   if (mbf_opts.can_expand)
      {
         string_t mbf_expanded = mbf_preprocess (program.elems);

         if (mbf_opts.should_save_expansion_to_file)
            {
               FILE *output_file = fopen (mbf_opts.output_file, "w");
               if (!output_file)
                  {
                     fprintf (stderr,
                              "[ERROR] could not open output file '%s' for "
                              "writing.\n",
                              mbf_opts.output_file);
                     return 1;
                  }
               fprintf (output_file, "%s", mbf_expanded.elems);
               fclose (output_file);
               string_free (&mbf_expanded);
               printf ("[SUCCESS] file was successfully written!\n");
            }
         else
            {
               mbf_exec_bf (mbf_expanded.elems);
               string_free (&mbf_expanded);
            }
      }
   else
      {
         mbf_exec_bf (program.elems);
      }

   fclose (prog_file);
   string_free (&program);
}
