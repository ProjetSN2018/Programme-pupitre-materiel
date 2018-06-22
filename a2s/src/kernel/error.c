/*
 * error.c
 *
 * Created: 10/03/2018 07:26:19
 *  Author: Thierry
 */ 


#include "kernel.h"



extern char buf[];
void Error(uint32_t sc, ...)
{
	uint32_t i,k;
#define er sc
#define exEr	pa1
	sprintf(buf, "\r\n\t*****ERROR:%d\tSC:%d*****\r\n", (int)er, (int)exEr);
	Putstr(buf);
	delay_ms(1000);
	cpu_irq_disable();
	gpio_configure_pin(ERROR_LED, ERROR_LED_FLAGS);
	//gpio_configure_pin(HEART_LED, HEART_LED_FLAGS);
	
//#if HEART_LED_ACTIVE_LEVEL==1	
	
	for(k=0;k<5;k++)
	{
		gpio_set_pin_high(ERROR_LED);
		delay_ms(3000);
		gpio_set_pin_low(ERROR_LED);
		delay_ms(800);

		if((er/10)==0)
		{
			gpio_set_pin_high(ERROR_LED);
			delay_ms(1500);
			gpio_set_pin_low(ERROR_LED);
		}
		else
		{
			for(i=0;i<(er/10)*2;i++)
			{
				gpio_toggle_pin(ERROR_LED);
				delay_ms(200);
			}
		}
		delay_ms(800);

		if((er%10)==0)
		{
			gpio_set_pin_high(ERROR_LED);
			delay_ms(1500);
			gpio_set_pin_low(ERROR_LED);
		}
		else
		{
			for(i=0;i<(er%10)*2;i++)
			{
				gpio_toggle_pin(ERROR_LED);
				delay_ms(200);
			}
		}
		delay_ms(1000);
#undef er
#define _param	pa1
		if((_param/100)==0)
		{
			gpio_set_pin_high(ERROR_LED);
			delay_ms(1500);
			gpio_set_pin_low(ERROR_LED);
		}
		else
		{
			for(i=0;i<(_param/100)*2;i++)
			{
				gpio_toggle_pin(ERROR_LED);
				delay_ms(200);
			}
		}
		delay_ms(800);

		if(((_param/10)%10)==0)
		{
			gpio_set_pin_high(ERROR_LED);
			delay_ms(1500);
			gpio_set_pin_low(ERROR_LED);
		}
		else
		{
			for(i=0;i<((_param/10)%10)*2;i++)
			{
				gpio_toggle_pin(ERROR_LED);
				delay_ms(200);
			}
		}
		delay_ms(800);


		if((_param%10)==0)
		{
			gpio_set_pin_high(ERROR_LED);
			delay_ms(1500);
			gpio_set_pin_low(ERROR_LED);
		}
		else
		{
			for(i=0;i<(_param%10)*2;i++)
			{
				gpio_toggle_pin(ERROR_LED);
				delay_ms(200);
			}
		}
		
		delay_ms(1000);

#undef _param
	}
	rstc_start_software_reset(RSTC);
}