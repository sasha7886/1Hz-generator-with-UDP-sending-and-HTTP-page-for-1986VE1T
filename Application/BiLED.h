#ifndef _BiLED_H_
#define _BiLED_H_

//----------------------------------------------------------------------------
typedef enum { LED_BLACK = 0, LED_RED, LED_GREEN, LED_YELLOW }TBiLEDColor;

//----------------------------------------------------------------------------
typedef struct 
{
	MDR_PORT_TypeDef *PORTx;
	uint16_t PORT_Pin;		

}TLED_PortPin;

//----------------------------------------------------------------------------
typedef struct 
{
	TLED_PortPin	Green;		
	TLED_PortPin 	Red;

}TBiLED;

void InitBiLED( const TBiLED *pBiLed );
void SetBiLED( const TBiLED *pBiLed, TBiLEDColor Color ); 

#endif
