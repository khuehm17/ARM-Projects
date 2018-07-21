///////////////////////////////////////////////////////////////////////////////
//  Includes
///////////////////////////////////////////////////////////////////////////////
#include "board.h"
#include "debug_console_imx.h"
#include "gpio_imx.h"
#include "rdc_semaphore.h"
////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////
static void GPIO_Ctrl_InitLedPin();

gpio_init_config_t ledInit= {
    .pin=6,
    .direction=gpioDigitalOutput,
    .interruptMode=gpioNoIntmode
};

static void GPIO_Ctrl_InitLedPin()
{
    RDC_SEMAPHORE_Lock(rdcPdapGpio4);
    GPIO_Init(GPIO4,&ledInit);
    RDC_SEMAPHORE_Unlock(rdcPdapGpio4);
}
volatile uint32_t msTicks;

void SysTick_Handler (void) //Enter here every 1 ms
{
  msTicks++;
}

//-------------------------------
void Delay(uint32_t dlyTicks)
{
  uint32_t curTicks;

  curTicks = msTicks;
  while ((msTicks - curTicks) < dlyTicks);
}
int main(void)
{
   /* Initialize demo application pins setting and clock setting */
   hardware_init();
   SystemInit();
   /* Activate SysTick */
   SysTick_Config(SystemCoreClock/1000);

   /* Update priority set by SysTick_Config */
   NVIC_SetPriority(SysTick_IRQn, (1<<2) - 1);

   /* Enable system tick interrupt */
   NVIC_EnableIRQ(SysTick_IRQn);

   /* Print out message */
   debug_printf("It works\r\n");

   GPIO_Ctrl_InitLedPin();

    // Should never reach this point.
    while (true)
    {
        RDC_SEMAPHORE_Lock(rdcPdapGpio4);
        GPIO_WritePinOutput(GPIO4,ledInit.pin,gpioPinSet);
        Delay(100);
        GPIO_WritePinOutput(GPIO4,ledInit.pin,gpioPinClear);
        Delay(100);
        RDC_SEMAPHORE_Unlock(rdcPdapGpio4);
    }
}



/*******************************************************************************
 * EOF
 ******************************************************************************/
