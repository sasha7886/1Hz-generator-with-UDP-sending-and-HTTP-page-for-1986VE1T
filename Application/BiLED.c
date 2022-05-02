#include "MDR32F9Qx_config.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_port.h"
#include "BiLED.h"


//----------------------------------------------------------------------------
void SetBiLED( const TBiLED *pBiLed, TBiLEDColor Color )
//----------------------------------------------------------------------------
{
	switch( Color )
	{
		case LED_BLACK:
			PORT_ResetBits( pBiLed->Red.PORTx, pBiLed->Red.PORT_Pin );
			PORT_ResetBits( pBiLed->Green.PORTx, pBiLed->Green.PORT_Pin );
		break;

		case LED_RED:
			PORT_SetBits( pBiLed->Red.PORTx, pBiLed->Red.PORT_Pin );
			PORT_ResetBits( pBiLed->Green.PORTx, pBiLed->Green.PORT_Pin );
		break;

		case LED_GREEN:
			PORT_ResetBits( pBiLed->Red.PORTx, pBiLed->Red.PORT_Pin );
			PORT_SetBits( pBiLed->Green.PORTx, pBiLed->Green.PORT_Pin );
		break;

		case LED_YELLOW:
			PORT_SetBits( pBiLed->Red.PORTx, pBiLed->Red.PORT_Pin );
			PORT_SetBits( pBiLed->Green.PORTx, pBiLed->Green.PORT_Pin );
		break;
	}
}

//----------------------------------------------------------------------------
static void InitLED_Pin( const TLED_PortPin *PortPin )  
//----------------------------------------------------------------------------
{
  PORT_InitTypeDef port_init_struct;

	// Enable peripheral clocks for PORT
	//
	RST_CLK_PCLKcmd( PCLK_BIT( PortPin->PORTx ), ENABLE );

	// Configure pin
	//
	PORT_StructInit( &port_init_struct );

	port_init_struct.PORT_Pin   = PortPin->PORT_Pin;
	port_init_struct.PORT_OE    = PORT_OE_OUT;
	port_init_struct.PORT_FUNC  = PORT_FUNC_PORT;
	port_init_struct.PORT_MODE  = PORT_MODE_DIGITAL;
#if 1
	port_init_struct.PORT_SPEED = PORT_SPEED_SLOW;
#else
	#warning Debug mode! PORT_SPEED_MAXFAST
	port_init_struct.PORT_SPEED = PORT_SPEED_MAXFAST;
#endif

	PORT_Init( PortPin->PORTx, &port_init_struct );
}

//----------------------------------------------------------------------------
void InitBiLED( const TBiLED *pBiLed )  
//----------------------------------------------------------------------------
{
	InitLED_Pin( &pBiLed->Red );
	InitLED_Pin( &pBiLed->Green );
}
