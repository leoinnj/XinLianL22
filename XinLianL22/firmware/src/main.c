/*******************************************************************************
  Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This file contains the "main" function for a project.

  Description:
    This file contains the "main" function for a project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state
    machines of all modules in the system
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "config\default\definitions.h"                // SYS function prototypes


// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************
volatile uint32_t t2Cnt[32];
volatile uint32_t t2a = 0;

void Timer0CallBK(void)
{
   
    t2Cnt[t2a++] = TC2_Timer32bitCounterGet();
    t2a %= 32;
    LED0_Toggle();
}

void delayTime(uint32_t dly)
{
    volatile uint32_t i;
    
    while(dly--)
    {
        i = 10000;
        while(i--);
    }
}

volatile struct tm curTime;
uint8_t pbuf[64];

int main ( void )
{
    /* Initialize all modules */
    SYS_Initialize ( NULL );
    
    if(RTC_SET_Get() == 0)
    {
        curTime.tm_year = 20;
        curTime.tm_mon = 4;
        curTime.tm_mday = 4;
        curTime.tm_wday = 2;
        curTime.tm_zone = 8;
        curTime.tm_hour = 23;
        curTime.tm_min = 59;
        curTime.tm_sec = 50;
        
        RTC_Initialize();
        RTC_RTCCTimeSet (&curTime);
    }
    
    TC0_TimerCallbackRegister(  (TC_TIMER_CALLBACK) Timer0CallBK,0 );
    TC0_TimerStart();
    TC2_TimerStart();
    while ( true )
    {
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );
        delayTime(200);
        RTC_RTCCTimeGet(&curTime); 
        sprintf(pbuf,"%04d:%02d:%02d--- ",curTime.tm_year,curTime.tm_mon,curTime.tm_mday);
        SERCOM3_USART_Write(pbuf,strlen(pbuf));
        delayTime(200);
        sprintf(pbuf,"%02d:%02d:%02d \n\r",curTime.tm_hour,curTime.tm_min,curTime.tm_sec);
        SERCOM3_USART_Write(pbuf,strlen(pbuf));
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

