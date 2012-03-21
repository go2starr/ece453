/*
 *  print.c - Task printing routine
 */

#include <stdio.h>
#include "lwip/api.h"

extern xSemaphoreHandle pLock;

void _print(void* params)
{
  xSemaphoreHandle myLock;
  int numTasks;
  signed portCHAR *pcWriteBuff;

  while (1)
    {
      /* Wait for interrupt... */
      myLock = xSemaphoreTake(pLock, portMAX_DELAY);
      
      if (myLock == pdTRUE) 
        {
          numTasks = uxTaskGetNumberOfTasks();
          pcWriteBuff = malloc(numTasks * 40);
          vTaskList(pcWriteBuff);
          printf("%s\n", pcWriteBuff);
        }
    }
}
