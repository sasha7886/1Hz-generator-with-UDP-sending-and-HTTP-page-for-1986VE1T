#ifndef __ETHERNETIF_H__
#define __ETHERNETIF_H__


#include "lwip/err.h"
#include "lwip/netif.h"

#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RST_CLK_ETH_CLOCK_PHY_BRG				RST_CLK_ETH_CLOCK_PHY_BRG_PHY1_CLK
#define RST_CLK_ETH_CLOCK_PHY_CLK_SEL		RST_CLK_ETH_CLOCK_PHY_CLK_SEL_HSE2

#define ETH_PHY_ADDR				0x1C
#define ETH_MDIO_CTRL_DIV		1

#define ETH_MODE		ETH_PHY_MODE_100BaseT_Full_Duplex

#define ETH_FRAME_SIZE	1514

#define LINK_SPEED_OF_NETIF_IN_BPS 100e6

err_t ethernetif_init(struct netif *netif);
void EthInit (void);
void EthOn (void);
void  ethernetif_poll(struct netif *netif);
void  ethernetif_check_link (struct netif *netif);
void ethernetif_input(void);

#ifdef __cplusplus
}
#endif

#endif 
