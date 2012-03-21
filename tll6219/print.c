#include <stdio.h>
#include "../../lwip/include/lwip/api.h"
#include <eth.h>
#include <stdint.h>
#include "sha256sum.h"
#include "semphr.h"
#include "board.h"

extern xSemaphoreHandle printLock;
void _print(void* parameter)

{
	int i = 0;
	xSemaphoreHandle print = NULL;
	uint32_t num = uxTaskGetNumberOfTasks();
	signed portCHAR *pcWriteBuff;
	pcWriteBuff = malloc(num*40);
	while(1)
	{
		print = xSemaphoreTake(printLock, portMAX_DELAY);
		if(print == pdTRUE)
		{
			vTaskList(pcWriteBuff);

				printf("%s",pcWriteBuff);
				printf("\n");
			
		}
	}
}


