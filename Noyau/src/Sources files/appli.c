/*
 * appli.c
 *
 * Created: 12/02/2018 17:23:54
 *  Author: Mod_loc
 */ 

 #include "./Headers/appli.h"

 uint32_t Appli(uint32_t sc, ...)
 {
	switch (sc)
	{
	case APPLI_NEW:
		Lcd(LCD_NEW);
		break;
	default:
		Error(ERR_APPLI_SWITCH_BAD_SC, sc);
	}
 }

