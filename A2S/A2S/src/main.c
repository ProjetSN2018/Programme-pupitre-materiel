/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include "appli.h"
#include <stdio.h>
#include "menu.h"

char buf[256];

void Sablier(uint32_t sc,...);
#define SABLIER_NEW			100



int main (void)
{
	//osc_enable(OSC_MAINCK_XTAL);
	/* Insert system clock initialization code here (sysclk_init()). */
	cpu_irq_disable();
	//rstc_enable_user_reset(RSTC);
	wdt_disable(WDT);
	sysclk_init();
	board_init();
	delay_init(sysclk_get_cpu_hz());

	/* Insert application code here, after the board has been initialized. */
	KernelNew();
	cpu_irq_enable();
	PushTask(Appli,APPLI_NEW,0,0);
	KernelRun();
}




void Sablier(uint32_t sc,...)
{
#define k pa1
	switch(sc)
	{
	case SABLIER_NEW:
		Putstr("\r\n");
	default:
		sprintf(buf,"\r%08X",(unsigned int)k);
		Putstr(buf);
		PushTask(Sablier,0,++k,500);
		break;
	}
#undef k	
}
