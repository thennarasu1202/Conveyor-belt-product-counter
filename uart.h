void uart_config(void){
	PINSEL0|=0x05;
	U0LCR=0X083;
	U0DLL=97;
	U0DLM=0X00;
	U0LCR=0X03;
}
void uart_tx(unsigned char tx){
		while(((U0LSR>>5)&1)==0);
		U0THR=tx;
	}
void uart_str(unsigned char *s){
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
   while(n>0)
   {
    buf[i++]=(n%10)+'0';
    n/=10;
   }
   while(i>0)
     uart_tx(buf[--i]);
}
