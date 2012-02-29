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
#include "eth.h"

#define PORT 2001
#define NETBUF_SIZE 40
#define SEED_RQST_BUF_SIZE 8
#define REG_ADDR (*(volatile unsigned long*) 0xD3000000)
/****************************************************************************
 *
 ****************************************************************************/
void _main(void* parameter)
{
  struct netconn *conn;
  struct netbuf *buf;
  struct netbuf *seed_rqst_buf;
  struct ip_addr addr;

  int req_seed_msg[2];
  int chksum_pkt_msg[10];

  err_t err;
  unsigned int count = 0; 

  struct req_seed_pkt {
    uint32_t command_val;
    uint32_t block_size;
  };

  struct seed_pkt {
    uint32_t command_val;
    uint32_t seed;
  };

  struct chksum_pkt {
    uint32_t command_val;
    uint32_t count_id;
    uint32_t SHA256[8];
  };

  /* Wait for eth */
  while (!ethIsUp());

  /* Allocate a new connection */
  printf("Opening a port for connection\n");
  conn = netconn_new(NETCONN_UDP);

  /* Bind to port */
  printf("Binding to port %d\n", PORT);
  err = netconn_bind(conn, IP_ADDR_ANY, PORT);	
  if (err != ERR_OK) {
    printf("Failed to bind to port %d", PORT);
  }

  /* Create a new net buffer */
  printf("Creating a new netbuff\n");
  buf = netbuf_new();
  seed_rqst_buf = netbuf_new();
 

  /* Reference buffer for sending checksum pkt */
  printf("Creating reference to network buffer\n");
  err = netbuf_ref(buf, (void*)chksum_pkt_msg, NETBUF_SIZE);
  if (err != ERR_OK) {
    printf("Error calling netbuf_ref for buf\n");
    return;
  } 
  
  /* Reference buffer for requesting the seed */
  printf("Creating reference to req_seed_msg buffer\n");
  err = netbuf_ref(seed_rqst_buf, (void*)req_seed_msg, SEED_RQST_BUF_SIZE);
  if (err != ERR_OK) {
    printf("Error calling netbuf_ref for seed_rqst_buf\n");
    return;
  }

  // fill message
  struct req_seed_pkt rsp;
  rsp.command_val = 1;
  rsp.block_size = 1;
  req_seed_msg[0] = rsp.command_val;
  req_seed_msg[1] = rsp.block_size;

  // TODO create seed request packet
  // All fields 32-bits and in network byte order
  // Offset0 Command value = 1
  // Offset4 Block size = 0-1023
  //   Block size parameter 0; block size = 8KB
  //   Block size parameter 1; block size = 16KB
  //   Block size parameter 1023; block size = 8MB 
  

  // convert between little endian and big endian (htonl & ntohl)

  /* Fill target IP address */
  IP4_ADDR(&addr, 128, 104, 180, 237);

  // send packet to server
  printf("Sending data to server\n");
  err = netconn_sendto (conn, seed_rqst_buf, &addr, PORT);
  if (err != ERR_OK) {
    printf("Error %d: calling netconn_sendto\n", err);
    return;
  }
    
  // receive from server
  printf("Waiting for data from server\n");
  buf = netconn_recv (conn);
  if (buf == NULL) {
    printf("Error receiving data\n");
    return;
  } 
  printf("Data received");
  //  netbuf_copy(buf, data_ptr, 13);
  // printf("%s", (char*)data_ptr);
  printf("Data received");
  

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


