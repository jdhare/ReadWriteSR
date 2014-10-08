/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#endif

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

#include "user.h"

/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/

/* <Initialize variables in user.h and insert code for user algorithms.> */

void InitApp(void)
{
    TRISBbits.TRISB6=0;//LED
    TRISAbits.TRISA5=0;//OUTCLK
    TRISCbits.TRISC4=0;//SRCLK
    TRISCbits.TRISC5=0;//OUT
    TRISBbits.TRISB7=0;//INCLK
    //set B5 as input
    TRISBbits.TRISB5=1;//IN
    //turn off analog for pin B5 - other pins can't be analog anyway
    ANSELBbits.ANSB=0;

    // Set Clocks low to start

    // Register Clock (SC)
    PORTAbits.RA5=0;
    // Output Serial Clock (RCK)
    PORTCbits.RC4=0;
    // Input Serial Clock (RCK)
    PORTBbits.RB7=0;

}

