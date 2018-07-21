/*******************************************************************************
 *    EPIT Timer
 ******************************************************************************/
/* Include Library */
#include "epit.h"
#include "board.h"
#include "clock_freq.h"
#include "debug_console_imx.h"

/* This case will run for 4.5 seconds */
static volatile uint32_t num = 10;
static uint32_t counterA;

// EPIT Timer Configure
epit_init_config_t config = {
    .freeRun     = true,
    .waitEnable  = true,
    .stopEnable  = true,
    .dbgEnable   = true,
    .enableMode  = true
};
uint32_t freqA;

int main(void)
{
    hardware_init();

    /* Initialize EPIT module */
    EPIT_Init(BOARD_EPITA_BASEADDR, &config);

    /* Set EPIT clock source */
    EPIT_SetClockSource(BOARD_EPITA_BASEADDR, epitClockSourcePeriph);
    //EPIT_SetClockSource(BOARD_EPITB_BASEADDR, epitClockSourceLowFreq);

    /* Divide EPITA clock source frequency by 2 inside EPIT module */
    EPIT_SetPrescaler(BOARD_EPITA_BASEADDR, 1);

    /* Get EPITA clock frequency */
    freqA = get_epit_clock_freq(BOARD_EPITA_BASEADDR)/2;
    /* Get EPITB clock frequency, 32768Hz */
    //freqB = 32768/2;

    /* Set both EPIT modules to 1 second duration */
    EPIT_SetCounterLoadValue(BOARD_EPITA_BASEADDR, freqA);

    /* The first interrupt occured at 0.5 second */
    EPIT_SetOutputCompareValue(BOARD_EPITA_BASEADDR, freqA/2);

    /* Set EPIT interrupt priority to same value to avoid handler preemption */
    NVIC_SetPriority(BOARD_EPITA_IRQ_NUM, 3);

    /* Enable NVIC interrupt */
    NVIC_EnableIRQ(BOARD_EPITA_IRQ_NUM);

    /* Enable EPIT Output Compare interrupt */
    EPIT_SetIntCmd(BOARD_EPITA_BASEADDR, true);

    PRINTF("EPIT timer will now start\n\r");
    PRINTF("counter/freq ratio should be close to 0.5 ...\n\r");
    /* EPIT start */
    EPIT_Enable(BOARD_EPITA_BASEADDR);

    while (true)
    {
        __WFI();

        while ((num & 1) != 0); /* wait both timer handled */

        PRINTF("\tEPIT A freq %u, counter %u.\n\r", freqA, counterA);

        if (num == 0)
        {
            /* Stop EPIT */
            EPIT_Disable(BOARD_EPITA_BASEADDR);
            PRINTF("EPIT example finished...\n\r");
        }
    }
}

void BOARD_EPITA_HANDLER()
{
    EPIT_ClearStatusFlag(BOARD_EPITA_BASEADDR);

    if (num && --num)
        counterA = EPIT_ReadCounter(BOARD_EPITA_BASEADDR);
}


/*******************************************************************************
 * EOF
 ******************************************************************************/
