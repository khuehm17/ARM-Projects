/*
**
**                           Main.c
**
**
**********************************************************************/
/*
   Last committed:     $Revision: 00 $
   Last changed by:    $Author: $
   Last changed date:  $Date:  $
   ID:                 $Id:  $

**********************************************************************/
#include "stm32f4xx_conf.h"
#include <stm32f4xx_GPIO.h>
#include <stm32f4xx_rcc.h>

void IO_Init(void)
{
    // Enable Port_A
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    // Enable Port_D
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

    // Configure GPIO
    GPIO_InitTypeDef GPIO_InitStruct;

    /* Button Pin_0 Port_A */
        // Pin
        GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
        // Mode
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
        // Speed
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
        // Output Type
        GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
        //
        GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
        // GPIO_A Initialization
        GPIO_Init(GPIOA, &GPIO_InitStruct);
    /* Led Pin_15 Port_D*/
        // Pin
        GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;
        // Mode
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
        // Speed
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
        // Output Type
        GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
        // Pull up, Pull down
        GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
        // GPIO_D Initialization
        GPIO_Init(GPIOD, &GPIO_InitStruct);
}

int main(void)
{
    // Initialization
    IO_Init();

    while(1)
    {
        if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 1)
        {
            GPIO_SetBits(GPIOD, GPIO_Pin_15);
        }
        else
        {
            GPIO_ResetBits(GPIOD, GPIO_Pin_15);
        }
    }
    return 0;

}

























