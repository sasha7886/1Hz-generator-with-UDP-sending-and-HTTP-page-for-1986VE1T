/**
  ******************************************************************************
  * @file    MDR1986VE1T_IT.c
  * @author  Milandr Application Team
  * @version V1.2.0
  * @date    03/04/2013
  * @brief   Main Interrupt Service Routines.
  *
  ******************************************************************************
  * <br><br>
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, MILANDR SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT
  * OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2013 Milandr</center></h2>
  */

/** @addtogroup __MDR32F9Qx_StdPeriph_Examples MDR32F9Qx StdPeriph Examples
  * @{
  */

/** @addtogroup __MDR1986VE1T_EVAL MDR1986VE1T Opora Evaluation Board
  * @{
  */

/** @addtogroup Ethernet_IT Ethernet
  * @{
  */

/* Includes ------------------------------------------------------------------*/
#include "MDR32F9Qx_config.h"
#include "MDR1986VE1T.h"
#include "MDR1986VE1T_IT.h"



/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void Hard_fault_handler_c(unsigned int* hardfault_args);
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : NMI_Handler
* Description    : This function handles NMI exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NMI_Handler(void)
{
	while (1);
}

/*******************************************************************************
* Function Name  : HardFault_Handler
* Description    : This function handles Hard Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void HardFault_Handler(void)
{
	while (1);
}

/*******************************************************************************
* Function Name  : SVC_Handler
* Description    : This function handles SVCall exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SVC_Handler(void)
{
	while (1);
}

/*******************************************************************************
* Function Name  : PendSV_Handler
* Description    : This function handles Debug PendSV exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PendSV_Handler(void)
{
	while (1);
}

/*******************************************************************************
* Function Name  : SysTick_Handler
* Description    : This function handles SysTick Handler.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
//void SysTick_Handler(void)
//{
//}

/*******************************************************************************
* Function Name  : MIL_STD_1553B2_IRQHandler
* Description    : This function handles MIL_STD_1553B2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
//void MIL_STD_1553B2_IRQHandler(void)
//{
//}

/*******************************************************************************
* Function Name  : MIL_STD_1553B1_IRQHandler
* Description    : This function handles MIL_STD_1553B1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
//void MIL_STD_1553B1_IRQHandler(void)
//{
//}

/*******************************************************************************
* Function Name  : USB_IRQHandler
* Description    : This function handles USB global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USB_IRQHandler(void)
{
}


/*******************************************************************************
* Function Name  : CAN1_IRQHandler
* Description    : This function handles CAN1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CAN1_IRQHandler(void)
{
}

/*******************************************************************************

* Function Name  : CAN2_IRQHandler
* Description    : This function handles CAN2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CAN2_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMA_IRQHandler
* Description    : This function handles DMA global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : UART1_IRQHandler
* Description    : This function handles UART1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UART1_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : UART2_IRQHandler
* Description    : This function handles UART2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UART2_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : SSP1_IRQHandler
* Description    : This function handles SSP1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SSP1_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : BUSY_IRQHandler
* Description    : This function handles BUSY global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void BUSY_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : ARINC429R_IRQHandler
* Description    : This function handles ARINC429R global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ARINC429R_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : POWER_IRQHandler
* Description    : This function handles POWER global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void POWER_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : WWDG_IRQHandler
* Description    : This function handles WWDG global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WWDG_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : Timer4_IRQHandler
* Description    : This function handles TIMER4 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Timer4_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : Timer1_IRQHandler
* Description    : This function handles Timer1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
//void Timer1_IRQHandler(void)
//{
//}

/*******************************************************************************
* Function Name  : Timer2_IRQHandler
* Description    : This function handles Timer2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Timer2_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : Timer3_IRQHandler
* Description    : This function handles Timer3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Timer3_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : ADC_IRQHandler
* Description    : This function handles ADC global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ADC_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : ETHERNET_IRQHandler
* Description    : This function handles ETHERNET global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ETHERNET_IRQHandler(void)
{

}

/*******************************************************************************
* Function Name  : SSP3_IRQHandler
* Description    : This function handles SSP3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SSP3_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : SSP2_IRQHandler
* Description    : This function handles SSP2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SSP2_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : ARINC429T1_IRQHandler
* Description    : This function handles ARINC429T1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ARINC429T1_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : ARINC429T2_IRQHandler
* Description    : This function handles ARINC429T2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ARINC429T2_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : ARINC429T3_IRQHandler
* Description    : This function handles ARINC429T3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ARINC429T3_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : ARINC429T3_IRQHandler
* Description    : This function handles ARINC429T3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ARINC429T4_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : BKP_IRQHandler
* Description    : This function handles BKP global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void BKP_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : EXT_INT1_IRQHandler
* Description    : This function handles EXT_INT1 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXT_INT1_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : EXT_INT2_IRQHandler
* Description    : This function handles EXT_INT2 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXT_INT2_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : EXT_INT3_IRQHandler
* Description    : This function handles EXT_INT3 global interrupt request.
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXT_INT3_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : EXT_INT4_IRQHandler
* Description    : This function handles EXT_INT4 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXT_INT4_IRQHandler(void)
{
}

/**
  * @brief
  * @param
  * @retval
  */
void Hard_fault_handler_c(unsigned int* hardfault_args)
{
	/* Go to infinite loop when Hard Fault exception occurs */
	while (1);
}

/** @} */ /* End of group Ethernet_1T */

/** @} */ /* End of group __MDR1986VE1T_EVAL MDR1986VE1T Opora Evaluation Board */

/** @} */ /* End of group __MDR32F9Qx_StdPeriph_Examples */

/******************* (C) COPYRIGHT 2013 Milandr *********/

/* END OF FILE MDR1986VE1T_IT.c */
