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

 /* ---------------------------------------------------------------------------*/
 /*----------------------------------------------------------------------------*/
 /*! Assignment 5: Increase/Decrease brightness of LED with 2 Buttons */
 /*----------------------------------------------------------------------------*/
 /*----------------------------------------------------------------------------*/

#include "MCIMX6X_M4.h"
#include "ccm_imx6sx.h"
#include "ccm_analog_imx6sx.h"

uint32_t brightness = 2000;

/* Configure GPIO BUTTON 1 */
void Init_GPIO_BUTTON_1(void)
{
    /* GPIO4_IO08 */

    /* Configure GPIO IOMUX*/
	IOMUXC_SW_MUX_CTL_PAD_NAND_DATA04 = IOMUXC_SW_MUX_CTL_PAD_NAND_DATA04_MUX_MODE(5);
	IOMUXC_SW_PAD_CTL_PAD_NAND_DATA04 = IOMUXC_SW_PAD_CTL_PAD_NAND_DATA04_DSE(6)|
                                        IOMUXC_SW_PAD_CTL_PAD_NAND_DATA04_SPEED(2)|
                                        IOMUXC_SW_PAD_CTL_PAD_NAND_DATA04_PKE_MASK;

    /* Configure GPIO pin direction as input */
	GPIO4->GDIR &= ~(1UL << 8);

	/* Configure GPIO interrupts */
	GPIO4->IMR |= (1UL << 8);
	GPIO4->ICR1 &= ~(1UL << 16);
    GPIO4->ICR1 |= (1UL << 17);
}

/* Configure GPIO BUTTON 2 */
void Init_GPIO_BUTTON_2(void)
{
    /* GPIO5_IO14 */

    /* Configure GPIO IOMUX*/
	IOMUXC_SW_MUX_CTL_PAD_RGMII2_RD2 = IOMUXC_SW_MUX_CTL_PAD_RGMII2_RD2_MUX_MODE(5);
    IOMUXC_SW_PAD_CTL_PAD_RGMII2_RD2 = IOMUXC_SW_PAD_CTL_PAD_RGMII2_RD2_DSE(6)|
                                       IOMUXC_SW_PAD_CTL_PAD_RGMII2_RD2_SPEED(2)|
                                       IOMUXC_SW_PAD_CTL_PAD_RGMII2_RD2_PKE_MASK;

    /* Configure GPIO pin direction as input */
	GPIO5->GDIR &= ~(1UL << 14);

	/* Configure GPIO interrupts */
	GPIO5->IMR |= (1UL << 14);
	GPIO5->ICR1 &= ~(1UL << 28);
    GPIO5->ICR1 |= (1UL << 29);
}

/* Configure PWM Clock */
void Init_PWM_CLOCK(void)
{
    /* Power up PLL and enable PLL clock output 528M. */
    CCM_ANALOG_EnablePllClock(CCM_ANALOG, ccmAnalogPllSysClock);
    CCM_ANALOG_PowerUpPll(CCM_ANALOG, ccmAnalogPllSysControl);

    /* PLL is not bypass */
    CCM_ANALOG_SetPllBypass(CCM_ANALOG, ccmAnalogPllSysControl, false);

    /* IPG root clock configuration */
    CCM_SetRootMux(CCM, ccmRootPrePeriphClkSel, ccmRootmuxPrePeriphClkPll2);
    CCM_SetRootMux(CCM,  ccmRootPeriphClkSel, ccmRootmuxPeriphClkPrePeriphClkSel);

    CCM_SetRootDivider(CCM, ccmRootAhbPodf, 3);
    CCM_SetRootDivider(CCM, ccmRootIpgPodf, 1);
}

/* Configure PWM */
void Init_PWM(void)
{
    /* PWM1 */

    /* Configure GPIO IOMUX*/
    IOMUXC_SW_MUX_CTL_PAD_RGMII2_RD3 = IOMUXC_SW_MUX_CTL_PAD_RGMII2_RD3_MUX_MODE(2);
    IOMUXC_SW_PAD_CTL_PAD_RGMII2_RD3 = IOMUXC_SW_PAD_CTL_PAD_RGMII2_RD3_DSE(6)|
                                       IOMUXC_SW_PAD_CTL_PAD_RGMII2_RD3_SPEED(2)|
                                       IOMUXC_SW_PAD_CTL_PAD_RGMII2_RD3_PKE_MASK;
    /* Disable PWM */
	PWM1_PWMCR &= ~(1UL << 0);

	PWM1_PWMCR = PWM_PWMCR_POUTC(0)| PWM_PWMCR_CLKSRC(1)| PWM_PWMCR_PRESCALER(0);

    PWM1_PWMSAR = 2000;
    PWM1_PWMPR = 10000;

    /* Enable PWM */
    PWM1_PWMCR |= (1UL << 0);
}

/* Increase brightness of LED */
void Inc_Brightness(void)
{
    if (brightness <= 9000)
    {
        brightness += 1000;
    }

    PWM1_PWMSAR = brightness;
}

/* Decrease brightness of LED */
void Dec_Brightness(void)
{
    if (brightness >= 1000 )
    {
        brightness -= 1000;
    }

    PWM1_PWMSAR = brightness;
}

/* GPIO4_IO08 Interrupt Handler*/
void GPIO4_Combined_0_15_Handler(void)
{
    Inc_Brightness();
    GPIO4_ISR |= (1UL << 8);
}

/* GPIO5_IO14 Interrupt Handler */
void GPIO5_Combined_0_15_Handler(void)
{
    Dec_Brightness();
    GPIO5_ISR |= (1UL << 14);
}


int main(void)
{
    /* Update system core clock */
    SystemCoreClockUpdate();

    /* Initialize GPIO Button 1 */
    Init_GPIO_BUTTON_1();

    /* Initialize GPIO Button 2 */
    Init_GPIO_BUTTON_2();

    /* Initialize PWM Clock */
    Init_PWM_CLOCK();

    /* Initialize PWM */
    Init_PWM();

    /* Set priority for GPIO4_INT15_0 interrupt */
    NVIC_SetPriority(GPIO4_INT15_0_IRQn, 3);

    /* Enable GPIO4_INT15_0 interrupt */
    NVIC_EnableIRQ(GPIO4_INT15_0_IRQn);

    /* Set priority for GPIO5_INT15_0 interrupt */
    NVIC_SetPriority(GPIO5_INT15_0_IRQn, 3);

    /* Enable GPIO4_INT15_0 interrupt */
    NVIC_EnableIRQ(GPIO5_INT15_0_IRQn);

    while(1);
}
