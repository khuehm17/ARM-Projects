///////////////////////////////////////////////////////////////////////////////
//  Includes
///////////////////////////////////////////////////////////////////////////////
#include <stdint.h>
#include <stdbool.h>
#include "uart_imx.h"
#include "board.h"
#include "debug_console_imx.h"
#include "gpio_imx.h"
#include "rdc_semaphore.h"
////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

const uint8_t bufferData1[] = "\n\r+++ UART Send/Receive Polling Example +++\n\r";
const uint8_t bufferData2[] = "\n\rType 1 to turn on LED, 0 to turn off LED\n\r";

// Setup LED Init structure.
gpio_init_config_t ledInit= {
    .pin=6,
    .direction=gpioDigitalOutput,
    .interruptMode=gpioNoIntmode,
};

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
// LED Control Initialization
static void GPIO_Ctrl_InitLedPin()
{
    RDC_SEMAPHORE_Lock(rdcPdapGpio4);
    GPIO_Init(GPIO4,&ledInit);
    RDC_SEMAPHORE_Unlock(rdcPdapGpio4);
}

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

    GPIO_Ctrl_InitLedPin();

    byteCount = sizeof(bufferData1);
    UART_SendDataPolling(BOARD_DEBUG_UART_BASEADDR, bufferData1, byteCount);

    byteCount = sizeof(bufferData2);
    UART_SendDataPolling(BOARD_DEBUG_UART_BASEADDR, bufferData2, byteCount);

    while (true)
    {
        UART_ReceiveDataPolling(BOARD_DEBUG_UART_BASEADDR, &rxChar, 1u);
        //UART_SendDataPolling(BOARD_DEBUG_UART_BASEADDR, "\n\r", 1u);

            if (rxChar == 49)
            {
                RDC_SEMAPHORE_Lock(rdcPdapGpio4);
                GPIO_WritePinOutput(GPIO4,ledInit.pin,gpioPinSet);
                RDC_SEMAPHORE_Unlock(rdcPdapGpio4);
                UART_SendDataPolling(BOARD_DEBUG_UART_BASEADDR, "\n\rLED ON\n\r", 10u);
            }
            if (rxChar == 48)
            {
                RDC_SEMAPHORE_Lock(rdcPdapGpio4);
                GPIO_WritePinOutput(GPIO4,ledInit.pin,gpioPinClear);
                RDC_SEMAPHORE_Unlock(rdcPdapGpio4);
                UART_SendDataPolling(BOARD_DEBUG_UART_BASEADDR, "\n\rLED OFF\n\r", 10u);
            }

    }
}

/*******************************************************************************
 * EOF
 ******************************************************************************/
