/**
 * @file
 * Ethernet Interface Skeleton
 *
 */

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

/*
 * This file is a skeleton for developing Ethernet network interface
 * drivers for lwIP. Add code to the low_level functions and do a
 * search-and-replace for the word "ethernetif" to replace it with
 * something that better describes your network interface.
 */

#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/sys.h"
#include "lwip/err.h"
#include "netif/etharp.h"

#include "ethernetif.h"
#include "main.h"


/* Define those to better describe your network interface. */
#define IFNAME0 's'
#define IFNAME1 't'


#define MDR_ETHERNET1_BUF_BASE 		(((uint32_t)MDR_ETHERNET1) + 0x08000000)
#define MDR_ETHERNET1_BUF_SIZE		((uint32_t)0x2000)

static struct netif *s_pxNetIf = NULL;
          
__align(4) uint8_t EthFrameRX[ETH_FRAME_SIZE]; // Intermediate buffer
__align(4) uint8_t EthFrameTX[ETH_FRAME_SIZE]; // Intermediate buffer


static void ethernetif_input( void * pvParameters );
static void arp_timer(void *arg);

void FlushFIFO(void)
{
	uint32_t i, *pBuf;
	
	pBuf = (uint32_t *) MDR_ETHERNET1_BUF_BASE;
	
	for (i=0; i<(MDR_ETHERNET1_BUF_SIZE/4); i++)
		*pBuf++ = 0;
}

void EthInit(void)
{
	ETH_InitTypeDef  ETH_InitStruct;

	/* Reset ehernet clock settings */
	ETH_ClockDeInit();

	RST_CLK_PCLKcmd(RST_CLK_PCLK_DMA, ENABLE);

	/* Enable HSE2 oscillator */
	RST_CLK_HSE2config(RST_CLK_HSE2_ON);
	if(RST_CLK_HSE2status() == ERROR)
		while(1); /* Infinite loop */

	/* Config PHY clock */
	ETH_PHY_ClockConfig(ETH_PHY_CLOCK_SOURCE_HSE2, ETH_PHY_HCLKdiv1);

	/* Init the BRG ETHERNET */
	ETH_BRGInit(ETH_HCLKdiv1);

	/* Enable the ETHERNET clock */
	ETH_ClockCMD(ETH_CLK1, ENABLE);

	/* Reset to default ethernet settings */
	ETH_DeInit(MDR_ETHERNET1);

	/* Init ETH_InitStruct members with its default value */
	ETH_StructInit((ETH_InitTypeDef * ) &ETH_InitStruct);
	/* Set the speed of the chennel */
	ETH_InitStruct.ETH_PHY_Mode = ETH_PHY_MODE_AutoNegotiation;
	ETH_InitStruct.ETH_Transmitter_RST = SET;
	ETH_InitStruct.ETH_Receiver_RST = SET;
	/* Set the buffer mode */
	ETH_InitStruct.ETH_Buffer_Mode = ETH_BUFFER_MODE_LINEAR;

	ETH_InitStruct.ETH_Receive_All_Packets = ENABLE;

	ETH_InitStruct.ETH_Source_Addr_HASH_Filter = DISABLE;
	ETH_InitStruct.ETH_Long_Frames_Reception = DISABLE;

	ETH_InitStruct.ETH_Control_Frames_Reception = ENABLE;
	ETH_InitStruct.ETH_Short_Frames_Reception = ENABLE;

	/* Set the buffer size of transmitter and receiver */
	ETH_InitStruct.ETH_Dilimiter = 0x1000;

	/* Init the ETHERNET 1 */
	ETH_Init(MDR_ETHERNET1, (ETH_InitTypeDef *) &ETH_InitStruct);

	MDR_ETHERNET1->ETH_IMR				= 0x0000;
	MDR_ETHERNET1->ETH_IFR				= 0xDFFF;
	
	MDR_ETHERNET1->ETH_Dilimiter 		= 4096;
	MDR_ETHERNET1->ETH_R_Head			= 0;
	MDR_ETHERNET1->ETH_X_Tail			= 4096;
	MDR_ETHERNET1->ETH_R_Tail			= 0;
	MDR_ETHERNET1->ETH_X_Head			= 4096;
	
	MDR_ETHERNET1->ETH_G_CFGh |= ETH_G_CFGh_XRST | ETH_G_CFGh_RRST;
	FlushFIFO ();
	MDR_ETHERNET1->ETH_G_CFGh &= ~(ETH_G_CFGh_XRST | ETH_G_CFGh_RRST);

	/* Enable PHY module */
	ETH_PHYCmd(MDR_ETHERNET1, ENABLE);
}

void EthOff (void)
{
	MDR_ETHERNET1->ETH_R_CFG &= ~ETH_R_CFG_EN;
	MDR_ETHERNET1->ETH_X_CFG &= ~ETH_X_CFG_EN;
}

void EthOn (void)
{
	while (MDR_ETHERNET1->ETH_STAT & ETH_STAT_R_COUNT_Msk)
		MDR_ETHERNET1->ETH_STAT -= (1 << ETH_STAT_R_COUNT_Pos);
	
	MDR_ETHERNET1->ETH_IFR				= 0xDFFF;
	
	MDR_ETHERNET1->ETH_Dilimiter 	= 4096;
	MDR_ETHERNET1->ETH_R_Head			= 0;
	MDR_ETHERNET1->ETH_X_Tail			= 4096;
	MDR_ETHERNET1->ETH_R_Tail			= 0;
	MDR_ETHERNET1->ETH_X_Head			= 4096;
		
	MDR_ETHERNET1->ETH_G_CFGh |= ETH_G_CFGh_XRST | ETH_G_CFGh_RRST;
	FlushFIFO ();
	MDR_ETHERNET1->ETH_G_CFGh &= ~(ETH_G_CFGh_XRST | ETH_G_CFGh_RRST);
		
	MDR_ETHERNET1->ETH_R_CFG |= ETH_R_CFG_EN;
	MDR_ETHERNET1->ETH_X_CFG |= ETH_X_CFG_EN;
}

void EthRecTrEn (void)
{
	MDR_ETHERNET1->ETH_R_CFG |= ETH_R_CFG_EN;
	MDR_ETHERNET1->ETH_X_CFG |= ETH_X_CFG_EN;
}

uint16_t EthReadFrame (uint8_t *Frame)
{
	uint16_t space_start = 0, space_end = 0, tail, head;
	uint32_t *src, *dst;
	uint32_t size, i, buf;
	uint16_t tmp[2], len;
	
	tail = MDR_ETHERNET1->ETH_R_Tail;
	head = MDR_ETHERNET1->ETH_R_Head;

	if (tail > head)
	{
		space_end = tail-head;
		space_start = 0;
	}
	else
	{
		space_end = MDR_ETHERNET1->ETH_Dilimiter - head;
		space_start = tail;
	}

	src = (uint32_t *)(MDR_ETHERNET1_BUF_BASE + head);
			
	*((uint32_t *)tmp) = *src++;
	space_end -= 4;
	
	len = tmp[0]-4;
	
	if ((uint16_t)src > (MDR_ETHERNET1->ETH_Dilimiter - 1))
		src = (uint32_t *)MDR_ETHERNET1_BUF_BASE;
					
	dst = (uint32_t *)Frame;
						
	size = (tmp[0]+3)/4;
			
	if (tmp[0] <= space_end)
	{
		for (i=0; i<(size-1); i++)
			*dst++ = *src++;
			
		buf = *src++;
	}
	else
	{
		size = size - space_end/4;
				
		for (i=0; i<(space_end/4); i++)
			*dst++ = *src++;
				
		src = (uint32_t *)MDR_ETHERNET1_BUF_BASE;
				
		for (i=0; i<(size-1); i++)
			*dst++ = *src++;
		
		buf = *src++;
	}
			
	if ((uint16_t)src > (MDR_ETHERNET1->ETH_Dilimiter - 1))
		src = (uint32_t *)MDR_ETHERNET1_BUF_BASE;

	MDR_ETHERNET1->ETH_R_Head = (uint16_t)src;
	MDR_ETHERNET1->ETH_STAT -= (1 << ETH_STAT_R_COUNT_Pos);
	
	return len;
}

int8_t EthWriteFrame (uint8_t *Frame, uint16_t Len)
{
	uint16_t i, tmp, head, tail;
	uint32_t *src, *dst;
	uint16_t space[2];
	
	head = MDR_ETHERNET1->ETH_X_Head;
	tail = MDR_ETHERNET1->ETH_X_Tail;
		
	if (head > tail)
	{
		space[0] = head-tail;
		space[1] = 0;
	}
	else
	{
		space[0] = MDR_ETHERNET1_BUF_SIZE - tail;
		space[1] = head - MDR_ETHERNET1->ETH_Dilimiter;
	}

	if (Len > (space[0]+space[1]-8))
		return ERR_MEM;
	
	tmp = Len;
	src = (uint32_t *)Frame;
	dst = (uint32_t *)(MDR_ETHERNET1_BUF_BASE + tail);
			
	*dst++ = tmp;
	space[0] -= 4;
			
	if ((uint16_t)dst > (MDR_ETHERNET1_BUF_SIZE-4))
		dst = (uint32_t *)(MDR_ETHERNET1_BUF_BASE + MDR_ETHERNET1->ETH_Dilimiter);

	tmp = (Len+3)/4;

	if (Len <= space[0])
	{
		for (i=0; i<tmp; i++)
			*dst++ = *src++;
	}
	else
	{
		tmp -= space[0]/4;
				
		for (i=0; i<(space[0]/4); i++)
			*dst++ = *src++;
				
		dst = (uint32_t *)(MDR_ETHERNET1_BUF_BASE + MDR_ETHERNET1->ETH_Dilimiter);
				
		for (i=0; i<tmp; i++)
			*dst++ = *src++;
	}
			
	if ((uint16_t)dst > (MDR_ETHERNET1_BUF_SIZE-4))
		dst = (uint32_t *)(MDR_ETHERNET1_BUF_BASE + MDR_ETHERNET1->ETH_Dilimiter);
			
	tmp = 0;
			
	*dst++ = tmp;
			
	if ((uint16_t)dst > (MDR_ETHERNET1_BUF_SIZE-4))
		dst = (uint32_t *)(MDR_ETHERNET1_BUF_BASE + MDR_ETHERNET1->ETH_Dilimiter);

	MDR_ETHERNET1->ETH_X_Tail = (uint16_t)dst;
	
	return ERR_OK;
}

/**
 * In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
static void low_level_init(struct netif *netif)
{
  EthInit();
 
  /* set netif MAC hardware address length */
  netif->hwaddr_len = ETHARP_HWADDR_LEN;
	
  /* set netif MAC hardware address */
  netif->hwaddr[0] =  MAC_ADDR0;
  netif->hwaddr[1] =  MAC_ADDR1;
  netif->hwaddr[2] =  MAC_ADDR2;
  netif->hwaddr[3] =  MAC_ADDR3;
  netif->hwaddr[4] =  MAC_ADDR4;
  netif->hwaddr[5] =  MAC_ADDR5;

  /* initialize MAC address in ethernet MAC */ 
  MDR_ETHERNET1->ETH_MAC_T = (netif->hwaddr[4] <<  8) |  netif->hwaddr[5];
  MDR_ETHERNET1->ETH_MAC_M = (netif->hwaddr[2] <<  8) |  netif->hwaddr[3];
  MDR_ETHERNET1->ETH_MAC_H = (netif->hwaddr[0] <<  8) |  netif->hwaddr[1];
  
  /* set netif maximum transfer unit */
  netif->mtu = 1500;

  /* Accept broadcast address and ARP traffic */
  netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;
  
  s_pxNetIf =netif;
  
  /* Enable MAC and DMA transmission and reception */
  EthRecTrEn();   
  //ETH_Start(MDR_ETHERNET1);
}


/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become availale since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */

static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
  struct pbuf *q;
  uint32_t l = 0;
  err_t Err;
  
  for(q = p; q != NULL; q = q->next) 
  {
      memcpy(&EthFrameTX[l], q->payload, q->len);
      l = l + q->len;
  }
  Err = EthWriteFrame(EthFrameTX, l);

  return Err;
}


	
  	

/**
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return a pbuf filled with the received packet (including MAC header)
 *         NULL on memory error
 */
static struct pbuf * low_level_input(struct netif *netif)
{
  struct pbuf *p, *q;
  uint16_t len;
  uint32_t l=0;
  
  p = NULL;
  
  /* Get received frame */
  len = EthReadFrame(EthFrameRX);

  /* We allocate a pbuf chain of pbufs from the pool. */
  p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
 
  /* Copy received frame from ethernet driver buffer to stack buffer */
  if (p != NULL)
  { 
    for (q = p; q != NULL; q = q->next)
    {
      memcpy((u8_t*)q->payload, &EthFrameRX[l], q->len);
      l = l + q->len;
    } 
  }
   
  return p;
}


/**
 * This function is the ethernetif_input task, it is processed when a packet 
 * is ready to be read from the interface. It uses the function low_level_input() 
 * that should handle the actual reception of bytes from the network
 * interface. Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param netif the lwip network interface structure for this ethernetif
 */
void ethernetif_input(void)
{
  struct pbuf *p;
  
  for( ;; )
  {
	if (MDR_ETHERNET1->ETH_R_Head != MDR_ETHERNET1->ETH_R_Tail)
    {
      p = low_level_input( s_pxNetIf );
      if (ERR_OK != s_pxNetIf->input( p, s_pxNetIf))
      {
        pbuf_free(p);
        p=NULL;
      }
    }

	// Link LED
  	if ((MDR_ETHERNET1->PHY_Status & ETH_PHY_STATUS_LED1) == 0)
	{
 	  PORT_SetBits(MDR_PORTD, PORT_Pin_7);   
	}
	else
	{
	  PORT_ResetBits(MDR_PORTD, PORT_Pin_7);
	}

	// CRS LED
	if ((MDR_ETHERNET1->PHY_Status & ETH_PHY_STATUS_LED2) == 0)
	{
 	  PORT_SetBits(MDR_PORTD, PORT_Pin_8);
	}
	else
	{
	  PORT_ResetBits(MDR_PORTD, PORT_Pin_8);
	}

  }
}

/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t ethernetif_init(struct netif *netif)
{
  LWIP_ASSERT("netif != NULL", (netif != NULL));

#if LWIP_NETIF_HOSTNAME
  /* Initialize interface hostname */
  netif->hostname = DEVICE_NAME;
#endif /* LWIP_NETIF_HOSTNAME */

  netif->name[0] = IFNAME0;
  netif->name[1] = IFNAME1;

  netif->output = etharp_output;
  netif->linkoutput = low_level_output;

  /* initialize the hardware */
  low_level_init(netif);

  return ERR_OK;
}