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
const uint8_t buffStart[] = "\n\r+++ UART Send/Receive Interrupt Driven Example +++\n\r";
const uint8_t bufferData[] = "\n\rType 1 to turn on LED, 0 to turn off LED\n\r";

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
uint8_t rxChar, txChar;

typedef struct _uart_state {
    const uint8_t*    txBuff;          /*!< The buffer of data being sent.*/
    uint8_t*          rxBuff;          /*!< The buffer of received data. */
    volatile uint32_t txSize;          /*!< The remaining number of bytes to be transmitted. */
    volatile uint32_t rxSize;          /*!< The remaining number of bytes to be received. */
    volatile bool     isTxBusy;        /*!< True if there is an active transmit. */
    volatile bool     isRxBusy;        /*!< True if there is an active receive. */
} uart_state_t;

/* Uart runtime state structure */
static uart_state_t uartState;

static void UART_XFER_Config(uart_init_config_t* initConfig)
{
    /* Assert */

    /* Initialize UART state structure content. */
    uartState.txBuff = 0;
    uartState.rxBuff = 0;
    uartState.txSize = 0;
    uartState.rxSize = 0;
    uartState.isTxBusy = false;
    uartState.isRxBusy = false;

    /* Initialize UART baud rate, bit count, parity, stop bit and direction. */
    UART_Init(BOARD_DEBUG_UART_BASEADDR, initConfig);

    UART_SetTxFifoWatermark(BOARD_DEBUG_UART_BASEADDR, 16);
    UART_SetRxFifoWatermark(BOARD_DEBUG_UART_BASEADDR, 1);

    /* Set UART Interrupt priority */
    NVIC_SetPriority(BOARD_DEBUG_UART_IRQ_NUM, 3);

    /* Call core API to enable the IRQ. */
    NVIC_EnableIRQ(BOARD_DEBUG_UART_IRQ_NUM);

    /* Finally, enable the UART module */
    UART_Enable(BOARD_DEBUG_UART_BASEADDR);
}

static bool UART_XFER_Write(uint8_t* txBuffer, uint32_t length)
{
    if ((uartState.isTxBusy) || (0 == length))
        return false;

    uartState.txBuff = txBuffer;
    uartState.txSize = length;
    uartState.isTxBusy = true;

    UART_SetIntCmd(BOARD_DEBUG_UART_BASEADDR, uartIntTxReady, true);
    return true;
}

static uint32_t UART_XFER_GetWriteStatus(void)
{
    return uartState.txSize;
}

static bool UART_XFER_Read(uint8_t* rxBuffer, uint32_t length)
{
    if ((uartState.isRxBusy) || (0 == length))
        return false;
    uartState.rxBuff = rxBuffer;
    uartState.rxSize = length;
    uartState.isRxBusy = true;

    UART_SetIntCmd(BOARD_DEBUG_UART_BASEADDR, uartIntRxReady, true);
    return true;
}

static uint32_t UART_XFER_GetReadStatus(void)
{
    return uartState.rxSize;
}

void BOARD_DEBUG_UART_HANDLER(void)
{
    /* Exit the ISR if no transfer is happening for this instance. */
    if ((!uartState.isTxBusy) && (!uartState.isRxBusy))
        return;

    if (UART_GetStatusFlag(BOARD_DEBUG_UART_BASEADDR, uartStatusRxReady))
    {
        /* Check to see if there are any more bytes to receive. */
        if (uartState.rxSize)
        {
            while (UART_GetStatusFlag(BOARD_DEBUG_UART_BASEADDR, uartStatusRxReady))
            {
                *(uartState.rxBuff) = (uint8_t)UART_Getchar(BOARD_DEBUG_UART_BASEADDR);
                uartState.rxBuff++;
                uartState.rxSize--;
                if (uartState.rxSize == 0U)
                {
                    UART_SetIntCmd(BOARD_DEBUG_UART_BASEADDR, uartIntRxReady, false);

                    uartState.isRxBusy = false;
                    break;
                }
            }
        }
    }

    if (UART_GetStatusFlag(BOARD_DEBUG_UART_BASEADDR, uartStatusTxReady))
    {
        /* Check to see if there are any more bytes to send. */
        if (uartState.txSize)
        {
            while (UART_GetStatusFlag(BOARD_DEBUG_UART_BASEADDR, uartStatusTxReady))
            {
                /* Transmit data and update tx size/buff */
                UART_Putchar(BOARD_DEBUG_UART_BASEADDR, *(uartState.txBuff));
                uartState.txBuff++;
                uartState.txSize--;
                if (uartState.txSize == 0U)
                {
                    /* Transmit complete */
                    /* Disable the transmitter ready interrupt */
                    UART_SetIntCmd(BOARD_DEBUG_UART_BASEADDR, uartIntTxReady, false);

                    /* Update the information of the module driver state */
                    uartState.isTxBusy = false;
                    break;
                }
            }
        }
    }
}


int main(void)
{

   /* Initialize demo application pins setting and clock setting */
   hardware_init();
   SystemInit();

    // Get current module clock frequency.
    initConfig.clockRate  = get_uart_clock_freq(BOARD_DEBUG_UART_BASEADDR);

    // Initialize the uart module with initialize structure.
    UART_XFER_Config(&initConfig);

   /* Configure LED D10 pad */
   configure_gpio_pin(&gpioLed);

   /* Configure GPIO LED D10 */
   Init_GPIO_LED();

   /* Turn off the LED */
   GPIO_WritePinOutput(gpioLed.base, gpioLed.pin, gpioPinClear);

   /* Start the OSEK */
   debug_printf("Starting OSEK-OS in AppMode1\r\n");
    // Inform to start non blocking example.
    UART_XFER_Write((uint8_t*)buffStart, sizeof(buffStart));
    // Wait until transmission is finished.
    while (UART_XFER_GetWriteStatus());

    // Inform user of what to do
    UART_XFER_Write((uint8_t*)bufferData, sizeof(bufferData));
    // Wait until transmission is finished.
    while (UART_XFER_GetWriteStatus());

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
        // Call received API
        UART_XFER_Read(&rxChar, 1u);

        // Wait until we receive a character
        while (UART_XFER_GetReadStatus());
        if(rxChar != NULL)
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
            UART_XFER_Write("\n\rLED OFF\n\r", 10u);
		}
		if (rxChar == 49)
		{
			GPIO_WritePinOutput(gpioLed.base, gpioLed.pin, gpioPinSet);
            UART_XFER_Write("\n\rLED ON\n\r", 10u);
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
