/*******************************************************************************
 CLOCK PLIB

  Company:
    Microchip Technology Inc.

  File Name:
    plib_clock.c

  Summary:
    CLOCK PLIB Implementation File.

  Description:
    None

*******************************************************************************/

/*******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/

#include "plib_clock.h"
#include "device.h"
#include "interrupts.h"




static void OSCCTRL_Initialize(void)
{
    /**************** OSC16M IniTialization *************/
    OSCCTRL_REGS->OSCCTRL_OSC16MCTRL = OSCCTRL_OSC16MCTRL_FSEL(0x0U) | OSCCTRL_OSC16MCTRL_ENABLE_Msk;
/****************** XOSC Initialization   ********************************/


/* Configure External Oscillator */
    OSCCTRL_REGS->OSCCTRL_XOSCCTRL = OSCCTRL_XOSCCTRL_STARTUP(6U) | OSCCTRL_XOSCCTRL_GAIN(2U) | OSCCTRL_XOSCCTRL_XTALEN_Msk | OSCCTRL_XOSCCTRL_ENABLE_Msk;
    while((OSCCTRL_REGS->OSCCTRL_STATUS & OSCCTRL_STATUS_XOSCRDY_Msk) != OSCCTRL_STATUS_XOSCRDY_Msk)
    {
        /* Waiting for the XOSC Ready state */
    }

}

static void OSC32KCTRL_Initialize(void)
{
    /****************** XOSC32K initialization  ******************************/

    /* Configure 32K External Oscillator */
    OSC32KCTRL_REGS->OSC32KCTRL_XOSC32K = OSC32KCTRL_XOSC32K_STARTUP(0U) | OSC32KCTRL_XOSC32K_ENABLE_Msk | OSC32KCTRL_XOSC32K_EN1K_Msk | OSC32KCTRL_XOSC32K_EN32K_Msk | OSC32KCTRL_XOSC32K_XTALEN_Msk;


    while(!((OSC32KCTRL_REGS->OSC32KCTRL_STATUS & OSC32KCTRL_STATUS_XOSC32KRDY_Msk) == OSC32KCTRL_STATUS_XOSC32KRDY_Msk))
    {
        /* Waiting for the XOSC32K Ready state */
    }
    OSC32KCTRL_REGS->OSC32KCTRL_RTCCTRL = OSC32KCTRL_RTCCTRL_RTCSEL(4U);
    OSC32KCTRL_REGS->OSC32KCTRL_SLCDCTRL = 0U;
}

static void DFLL_Initialize(void)
{
    /****************** DFLL Initialization  *********************************/
    OSCCTRL_REGS->OSCCTRL_DFLLCTRL = 0U ;

    while((OSCCTRL_REGS->OSCCTRL_STATUS & OSCCTRL_STATUS_DFLLRDY_Msk) != OSCCTRL_STATUS_DFLLRDY_Msk)
    {
        /* Waiting for the Ready state */
    }

    /*Load Calibration Value*/
    uint8_t calibCoarse = (uint8_t)(((*(uint32_t*)0x806020U) >> 26U ) & 0x3fU);

    OSCCTRL_REGS->OSCCTRL_DFLLVAL = OSCCTRL_DFLLVAL_COARSE((uint32_t)calibCoarse) | OSCCTRL_DFLLVAL_FINE(512U);

    GCLK_REGS->GCLK_PCHCTRL[0] = GCLK_PCHCTRL_GEN(0x1U)  | GCLK_PCHCTRL_CHEN_Msk;

    while ((GCLK_REGS->GCLK_PCHCTRL[0] & GCLK_PCHCTRL_CHEN_Msk) != GCLK_PCHCTRL_CHEN_Msk)
    {
        /* Wait for synchronization */
    }
    while((OSCCTRL_REGS->OSCCTRL_STATUS & OSCCTRL_STATUS_DFLLRDY_Msk) != OSCCTRL_STATUS_DFLLRDY_Msk)
    {
        /* Waiting for the Ready state */
    }
    OSCCTRL_REGS->OSCCTRL_DFLLMUL = OSCCTRL_DFLLMUL_MUL(24U) | OSCCTRL_DFLLMUL_FSTEP(1U) | OSCCTRL_DFLLMUL_CSTEP(1U);

    while((OSCCTRL_REGS->OSCCTRL_STATUS & OSCCTRL_STATUS_DFLLRDY_Msk) != OSCCTRL_STATUS_DFLLRDY_Msk)
    {
        /* Waiting for the Ready state */
    }

    /* Configure DFLL    */
    OSCCTRL_REGS->OSCCTRL_DFLLCTRL = OSCCTRL_DFLLCTRL_ENABLE_Msk | OSCCTRL_DFLLCTRL_MODE_Msk ;

    while((OSCCTRL_REGS->OSCCTRL_STATUS & OSCCTRL_STATUS_DFLLLCKF_Msk) != OSCCTRL_STATUS_DFLLLCKF_Msk)
    {
        /* Waiting for DFLL to fully lock to meet clock accuracy */
    }
}

static void FDPLL_Initialize(void)
{
    GCLK_REGS->GCLK_PCHCTRL[1] = GCLK_PCHCTRL_GEN(0x1U)  | GCLK_PCHCTRL_CHEN_Msk;
    while ((GCLK_REGS->GCLK_PCHCTRL[1] & GCLK_PCHCTRL_CHEN_Msk) != GCLK_PCHCTRL_CHEN_Msk)
    {
        /* Wait for synchronization */
    }

    /****************** DPLL Initialization  *********************************/

    /* Configure DPLL    */
    OSCCTRL_REGS->OSCCTRL_DPLLCTRLB = OSCCTRL_DPLLCTRLB_FILTER(0U) | OSCCTRL_DPLLCTRLB_LTIME(0U)| OSCCTRL_DPLLCTRLB_REFCLK(2U) ;


    OSCCTRL_REGS->OSCCTRL_DPLLRATIO = OSCCTRL_DPLLRATIO_LDRFRAC(7U) | OSCCTRL_DPLLRATIO_LDR(38U);

    while((OSCCTRL_REGS->OSCCTRL_DPLLSYNCBUSY & OSCCTRL_DPLLSYNCBUSY_DPLLRATIO_Msk) == OSCCTRL_DPLLSYNCBUSY_DPLLRATIO_Msk)
    {
        /* Waiting for the synchronization */
    }

    /* Selection of the DPLL Enable */
    OSCCTRL_REGS->OSCCTRL_DPLLCTRLA = OSCCTRL_DPLLCTRLA_ENABLE_Msk   ;

    while((OSCCTRL_REGS->OSCCTRL_DPLLSYNCBUSY & OSCCTRL_DPLLSYNCBUSY_ENABLE_Msk) == OSCCTRL_DPLLSYNCBUSY_ENABLE_Msk )
    {
        /* Waiting for the DPLL enable synchronization */
    }

    while((OSCCTRL_REGS->OSCCTRL_DPLLSTATUS & (OSCCTRL_DPLLSTATUS_LOCK_Msk | OSCCTRL_DPLLSTATUS_CLKRDY_Msk)) !=
                (OSCCTRL_DPLLSTATUS_LOCK_Msk | OSCCTRL_DPLLSTATUS_CLKRDY_Msk))
    {
        /* Waiting for the Ready state */
    }
}


static void GCLK0_Initialize(void)
{

    GCLK_REGS->GCLK_GENCTRL[0] = GCLK_GENCTRL_DIV(1U) | GCLK_GENCTRL_SRC(6U) | GCLK_GENCTRL_GENEN_Msk;

    while((GCLK_REGS->GCLK_SYNCBUSY & GCLK_SYNCBUSY_GENCTRL0_Msk) == GCLK_SYNCBUSY_GENCTRL0_Msk)
    {
        /* wait for the Generator 0 synchronization */
    }
}


static void GCLK1_Initialize(void)
{
    GCLK_REGS->GCLK_GENCTRL[1] = GCLK_GENCTRL_DIV(6U) | GCLK_GENCTRL_SRC(0U) | GCLK_GENCTRL_GENEN_Msk;

    while((GCLK_REGS->GCLK_SYNCBUSY & GCLK_SYNCBUSY_GENCTRL1_Msk) == GCLK_SYNCBUSY_GENCTRL1_Msk)
    {
        /* wait for the Generator 1 synchronization */
    }
}


static void GCLK2_Initialize(void)
{
    GCLK_REGS->GCLK_GENCTRL[2] = GCLK_GENCTRL_DIV(47U) | GCLK_GENCTRL_SRC(6U) | GCLK_GENCTRL_GENEN_Msk;

    while((GCLK_REGS->GCLK_SYNCBUSY & GCLK_SYNCBUSY_GENCTRL2_Msk) == GCLK_SYNCBUSY_GENCTRL2_Msk)
    {
        /* wait for the Generator 2 synchronization */
    }
}

void CLOCK_Initialize (void)
{
    /* Function to Initialize the Oscillators */
    OSCCTRL_Initialize();

    /* Function to Initialize the 32KHz Oscillators */
    OSC32KCTRL_Initialize();

    /*Initialize Backup Divider*/
    MCLK_REGS->MCLK_BUPDIV = MCLK_BUPDIV_BUPDIV(0x04U);

    GCLK1_Initialize();
    DFLL_Initialize();
    FDPLL_Initialize();
    GCLK0_Initialize();
    GCLK2_Initialize();


    /* Selection of the Generator and write Lock for OSCCTRL_FDPLL */
    GCLK_REGS->GCLK_PCHCTRL[1] = GCLK_PCHCTRL_GEN(0x1U)  | GCLK_PCHCTRL_CHEN_Msk;

    while ((GCLK_REGS->GCLK_PCHCTRL[1] & GCLK_PCHCTRL_CHEN_Msk) != GCLK_PCHCTRL_CHEN_Msk)
    {
        /* Wait for synchronization */
    }
    /* Selection of the Generator and write Lock for SERCOM3_CORE */
    GCLK_REGS->GCLK_PCHCTRL[19] = GCLK_PCHCTRL_GEN(0x0U)  | GCLK_PCHCTRL_CHEN_Msk;

    while ((GCLK_REGS->GCLK_PCHCTRL[19] & GCLK_PCHCTRL_CHEN_Msk) != GCLK_PCHCTRL_CHEN_Msk)
    {
        /* Wait for synchronization */
    }
    /* Selection of the Generator and write Lock for TC0 TC1 */
    GCLK_REGS->GCLK_PCHCTRL[23] = GCLK_PCHCTRL_GEN(0x0U)  | GCLK_PCHCTRL_CHEN_Msk;

    while ((GCLK_REGS->GCLK_PCHCTRL[23] & GCLK_PCHCTRL_CHEN_Msk) != GCLK_PCHCTRL_CHEN_Msk)
    {
        /* Wait for synchronization */
    }
    /* Selection of the Generator and write Lock for TC2 TC3 */
    GCLK_REGS->GCLK_PCHCTRL[24] = GCLK_PCHCTRL_GEN(0x2U)  | GCLK_PCHCTRL_CHEN_Msk;

    while ((GCLK_REGS->GCLK_PCHCTRL[24] & GCLK_PCHCTRL_CHEN_Msk) != GCLK_PCHCTRL_CHEN_Msk)
    {
        /* Wait for synchronization */
    }


    /*Disable internal RC oscillator*/
    OSCCTRL_REGS->OSCCTRL_OSC16MCTRL = 0;
}



