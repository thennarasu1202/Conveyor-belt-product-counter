#include<LPC21XX.H>
#include"delay.h"

#define LCD_D 0xff<<17
#define RS 1<<5
#define E 1<<6
void LCD_INIT(void);
void LCD_COMMAND(unsigned char);
void LCD_DATA(unsigned char);
void LCD_INIT(void)
	{
	//PINSEL0=0;
	IODIR0=LCD_D|RS|E;
	LCD_COMMAND(0X01);
	LCD_COMMAND(0X02);
	LCD_COMMAND(0X0C);
	LCD_COMMAND(0X38);
	}
void LCD_COMMAND(unsigned char c)
	{
	IOCLR0=LCD_D;
	IOSET0=c<<17;
	IOCLR0=RS;
	IOSET0=E;
	delay_ms(10);
	IOCLR0=E;
	}
void LCD_DATA(unsigned char d)
	{
	IOCLR0=LCD_D;
	IOSET0=d<<17;
	IOSET0=RS;
	IOSET0=E;
	delay_ms(10);
	IOCLR0=E;
	}
void LCD_STRING(char*str)
{
unsigned int i=0;
while(*str!='\0')
{
LCD_DATA(*str++);
i++;
if(i==16)
LCD_COMMAND(0xc0);
}
}
void LCD_INT(unsigned int val){

	unsigned int arr[50];

	int j=0;

	while(val!=0){

		arr[j++]=val%10;

		val=val/10;

	}

	for(j--;j>=0;j--){

		LCD_DATA(arr[j]+48);

	}

}



