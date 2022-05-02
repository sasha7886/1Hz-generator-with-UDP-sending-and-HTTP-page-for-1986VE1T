#ifndef _HW_Profile_H_
#define _HW_Profile_H_

/* Includes ------------------------------------------------------------------*/
#include "MDR32F9Qx_config.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define CPU_CLOCK_VALUE	(100000000UL)	/* Частота контроллера */

//----------------------------------
// -- LEDS ----
#define LED_GREEN_PIN				PORT_Pin_6
#define LED_GREEN_PORT				MDR_PORTF

#define LED_RED_PIN					PORT_Pin_5
#define LED_RED_PORT				MDR_PORTF

// -- UART ----
#define MY_UART						MDR_UART1

#define MY_UART_RX_PIN				PORT_Pin_3
#define MY_UART_RX_PORT				MDR_PORTC
#define MY_UART_RX_PORT_FUNC		PORT_FUNC_MAIN

#define MY_UART_TX_PIN				PORT_Pin_4
#define MY_UART_TX_PORT				MDR_PORTC
#define MY_UART_TX_PORT_FUNC		PORT_FUNC_MAIN


/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
//-------------------------------------------------------------------------------------------------
	
#endif
