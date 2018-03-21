#include <string.h>

#include "./kernel/kernel.h"


///////////////////////////////ON CREE LE MENU!!!//////////////////////////////////////////

 typedef struct tt_menu {
	const char			 *pMenuLabel;
	const struct tt_menu *pParentMenu;
	const struct tt_menu *pSubMenu;
	const t_pFunc		  pMenuFunc;
 }t_menu;

 extern const t_menu _mainMenu[];
 extern const t_menu _porteMenu[];

 uint32_t _menuOptionSasFunc(uint32_t sc, ...);
 uint32_t _menuPorteFermer1(uint32_t sc, ...);
 uint32_t _menuPorteFermer2(uint32_t sc, ...);

  uint32_t _menuEtatPorte1(uint32_t sc, ...);
  uint32_t _menuEtatPorte2(uint32_t sc, ...);
  uint32_t _menuEtatPortes(uint32_t sc, ...);

const t_menu _etatMenu[] = {
	{"Etat porte 1",	_porteMenu, NULL, _menuEtatPorte1 },
	{"Etat porte 2",	_porteMenu, NULL, _menuEtatPorte2 },
	{"Etat portes",		_porteMenu, NULL, _menuEtatPortes },
	///////////////////////////////////////////////////////////
	{ NULL, NULL, NULL	}
};

const t_menu _porteMenu[] = {
	{"Fermer porte 1",	_mainMenu,	NULL,		_menuPorteFermer1 },
	{"Fermer porte 2",	_mainMenu,	NULL,		_menuPorteFermer2 },
	{"Etat portes",		_mainMenu,	_etatMenu,  NULL },
	//////////////////////////////////////////////////////
	{ NULL, NULL, NULL	}
};



const t_menu _mainMenu[]={
	{"Option sas",		NULL,	NULL,		_menuOptionSasFunc },
	{"Option portes",	NULL,	_porteMenu,	NULL },
	{"Option fenetre",	NULL,	_porteMenu,	NULL },
	{"Option trappe",	NULL,	_porteMenu,	NULL },

	//////////////////////////////////////////////////////
	{ NULL, NULL, NULL	}
};

/////////////////////////////////INTERRUPT TEST//////////////
void ButtonSwitch_ISR_Handler(void);	
void ButtonSelect_ISR_Handler(void);
void ButtonClear_ISR_Handler(void);

struct {
	int		iIndexMenu;
	const t_menu	*pCurrentMenu;
}menu;




uint32_t Menu(uint32_t sc, ...)
{
	switch(sc)
	{
		case MENU_NEW:
			Putstr("MENU_NEW\r");

			//Init ButtonSwitch
			pio_set_input(PIOA, PIN_BUTTON_SWITCH, PIO_PULLUP); //A0 (Bouton Gauche)
			pio_handler_set(PIOA, ID_PIOA, PIN_BUTTON_SWITCH, PIO_IT_FALL_EDGE, ButtonSwitch_ISR_Handler);
			pio_enable_interrupt(PIOA, PIN_BUTTON_SWITCH);
			//Init ButtonSelect
			pio_set_input(PIOA, PIN_BUTTON_SELECT, PIO_PULLUP); //A1 (Bouton Milieu)
			pio_handler_set(PIOA, ID_PIOA, PIN_BUTTON_SELECT, PIO_IT_FALL_EDGE, ButtonSelect_ISR_Handler);
			pio_enable_interrupt(PIOA, PIN_BUTTON_SELECT);
			//Init ButtonClear
			pio_set_input(PIOA, PIN_BUTTON_CLEAR, PIO_PULLUP); //A1 (Bouton Droite)
			pio_handler_set(PIOA, ID_PIOA, PIN_BUTTON_CLEAR, PIO_IT_FALL_EDGE, ButtonClear_ISR_Handler);
			pio_enable_interrupt(PIOA, PIN_BUTTON_CLEAR);
		
			NVIC_EnableIRQ(PIOA_IRQn);
			
			
			

			sprintf(buf, "MENU NEW FINISHED \r\n");
			Putstr(buf);
			menu.iIndexMenu = 0;
			menu.pCurrentMenu = _mainMenu;
			break;
		case MENU_SWITCH_BUTTON:			//Quand press bouton Gauche
			menu.iIndexMenu++;
			if(menu.pCurrentMenu[menu.iIndexMenu].pMenuLabel == NULL) menu.iIndexMenu = 0;
			Menu(MENU_PROMPT);
			break;
		case MENU_SELECT_BUTTON:			//Quand press bouton Milieu
			if(menu.pCurrentMenu[menu.iIndexMenu].pMenuFunc)
			{
				menu.pCurrentMenu[menu.iIndexMenu].pMenuFunc(0);
			}
			else if(menu.pCurrentMenu[menu.iIndexMenu].pSubMenu)
			{
				menu.pCurrentMenu = menu.pCurrentMenu[menu.iIndexMenu].pSubMenu;
				menu.iIndexMenu = 0;
				Menu(MENU_PROMPT);
			}
			break;
		case MENU_CLEAR_BUTTON:			//Quand press bouton Droite
			if(menu.pCurrentMenu[menu.iIndexMenu].pParentMenu)
			{
				menu.pCurrentMenu = menu.pCurrentMenu[menu.iIndexMenu].pParentMenu;
				menu.iIndexMenu = 0;
				Menu(MENU_PROMPT);
			}
			
			break;
		case MENU_PROMPT:
			LcdPutstr("                    ", 2, 0);
			Putstr(menu.pCurrentMenu[menu.iIndexMenu].pMenuLabel);
			LcdPutstr(menu.pCurrentMenu[menu.iIndexMenu].pMenuLabel,2,LcdFindHalf(strlen(menu.pCurrentMenu[menu.iIndexMenu].pMenuLabel)));
			break;
		case MENU_INIT:
		
		
			break;
		default:
			Error(ERROR_MENU_SWITCH_BAD_SC, sc);
	}
	return 0;
}

void ButtonSwitch_ISR_Handler()
{
	PushTask(Menu, MENU_SWITCH_BUTTON,0,0);
	

	// if(ShellGetState() == IDLE) ShellSetState(SWITCH);
}

void ButtonSelect_ISR_Handler()
{

	PushTask(Menu, MENU_SELECT_BUTTON,0,0);

	//f(ShellGetState() == SWITCH) ShellSetState(IDLE);
}

void ButtonClear_ISR_Handler()
{
	PushTask(Menu, MENU_CLEAR_BUTTON,0,0);
}

 uint32_t _menuOptionSasFunc(uint32_t sc, ...)
 {
	switch(sc)
	{
	default:
		Putstr("_menuOptionSasFunc: BAD SC");
		LcdPutstr("                     ", 3, 0);
		LcdPutstr("FERMETURE SAS",3,0);
		break;
	}

	return 0;
 }

uint32_t _menuPorteFermer1(uint32_t sc, ...)
{
	switch(sc)
	{
		default:
		Putstr("_menuPorteFermer1: BAD SC");
		break;
	}

	return 0;
}

uint32_t _menuPorteFermer2(uint32_t sc, ...)
{
	switch(sc)
	{
		default:
		Putstr("_menuPorteFermer2: BAD SC");
		break;
	}

	return 0;
}

uint32_t _menuEtatPorte1(uint32_t sc, ...)
{
	switch(sc)
	{
		default:
		Putstr("_menuOptionSasFunc: BAD SC");
		LcdPutstr("                     ", 3, 0);
		LcdPutstr("ETAT PORTE 1",3,0);
		break;
	}

	return 0;
}

uint32_t _menuEtatPorte2(uint32_t sc, ...)
{
	switch(sc)
	{
		default:
		Putstr("_menuOptionSasFunc: BAD SC");
		LcdPutstr("                     ", 3, 0);
		LcdPutstr("ETAT PORTE 1",3,0);
		break;
	}

	return 0;
}

uint32_t _menuEtatPortes(uint32_t sc, ...)
{
	switch(sc)
	{
		default:
		Putstr("_menuOptionSasFunc: BAD SC");
		LcdPutstr("                     ", 3, 0);
		LcdPutstr("ETAT PORTES",3,0);
		break;
	}

	return 0;
}