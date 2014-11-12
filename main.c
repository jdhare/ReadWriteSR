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
unsigned char read_byte(void);
void write(unsigned char o0,unsigned char o1,unsigned char o2);
void output(unsigned char d0,unsigned char d1,unsigned char d2);

//global variables to be used by the automatic recirculator
unsigned char cur_bank=0b10000000; //0:G, 1:H,2:C,3:Z start with groucho
unsigned int time_recircd=0; 

void main(void)
{

    ConfigureOscillator();
    InitApp();


    //three bytes to hold input data
    unsigned char d0,d1,d2;

    while(1){

        //clock parallel inputs into input shift register
        InCLK();
        //clock inputs in serial into three bytes
        d0=read_byte();
        d1=read_byte();
        d2=read_byte();

        output(d0,d1,d2);
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

void write(unsigned char o0,unsigned char o1,unsigned char o2)
{
    unsigned char mask=0b1;
    unsigned char write_bit;
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

unsigned char read_byte(void)
{
    unsigned char read_bit=1;
    unsigned char input=0;
    int i=0;
    unsigned char mask=0b1;

    for(i=0;i<8;i++){
        //clock data onto pin
        //read digital value on pin
        read_bit=PORTBbits.RB5; 
        //pins held high, so invert
        read_bit=~read_bit;
        //get rid of the rest of the bits
        read_bit=read_bit&mask;
        //add the bit in
        input=input+read_bit;
        //shift it along ready for the next bit
        input=input << 1;
        SRCLK();
    }
    //undo final shift
    input=input >> 1;

    
    return input;
}

/*expected structure of input bytes:
 * d0: GHCZ marx switch fill, GHCZ line switch fill
 * d1: GHCZ marx switch dump, GHCZ line switch dump
 * d2: GHCZ recirculator, recirculator on/off, recirculator auto/manual, final two unused
 * o0: GHCZ marx fill valve, GHCZ line switch fill valve
 * o1: GHCZ marx dump valve, GHCZ line empty valve
 * o2: bottle, dump, recirculator in, recirculator out, final four unused
 * 
 */
void output(unsigned char d0,unsigned char d1,unsigned char d2)
{
    unsigned char o0,o1,o2;
    __delay_us(1);
    bool recirc_on=d2&0b00001000;
    bool recirc_auto=d2&0b00000100;
    //if recirculator is on
    if(recirc_on){
        //if recirculator on manual
        if(recirc_auto==0){
            //open one bank for filling
            o0=d2&0b11110000;
            //and the same bank for emptying
            o1=o0;
            //open recirculator in and out
            o2=0b00110000;
        }
        //explicit check recirculator on automatic
        else if(recirc_auto){
            //open one bank for filling
            o0=cur_bank&0b11110000;
            //and the same bank for emptying
            o1=o0;
            //open recirculator in and out
            o2=0b00110000;
            //run the recirculator for at least a second
            __delay_ms(1000);
            time_recircd++;
            //if current bank has been recirculated for 5 mins, switch to the next bank
            if(time_recircd>10){
                if(cur_bank==0b10000000){
                    cur_bank=0b01000000;
                    time_recircd=0;
                }
                else if(cur_bank==0b01000000){
                    cur_bank=0b00100000;
                    time_recircd=0;
                }
                else if(cur_bank==0b00100000){
                    cur_bank=0b00010000;
                    time_recircd=0;
                }
                else if(cur_bank==0b00010000){
                    cur_bank=0b10000000;
                    time_recircd=0;
                }
            }
        }
    }
    //explicit check that the recirculator is off
    else if(recirc_on==0){
        //open the appropriate valves for filling
        o0=d0;
        //open the appropriate valves for dumping
        o1=d1;
        //if any fill bit set AND any dump bit set
        if(d0!=0&&d1!=0){
            o2=0b11000000;
        }
        //if any dump bit set AND no fill bit set
        if(d0==0&&d1!=0){
            o2=0b01000000;
        }
        //if any fill bit set AND no dump bit set
        if(d0!=0&&d1==0){
            o2=0b10000000;
        }
         //if no fill bit set AND no dump bit set DO NOTHING
        if(d0==0&&d1==0){
            o2=0b00000000;
        }
    }
    

    //write three bytes i
    __delay_us(1);
    write(o0,o1,o2);
    //clock the data held on the output latches to the pins in parallel.
    OutCLK();

    return;
}
