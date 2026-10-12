#ifndef UART_H
#define UART_H
#include<LPC21xx.H>

void uart_config(void)
{
    PINSEL0=(PINSEL0&~0x0FUL)|0x05UL; /* P0.0 TXD0, P0.1 RXD0 */
    U0LCR=0X83;
    U0DLL=97;       /* 9600 baud if UART PCLK is about 15 MHz */
    U0DLM=0X00;
    U0LCR=0X03;
}

void uart_tx(unsigned char tx)
{
    while(((U0LSR>>5)&1)==0)
    {
    }
    U0THR=tx;
}

void uart_str(const char *s)
{
    while(*s)
        uart_tx(*s++);
}

void uart_int(unsigned int n)
{
    unsigned char buf[10];
    int i=0;

    if(n==0)
    {
        uart_tx('0');
        return;
    }

    while(n>0 && i<10)
    {
        buf[i++]=(n%10)+'0';
        n/=10;
    }

    while(i>0)
        uart_tx(buf[--i]);
}
#endif
