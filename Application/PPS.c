#include "MDR32F9Qx_config.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_timer.h"
#include <MDR32F9Qx_port.h>

#include <PPS.h>
#define alpha (CPU_CLOCK_VALUE/100000000) // Показывает во сколько раз частота МК отличается от 100МГц
static volatile uint32_t ticks_two =0;
void FlagTimerInit (void)
{
		TIMER_CntInitTypeDef sTimerCnt;
	TIMER_ChnInitTypeDef sTimerChnInit;
	TIMER_ChnOutInitTypeDef sTimerChnOut;
	PORT_InitTypeDef PortInit; //объявление структуры PortInit

	 RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTE, ENABLE);

  
  // направление передачи данных = Выход
  PortInit.PORT_OE = PORT_OE_OUT;
  // режим работы вывода порта = Порт
  PortInit.PORT_FUNC = PORT_FUNC_MAIN;
  // режим работы вывода = Цифровой
  PortInit.PORT_MODE = PORT_MODE_DIGITAL;
  // скорость фронта вывода = медленный
  PortInit.PORT_SPEED = PORT_SPEED_FAST;
  // выбор всех выводов для инициализации
  PortInit.PORT_Pin = PORT_Pin_7;
  //инициализация заданными параметрами порта C
  PORT_Init(MDR_PORTE, &PortInit);
	
	

	RST_CLK_PCLKcmd( RST_CLK_PCLK_TIMER2, ENABLE );

	// Reset all XTICK_TIMER settings
	//
	TIMER_DeInit( MDR_TIMER2 );

	// Init TIMER count
	//
	TIMER_CntStructInit( &sTimerCnt );

	sTimerCnt.TIMER_Prescaler			= CPU_CLOCK_VALUE/10000;
	sTimerCnt.TIMER_Period				= 10000-1;	
	sTimerCnt.TIMER_CounterMode			= TIMER_CntMode_ClkFixedDir;
	sTimerCnt.TIMER_CounterDirection	= TIMER_CntDir_Up;	
	sTimerCnt.TIMER_EventSource			= TIMER_EvSrc_None;
	sTimerCnt.TIMER_FilterSampling		= TIMER_FDTS_TIMER_CLK_div_1;    
	sTimerCnt.TIMER_ARR_UpdateMode		= TIMER_ARR_Update_Immediately;  
	sTimerCnt.TIMER_ETR_FilterConf		= TIMER_Filter_1FF_at_TIMER_CLK; 
	sTimerCnt.TIMER_ETR_Prescaler		= TIMER_ETR_Prescaler_None;      
	sTimerCnt.TIMER_ETR_Polarity		= TIMER_ETRPolarity_NonInverted; 
	sTimerCnt.TIMER_BRK_Polarity		= TIMER_BRKPolarity_NonInverted; 
	TIMER_CntInit( MDR_TIMER2, &sTimerCnt );
	
	TIMER_ChnStructInit(&sTimerChnInit);
	sTimerChnInit.TIMER_CH_Mode = TIMER_CH_MODE_PWM;
	sTimerChnInit.TIMER_CH_REF_Format = TIMER_CH_REF_Format7;
	sTimerChnInit.TIMER_CH_Number = TIMER_CHANNEL1;
	TIMER_ChnInit(MDR_TIMER2, &sTimerChnInit);
	TIMER_SetChnCompare(MDR_TIMER2, TIMER_CHANNEL1,300);
	TIMER_ChnOutStructInit(&sTimerChnOut);
	sTimerChnOut.TIMER_CH_DirOut_Source = TIMER_CH_OutSrc_REF;
	sTimerChnOut.TIMER_CH_DirOut_Mode = TIMER_CH_OutMode_Output;
	sTimerChnOut.TIMER_CH_NegOut_Source = TIMER_CH_OutSrc_REF;
	sTimerChnOut.TIMER_CH_NegOut_Mode = TIMER_CH_OutMode_Output;
	sTimerChnOut.TIMER_CH_Number = TIMER_CHANNEL1;
	
	TIMER_ChnOutInit(MDR_TIMER2, &sTimerChnOut);
	TIMER_BRGInit( MDR_TIMER2, TIMER_HCLKdiv1 );

	TIMER_ITConfig( MDR_TIMER2, TIMER_STATUS_CNT_ZERO, ENABLE);
	NVIC_SetPriority( TIMER2_IRQn, 3 );
	NVIC_EnableIRQ( TIMER2_IRQn );

	TIMER_Cmd( MDR_TIMER2, ENABLE );

 
}

//-----------------------------------------------------------------------------
TIMER2_IRQHandler()
//-----------------------------------------------------------------------------
{
	ticks_two++;
	#if defined( USE_MDR1986VE1T )
	MDR_TIMER2->STATUS &= ~TIMER_STATUS_CNT_ZERO;
  #endif
}
//-----------------------------------------------------------------------------
uint32_t GetSecond (void)
{
	return ticks_two;
}
