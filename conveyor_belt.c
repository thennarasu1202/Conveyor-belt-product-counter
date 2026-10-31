
#include <LPC21xx.H>
#include "lcd.h"
#include"KEYPAD.H"
#include "project_title.h"
#include "uart.h"

#define IN1 (1<<12)
#define IN2 (1<<13)


unsigned int batch = 0;
volatile unsigned int count = 0;
unsigned int key_flag = 0;
unsigned int batch_count = 0;
volatile unsigned int terminate = 0;
unsigned char value=0;
/* Kill switch: EINT1 */
void kill(void) __irq
{
 	EXTINT = 0x02;
    IOCLR0 = IN1 | IN2;
    terminate = 1;
    VICVectAddr = 0;
}

int main(void)
{	
  	PINSEL0 |=  (1<<29);
    PINSEL0 &= ~(1<<28);

    VICIntSelect = 0x00000000;
    VICVectAddr0 = (unsigned long)kill;
    VICVectCntl0 = 0x20 | 15;

    EXTMODE = 0x01;
    EXTPOLAR =0x00;
    VICIntEnable = (1<<15);

   proj_title();
   uart_config();

    while(1)
    {
        batch_count++;
        batch = 0;
        count = 0;
        key_flag = 0;

        LCD_COMMAND(0x01);
        LCD_STRING("ENTER BATCH ");
        LCD_DATA(batch_count + '0');
        LCD_DATA(':');
        LCD_COMMAND(0xC0);

        while(key_flag == 0)
        {
            value =  keyscan();
            if(value == '@')
            {
                IOCLR0 = IN1 | IN2;
                LCD_COMMAND(0x01);
                LCD_STRING("TERMINATED");
                delay_ms(1000);
                return 0;
            }

            if(value != '=')
            {
                if(value < 10)
                {
                    LCD_DATA(value + '0');
                    batch = (batch * 10) + value;
                }
                else
                {
                    LCD_COMMAND(0x01);
                    LCD_STRING("INVALID KEY");
                    delay_ms(1000);

                    LCD_COMMAND(0x01);
                    LCD_STRING("ENTER BATCH ");
                    LCD_DATA(batch_count + '0');
                    LCD_DATA(':');
                    LCD_COMMAND(0xC0);

                    batch = 0;
                }
              
			}
            if(value == '=')
            {
                if(batch == 0)
                {
                    LCD_COMMAND(0x01);
                    LCD_STRING("INVALID BATCH");
                    delay_ms(1000);
                    LCD_COMMAND(0x01);
                    LCD_STRING("ENTER BATCH ");
                    LCD_DATA(batch_count + '0');
                    LCD_DATA(':');
                    LCD_COMMAND(0xC0);
                    batch = 0;
                    continue;
                }

                LCD_COMMAND(0x01);
                LCD_STRING("BATCH:");
                LCD_COMMAND(0xC0);
                LCD_INT(batch);
                delay_ms(1500);

                LCD_COMMAND(0x01);
                LCD_STRING("MOTOR ON");

                key_flag = 1;
            }
        }

        
        IODIR0 |= IN1 | IN2 ;

       
        IOSET0 = IN1 ;
        IOCLR0 = IN2;

       while((count < batch) && (terminate == 0))
{
  
    while((IOPIN0 & (1<< 16)) != 0)
    {
        if(terminate != 0)
            break;
    }

    if(terminate != 0)
        break;

    count++;
    while((IOPIN0 & (1 << 16)) == 0)
    {
		delay_ms(300);
        if(terminate != 0)
            break;
    }

    LCD_COMMAND(0xC0);
    LCD_STRING("COUNT:");
    LCD_INT(count);
}

        /* Stop motor */
        IOCLR0 = IN1 | IN2;

        LCD_COMMAND(0x01);

        if(terminate != 0)
            LCD_STRING("BATCH STOPPED");
        else
            LCD_STRING("BATCH COMPLETE");

        delay_ms(1000);

        /* Send SMS */
        LCD_COMMAND(0x01);
        LCD_STRING("SENDING SMS...");

        uart_str("AT\r\n");
        delay_ms(1000);

        uart_str("AT+CMGF=1\r\n");
        delay_ms(1000);

        uart_str("AT+CMGS=\"+916374775667\"\r\n");
        delay_ms(1000);

        uart_str("Batch ");
        uart_int(batch_count);

        if(terminate != 0)
            uart_str(" stopped.\r\nQuantity: ");
        else
            uart_str(" completed.\r\nQuantity: ");

        uart_int(count);
        uart_str("\r\n");

        uart_tx(26);
        delay_ms(1000);	

        if(terminate != 0)
        {
            LCD_COMMAND(0x01);
            LCD_STRING("RESET REQUIRED");

            /* Remain stopped after emergency termination */
            while(1)
            {
                IOCLR0 = IN1 | IN2;
            }
        }

        key_flag = 0;
        count = 0;
        batch = 0;
    }
}
