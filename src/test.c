#include <assert.h>
#include <stdio.h>
#include <string.h>

#define DSTR_IMPL
#include "mbf.h"

unsigned int tests_run    = 0;
unsigned int tests_failed = 0;

/* preprocess_eq */
#define peq(mbf, test_str)                                                    \
   do                                                                         \
      {                                                                       \
         dstr_t exp = mbf_preprocess (mbf);                                   \
         tests_run++;                                                         \
         if (strcmp (exp.str, test_str) != 0)                                 \
            {                                                                 \
               fprintf (stderr, "[FAILED:%u] '%s' -/-> '%s',", __LINE__, mbf, \
                        test_str);                                                 \
               fprintf (stderr, "  actual: '%s'\n", exp.str);                 \
               tests_failed++;                                                \
            }                                                                 \
      }                                                                       \
   while (0);

#define expect_streq(str1, str2)                                              \
   do                                                                         \
      {                                                                       \
         tests_run++;                                                         \
         if (strcmp (str1, str2) != 0)                                        \
            {                                                                 \
               fprintf (stderr, "[FAILED:%u] '%s' != '%s'\n", __LINE__, str1, \
                        str2);                                                \
               tests_failed++;                                                \
            }                                                                 \
      }                                                                       \
   while (0)

#define expect_eq(val1, val2)                                                 \
   do                                                                         \
      {                                                                       \
         tests_run++;                                                         \
         if ((val1) != (val2))                                                \
            {                                                                 \
               fprintf (stderr, "[FAILED:%u] %s != %s\n", __LINE__, #val1,    \
                        #val2, );                                             \
               tests_failed++;                                                \
            }                                                                 \
         else                                                                 \
            {                                                                 \
               printf ("[PASS:%u] %s == %s\n", __LINE__, #val1, #val2);       \
            }                                                                 \
      }                                                                       \
   while (0)

void
tests (void)
{
   /* basic count tests */
   peq ("+", "+");
   peq ("1+", "+");
   peq ("0+", "");
   peq ("-+", "-+");
   peq ("-1-", "--");

   /* zero */
   peq ("00+", "");
   peq ("00000000000000+", "");
   peq ("", "");

   /* macro expansion */
   peq ("a{2+}a;", "++");
   peq ("z{[-]} a{z;2+} a;", "[-]++");
}

int
main (void)
{
   puts ("~=~=~=~=~=~=~=~=~=~=~=~=~=~=~");
   puts ("|| RUNNNG TESTS FOR MBF... ||");
   puts ("~=~=~=~=~=~=~=~=~=~=~=~=~=~=~");
   tests ();

   puts ("");

   puts ("~=~=~=~=~=~=~");
   puts ("|| RESULTS ||");
   puts ("~=~=~=~=~=~=~");
   printf ("TOTAL TESTS: %u, PASSED: %u, FAILED: %u\n", tests_run,
           tests_run - tests_failed, tests_failed);
   return (tests_failed == 0) ? 0 : 1;
}
