/* Includes ------------------------------------------------------------------*/
#include "MDR32F9Qx_config.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_eeprom.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_iwdg.h"
#include "MDR32F9Qx_uart.h"
#include "MDR32F9Qx_timer.h"
#include "MDR32F9Qx_eth.h"

#include "Application.h"

#include "HW_Profile.h"
#include "BiLED.h"
#include "XTick.h"
#include "PPS.h"

#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <stdint.h>
#include <lwip/udp.h>
#include <ip.h>
#include <apps/httpd.h>


#include <netconf.h>
#include <ethernetif.h>
#include <eeprom_new.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Variables -----------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static const TBiLED m_Led = { { LED_GREEN_PORT, LED_GREEN_PIN }, { LED_RED_PORT, LED_RED_PIN } }; 
volatile struct ATTRIBUTES
{
	uint8_t validity;
	uint8_t efficency;
};
volatile struct TIME
{
	uint8_t second;
	uint8_t minute;
	uint8_t hour;
	uint8_t day;
	uint8_t month;
	uint16_t year;
};
volatile struct PAYLOAD
{
	uint8_t  number;
	uint16_t identificator;
  uint16_t useful_info;
  uint8_t  timering;
  uint8_t  timering_sign;
  uint8_t  reserve_first;
  struct   TIME msk;
	uint8_t  reserve_second;
  struct   TIME local;
	uint8_t  reserve_third;
	struct   ATTRIBUTES attributes;
	uint8_t  check_sum;
} Datagram;
  	

uint8_t DATA       [26];
uint32_t IPdest [4] = {255,255,255,255};
uint8_t MACdest   [ 6] = {0x00, 0xD2, 0x61, 0x00E3, 0xA2, 0xC6}; //MAC-адрес изделия 
uint8_t MACdevice [ 6];
uint8_t MACdevice_new [ 6];
uint16_t MACdevice_test [ 6];//MAC-адрес устройства
uint8_t IPdevicee [4] = {192,168,1,233};
uint16_t PORTdevice = 0; //Порт устройства
uint16_t PORTdest = 12233;//Порт изделия
uint32_t ticks;
uint8_t DaysInMonth [12] = {31,28,31,30,31,30,31,31,30,31,30,31};
struct udp_pcb *UDPSock;
//uint16_t psh [6];   //Псевдозаголовок для КС UDP
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void UDPTransportInit(void)
{    struct ip4_addr local;
	struct ip4_addr server;
	err_t status;
	IP4_ADDR(&local, IPdevicee[0],IPdevicee[1],IPdevicee[2],IPdevicee[3]);
	IP4_ADDR(&server, IPdest[0],IPdest[1],IPdest[2],IPdest[3]);
   UDPSock = udp_new();   
   status = udp_bind(UDPSock,&local , PORTdevice);
	 printf ("UDP_bind: %d\n", status);
   udp_recv(UDPSock, NULL, NULL);
   status = udp_connect(UDPSock, &server, PORTdest);
	 printf ("UDP_connect: %d\n", status);
}
 
void UDPSend(uint8_t* buf, int buf_len)
{
	struct netif *netif;
	err_t status;
    struct pbuf *p;
	  struct ip4_addr server, local;
	
	
    p = pbuf_alloc(PBUF_TRANSPORT, buf_len, PBUF_RAM);
    if (p == NULL)
		{
			printf("error with pbuf_alloc");
		}
    memcpy(p->payload, buf, buf_len);
 
    //struct ip_addr server;
       
    IP4_ADDR(&server, IPdest[0],IPdest[1],IPdest[2],IPdest[3]);
		IP4_ADDR(&local, IPdevicee[0],IPdevicee[1],IPdevicee[2],IPdevicee[3]);
    netif = ip4_route(&server);
		//показания для отладки
printf ("Netif parameter ip_addr   0x%"X32_F"\n", netif->ip_addr);
printf ("Netif parameter hostname  %s\n", netif->name);
printf ("Netif parameter hwaddr    0x%"X32_F"\n", netif->hwaddr);
printf ("Netif parameter netmask   0x%"X32_F"\n", netif->netmask);
printf ("UDP parameter local_ip    0x%"X32_F"\n", UDPSock->local_ip);
printf ("UDP parameter local_port  0x%"X32_F"\n", UDPSock->local_port);
printf ("UDP parameter remote_ip   0x%"X32_F"\n", UDPSock->remote_ip);
printf ("UDP parameter remote_port 0x%"X32_F"\n", UDPSock->remote_port);		
    status = udp_sendto_if(UDPSock,p,&server,PORTdest,netif);
		printf ("UDP_send: %d\n", status);
        
    pbuf_free(p);
}
//-----------------------------------------------------------------------------
void Asking ()
{
	Datagram.attributes.efficency=0;
	Datagram.attributes.validity=0;
	
	if(PORT_ReadInputDataBit(MDR_PORTD,PORT_Pin_1) != 0)
	{
		Datagram.attributes.efficency=1;
	}
	else
	{
		Datagram.attributes.efficency=0;
	}
	if(PORT_ReadInputDataBit(MDR_PORTD,PORT_Pin_2) != 0)
	{
		Datagram.attributes.validity=1;
	}
	else
	{
	
		Datagram.attributes.validity=0;
	}
}
//-----------------------------------------------------------------------------
void InputDisturbancePortInit (void)
{
	  PORT_InitTypeDef PORTDInit;
	  RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTD, ENABLE); 
	  PORTDInit.PORT_Pin = PORT_Pin_2 | PORT_Pin_1;
    PORTDInit.PORT_OE = PORT_OE_IN; 
    PORTDInit.PORT_FUNC = PORT_FUNC_PORT; 
    PORTDInit.PORT_MODE = PORT_MODE_DIGITAL; 
    PORTDInit.PORT_SPEED = PORT_SPEED_FAST;
    PORT_Init(MDR_PORTD, &PORTDInit);

}
//-----------------------------------------------------------------------------
bool ClockConfigure ( void )
//-----------------------------------------------------------------------------
{
  uint32_t cntr;

	// Switch on HSE clock generator
	//
	cntr = 0;
	RST_CLK_HSEconfig( RST_CLK_HSE_Bypass );
    while( RST_CLK_HSEstatus() != SUCCESS && cntr++ < 0x40000 )
		__NOP();

    if( RST_CLK_HSEstatus() != SUCCESS )
		return false;

	// Select HSE clock as CPU_PLL input clock source
	// Set PLL multiplier
	//
	#if !defined( CPU_CLOCK_VALUE )
		#error CPU_CLOCK_VALUE not defined!
	#endif
	#define PLL_MULL_VALUE (CPU_CLOCK_VALUE / HSE_Value - 1)
	RST_CLK_CPU_PLLconfig(RST_CLK_CPU_PLLsrcHSEdiv1, PLL_MULL_VALUE );

	// Enable CPU_PLL
	//
	cntr = 0;
	RST_CLK_CPU_PLLcmd(ENABLE);
	while( RST_CLK_CPU_PLLstatus() != SUCCESS && cntr++ < 0x40000 )
		__NOP();

    if( RST_CLK_CPU_PLLstatus() != SUCCESS )
		return false;

	// Enable the RST_CLK_PCLK_EEPROM 
	//
	RST_CLK_PCLKcmd(RST_CLK_PCLK_EEPROM, ENABLE);

	// Set the code latency value
	//
	#if CPU_CLOCK_VALUE < 25000000UL					// Freqency < 25MHz
		#define EEPROM_LATENCY_VALUE EEPROM_Latency_0
	#elif CPU_CLOCK_VALUE < 50000000UL					// 25MHz <= Freqency < 50MHz
		#define EEPROM_LATENCY_VALUE EEPROM_Latency_1				
	#elif CPU_CLOCK_VALUE < 75000000UL					// 50MHz <= Freqency < 75MHz
		#define EEPROM_LATENCY_VALUE EEPROM_Latency_2				
	#elif CPU_CLOCK_VALUE < 100000000UL					// 75MHz <= Freqency < 100MHz
		#define EEPROM_LATENCY_VALUE EEPROM_Latency_3				
	#elif CPU_CLOCK_VALUE < 125000000UL					// 100MHz <= Freqency < 125MHz
		#define EEPROM_LATENCY_VALUE EEPROM_Latency_4				
	#elif CPU_CLOCK_VALUE < 150000000UL					// 125MHz <= Freqency < 150MHz
		#define EEPROM_LATENCY_VALUE EEPROM_Latency_5				
	#else												// 150MHz <= Freqency
		#define EEPROM_LATENCY_VALUE EEPROM_Latency_7
	#endif
	EEPROM_SetLatency( EEPROM_LATENCY_VALUE );

	// Set CPU_C3_prescaler to 1
	//
	RST_CLK_CPUclkPrescaler( RST_CLK_CPUclkDIV1 );

	// Set CPU_C2_SEL to CPU_PLL output instead of CPU_C1 clock
	//
	RST_CLK_CPU_PLLuse( ENABLE );

	// Select CPU_C3 clock on the CPU clock MUX 
	//
	RST_CLK_CPUclkSelection( RST_CLK_CPUclkCPU_C3 );

	// Get Core Clock Frequency
	//
	SystemCoreClockUpdate();                            
	return true;
}

//-----------------------------------------------------------------------------
void InitWatchDog( void )
//-----------------------------------------------------------------------------
{
	RST_CLK_PCLKcmd(RST_CLK_PCLK_IWDG,ENABLE);
	IWDG_WriteAccessEnable();
	IWDG_SetPrescaler(IWDG_Prescaler_64);	// 625 Гц
	while( IWDG_GetFlagStatus( IWDG_FLAG_PVU ) != 1 )
	{}
	IWDG_SetReload( 2500 );	// 2500 / 652 = 4 сек
	IWDG_Enable();
	IWDG_ReloadCounter();
}
 
//-----------------------------------------------------------------------------
void InitUART( void )
//-----------------------------------------------------------------------------
{
  UART_InitTypeDef UART_InitStructure;
  PORT_InitTypeDef PortInitStruct;

	// Fill PortInit structure 
	//
	PortInitStruct.PORT_SPEED = PORT_SPEED_FAST;
	PortInitStruct.PORT_MODE = PORT_MODE_DIGITAL;
	
	// Configure UART TX pin
	//
	PortInitStruct.PORT_OE = PORT_OE_OUT;
	PortInitStruct.PORT_Pin = MY_UART_TX_PIN;
	PortInitStruct.PORT_FUNC = MY_UART_TX_PORT_FUNC;
	RST_CLK_PCLKcmd( PCLK_BIT(MY_UART_TX_PORT), ENABLE );
	PORT_Init( MY_UART_TX_PORT, &PortInitStruct );
	
	// Configure UART RX pin
	//
	PortInitStruct.PORT_OE = PORT_OE_IN;
	PortInitStruct.PORT_Pin = MY_UART_RX_PIN;
	PortInitStruct.PORT_FUNC = MY_UART_RX_PORT_FUNC;
	RST_CLK_PCLKcmd( PCLK_BIT(MY_UART_RX_PORT), ENABLE );
	PORT_Init( MY_UART_RX_PORT, &PortInitStruct );
	
	// Enables the CPU_CLK clock on UART
	RST_CLK_PCLKcmd( PCLK_BIT(MY_UART), ENABLE );
	
	// Set the HCLK division factor = 1 for UART
	//
	UART_BRGInit( MY_UART, UART_HCLKdiv1 );
	
	// Configure UART parameters 
	//
	UART_InitStructure.UART_BaudRate                = 115200;
	UART_InitStructure.UART_WordLength              = UART_WordLength8b;
	UART_InitStructure.UART_StopBits                = UART_StopBits1;
	UART_InitStructure.UART_Parity                  = UART_Parity_No;
	UART_InitStructure.UART_FIFOMode                = UART_FIFO_ON;
	UART_InitStructure.UART_HardwareFlowControl     = UART_HardwareFlowControl_RXE | UART_HardwareFlowControl_TXE;
	UART_Init( MY_UART, &UART_InitStructure );

	// Enables UART peripheral
	//
	UART_Cmd( MY_UART, ENABLE );
}

//-----------------------------------------------------------------------------
int fputc(int ch, FILE *f) 
//-----------------------------------------------------------------------------
{
	while( UART_GetFlagStatus( MY_UART, UART_FLAG_TXFE ) != SET );

	UART_SendData( MY_UART, ch );

	return ch;
}

//-----------------------------------------------------------------------------
uint8_t Get_check_sum (struct PAYLOAD Datagram, uint8_t size)
{
	uint8_t result =0;
	uint8_t i;
	char b[sizeof(Datagram)-1];
memcpy(b, &Datagram, sizeof(Datagram)-1);
	for(i=0; i<sizeof(Datagram)-2;i++)
	{ 
		result = result ^ (uint8_t) *(b+i);
	}

	return result;
}
//-----------------------------------------------------------------------------
void DATA_construct (struct PAYLOAD Datagram)
{
	uint8_t identificator_first          =  Datagram.identificator         & 0xFF;
	uint8_t identificator_second         = (Datagram.identificator>>8)     & 0xFF;
	uint8_t useful_info_first            =  Datagram.useful_info           & 0xFF;
	uint8_t useful_info_second           = (Datagram.useful_info>>8)       & 0xFF;
	uint8_t msk_year_first               =  Datagram.msk.year              & 0xFF;
	uint8_t msk_year_second              = (Datagram.msk.year>>8)          & 0xFF;
	uint8_t local_year_first             =  Datagram.local.year            & 0xFF;
	uint8_t local_year_second            = (Datagram.local.year>>8)        & 0xFF;
	
	DATA[ 0] = Datagram.number;
	DATA[ 1] = identificator_first;
	DATA[ 2] = identificator_second;
	DATA[ 3] = useful_info_first;
	DATA[ 4] = useful_info_second;
	DATA[ 5] = Datagram.timering;
	DATA[ 6] = Datagram.timering_sign;
	DATA[ 7] = Datagram.reserve_first;
	DATA[ 8] = Datagram.msk.second;
	DATA[ 9] = Datagram.msk.minute;
	DATA[10] = Datagram.msk.hour;
	DATA[11] = Datagram.msk.day;
	DATA[12] = Datagram.msk.month;
	DATA[13] = msk_year_first;
	DATA[14] = msk_year_second;
	DATA[15] = Datagram.reserve_second;
	DATA[16] = Datagram.local.second;
	DATA[17] = Datagram.local.minute;
	DATA[18] = Datagram.local.hour;
	DATA[19] = Datagram.local.day;
	DATA[20] = Datagram.local.month;
	DATA[21] = local_year_first;
	DATA[22] = local_year_second;
	DATA[23] = Datagram.reserve_third;
	DATA[24] = (Datagram.attributes.validity<<1|Datagram.attributes.efficency);
	DATA[25] = Datagram.check_sum;
	
}
//-----------------------------------------------------------------------------
void Ethernet_Init(void)
{	
	static ETH_InitTypeDef  ETH_InitStruct;
	volatile	uint32_t			ETH_Dilimiter;
	
	// Сброс тактирования Ethernet
	ETH_ClockDeInit();
	
	//	Включение генератора HSE2 = 25МГц
	RST_CLK_HSE2config(RST_CLK_HSE2_Bypass);
    while (RST_CLK_HSE2status() != SUCCESS);	
	
	// Тактирование PHY от HSE2 = 25МГц
	ETH_PHY_ClockConfig(ETH_PHY_CLOCK_SOURCE_HSE2, ETH_PHY_HCLKdiv1);

	// Без делителя
	ETH_BRGInit(ETH_HCLKdiv1);

	// Включение тактирования блока MAC
	ETH_ClockCMD(ETH_CLK1, ENABLE);


	//	Сброс регистров блока MAC
	ETH_DeInit(MDR_ETHERNET1);

	//  Инициализация настроек Ethernet по умолчанию
	ETH_StructInit(&ETH_InitStruct);
	
	//	Переопределение настроек PHY:
	//   - разрешение автонастройки, передатчик и приемник включены
	ETH_InitStruct.ETH_PHY_Mode = ETH_PHY_MODE_AutoNegotiation;
	ETH_InitStruct.ETH_Transmitter_RST = SET;
	ETH_InitStruct.ETH_Receiver_RST = SET;
	
	//	Режим работы буферов
	//ETH_InitStruct.ETH_Buffer_Mode = ETH_BUFFER_MODE_LINEAR;	
	//ETH_InitStruct.ETH_Buffer_Mode = ETH_BUFFER_MODE_FIFO;	
	ETH_InitStruct.ETH_Buffer_Mode = ETH_BUFFER_MODE_AUTOMATIC_CHANGE_POINTERS;

  // HASH - Фильтрация отключена 
	ETH_InitStruct.ETH_Source_Addr_HASH_Filter = DISABLE;

	//	Задание МАС адреса микроконтроллера
	ETH_InitStruct.ETH_MAC_Address[2] = (MACdevice[0] << 8) | MACdevice[1];
	ETH_InitStruct.ETH_MAC_Address[1] = (MACdevice[2] << 8) | MACdevice[3];
	ETH_InitStruct.ETH_MAC_Address[0] = (MACdevice[4] << 8) | MACdevice[5];

	//	Разделение общей памяти на буферы для приемника и передатчика
	ETH_InitStruct.ETH_Dilimiter = 0x1000;

	//	Разрешаем прием пакетов только на свой адрес, 
	//	Прием коротких пакетов также разрешен
	ETH_InitStruct.ETH_Receive_All_Packets 			  = DISABLE;
	ETH_InitStruct.ETH_Short_Frames_Reception 		= ENABLE;
	ETH_InitStruct.ETH_Long_Frames_Reception 	    = DISABLE;
	ETH_InitStruct.ETH_Broadcast_Frames_Reception = DISABLE;
	ETH_InitStruct.ETH_Error_CRC_Frames_Reception = DISABLE;
	ETH_InitStruct.ETH_Control_Frames_Reception 	= DISABLE;
	ETH_InitStruct.ETH_Unicast_Frames_Reception 	= ENABLE;
	ETH_InitStruct.ETH_Source_Addr_HASH_Filter 	  = DISABLE;

	//	Инициализация блока Ethernet
	ETH_Init(MDR_ETHERNET1, &ETH_InitStruct);

	// Запуск блока PHY
	ETH_PHYCmd(MDR_ETHERNET1, ENABLE);		
}

void Ethernet_Start(void)
{
	// Запуск блока Ethernet
	ETH_Start(MDR_ETHERNET1);
}
//-----------------------------------------------------------------------------
void Datagram_parameters_initialisation (void)
{
	Datagram.number         = 0;
	Datagram.identificator  = 0xFF;
	Datagram.useful_info    = 26;
	Datagram.timering       = 1;
	Datagram.timering_sign  = 1;
	Datagram.reserve_first  = 0;
		
	Datagram.msk.year       = 2022;
	Datagram.msk.month      = 1;
	Datagram.msk.day        = 1;
	Datagram.msk.hour       = 0;
	Datagram.msk.minute     = 59;
	Datagram.msk.second     = 30;
	Datagram.reserve_second = 0;
	Datagram.local = Datagram.msk;
		
		//local time correction
		{
			if(Datagram.timering_sign == 1)
		{
			if(Datagram.msk.hour < Datagram.timering)
			{
		    Datagram.local.day--;
			Datagram.local.hour = 24 + Datagram.msk.hour - Datagram.timering;
		  }
		else
		{
			Datagram.local.hour = Datagram.msk.hour + Datagram.timering;
		}
	  }
		else
		{
			Datagram.local.hour = Datagram.local.hour + Datagram.timering;
		}
	if(Datagram.local.day==0)
	{
		Datagram.local.month--;
		if(Datagram.local.month!=0 && Datagram.local.month!=2)
		{
			Datagram.local.day = DaysInMonth[Datagram.local.month-1];
		}
    if(Datagram.local.month ==0 )
		{
			Datagram.local.day =31;
			Datagram.local.month = 12;
			Datagram.local.year--;
		}
    if(Datagram.local.month== 2 && Datagram.local.year%4==0 )
		{
			Datagram.local.day =29;
		}
		if(Datagram.local.month== 2 && Datagram.local.year%4!=0 )
		{
			Datagram.local.day =28;
		}
	}
    }
	Datagram.reserve_third       = 0;
	Datagram.attributes.validity = 0;
	Datagram.attributes.efficency = 0;
	Datagram.check_sum           = 0;
}
	
//-----------------------------------------------------------------------------
void Time_counting (struct TIME *msk)
	{
			
		if(msk->second >= 60)
			{
				msk->second=0;
				msk->minute++;
			}
			if(msk->minute >= 60)
			{
				msk->minute=0;
				msk->hour++;
			}
			if(msk->hour >= 24)
			{
				msk->hour=msk->hour-24;
				msk->day++;
			}
			if(msk->day>DaysInMonth[msk->month-1])
			{
				if(msk->month == 2 && msk->day == 29 && ((msk->year%4==0 && msk->year%100!=0) || (msk->year%4==0 && msk->year%400==0)))
				{
				}
				else
				{
				msk->month++;
				msk->day=1;
				}
			}
			if(msk->month >= 13)
			{
				msk->month=1;
				msk->year++;
			}
		
		}
//-----------------------------------------------------------------------------
void TestLedInit (void)
{
	PORT_InitTypeDef PORTFInit;
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTC, ENABLE); 
	
  PORTFInit.PORT_Pin = PORT_Pin_5 | PORT_Pin_7 ;
  PORTFInit.PORT_OE = PORT_OE_OUT; 
  PORTFInit.PORT_FUNC = PORT_FUNC_PORT;
  PORTFInit.PORT_MODE = PORT_MODE_DIGITAL;
  PORTFInit.PORT_SPEED = PORT_SPEED_MAXFAST;
  PORT_Init(MDR_PORTC, &PORTFInit); 
}
//-----------------------------------------------------------------------------
void Counting_seconds (void)
{
	int i;
			
			if((GetSecond() - ticks) >= 1)
			{
				ticks = GetSecond();
				Datagram.number++;
				Datagram.msk.second++;
				Datagram.local.second++;
				Asking();
				Datagram.check_sum = Get_check_sum (Datagram,sizeof(Datagram));
				DATA_construct(Datagram);
				
				switch ((Datagram.attributes.validity<<1)|Datagram.attributes.efficency)
				{
					case 0:
						SetBiLED( &m_Led, LED_BLACK );
					break;
					case 1:
						SetBiLED( &m_Led, LED_RED );
					break;
					case 2:
						SetBiLED( &m_Led, LED_YELLOW );
					break;
					case 3:
						SetBiLED( &m_Led, LED_GREEN );
					break;
				}
				UDPSend(DATA, 26); 
				SetBiLED( &m_Led, LED_BLACK );
				ETH_GetMACAddress(MDR_ETHERNET1, MACdevice_test);
				for ( i=0; i<3; i++)
        {
          printf ("%2X ",MACdevice_test[i]);
        }	
				
				printf("\nProg SPI MAC: ");
				for ( i=0; i<6; i++)
        {
          printf ("%2X :",MACdevice[i]);
        }	
				printf("\n");
        for ( i=0; i<26; i++)
        {
          printf ("%X",DATA[i]);
        }	
				printf("\nattributes: %d\n",(Datagram.attributes.efficency<<1|Datagram.attributes.validity));
        printf("\n  msk: %d %d %d %d %d %d\nlocal: %d %d %d %d %d %d\n\n\n\n\n",Datagram.msk.year, Datagram.msk.month, Datagram.msk.day, Datagram.msk.hour, Datagram.msk.minute, Datagram.msk.second, Datagram.local.year, Datagram.local.month, Datagram.local.day, Datagram.local.hour,Datagram.local.minute,Datagram.local.second);				
			}
		}
//-----------------------------------------------------------------------------

		
int main( void )
//-----------------------------------------------------------------------------
{
	
	//Init block
	{
	SystemInit();
	ClockConfigure();
	InitBiLED( &m_Led );
	SetBiLED( &m_Led, LED_YELLOW );
  #if 1
	InitWatchDog();
  #else
 	#warning DEBUG! WatchDog disabled!
  #endif
	InitXTick();
	InputDisturbancePortInit ();
	DelayMs(50);
	InitUART();
	eeprom_new_init(MACdevice);
	eeprom_read(MACdevice);
	
	
  LwIP_Init();
	httpd_init();
	UDPTransportInit();
	Datagram_parameters_initialisation();
	FlagTimerInit();
	ticks = 0;
	}
	
	while( 1 )
	{
		LwIP_Pkt_Handle();
		LwIP_Periodic_Handle(ticks);
		Time_counting(&Datagram.msk);
		Time_counting(&Datagram.local);
		//Counting seconds + check_sum
		Counting_seconds();
		IWDG_ReloadCounter();
	}
}



//-----------------------------------------------------------------------------
#if (USE_ASSERT_INFO == 1)
void assert_failed(uint32_t file_id, uint32_t line)
{
	while (1)
	{
	}
}
#elif (USE_ASSERT_INFO == 2)
void assert_failed(uint32_t file_id, uint32_t line, const uint8_t* expr)
{
	while (1)
	{
	}
}
#endif /* USE_ASSERT_INFO */

