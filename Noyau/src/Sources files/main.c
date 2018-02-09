/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# "Insert system clock initialization code here" comment
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <asf.h>
#include "..\Headers\appli.h"

void Prompt(uint32_t sc, uint32_t i);

char buf[256];

int main (void)
{
	/* Insert system clock initialization code here (sysclk_init()). */
		
	cpu_irq_disable();
	sysclk_init();
	board_init();
	delay_init(sysclk_get_cpu_hz());
	//////////////////////////////////////////////////////////////////
	
	Taskman(TASKMAN_NEW);
	Shell(SHELL_NEW);
	Putstr("\r\n\n\tATMEL ATSAM3U STARTED\r\n\n");



	//PushTask(Sablier, SABLIER_NEW, 0, 0);
	//PushTask(Prompt, 0, 0, 800);
	
	cpu_irq_enable();



	for(;;)
	{
		PopTask();
	}
}



void Prompt(uint32_t sc, uint32_t i)
{
	sprintf(buf,"\rTimer:%06d",i++);
	Putstr(buf);
	PushTask(Prompt,0,i,2000);
}
