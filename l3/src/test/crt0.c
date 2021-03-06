/****************************************************************************
 *
 ****************************************************************************/
#include <stdint.h>
#include "monlib.h"

/****************************************************************************
 *
 ****************************************************************************/
#define MONCOMPTR ((uint32_t*) 0xC8000038)

#ifndef NULL
#define NULL 0
#endif

/****************************************************************************
 *
 ****************************************************************************/
extern int main(int argc, char* argv[]);

/****************************************************************************
 *
 ****************************************************************************/
int _start()
{
   int argc;
   char** argv;

   monConnect((void*) *MONCOMPTR, NULL, NULL);
   mon_getargv(&argc, &argv);

   return main(argc, argv);
}
