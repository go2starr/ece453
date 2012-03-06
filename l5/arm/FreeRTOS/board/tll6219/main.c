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

#include "iodefine.h"

#include "sha256/sha256sum.h"

#define PORT 2000

/****************************************************************************
 *  Block Sizes
 ****************************************************************************/
#define BS_KEY 1
#define BLOCKSIZE (8192 * (BS_KEY + 1) / 4)
#define LED (*(volatile unsigned long*) 0xD3000004)
/****************************************************************************
 *  PRNG
 ****************************************************************************/
static inline void next_rand(uint32_t *rand) {
  *rand = (*rand >> 1) ^ (-(*rand & 1) & 0x80200003);
}

/****************************************************************************
 *  Structure Definitions
 ****************************************************************************/
#define CMD_REQ_SEED 1
struct req_seed_pkt {
  uint32_t cmd;
  uint32_t bs;
};

struct seed_pkt {
  uint32_t cmd;
  uint32_t seed;
};

struct chksum_pkt {
  uint32_t cmd;
  uint32_t count_id;
  uint32_t sha256[8];
};


/****************************************************************************
 *  Main
 ****************************************************************************/
void _main(void *parameter)
{
  /* Network */
  struct netconn *conn;
  struct netbuf *netbuf;
  struct ip_addr addr;

  /* Packet Storage */
  struct req_seed_pkt req;
  struct seed_pkt seed;
  struct chksum_pkt chksum;

  /* Checksum calculation */
  uint32_t rand;
  uint32_t sha256[8];
  uint32_t block[BLOCKSIZE];

  /* Counters */
  int i, j, k;
  uint32_t count_id;

  /* Configure GPIO */
  PTF_GIUS |= (1 << 16);       /* Enable PF16 (tied to intterupt) */
  PTF_DDIR &= ~(1 << 16);       /* GPIO as input */
  PTF_ICR1 |= 0;
  PTF_ICR2 |= 0;
  PTF_PUEN |= (1 << 16);
  PTF_IMR  |= (1 << 16);       /* Unmask interrupt on pin 0 */
  PMASK |= 1 << 5;             /* Unmask port register */
  
  /* Wait for ethernet up */
  while (!ethIsUp());

  /* Create connection */
  conn = netconn_new(NETCONN_UDP);

  /* Bind */
  netconn_bind(conn, NULL, PORT);

  /* Address */
  IP4_ADDR(&addr, 128, 104, 180, 237);

  /* Request seed */
  req.cmd = htonl(CMD_REQ_SEED);
  req.bs  = htonl(BS_KEY);

  /* Map packet to netbuf */
  netbuf = netbuf_new();
  netbuf_ref(netbuf, (void*)&req, sizeof(req));

  /* Send data */
  netconn_sendto(conn, netbuf, &addr, PORT);

  /* Listen */
  netbuf = netconn_recv(conn); 
  
  /* Read */
  netbuf_copy(netbuf, (void*) &seed, sizeof(seed)); 

  ////////////////////////////////////////
  printf("Seed: 0x%8x\n", seed.seed);
  ////////////////////////////////////////

  /* Source the seed */
  rand = ntohl(seed.seed);

  for (count_id = 0; ; count_id++) {
    /* Fill buffer */
    for (i = 0; i < BLOCKSIZE; i++) {
      next_rand(&rand);
      block[i] = htonl(rand); 
    }
   
   if ((count_id % 100) == 0) {
      LED = 1;
   }
    /* Calculate sum */
    sha256sum(sha256, (void*)block, BLOCKSIZE * 4);
    
    /* Fill checksum packet */
    chksum.cmd = htonl(3);
    chksum.count_id = htonl(count_id);
    for (i = 0; i < 8; i++)
      chksum.sha256[i] = htonl(sha256[i]);
    
    /* Send checksum back to server */
    netbuf_ref(netbuf, (void*)&chksum, sizeof(chksum));
    netconn_sendto(conn, netbuf, &addr, PORT);
  }
}
#endif /* LWIP_NETCON */
