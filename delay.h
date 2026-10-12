#ifndef DELAY_H
#define DELAY_H
#include<LPC21xx.H>

/* Assumes Timer 0 PCLK = 15 MHz */
void delay_ms(unsigned int ms)
{
    T0TCR=0X02;          /* Reset Timer 0 */
    T0PR=15000-1;        /* 1 ms tick at 15 MHz */
    T0MR0=ms;
    T0MCR=0X04;          /* Stop on MR0 match */
    T0TC=0;
    T0PC=0;
    T0IR=0xFF;
    T0TCR=0X01;          /* Start Timer 0 */

    while((T0TCR&1)!=0)
    {
    }

    T0TCR=0X02;
    T0TCR=0X00;
}
#endif
