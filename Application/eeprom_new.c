#include "eeprom_new.h"
#include "MDR32F9Qx_config.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_port.h"
#include <XTick.h>
#include <BiLED.h>
#define EUI_48_START_ADDR		0xFA // начальный адрес блока EUI для 25AA02E48
#define EUI_48_SIZE					6 // размер блока EUI для 25AA02E48
uint8_t read_instruction = 0x03;
uint8_t SPI_delay = 40;
static const TBiLED m_Ledd = { { LED_GREEN_PORT, LED_GREEN_PIN }, { LED_RED_PORT, LED_RED_PIN } }; 
const uint8_t BAD_EUII[2][6] = {
	{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
};

uint8_t eeprom_new_init(uint8_t *eui)
{
	  PORT_InitTypeDef PORTCInit;
	  RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTC, ENABLE); 
	
	  PORTCInit.PORT_Pin = PORT_Pin_6; // MISO
    PORTCInit.PORT_OE = PORT_OE_IN; 
    PORTCInit.PORT_FUNC = PORT_FUNC_PORT; 
    PORTCInit.PORT_MODE = PORT_MODE_DIGITAL; 
    PORTCInit.PORT_SPEED = PORT_SPEED_FAST;
    PORT_Init(MDR_PORTC, &PORTCInit);
	
	  
	  PORTCInit.PORT_Pin = PORT_Pin_8 | PORT_Pin_9 | PORT_Pin_5 | PORT_Pin_7 |PORT_Pin_10; // CS,WP,MOSI,SCLK,HOLD
    PORTCInit.PORT_OE = PORT_OE_OUT; 
    PORTCInit.PORT_FUNC = PORT_FUNC_PORT; 
    PORTCInit.PORT_MODE = PORT_MODE_DIGITAL; 
    PORTCInit.PORT_SPEED = PORT_SPEED_FAST;
    PORT_Init(MDR_PORTC, &PORTCInit);
	
	PORT_SetBits(MDR_PORTC, PORT_Pin_8); // CS - '1'
	PORT_ResetBits(MDR_PORTC, PORT_Pin_9 | PORT_Pin_10); // WP - '0', HOLD - '0' 
}



void Delay (uint16_t ticks)
{
	uint16_t i;
	for(i=0;i<ticks;i++)
	{
		__NOP;
	}
}
void SendByte (uint8_t byte)
{
	uint8_t i;
	for(i=0;i<8;i++)
	{
		PORT_ResetBits(MDR_PORTC, PORT_Pin_7);//SCLK - 0
		if((byte & 0x80) != 0)
		{
			PORT_SetBits(MDR_PORTC, PORT_Pin_5);//MOSI - 1
			printf("1");
		}
		else
		{
			PORT_ResetBits(MDR_PORTC, PORT_Pin_5);//MOSI - 0
			printf("0");
		}
		byte = byte<<1;
		Delay(10*SPI_delay);
		PORT_SetBits(MDR_PORTC, PORT_Pin_7);//SCLK - 1
		Delay(20*SPI_delay);
		PORT_ResetBits(MDR_PORTC, PORT_Pin_7);//SCLK - 0
		Delay(5*SPI_delay);
	}
}	

uint8_t ReadByteSPI (void)
{
	uint8_t ret;
	uint8_t i;
	for(i=0;i<8;i++)
	{
		PORT_ResetBits(MDR_PORTC, PORT_Pin_7);//SCLK - 0
	Delay(5*SPI_delay);
	PORT_SetBits(MDR_PORTC, PORT_Pin_7);//SCLK - 1
	Delay(5*SPI_delay);
	if(PORT_ReadInputDataBit(MDR_PORTC,PORT_Pin_6)!=0)//Read from MISO
		{
			ret++;
			if(i<7)
			{
			ret=ret<<1;
			}
			printf ("1");
		}
		else
		{
			if(i<7)
			{
			ret=ret<<1;
			}
			printf ("0");
		}
	Delay(5*SPI_delay);
	}
	PORT_ResetBits(MDR_PORTC, PORT_Pin_7);//SCLK - 0
	Delay(5*SPI_delay);
		return ret;
}

static uint8_t eephrom_read_byte (uint8_t addr)
{
	uint8_t ret;
	uint8_t buffer [8];
	uint8_t addres   [8];
  uint8_t i;
	uint32_t time;
	//Clean buffer and addres
	for(i=0;i<8;i++)
	{
		buffer[i]=0;
		addres[i]=0;
	}
	printf ("buffer cleaned\n");

	PORT_ResetBits(MDR_PORTC, PORT_Pin_8); // CS - '0'

	//Send read instruction
	SendByte(read_instruction);
	printf ("read instruction sended\n");
	SendByte(addr);
	printf ("\naddres %d sended\n",addr);
	ret = ReadByteSPI(); 
	printf ("\ndata recieved\n");
	PORT_SetBits(MDR_PORTC, PORT_Pin_8); // CS - '1'
  return ret;
}

uint8_t eeprom_read(uint8_t *eui)
{
	uint8_t i, ret;
	printf ("eeprom read start\n");
	PORT_SetBits(MDR_PORTC, PORT_Pin_10); // HOLD - '1' 
	for( i = 0; i < EUI_48_SIZE; i++ )
	{
	printf ("stage %d\n",i);
		eui[i] = eephrom_read_byte(EUI_48_START_ADDR + i);
	}
	PORT_ResetBits(MDR_PORTC, PORT_Pin_10); // HOLD - '0'
		
	if ( memcmp(&eui[0], &BAD_EUII[0], 6) != 0 && memcmp(&eui[0], &BAD_EUII[1], 6) != 0	) {
		ret=1;
	}
	else {
		ret = 0;
		printf("\nMAC error\N");
		while(1)
		{
			SetBiLED( &m_Ledd, LED_RED );
		}
	}

	return ret;
}