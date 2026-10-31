#define C0 (IOPIN1 & (1<<23))	//col1
#define C1 (IOPIN1 & (1<<24))	//col2
#define C2 (IOPIN1 & (1<<25))	//col3
#define C3 (IOPIN1 & (1<<26))	//col4

#define r0 (1<<27)		//row1
#define r1 (1<<28)		//row2
#define r2 (1<<29)		//row3
#define r3 (1<<30)		//row4

char key_lut[4][4] = {
    {7, 8, 9, '/'},
    {4, 5, 6, 'x'},
    {1, 2, 3, '-'},
    {'@', 0, '=', '+'}
};
char keyscan(void)
{
unsigned char row_val, col_val;
IODIR1 = r0 | r1 | r2 | r3;
while(1)
{
IOCLR1 = r0 | r1 | r2 | r3;
while((C0 && C1 && C2 && C3) == 1);
IOCLR1 = r0;
IOSET1 = r1 | r2 | r3;
if((C0 && C1 && C2 && C3) == 0)
{
   row_val = 0;
   break;
}
IOCLR1 = r1;
IOSET1 = r0 | r2 | r3;
if((C0 && C1 && C2 && C3) == 0)
{
    row_val = 1;
    break;
}
IOCLR1 = r2;
IOSET1 = r0 | r1 | r3;
if((C0 && C1 && C2 && C3) == 0)
{
   row_val = 2;
   break;
}

IOCLR1 = r3;
IOSET1 = r0 | r1 | r2;
if((C0 && C1 && C2 && C3) == 0)
{
    row_val = 3;
    break;
}
}
if(C0 == 0)
  col_val = 0;
else if(C1 == 0)
   col_val = 1;
else if(C2 == 0)
    col_val = 2;
else
    col_val = 3;
delay_ms(150);
while((C0 && C1 && C2 && C3) == 0);
return key_lut[row_val][col_val];
}
