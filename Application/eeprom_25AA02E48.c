#include "eeprom_25AA02E48.h"
#include "MDR32F9Qx_ssp.h"              // Keil::Drivers:SSP


// объявление локальных ф-ий
static uint8_t ReadByte_25AA02E48(uint8_t addr);

const uint8_t BAD_EUI[2][6] = {
	{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
};

uint8_t EUI_25AA02E48_Read(uint8_t *eui)
{
	uint8_t i, ret;
	
	PORT_SetBits(MDR_PORTC, PORT_Pin_10); // HOLD - '1' 
	for( i = 0; i < EUI_48_SIZE; i++ )
	{
		eui[i] = ReadByte_25AA02E48(EUI_48_START_ADDR + i);
	}
	PORT_ResetBits(MDR_PORTC, PORT_Pin_10); // HOLD - '0'
		
	
	return 0;
}


static uint8_t ReadByte_25AA02E48(uint8_t addr)
{
	uint8_t ret;

	while (SSP_GetFlagStatus(MDR_SSP1, SSP_FLAG_RNE) != RESET) // очистка FIFO приемника
	{
		SSP_ReceiveData(MDR_SSP1);	
	}

	PORT_ResetBits(MDR_PORTC, PORT_Pin_8); // CS - '0'

	SSP_SendData(MDR_SSP1, READ_INSTR);
	while (SSP_GetFlagStatus(MDR_SSP1, SSP_FLAG_BSY) != RESET); // ожидание завершения активности SPI

	SSP_SendData(MDR_SSP1, addr);
	while (SSP_GetFlagStatus(MDR_SSP1, SSP_FLAG_BSY) != RESET); // ожидание завершения активности SPI

	SSP_SendData(MDR_SSP1, 0); // NOP
	while (SSP_GetFlagStatus(MDR_SSP1, SSP_FLAG_BSY) != RESET); // ожидание завершения активности SPI
	
	while (SSP_GetFlagStatus(MDR_SSP1, SSP_FLAG_RNE) != RESET) // чтение из FIFO приемника, до его опустошения
	{
		ret = SSP_ReceiveData(MDR_SSP1);	
	}
	
	PORT_SetBits(MDR_PORTC, PORT_Pin_8); // CS - '1'

  return ret;
}

uint8_t EUI_25AA02E48_Init(uint8_t *eui)
{
	SSP_InitTypeDef sSSP;
	PORT_InitTypeDef PORT_InitStructure;

	uint8_t err = 0;
	uint8_t i;
	
	RST_CLK_PCLKcmd(RST_CLK_PCLK_SSP1 | RST_CLK_PCLK_PORTC, ENABLE);
	
  PORT_StructInit(&PORT_InitStructure);
	
	PORT_InitStructure.PORT_MODE = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_PD = PORT_PD_DRIVER;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_MAXFAST;
	// MOSI - PC5, SCK - PC7
	PORT_InitStructure.PORT_Pin = PORT_Pin_5 | PORT_Pin_7;
	PORT_InitStructure.PORT_OE = PORT_OE_OUT;
	PORT_InitStructure.PORT_FUNC = /*PORT_FUNC_PORT;//*/PORT_FUNC_ALTER;
	PORT_Init(MDR_PORTC, &PORT_InitStructure);
	// MISO - PC6
	PORT_InitStructure.PORT_Pin = PORT_Pin_6;
	PORT_InitStructure.PORT_OE = PORT_OE_IN;
	PORT_InitStructure.PORT_FUNC = /*PORT_FUNC_PORT;//*/PORT_FUNC_ALTER;
	PORT_Init(MDR_PORTC, &PORT_InitStructure);
	// CS - PC8, WP - PC9, HOLD - PC10
	PORT_InitStructure.PORT_Pin	= PORT_Pin_8 | PORT_Pin_9 | PORT_Pin_10;
	PORT_InitStructure.PORT_OE = PORT_OE_OUT;
	PORT_InitStructure.PORT_FUNC = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_MAXFAST;
	PORT_Init(MDR_PORTC, &PORT_InitStructure);
	
	PORT_SetBits(MDR_PORTC, PORT_Pin_8); // CS - '1'
	PORT_ResetBits(MDR_PORTC, PORT_Pin_9 | PORT_Pin_10); // WP - '0', HOLD - '0' 
	
	SSP_DeInit(MDR_SSP1);
	
	SSP_BRGInit(MDR_SSP1, SSP_HCLKdiv4);
	
	// SSP1 мастер, SCK ~2.5 МГц
	SSP_StructInit(&sSSP);

	sSSP.SSP_SCR = 0x05;
	sSSP.SSP_CPSDVSR = 2;
	sSSP.SSP_Mode = SSP_ModeMaster;
	sSSP.SSP_WordLength = SSP_WordLength8b;
	sSSP.SSP_SPH = SSP_SPH_1Edge;
	sSSP.SSP_SPO = SSP_SPO_Low;
	sSSP.SSP_FRF = SSP_FRF_SPI_Motorola;
	sSSP.SSP_HardwareFlowControl = SSP_HardwareFlowControl_SSE;
	SSP_Init(MDR_SSP1, &sSSP);

	SSP_Cmd(MDR_SSP1, ENABLE);
		
#ifdef __USE_HARDWARE_MAC	
	// Ethernet
	// MAC
	err = 1;
	for ( i = 0; i < READ_MAC_ATTEMPTS; i++ )
	{
		if ( EUI_25AA02E48_Read(eui) ) {
			err = 0;
			break;
		}
	}	
#endif	
	return err;
}