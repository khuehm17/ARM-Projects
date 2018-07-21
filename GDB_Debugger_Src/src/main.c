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
//#include <stdio.h>
#include "MKL46Z4.h"

#define size_t unsigned int

const uint32_t    led_mask[] = {1UL << 5, 1UL << 29};
volatile uint32_t msTicks; /* counts 1ms timeTicks */

/**
 * @brief         UART0_PinConf
 * @param[in]     void
 * @param[in,out] void
 * @return        void
 */
void UART0_PinConf(void)
{
	/* PTA1 : UART0_RX */
	PORTA->PCR[1] = 0x00000200UL;
	/* PTA2 : UART0_TX */
	PORTA->PCR[2] = 0x00000200UL;
}

/**
 * @brief         UART0_Init
 * @param[in]     void
 * @param[in,out] void
 * @return        void
 */
void UART0_Init(void)
{
	char dummy;
	/* Enable clock gate UART0 */
    SIM->SCGC4 |=   1UL << 10;

	/* Choose MCGPLLCLK/2 (48MHz/2) */
    SIM->SOPT2 |=   1UL << 16;
	SIM->SOPT2 &= ~(1UL << 27);
	SIM->SOPT2 |=   1UL << 26;

    /* Baudrate = clock/(SBR[12:0] x (OSR + 1))   */
	/* OSR = 15, clock = 24MHz, baudrate = 115200 */
	/* -> SBR = 13 */
	/* LBKDIE = 0, RXEDGIE = 0, SBNS = 0 */
	UART0->BDH = 0x00;
	UART0->BDL = 0x0D;

	/* UART0 control */
	UART0->C1 = 0x00;
	UART0->C2 = 0x00;
	UART0->C3 = 0x00;
	UART0->C4 = 0x0F; /* OSR = 15 */
	UART0->C5 = 0x00;

	/* Enable UART0 transmit/receive */
	UART0->C2 |= 0x03 << 2;

	/* Dummy read to ensure UART0
	   receive buffer is empty already */
	dummy = UART0->D;
	dummy = dummy;
}

/**
 * @brief         UART0_PutChar
 * @param[in]     void
 * @param[in,out] void
 * @return        void
 */
void UART0_PutChar(char c)
{
	UART0->D   = c;
    /* Wait for transmition complete */
	while(!(UART0->S1 & (0x01 << 6))) {}
}

/**
 * @brief         UART0_GetChar
 * @param[in]     void
 * @param[in,out] void
 * @return        char
 */
char UART0_GetChar(void)
{
	/* Wait for incomming character */
	while(!(UART0->S1 & (0x01 << 5))) {}
  return UART0->D;
}

int _read (int fd, const void *buf, size_t count)
{
  return 0;
}

int _write (int fd, const void *buf, size_t count)
{
  size_t CharCnt = 0x00;

  (void)fd;                            /* Parameter is not used, suppress unused argument warning */
   for (;count > 0x00; --count)
   {
     UART0_PutChar((unsigned char)*(uint8_t*)buf);
     (uint8_t*)buf++;
     CharCnt++;
   }
  return CharCnt; /* CORRECT */
}

/*----------------------------------------------------------------------------
  SysTick_Handler
 *----------------------------------------------------------------------------*/
void SysTick_Handler(void) {
  msTicks++;                        /* increment counter necessary in Delay() */
}

/*------------------------------------------------------------------------------
  delays number of tick Systicks (happens every 1 ms)
 *------------------------------------------------------------------------------*/
static void Delay (uint32_t dlyTicks) {
  uint32_t curTicks;

  curTicks = msTicks;
  while ((msTicks - curTicks) < dlyTicks);
}

/*------------------------------------------------------------------------------
  Configure LED pins
 *------------------------------------------------------------------------------*/
static void LED_Config(void) {

  SIM->SCGC5    |= (1UL << 12) | (1UL << 13);        /* Enable Clock to Port D and Port E*/
  PORTD->PCR[5]  = (1UL <<  8);                      /* Pin is GPIO */
  PORTE->PCR[29] = (1UL <<  8);                      /* Pin is GPIO */

  PTD->PDOR = led_mask[0];                           /* switch LEDs off  */
  PTE->PDOR = led_mask[1];

  PTD->PDDR = led_mask[0];                           /* enable Output  */
  PTE->PDDR = led_mask[1];
}

/*------------------------------------------------------------------------------
  Switch on LEDs
 *------------------------------------------------------------------------------*/
static void LED_On (uint32_t led) {
  if (led) PTE->PCOR   = led_mask[led];
  else PTD->PCOR       = led_mask[led];
}

/*------------------------------------------------------------------------------
  Switch off LEDs
 *------------------------------------------------------------------------------*/
static void LED_Off (uint32_t led) {
  if (led) PTE->PSOR   = led_mask[led];
  else PTD->PSOR       = led_mask[led];
}

int main(void)
{
  /* Update system clock */
  SystemCoreClockUpdate();
  /* Generate interrupt each 1 ms    */
  SysTick_Config(SystemCoreClock/1000);
  /* Pinmux config */
  UART0_PinConf();
  /* Initialize UART0 */
  UART0_Init();
  /* Configure LED */
  LED_Config();

  //printf("Hello frdm-lk25z\r\n");

  while(1)
  {
    LED_On (0);
	LED_On	(1);
    Delay	(500);
    LED_Off	(0);
    Delay	(500);
	LED_On	(0);
	LED_Off	(1);
	Delay	(500);
	LED_Off	(0);
	Delay (500);
  }
}
