/*
 * appli.c
 *
 * Created: 14/03/2018 08:20:14
 *  Author: Thierry
 */

#include "appli.h"





uint32_t Appli(uint32_t sc, ...)
{
	switch(sc)
	{
	case APPLI_NEW:
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


		gpio_configure_pin(SW1_PUSHBUTTON,		SWS_PUSHBUTTON_FLAGS);
		gpio_configure_pin(SW2_PUSHBUTTON,		SWS_PUSHBUTTON_FLAGS);
		gpio_configure_pin(SW3_PUSHBUTTON,		SWS_PUSHBUTTON_FLAGS);

		//Putstr(WELCOME_MSG);

		LcdPutstr("www.a-2-s.net",1,3);
		LcdPutstr("Naviguer Select Back",3,0);

		Menu(MENU_NEW);
		ComRS485(COMRS485_NEW);
		Module(MODULE_NEW);
		ModbusNew();
		
		break;

	case APPLI_RS485:
//#ifdef MASTER
//#define _ch	pa1
		////Putstr("  RS485 Putch("); Putch(_ch); Putch(')');
		//rs485_Putch(_ch);
		//LcdPutch(_ch,3,0);
		//_ch++; if(_ch>'z') _ch='A';
		//PushTask(Appli,APPLI_RS485,_ch,1000);
//#undef _ch
//#else
//#define _ch	sc
		//if((_ch=rs485_Getch())!=-1) LcdPutch(_ch,3,0);
		//PushTask(Appli,APPLI_RS485,0,500);
//#undef _ch
//#endif
		break;


	default:
		Error(ERROR_APPLI_SWITCH_BAD_SC,sc);
	}
	return 0;
}


