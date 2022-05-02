#include "MDR32F9Qx_config.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_timer.h"
#include <MDR32F9Qx_port.h>

#include "XTick.h"

#if defined( USE_MDR1986VE1T )

	#define XTICK_TIMER 				MDR_TIMER1
	#define XTICK_TIMER_IRQn			TIMER1_IRQn
	#define XTICK_TIMER_IRQ_Handler()	void TIMER1_IRQHandler( void )
	#define GetHWTimerVal()				(XTICK_TIMER->CNT)
	#define IsXTickTimerOver()			(XTICK_TIMER->STATUS & TIMER_STATUS_CNT_ZERO)
#else 
	#warning XTick for 1986V9x not tested!
	#define XTICK_TIMER_IRQn			SysTick_IRQn
	#define XTICK_TIMER_IRQ_Handler()	void SysTick_Handler( void )
	#define GetHWTimerVal()				(0xFFFFFFUL - SysTick->VAL)
	#define IsXTickTimerOver()			(SysTick->CTRL & (1 << 16))
#endif

#if TIMER_HIGH_BITS_CNT >= 24
	#error Неверное значение TIMER_HIGH_BITS_CNT!
#endif

#if TIMER_HIGH_BITS_CNT < 10
	#error Неверное значение TIMER_HIGH_BITS_CNT!
#endif



//-----------------------------------------------------------------------------

static volatile uint32_t m_dwInternalTicks = 0;


//-----------------------------------------------------------------------------
static void GetRawXTick( uint32_t *HighBits, uint32_t *LowBits )
//-----------------------------------------------------------------------------
{
	do
	{
		NVIC_EnableIRQ( XTICK_TIMER_IRQn );
		__NOP();
		__NOP();
		NVIC_DisableIRQ( XTICK_TIMER_IRQn );

		*LowBits = GetHWTimerVal();
		*HighBits = m_dwInternalTicks;

	}while( IsXTickTimerOver() );

	NVIC_EnableIRQ( XTICK_TIMER_IRQn );
}

//-----------------------------------------------------------------------------
uint32_t GetXTick( void )
//-----------------------------------------------------------------------------
{
  uint32_t highbits, lowbits;
  uint32_t result;

	GetRawXTick( &highbits, &lowbits );

	result = lowbits >> (24-TIMER_HIGH_BITS_CNT);
	result |= highbits << TIMER_HIGH_BITS_CNT;

	return result;
}

//-----------------------------------------------------------------------------
uint32_t GetXTickDiv1024( void )
//-----------------------------------------------------------------------------
{
  uint32_t highbits, lowbits;
  uint32_t result;

	GetRawXTick( &highbits, &lowbits );

	result = lowbits >> (24-(TIMER_HIGH_BITS_CNT-10));
	result |= highbits << (TIMER_HIGH_BITS_CNT-10);

	return result;
}

//-------------------------------------------------------------------------------------------
void InitXTick( void )
//-------------------------------------------------------------------------------------------
{
 #if defined( USE_MDR1986VE1T )
	

  TIMER_CntInitTypeDef sTimerCnt;

	RST_CLK_PCLKcmd( PCLK_BIT( XTICK_TIMER ), ENABLE );

	// Reset all XTICK_TIMER settings
	//
	TIMER_DeInit( XTICK_TIMER );

	// Init TIMER count
	//
	TIMER_CntStructInit( &sTimerCnt );

	sTimerCnt.TIMER_Prescaler			= 0;
	sTimerCnt.TIMER_Period				= (1 << 24) - 1;	
	sTimerCnt.TIMER_CounterMode			= TIMER_CntMode_ClkFixedDir;
	sTimerCnt.TIMER_CounterDirection	= TIMER_CntDir_Up;	
	sTimerCnt.TIMER_EventSource			= TIMER_EvSrc_None;
	sTimerCnt.TIMER_FilterSampling		= TIMER_FDTS_TIMER_CLK_div_1;    
	sTimerCnt.TIMER_ARR_UpdateMode		= TIMER_ARR_Update_Immediately;  
	sTimerCnt.TIMER_ETR_FilterConf		= TIMER_Filter_1FF_at_TIMER_CLK; 
	sTimerCnt.TIMER_ETR_Prescaler		= TIMER_ETR_Prescaler_None;      
	sTimerCnt.TIMER_ETR_Polarity		= TIMER_ETRPolarity_NonInverted; 
	sTimerCnt.TIMER_BRK_Polarity		= TIMER_BRKPolarity_NonInverted; 

	TIMER_CntInit( XTICK_TIMER, &sTimerCnt );

	TIMER_BRGInit( XTICK_TIMER, TIMER_HCLKdiv1 );

	TIMER_ITConfig( XTICK_TIMER, TIMER_STATUS_CNT_ZERO, ENABLE);
	NVIC_SetPriority( XTICK_TIMER_IRQn, 3 );
	NVIC_EnableIRQ( XTICK_TIMER_IRQn );

	TIMER_Cmd( XTICK_TIMER, ENABLE );

 #else

	SysTick->LOAD = 0xFFFFFFUL;
	SysTick->CTRL = SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk ;
	NVIC_SetPriority( SysTick_IRQn, 3 );

 #endif

}

//-----------------------------------------------------------------------------
XTICK_TIMER_IRQ_Handler()
//-----------------------------------------------------------------------------
{
	m_dwInternalTicks++;
	
  #if defined( USE_MDR1986VE1T )
	XTICK_TIMER->STATUS &= ~TIMER_STATUS_CNT_ZERO;
  #endif
}


//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void DelayMs( uint32_t delay )
//-----------------------------------------------------------------------------
{
  uint32_t ticks;

	ticks = GetXTick();
	while( ( GetXTick() - ticks ) < TICKS_PER_MILLISECOND * delay );
}
//-----------------------------------------------------------------------------
