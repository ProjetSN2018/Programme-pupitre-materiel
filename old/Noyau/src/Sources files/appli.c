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
		Putstr("APPLI_NEW\r\n");
		Lcd(LCD_NEW);
		PushTask(Appli,APPLI_TEST,0,3000);
		break;
	case APPLI_TEST:
		LcdPutstr("Hello World 0",0,0);
		LcdPutstr("Hello World 1",1,1);
		LcdPutstr("Hello World 2",2,2);
		LcdPutstr("Hello World 3",3,3);


		Putstr("LCD Hello World!\r\n");
		break;
	default:
		Error(ERR_APPLI_SWITCH_BAD_SC, sc);
	}
	return 0;
 }

