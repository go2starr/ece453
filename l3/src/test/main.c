/****************************************************************************
 *
 ****************************************************************************/
#include <stdint.h>

static inline void next(uint32_t *rand) {
  *rand = (*rand >> 1) ^ (-(*rand & 1) & 0x80200003);
}


#include "monlib.h"
int main(int argc, char* argv[])
{
  int i, j = 0;
  uint32_t rand = 123;
  mon_printf("Hello, world\n");

  while (1) {
    /* 15000 = min dat production rate */
    for (j = 0; j < 1; j++) {
      /* 8 MB chunk */
      for (i = 0; i < 2000000; i++) {
        next(&rand);
      }
    }
    mon_printf("%x\n", rand);
  }
  return 0;
}
