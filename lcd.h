#ifndef LCD_H
#define LCD_H
#include<LPC21XX.H>
#include"delay.h"

#define LCD_D (0xff<<16)
#define RS (1<<24)
#define E (1<<25)

void LCD_DATA(unsigned char);
void LCD_COMMAND(unsigned char);
void LCD_INIT(void);
void LCD_STRING(char *st);
void LCD_INT(unsigned int val);

void LCD_INIT(void)
{
    /* Do not reset PINSEL0 here; it is shared with UART/keypad */
    IODIR1|=LCD_D|RS|E;
    IOCLR1=LCD_D|RS|E;
    delay_ms(20);
    LCD_COMMAND(0X38);
    LCD_COMMAND(0X0C);
    LCD_COMMAND(0X01);
    delay_ms(2);
    LCD_COMMAND(0X06);
}

void LCD_COMMAND(unsigned char cmd)
{
    IOCLR1=LCD_D|RS|E;
    IOSET1=(cmd<<16);
    IOCLR1=RS;
    IOSET1=E;
    delay_ms(2);
    IOCLR1=E;
    delay_ms(2);
}

void LCD_DATA(unsigned char d)
{
    IOCLR1=LCD_D|RS|E;
    IOSET1=(d<<16);
    IOSET1=RS|E;
    delay_ms(2);
    IOCLR1=E;
    delay_ms(1);
}

void LCD_STRING(char *st)
{
    while(*st)
        LCD_DATA(*st++);
}

void LCD_INT(unsigned int val)
{
    unsigned int arr[10];
    int j=0;

    if(val==0)
    {
        LCD_DATA('0');
        return;
    }

    while(val!=0 && j<10)
    {
        arr[j++]=val%10;
        val=val/10;
    }

    for(j--;j>=0;j--)
        LCD_DATA(arr[j]+48);
}
#endif
