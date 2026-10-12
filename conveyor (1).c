#include<LPC21xx.H>
#include"lcd.h"
#include"keypad_init.h"
#include"project_title.h"
#include"uart.h"
#define IN1 (1<<23)
#define IN2 (1<<24)
#define EN1 (1<<25)

unsigned char key_lut[4][4]={{7,8,9,'/'},{4,5,6,'x'},{1,2,3,'-'},{'@',0,'=','+'}};
unsigned int batch=0;
volatile unsigned int count=0;
volatile unsigned int key_flag=0;
unsigned int batch_count=0;
volatile unsigned int motor_flag=0;
volatile unsigned int stop_flag=0;

/* count_isr */
void COUNT_ISR(void) __irq
{
    EXTINT=0x01;
    if(motor_flag==1)
        count++;
    VICVectAddr=0;
}
/* end of count isr */

/* kill_switch_isr */
void kill(void) __irq
{
    EXTINT=0x02;
    stop_flag=1;
    VICVectAddr=0;
}
/* end of kill isr */

int main(void)
{
    /* EINT0 = P0.16; EINT1 = P0.14 */
    PINSEL1=(PINSEL1&~0x03)|(0x01);
    PINSEL0=(PINSEL0&~(3<<28))|(2<<28);

    VICIntSelect=0x00;
    VICVectAddr1=(unsigned long)COUNT_ISR;
    VICVectCntl1=(0x20)|14;
    VICVectAddr0=(unsigned long)kill;
    VICVectCntl0=(0x20)|15;

    EXTMODE=0x03;
    EXTPOLAR=0x00;          /* falling edge; verify sensor/switch polarity */
    EXTINT=0x03;
    VICIntEnable=(1<<14)|(1<<15);

    IODIR0|=IN1|IN2|EN1;
    IOCLR0=IN1|IN2|EN1;

    proj_title();
    uart_config();

    while(1)
    {
        batch=0;
        count=0;
        key_flag=0;
        stop_flag=0;
        batch_count++;

        LCD_COMMAND(0X01);
        LCD_STRING("ENTER BATCH ");
        LCD_INT(batch_count);
        LCD_DATA(':');
        LCD_COMMAND(0XC0);

        while(key_flag==0)
        {
            keypad_init();
            value=key_lut[row_val][col_val];

            if(value=='@')
            {
                motor_flag=0;
                IOCLR0=EN1|IN1|IN2;
                LCD_COMMAND(0X01);
                LCD_STRING("TERMINATED");
                while(1);
            }

            if(value=='=')
            {
                if(batch>0)
                {
                    LCD_COMMAND(0X01);
                    LCD_STRING("BATCH:");
                    LCD_COMMAND(0XC0);
                    LCD_INT(batch);
                    delay_ms(1000);
                    LCD_COMMAND(0X01);
                    LCD_STRING("MOTOR ON");
                    key_flag=1;
                }
                else
                {
                    LCD_COMMAND(0X01);
                    LCD_STRING("ENTER QTY > 0");
                    delay_ms(1000);
                    LCD_COMMAND(0X01);
                    LCD_STRING("ENTER BATCH ");
                    LCD_INT(batch_count);
                    LCD_DATA(':');
                    LCD_COMMAND(0XC0);
                }
            }
            else if(value<10)
            {
                if(batch<=429496729UL/10UL)
                {
                    batch=(batch*10)+value;
                    LCD_DATA(value+48);
                }
                else
                {
                    batch=0;
                    LCD_COMMAND(0X01);
                    LCD_STRING("QTY TOO LARGE");
                    delay_ms(1000);
                    LCD_COMMAND(0X01);
                    LCD_STRING("ENTER BATCH ");
                    LCD_INT(batch_count);
                    LCD_DATA(':');
                    LCD_COMMAND(0XC0);
                }
            }
            else
            {
                LCD_COMMAND(0X01);
                LCD_STRING("INVALID KEY");
                delay_ms(700);
                LCD_COMMAND(0X01);
                LCD_STRING("ENTER BATCH ");
                LCD_INT(batch_count);
                LCD_DATA(':');
                LCD_COMMAND(0XC0);
                LCD_INT(batch);
            }
        }

        count=0;
        stop_flag=0;
        IOCLR0=IN1|IN2;
        IOSET0=IN1|EN1;
        motor_flag=1;

        while((count<batch)&&(stop_flag==0))
        {
            /* COUNT_ISR counts products */
        }

        motor_flag=0;
        IOCLR0=EN1|IN1|IN2;

        LCD_COMMAND(0X01);
        if(stop_flag==1)
            LCD_STRING("TERMINATED");
        else
            LCD_STRING("MOTOR OFF");

        delay_ms(1000);

        if(stop_flag==1)
        {
            LCD_COMMAND(0X01);
            LCD_STRING("RESET TO RESTART");
            while(1);
        }

        LCD_COMMAND(0X01);
        LCD_STRING("sending sms...");
        uart_str("AT\r\n");
        delay_ms(1000);
        uart_str("AT+CMGF=1\r\n");
        delay_ms(1000);
        uart_str("AT+CMGS=\"+919876543210\"\r\n"); /* replace destination number */
        delay_ms(1000);
        uart_str("Batch ");
        uart_int(batch_count);
        uart_str(" completed.\r\nQuantity: ");
        uart_int(count);
        uart_str("\r\n");
        uart_tx(26);
        delay_ms(3000);
    }
}
