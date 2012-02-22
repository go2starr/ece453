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
#include "FreeRTOS.h"
#include "lwip/opt.h"

#if LWIP_NETCONN
#include "lwip/api.h"
#include "lwip/sys.h"

#define MY_PORT 13
#define SENDTO_PORT 2000
#define NETBUF_SIZE 4096
#define REG_ADDR (*(volatile unsigned long*) 0xD3000000)
/****************************************************************************
 *
 ****************************************************************************/
void _main(void* parameter)
{
   printf("Connecting to server...\n");
   static struct netconn *conn;
   static struct netbuf *buf;
   err_t buf_ref_err;
   err_t sendto_err;
   struct ip_addr addr;
   unsigned int count = 0; 

   conn = netconn_new(NETCONN_UDP);
     
   // listen for a response   
   netconn_bind(conn, IP_ADDR_ANY, MY_PORT);	// Port I'm listening on
   void * data_ptr = netbuf_alloc (buf, NETBUF_SIZE);
   if (data_ptr == NULL) {
        printf("Unable to allocate netbuf buffer\n");
	return;
   }

   buf_ref_err = netbuf_ref(buf, data_ptr, NETBUF_SIZE);
   if (buf_ref_err != ERR_OK) {
   	printf("Error calling netbuf_ref\n");
	return;
   } 

   IP4_ADDR(&addr, 128, 104, 180, 1);

   // TODO create seed request packet
   // All fields 32-bits andi n network byte order
   // Offset0 Command value = 1
   // Offset4 Block size = 0-1023
   	// Block size parameter 0; block size = 8KB
   	// Block size parameter 1; block size = 16KB
   	// Block size parameter 1023; block size = 8MB 
   
   // send packet to server
   sendto_err = netconn_sendto (conn, buf, &addr, SENDTO_PORT);
   if (sendto_err != ERR_OK) {
	printf("Error calling netconn_sendto\n");
	return;
   }
   
   // receive from server
   buf = netconn_recv (conn);
   if (buf == NULL) {
   	printf("Error receiving data\n");
 	return;
   } 

   // TODO packet received from server -- convert to little endian
   // Offset0 Command value=1
   // Offset4 Seed value
   
   // TODO checksum packets sent to server; each time one is sent count++
   // Offset0 Command value=3
   // Offset4 Count ID (starts at 0 for first checksum pkt sent and increments for each subsequent packet
   // Offset8 - Offset36 SHA256

// u16_t netbuf_copy ( struct netbuf * aNetBuf, void * aData, u16_t aLen );  

   netbuf_delete (buf);	// doesn't free memory allocated with netbuf_ref

// NOW, we only send to server, don't ever receive from server again

}
  

#endif /* LWIP_NETCONN */


