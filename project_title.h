void proj_title(void){
	unsigned int i;
LCD_INIT();
LCD_COMMAND(0X80);
LCD_STRING("CONVEYOR BELT SYSTEM");
delay_ms(500);
for(i=0;i<24;i++){
	LCD_COMMAND(0X18);
	delay_ms(500);
}
LCD_COMMAND(0x01);
LCD_STRING("STARTING....");
delay_ms(1000);
LCD_COMMAND(0x01);
}
