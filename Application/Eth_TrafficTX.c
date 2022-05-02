#include <MDR32F9Qx_port.h>
#include <MDR32F9Qx_rst_clk.h>
#include <MDR32F9Qx_eeprom.h>

#include "MDR32F9Qx_eth.h"

//	Определение констант для удобства работы
#define FR_MAC_SIZE 		12												//	Длина МАС полей в заголовке
#define FR_L_SIZE   		2													//  Длина поля Lenth/Eth Type
#define FR_HEAD_SIZE   	(FR_MAC_SIZE + FR_L_SIZE) // 	Длина Заголовка

// Массивы для считывания входящих и формирования посылаемых фреймов, 
// Должны располагаться в адресах начиная с 0х2010_0000 для возможности работы DMA в режиме FIFO
#define  MAX_ETH_TX_DATA_SIZE 1514 / 4
#define  MAX_ETH_RX_DATA_SIZE 1514 / 4
uint8_t  FrameTx[MAX_ETH_TX_DATA_SIZE] __attribute__((section("EXECUTABLE_MEMORY_SECTION"))) __attribute__ ((aligned (4)));
uint32_t FrameRx[MAX_ETH_RX_DATA_SIZE] __attribute__((section("EXECUTABLE_MEMORY_SECTION"))) __attribute__ ((aligned (4)));

//	MAC адрес микроконтроллера
uint8_t  MAC_SRC [] = {0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc};

//	Обработка входящего фрейма и высылка ответа
void ETH_TaskProcess(MDR_ETHERNET_TypeDef * ETHERNETx);
//	Заполнение массива FrameTx фреймом заданной длины
void Ethernet_FillFrameTX(uint32_t frameL);

//	Тактирование ядра от HSE, 8МГц на демо-плате
void Clock_Init(void)
{
	/* Enable HSE (High Speed External) clock */
	RST_CLK_HSEconfig(RST_CLK_HSE_ON);
	while (RST_CLK_HSEstatus() != SUCCESS);

	/* Configures the CPU_PLL clock source */
	RST_CLK_CPU_PLLconfig(RST_CLK_CPU_PLLsrcHSEdiv1, RST_CLK_CPU_PLLmul16);

	/* Enables the CPU_PLL */
	RST_CLK_CPU_PLLcmd(ENABLE);
	while (RST_CLK_CPU_PLLstatus() == ERROR);

	/* Enables the RST_CLK_PCLK_EEPROM */
	RST_CLK_PCLKcmd(RST_CLK_PCLK_EEPROM, ENABLE);
	/* Sets the code latency value */
	EEPROM_SetLatency(EEPROM_Latency_5);

	/* Select the CPU_PLL output as input for CPU_C3_SEL */
	RST_CLK_CPU_PLLuse(ENABLE);
	/* Set CPUClk Prescaler */
	RST_CLK_CPUclkPrescaler(RST_CLK_CPUclkDIV1);

	/* Select the CPU clock source */
	RST_CLK_CPUclkSelection(RST_CLK_CPUclkCPU_C3);
}

//	Инициализация блока Ethernet
void Ethernet_Init(void)
{	
	static ETH_InitTypeDef  ETH_InitStruct;
	volatile	uint32_t			ETH_Dilimiter;
	
	// Сброс тактирования Ethernet
	ETH_ClockDeInit();
	
	//	Включение генератора HSE2 = 25МГц
	RST_CLK_HSE2config(RST_CLK_HSE2_ON);
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
	ETH_InitStruct.ETH_Buffer_Mode = ETH_BUFFER_MODE_FIFO;	
	//ETH_InitStruct.ETH_Buffer_Mode = ETH_BUFFER_MODE_AUTOMATIC_CHANGE_POINTERS;

  // HASH - Фильтрация отключена 
	ETH_InitStruct.ETH_Source_Addr_HASH_Filter = DISABLE;

	//	Задание МАС адреса микроконтроллера
	ETH_InitStruct.ETH_MAC_Address[2] = (MAC_SRC[5] << 8) | MAC_SRC[4];
	ETH_InitStruct.ETH_MAC_Address[1] = (MAC_SRC[3] << 8) | MAC_SRC[2];
	ETH_InitStruct.ETH_MAC_Address[0] = (MAC_SRC[1] << 8) | MAC_SRC[0];

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

//	Цикл обработки входящих фреймов
void Ethernet_ProcessLoop(void)
{
	while(1){
		 ETH_TaskProcess(MDR_ETHERNET1);
	}
}

//	Обработка входящего фрейма и высылка ответа
void ETH_TaskProcess(MDR_ETHERNET_TypeDef * ETHERNETx)
{
	//	Поле состояния приема пакета
	volatile ETH_StatusPacketReceptionTypeDef ETH_StatusPacketReceptionStruct;
	
	//	Указатель для работы с входными данными
	uint8_t * ptr_inpFrame = (uint8_t *) &FrameRx[0];
	//	Входные параметры от PC
	uint16_t frameL = 0;
	uint16_t frameCount = 0;
	//	Внутренние переменные
	uint32_t i;
	volatile uint32_t isTxBuffBusy = 0;

	//	Проверяем наличие в бефере приемника данным для считывания
	if(ETHERNETx->ETH_R_Head != ETHERNETx->ETH_R_Tail)
	{
		//	Считывание входного фрейма
		ETH_StatusPacketReceptionStruct.Status = ETH_ReceivedFrame(ETHERNETx, FrameRx);
		
		//	Считывание длины ответного фрейма
		frameL = (uint16_t)((ptr_inpFrame[FR_HEAD_SIZE] << 8) | (ptr_inpFrame[FR_HEAD_SIZE + 1]));
		//  Считывание количества ответных фреймов, посылаем как минимум один пакет
		frameCount = (uint16_t)((ptr_inpFrame[FR_HEAD_SIZE + 2] << 8) | (ptr_inpFrame[FR_HEAD_SIZE + 3]));
		if (frameCount <= 0)
			frameCount = 1;		
		
		// 	Заполнение массива FrameTx пакетом отправки
		Ethernet_FillFrameTX(frameL);
		
		//	Посылка пакетов в цикле
		for (i = 0; i < frameCount; ++i)
		{
			//	Заполняем индекс текущего пакета, в первых байтах Payload
			//	FR_HEAD_SIZE + 4 - Это заголовок + "Поле управления передачей пакета"
			FrameTx[FR_HEAD_SIZE + 4] = (uint8_t)(i >> 8);
			FrameTx[FR_HEAD_SIZE + 5] = (uint8_t)(i & 0xFF);
			
			//  Ожидаем опустошения буфера передатчика наполовину
			//  Размер буфера приемника задан Delimeter и равен 4Кбайт
			//  Максимальная длина пакета может составлять 1518 байт
			//	Выбор условия опустошения на половину дает бОльшую надежность при интенсивном обмене большими пакетами.			
			do {
				isTxBuffBusy = ETH_GetFlagStatus(ETHERNETx, ETH_MAC_FLAG_X_HALF) == SET;				
			}	
			while (isTxBuffBusy);

			//  Посылка пакета
			ETH_SendFrame(ETHERNETx, (uint32_t *) FrameTx, frameL);
		}
	}
}

//	Заполнение массива FrameTx фреймом заданной длины
void Ethernet_FillFrameTX(uint32_t frameL)
{
	uint32_t i = 0;
	//	Определение колличества данных в Payload
	uint32_t payloadL = frameL - FR_HEAD_SIZE;
	//	Указатель на входящий фрейм для копирования SrcMAC
	uint8_t * ptr_inpFrame = (uint8_t *) &FrameRx[0];
	//	Указатель на заполняемый фрейм, 
	//  оставлено место "Поле управления передачей пакета"
	uint8_t * ptr_TXFrame  = (uint8_t *) &FrameTx[4];

	//	Запись "Поле управления передачей пакета"
	//  Указывается длина фрейма
	*(uint32_t *)&FrameTx[0] = frameL;
	
	//	Копируем адрес PC из входного пакета в DestMAC
	ptr_TXFrame[0] 	= ptr_inpFrame[6];
	ptr_TXFrame[1] 	= ptr_inpFrame[7];
	ptr_TXFrame[2] 	= ptr_inpFrame[8];
	ptr_TXFrame[3] 	= ptr_inpFrame[9];
	ptr_TXFrame[4] 	= ptr_inpFrame[10];
	ptr_TXFrame[5] 	= ptr_inpFrame[11];		
	
	//	Заполняем SrcMAC
	ptr_TXFrame[6] 	= MAC_SRC[0];
	ptr_TXFrame[7] 	= MAC_SRC[1];
	ptr_TXFrame[8] 	= MAC_SRC[2];
	ptr_TXFrame[9] 	= MAC_SRC[3];
	ptr_TXFrame[10] = MAC_SRC[4];
	ptr_TXFrame[11] = MAC_SRC[5];	

  // Заполняем поле Length размером Payload  в байтах
	ptr_TXFrame[12] = (uint8_t)(payloadL >> 8);
	ptr_TXFrame[13] = (uint8_t)(payloadL & 0xFF);	

	//	Заполняем Payload значениями, например индексами
	for (i = 0; i < payloadL; ++i)
		ptr_TXFrame[FR_HEAD_SIZE + i] = i;	
}


