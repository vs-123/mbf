#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DSTR_IMPL
#include "dstr.h"

#include "bf.h"
#include "mbf.h"

#define bool int
#define true 1
#define false 0

typedef struct
{
   bool skip_expansion;
   const char *src_path;
   const char *out_path;
} opts_t;

void print_help(char *mbf_name);
void parse_args(unsigned int argc, char **argv, opts_t *mbf_opts);
bool read_file_to_dstr(const char *path, dstr_t *out);
int run_mbf(opts_t *opts);

int main(int argc, char **argv)
{
   opts_t opts = {0};
   parse_args(argc, argv, &opts);

   int exit_code = run_mbf(&opts);

   return exit_code;
}

void print_help(char *mbf_name)
{
   printf(
      "~=~=~=~=~=~=~=~=~\n"
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
      mbf_name,
      mbf_name,
      mbf_name,
      mbf_name
   );
}

void parse_args(unsigned int argc, char **argv, opts_t *mbf_opts)
{
   mbf_opts->skip_expansion                = false;
   mbf_opts->out_path                   = NULL;
   bool got_program                        = false;

   if (argc < 2) {
      print_help(argv[0]);
      exit(1);
   }
   for (unsigned int i = 1; i < argc; i++) {
      const char *arg = argv[i];

      /* CLI OPTIONS */
      if (strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0) {
         print_help(argv[0]);
         exit(0);
      } else if (strcmp(arg, "-E") == 0 || strcmp(arg, "--expand") == 0) {
         if (i + 1 >= argc) {
            fprintf(stderr, "[ERROR] expected output file after %s.\n", arg);
            exit(0);
         }
         i++;
         mbf_opts->skip_expansion = false;
         mbf_opts->out_path    = argv[i];
      } else if (strcmp(arg, "-ne") == 0 || strcmp(arg, "-bf") == 0 ||
                 strcmp(arg, "--no-expand") == 0) {
         mbf_opts->skip_expansion = true;
      } else if (strncmp(arg, "-", 1) == 0) {
         fprintf(
            stderr,
            "[ERROR] unknown option '%s'. Use --help flag.\n",
            arg
         );
         exit(1);
      } else {
         mbf_opts->src_path = arg;
         got_program            = true;
      }
   }
   if (!got_program) {
      fprintf(stderr, "[ERROR] mbf program file was not provided.\n");
      exit(1);
   }
}

bool read_file_to_dstr(const char *path, dstr_t *out)
{
   FILE *f = fopen(path, "rb");
   if (!f) {
      return false;
   }

   fseek(f, 0, SEEK_END);
   fseek(f, 0, SEEK_SET);

   char ch;
   while ((ch = fgetc(f)) != EOF) {
      dstr_putc(out, ch);
   }

   fclose(f);
   return true;
}

int run_mbf(opts_t *opts)
{
   dstr_t program = dstr_new();
   if (!read_file_to_dstr(opts->src_path, &program)) {
      fprintf(stderr, "[ERROR] Could not open '%s'\n", opts->src_path);
      return 1;
   }

   if (!opts->skip_expansion) {
      dstr_t expanded = mbf_preprocess(program.str);

      if (opts->out_path != NULL) {
         FILE *out = fopen(opts->out_path, "w");
         if (!out) {
            fprintf(
               stderr,
               "[ERROR] Could not write to '%s'\n",
               opts->out_path
            );
            dstr_free(&expanded);
            dstr_free(&program);
            return 1;
         }
         fprintf(out, "%s", expanded.str);
         fclose(out);
         printf("[SUCCESS] Written to %s\n", opts->out_path);
      } else {
         mbf_exec_bf(expanded.str);
      }
      dstr_free(&expanded);
   } else {
      mbf_exec_bf(program.str);
   }

   dstr_free(&program);
   return 0;
}
