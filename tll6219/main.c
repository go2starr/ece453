/****************************************************************************
 *
 * Copyright (c) 2012, The University of Wisconsin - Madison
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the name of The University of Wisconsin - Madison nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE UNIVERISTY OF
 * WISCONSIN - MADISON BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Author: Christopher Karle <ckarle@wisc.edu>
 *
 ****************************************************************************/
#include <stdio.h>
#include "../../lwip/include/lwip/api.h"
#include <eth.h>
#include <stdint.h>
#include "sha256sum.h"

//#define LSFR (*(volatile unsigned long*) 0xD3000020)
#define BUFFSIZE 8192
uint32_t randomBuffer[BUFFSIZE/4];

/****************************************************************************
 *
 ****************************************************************************/
void _main(void* parameter)
{
  struct netconn *conn;
  struct netbuf  *sendBuf;
  struct netbuf  *recBuf;
  struct ip_addr addr;
  uint32_t checkSum[8];
  uint32_t *checkSumPkt;
  uint32_t *initPkt;
  uint32_t seed[2];
  err_t err;
  
  
  //Wait for ethernet before proceeding
  while(!ethIsUp()){}
  
  //Create UDP Connection
  conn = netconn_new(NETCONN_UDP);
  //printf("Set Up Using UDP\n");
  //Use IP
  IP4_ADDR(&addr, 128, 104, 181,10);
  
  //Bind Connection
  err = netconn_bind(conn,NULL,2000);
  if(err != ERR_OK)
  {
	//printf("Error Binding\n");
  }
  //printf("Connection Bound\n");
  
  //Allocate Buffer
  sendBuf = netbuf_new();
  initPkt = netbuf_alloc(sendBuf, 8);
  
  //Fill Buffer With Data
  initPkt[0] = htonl(1);
  initPkt[1] = htonl(0);
  
  //Send Initial to Server
  err = netconn_sendto(conn,sendBuf,&addr,2000);
  if(err != ERR_OK)
  {
	//printf("Error Sending Packet\n");
  }
  //printf("Initialization Packet Sent\n");
  
  //Recieve Seed
  recBuf = netconn_recv(conn);
  //printf("Recieved Seed Value\n");
  
  //Copy Data From recBuf
  netbuf_copy(recBuf,seed,8);
  //printf("Command Value: %d\n",ntohl(seed[0]));
  //printf("0x%08x\n", ntohl(seed[1]));
  
  uint32_t i = 0;
  uint32_t temp;
  uint32_t count = 0;
  printf("Placing Seed\n");
  //LSFR = ntohl(seed[1]);
  temp = ntohl(seed[1]);
  while(1)
 {
		
		 //printf("Filling Buffer\n");
		//Fill Buffer With Random Values
		for(i = 0; i < BUFFSIZE/4; i++)
		{
			//temp = LSFR;
			temp = (temp >> 1) ^ (-(temp & 1) & 0x80200003);
			//LSFR = temp;
			randomBuffer[i] = htonl(temp);
		}
		//printf("Running CheckSum\n");
		// Run Checksum
		sha256sum(checkSum,randomBuffer,BUFFSIZE);
		//printf("Computed CheckSum\n");
		
		//printf("Filling sendBuf\n");
		//Alloc Space In sendBuf
		checkSumPkt = netbuf_alloc(sendBuf, 40);
		
		//printf("Building CheckSum Packet\n");
		//Build CheckSum Packet
		checkSumPkt[0] = htonl(3);
		checkSumPkt[1] = htonl(count);
		checkSumPkt[2] = htonl(checkSum[0]);
		checkSumPkt[3] = htonl(checkSum[1]);
		checkSumPkt[4] = htonl(checkSum[2]);
		checkSumPkt[5] = htonl(checkSum[3]);
		checkSumPkt[6] = htonl(checkSum[4]);
		checkSumPkt[7] = htonl(checkSum[5]);
		checkSumPkt[8] = htonl(checkSum[6]);
		checkSumPkt[9] = htonl(checkSum[7]);
		
		//printf("Sending CheckSum Packet\n");
		err = netconn_sendto(conn,sendBuf,&addr,2000);
		if(err != ERR_OK)
		{
			printf("Error Sending CheckSum Packet\n");
		}
		count++;
		//printf("Count: %d \n",count);
		}
}
