#ifndef KEYPAD_INIT_H
#define KEYPAD_INIT_H

#define r0 (1<<3)
#define r1 (1<<4)
#define r2 (1<<5)
#define r3 (1<<6)
#define c0 7
#define c1 8
#define c2 9
#define c3 10

unsigned int row_val=0;
unsigned int col_val=0;
unsigned char value=0;

void keypad_init(void)
{
    unsigned int col;

    /* Select GPIO only for keypad pins; preserve UART/peripheral selections */
    PINSEL0 &= ~(0xFFFFUL<<6);

    /* Rows are outputs. Columns remain inputs (their reset direction is input). */
    IODIR0 |= r0|r1|r2|r3;
    IODIR0 &= ~((1<<c0)|(1<<c1)|(1<<c2)|(1<<c3));
    IOSET0 = r0|r1|r2|r3;

    /* Wait for a key press: all columns should be HIGH when idle */
    while((((IOPIN0>>c0)&1) &&
           ((IOPIN0>>c1)&1) &&
           ((IOPIN0>>c2)&1) &&
           ((IOPIN0>>c3)&1)) != 0)
    {
    }

    delay_ms(20);

    /* Scan row 0 */
    IOSET0=r0|r1|r2|r3;
    IOCLR0=r0;
    col=(IOPIN0>>c0)&0x0F;
    if(col!=0x0F)
        row_val=0;
    else
    {
        /* Scan row 1 */
        IOSET0=r0|r1|r2|r3;
        IOCLR0=r1;
        col=(IOPIN0>>c0)&0x0F;
        if(col!=0x0F)
            row_val=1;
        else
        {
            /* Scan row 2 */
            IOSET0=r0|r1|r2|r3;
            IOCLR0=r2;
            col=(IOPIN0>>c0)&0x0F;
            if(col!=0x0F)
                row_val=2;
            else
            {
                /* Scan row 3 */
                IOSET0=r0|r1|r2|r3;
                IOCLR0=r3;
                col=(IOPIN0>>c0)&0x0F;
                row_val=3;
            }
        }
    }

    /* Read column for the selected row */
    col=(IOPIN0>>c0)&0x0F;
    if((col&0x01)==0)
        col_val=0;
    else if((col&0x02)==0)
        col_val=1;
    else if((col&0x04)==0)
        col_val=2;
    else
        col_val=3;

    IOSET0=r0|r1|r2|r3;

    /* Wait for release so one press is read once */
    while((((IOPIN0>>c0)&1) |
           ((IOPIN0>>c1)&1) |
           ((IOPIN0>>c2)&1) |
           ((IOPIN0>>c3)&1)) == 0)
    {
    }
    delay_ms(20);
}

#endif
