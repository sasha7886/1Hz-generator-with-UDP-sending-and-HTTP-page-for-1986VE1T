#ifndef __MAIN_H
#define __MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>


#include "MDR1986VE1T.h"
#include "MDR32F9Qx_config.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_iwdg.h"

#include "MDR32F9Qx_timer.h"

#include "MDR32F9Qx_eth.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CPU_CLOCK_VALUE  					100000000UL
#define TICKS_PER_SECOND					1000 	// 1мс
#define CAN_TIMEINFO_TIMEOUT_MS		500 	// 500мс лимит ожидания ШВ

#define __USE_IWDG
//#define __USE_DHCP
//#define __USE_1PPS_OUT							// ШИМ на PA12
#define __USE_ETH_PHY_STATUS_LED			// индикатор на активность Ethernet
#define __USE_CAN_ERROR								// самодиагностика CAN
//#define __USE_ONE_SYNC							// режим однократной синхронизации с внешнем источником
//#define __USE_DEFAULT_CONFIG  				// конфигурация без МИУ3
#define __USE_HARDWARE_MAC  					// MAC адрес из 25AA02E48

#define NTP_SERVER_PORT    				123

#define HSE_ON_ATTEMPTS						10

#ifdef __USE_HARDWARE_MAC
	#define READ_MAC_ATTEMPTS				5
#endif



#ifdef __cplusplus
}
#endif

#endif
