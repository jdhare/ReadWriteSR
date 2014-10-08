/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#endif

#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */


#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"          /* User funct/params, such as InitApp */

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/

/* i.e. uint8_t <variable_name>; */



/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/
/*
*/



#include <xc.h>
#define _XTAL_FREQ 16000000

void SRCLK(void);
void OutCLK(void);
void InCLK(void);
unsigned int read_byte(void);
void write(unsigned int o0,unsigned int o1,unsigned int o2);

void main(void)
{

    ConfigureOscillator();
    InitApp();


    //three bytes to hold input data
    unsigned int d0,d1,d2;

    while(1){

        //clock parallel inputs into input shift register
        InCLK();
        //clock inputs in serial into three bytes
        d0=read_byte();
        d1=read_byte();
        d2=read_byte();

        PORTBbits.RB6=1;
        __delay_ms(50);
        PORTBbits.RB6=0;



        //write three bytes i
        write(d0,d1,d2);
        OutCLK();

    }


return;
}


void SRCLK(void)
{
    PORTCbits.RC4=1;
    __delay_us(100);
    PORTCbits.RC4=0;
    __delay_us(100);
}

void OutCLK(void)
{
    PORTAbits.RA5=1;
    __delay_us(100);
    PORTAbits.RA5=0;
    __delay_us(100);
}

void InCLK(void)
{
    PORTBbits.RB7=0;
    __delay_us(100);
    SRCLK();
    PORTBbits.RB7=1;
    __delay_us(100);
}

void write(unsigned int o0,unsigned int o1,unsigned int o2)
{
    unsigned int mask=0b1;
    unsigned write_bit;
    int i;

    for(i=0;i<8;i++)
    {
        write_bit=o0 & mask;
        PORTCbits.RC5 = write_bit;
        __delay_us(10);
        SRCLK();
        o0 = o0 >> 1;
    }

    for(i=0;i<8;i++)
    {
        write_bit=o1 & mask;
        PORTCbits.RC5 = write_bit;
        __delay_us(10);
        SRCLK();
        o1 = o1 >> 1;
    }

    for(i=0;i<8;i++)
    {
        write_bit=o2 & mask;
        PORTCbits.RC5 = write_bit;
        __delay_us(10);
        SRCLK();
        o2 = o2 >> 1;
    }

}

unsigned int read_byte(void)
{
    unsigned int read_bit;
    unsigned char input=1;
    int i=0;

    for(i=0;i<8;i++){
        read_bit=PORTBbits.RB5;        
        input=input+read_bit;
        input=input << 1;
        SRCLK();
    }
    //undo final shift
    input=input >> 1;

    input=~input;
    
    return input;
}

