/******************************************************************************/
/*Files to Include                                                            */
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#endif

#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */

#include "system.h"

/* Refer to the device datasheet for information about available
oscillator configurations. */
void ConfigureOscillator(void)
{

    OSCCONbits.IRCF=0b1111; //16 MHz
    OSCCONbits.SCS=0b11;//internal

}

/**
 * Wait for a given number of milli-seconds using busy waiting scheme.
 * @param time - time in ms to wait.
 */
void wait_ms(uint16_t time)
{
    static long timel = 0;
    timel = time * 1000l; //16MHz is 4000 lines per ms. for loops takes 4 lines, so 1000 lines per ms
    for( ; timel; timel--){
        CLRWDT();
    };// no initial condition, while time is >0, decrement time each loop
}
