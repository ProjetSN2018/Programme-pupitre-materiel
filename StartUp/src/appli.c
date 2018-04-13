/*
 * appli.c
 *
 * Created: 14/03/2018 08:20:14
 *  Author: Thierry
 */ 
#include "appli.h"

#define ST_APPLI_IS_MENU_EDITING		0x0000001

struct {
	uint32_t status;
	uint32_t timer;
}appli;

//PRIVATE APPLICATION SERVICES////////////////////////////////////////
enum{
	_APPLI_TIMER=1,
};

uint32_t Appli(uint32_t sc, ...)
{
	switch(sc)
	{
	case APPLI_NEW:
		appli.status = appli.timer = 0;

		gpio_configure_pin(CMD_MOSFET,			CMD_MOSFET_FLAGS);
		gpio_configure_pin(CMD_MOT_SERRURE,		CMD_MOT_SERRURE_FLAGS);
		gpio_configure_pin(CMD_RELAIS,			CMD_RELAIS_FLAGS);
		gpio_configure_pin(CMD_RELAIS_1,		CMD_RELAIS_1_FLAGS);
		gpio_configure_pin(CMD_RELAIS_2,		CMD_RELAIS_2_FLAGS);
		
		pmc_enable_periph_clk(ID_PIOA);
		pmc_enable_periph_clk(ID_PIOB);
		
		gpio_configure_pin(PENE_OUVERT,			PENE_OUVERT_FLAGS);
		gpio_configure_pin(PENE_FERME,			PENE_FERME_FLAGS);
		gpio_configure_pin(IN_ILS,				IN_ILS_FLAGS);
		gpio_configure_pin(IN_SV,				IN_SV_FLAGS);
		gpio_configure_pin(IN_RADAR,			IN_RADAR_FLAGS);
		gpio_configure_pin(IN_DVU,				IN_DVU_FLAGS);
		gpio_configure_pin(IN_PREMIERE_ENTREE,	IN_PREMIERE_ENTREE_FLAGS);
		gpio_configure_pin(IN_DVU_GENERAL,		IN_DVU_GENERAL_FLAGS);
		gpio_configure_pin(IN_ENTREE_LIBRE,		IN_ENTREE_OPTION_FLAGS);
		gpio_configure_pin(IN_ENTREE_OPTION,	IN_ENTREE_OPTION_FLAGS);
		
		Putstr("\r\n\n\t*______________ CPU.ACX-ATSAM3U4CA-AU ______________*\r\n\n");
		LcdPutstr("CPU.ACX  ATSAM3U4C",2,1);
		LcdPutstr("www.a-2-s.net",3,4);
		break;

	case APPLI_SET_BUTTON_TIMEOUT:
		appli.timer=BUTTON_NO_ACTIVITY_TIMEOUT;	
		break;

	//PRIVATE SERVICES IMPLEMENTATION////////////////////////////////////////////////////
	case _APPLI_TIMER:		//Each ms
		if(appli.timer)
		{
			if(--appli.timer==0)
			{
				//if(mIsBitsSet(appli.status,ST_APPLI_IS_MENU_EDITING))
				{
					mBitsClr(appli.status,ST_APPLI_IS_MENU_EDITING); //We acknowledge the event.
					PushTask(Menu,MENU_RETURN_HOME,0,0);
				}
			}
		}
		break;

	//ERROR CODE TRAP////////////////////////////////////////////////////////////////////
	default:
		Error(ERROR_APPLI_SWITCH_BAD_SC,sc);
	}
	//DEFAULT RETURN VALUE////////////////////////////////////////////////////////////////
	return 0;
}