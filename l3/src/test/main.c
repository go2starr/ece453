/****************************************************************************
 *
 ****************************************************************************/
#include <stdlib.h>
#include "monlib.h"

/* Scratch registers */
#define SCRATCH_WR_COUNT   (*(volatile unsigned long*) 0xD3000000)
#define SCRATCH            ((volatile unsigned long*) 0xD3000004)
#define SCRATCH_COUNT 4
#define SCRATCH_WIDTH 32

/* Test bench */
#define COUNT 32000
#define WR_PRINT_COUNT 1000

/****************************************************************************
 *
 ****************************************************************************/
int main(int argc, char* argv[])
{
  int i, j, no_errs, no_xfers;
  
  mon_printf("Running tests...\n");

  /* Verify each register has read/write functionality */
  for (j = 0; j < SCRATCH_COUNT; j++)
    {
      /* COUNT times... */
      for (i = 0; i < COUNT; i++)
        {
          unsigned long value1 = rand();
          unsigned long value2;

          SCRATCH[j] = value1;
          value2 = SCRATCH[j];
          
          /* Compare */
          if (value1 != value2)
            {
              mon_printf("ERROR!  0x%x != 0x%x\n", value1, value2);
              return 0;
            }

          /* Verify count register */
          if ((i % WR_PRINT_COUNT) == 0)
            {
              int reads = SCRATCH_WR_COUNT & 0xFFFF;
              int writes = SCRATCH_WR_COUNT >> 16;
              mon_printf("Reads: %d\tWrites: %d\n", reads, writes);
            }
        }
    }

  /* Verify that SCRATCH_WR_COUNT is read-only */
  i = SCRATCH_WR_COUNT;
  SCRATCH_WR_COUNT = i + 1;
  if (SCRATCH_WR_COUNT != i) {
    mon_printf("ERROR! Write/Read count is not read-only.\n");
  }
  mon_printf("Lab2 testbench passed...\n");

  /* "For each Scratch register, write a single 1 to each bit location..." */
  mon_printf("Testing each bit individually... ");
  no_errs = no_xfers = 0;       /* reset */
  for (i = 0; i < SCRATCH_COUNT; i++) {
    for (j = 0; j < SCRATCH_WIDTH; j++) {
      SCRATCH[i] = 0;           /* Clear */
      SCRATCH[i] = 1 << j;      /* Set */
      SCRATCH[(i+1)%SCRATCH_COUNT] = rand(); /* write to a different reg */
      no_xfers++;
      if (!(SCRATCH[i] & 1 << j)) {
        mon_printf("Bit %d not set in register %d.\n", j, i);
        no_errs++;
      }
    }
  }
  if (no_errs == 0)
    mon_printf("passed.\n");
  mon_printf("Total errors: 0x%x\n", no_errs);
  mon_printf("Total transfers: 0x%x\n", no_xfers);
  
  /* 512k words of random data... */
  mon_printf("Testing each register 512k times... ");
  no_errs = no_xfers = 0;       /* reset */
  for (i = 0; i < SCRATCH_COUNT; i++) {
    for (j = 0; j < 512000; j++) {
      int val;
      SCRATCH[i] = 0;           /* Clear */
      SCRATCH[i] = val = rand();      /* Set */
      SCRATCH[(i-1)%SCRATCH_COUNT] = rand(); /* write to a different reg */
      no_xfers++;
      if (!(SCRATCH[i] == val)) {
        mon_printf("Value mismatch in register %d. 0x%x != 0x%x\n", i, val, SCRATCH[i]);
        no_errs++;
      }
      if (!(j % 10000))
        mon_printf(".");
    }
  }      
  if (no_errs == 0)
    mon_printf("passed.\n");
  mon_printf("\nTotal errors: 0x%x\n", no_errs);
  mon_printf("Total transfers: 0x%x\n", no_xfers);

  return 0;
}
