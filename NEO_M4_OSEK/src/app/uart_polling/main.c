/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

///////////////////////////////////////////////////////////////////////////////
//  Includes
///////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "board.h"
#include "gpio_pins.h"
#include "gpio_imx.h"
#include "uart_imx.h"
#include "debug_console_imx.h"
#include "os.h"
#include "message.h"

////////////////////////////////////////////////////////////////////////////////
// Macro definition
////////////////////////////////////////////////////////////////////////////////

#define BUFF_SIZE 32

////////////////////////////////////////////////////////////////////////////////
// Global variables definition
////////////////////////////////////////////////////////////////////////////////
const uint8_t bufferData1[] = "\n\r+++ UART Send/Receive Polling Example +++\n\r";
const uint8_t bufferData2[] = "\n\rType 1 to turn on LED, 0 to turn off LED\n\r";

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

void Init_GPIO_LED(void)
{
    gpio_init_config_t led_d10 = {
        .pin = gpioLed.pin,
        .direction = gpioDigitalOutput,
        .interruptMode = gpioNoIntmode
    };

   GPIO_Init(gpioLed.base, &led_d10);

}

// Setup UART init structure.
uart_init_config_t initConfig = {
    .baudRate   = 115200u,
    .wordLength = uartWordLength8Bits,
    .stopBitNum = uartStopBitNumOne,
    .parity     = uartParityDisable,
    .direction  = uartDirectionTxRx
};
unsigned char rxChar = 0;
uint32_t byteCount = 0;

// UART Sending Data Function

static void UART_SendDataPolling(UART_Type *base, const uint8_t *txBuff, uint32_t txSize)
{
    while (txSize--)
    {
        while (!UART_GetStatusFlag(base, uartStatusTxComplete));
        UART_Putchar(base, *txBuff++);
    }
}

// UART Receiving Data Function
static void UART_ReceiveDataPolling(UART_Type *base, uint8_t *rxBuff, uint32_t rxSize)
{
    while (rxSize--)
    {
        while (!UART_GetStatusFlag(base, uartStatusRxReady));
        *rxBuff = UART_Getchar(base);
        rxBuff++;

        if (UART_GetStatusFlag(base, uartStatusRxOverrun))
            UART_ClearStatusFlag(base, uartStatusRxOverrun);
    }
}

int main(void)
{

   /* Initialize demo application pins setting and clock setting */
   hardware_init();
   SystemInit();

    // Get current module clock frequency.
    initConfig.clockRate  = get_uart_clock_freq(BOARD_DEBUG_UART_BASEADDR);

    /* Initialize UART baud rate, bit count, parity, stop bit and direction. */
    UART_Init(BOARD_DEBUG_UART_BASEADDR, &initConfig);

    /* Set UART build-in hardware FIFO Watermark. */
    UART_SetTxFifoWatermark(BOARD_DEBUG_UART_BASEADDR, 16);
    UART_SetRxFifoWatermark(BOARD_DEBUG_UART_BASEADDR, 1);

    /* Finally, enable the UART module */
    UART_Enable(BOARD_DEBUG_UART_BASEADDR);

   /* Configure LED D10 pad */
   configure_gpio_pin(&gpioLed);

   /* Configure GPIO LED D10 */
   Init_GPIO_LED();

   /* Turn off the LED */
   GPIO_WritePinOutput(gpioLed.base, gpioLed.pin, gpioPinClear);

   /* Start the OSEK */
   debug_printf("Starting OSEK-OS in AppMode1\r\n");

    byteCount = sizeof(bufferData1);
    UART_SendDataPolling(BOARD_DEBUG_UART_BASEADDR, bufferData1, byteCount);

    byteCount = sizeof(bufferData2);
    UART_SendDataPolling(BOARD_DEBUG_UART_BASEADDR, bufferData2, byteCount);

   StartOS(AppMode1);

   /* we shouldn't return here */
   while(1);
}

void ErrorHook(void)
{
	/* kernel panic :( */
    debug_printf("kernel panic\r\n");
	ShutdownOS(0);
}


/*
 * This task waits for an event to be set in order
 * to continue execution.
 */
TASK(TaskUartReceive)
{

	while(1)
	{
        /* Get message */
        UART_ReceiveDataPolling(BOARD_DEBUG_UART_BASEADDR, &rxChar, 1u);
        if(&rxChar != NULL)
        {
            ActivateTask(TaskLed);
        }
	}

	TerminateTask();
}

/*
 * This task waits for an event to be set in order
 * to continue execution.
 */
TASK(TaskLed)
{
    if (rxChar == 48)
		{
			GPIO_WritePinOutput(gpioLed.base, gpioLed.pin, gpioPinClear);
            UART_SendDataPolling(BOARD_DEBUG_UART_BASEADDR, "\n\rLED OFF\n\r", 10u);
		}
		if (rxChar == 49)
		{
			GPIO_WritePinOutput(gpioLed.base, gpioLed.pin, gpioPinSet);
            UART_SendDataPolling(BOARD_DEBUG_UART_BASEADDR, "\n\rLED ON\n\r", 10u);
		}

	TerminateTask();
}


/*
 * Just a idle task with an infinite loop,
 * it has to be defined with the minimum priority!!!
 */
TASK(TaskIdle)
{
	while(1)
	{

	}
}


/*******************************************************************************
 * EOF
 ******************************************************************************/
