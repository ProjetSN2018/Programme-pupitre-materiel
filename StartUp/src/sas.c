/*
* Created: 19/03/2018 15:06:35
*  Author: eleve
*/

#include "./kernel/kernel.h"
#include <string.h>

struct {
	uint32_t mode;
}sas;

uint32_t Sas(uint32_t sc, ...)
{
	switch(sc)
	{
		case SAS_FERMER_SAS:
		gpio_toggle_pin(POWER_LED);
		sprintf(buf, "Fermeture sas");
		Putstr(buf);
		LcdPutstr(buf, 3, LcdFindCenter(strlen(buf)));
		break;
		case SAS_FERMER_PORTE:
		sprintf(buf, "Fermeture Porte 1");
		Putstr(buf);
		break;
		default:
		Error(ERROR_SAS_SWITCH_BAD_SC, sc);
		break;
	}
	return 0;
}