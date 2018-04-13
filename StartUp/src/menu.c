#include <string.h>
#include "appli.h"
///////////////////////////////ON CREE LE MENU!!!//////////////////////////////////////////

typedef struct tt_menu {
	const char			 *pMenuLabel;
	const struct tt_menu *pParentMenu;
	const struct tt_menu *pSubMenu;
	const t_pFunc		  pMenuFunc;
}t_menu;

extern const t_menu _mainMenu[];
extern const t_menu _modesMenu[];

uint32_t _menuActualMode(uint32_t sc, ...);
uint32_t _menuDoorNumber(uint32_t sc, ...);
uint32_t _menuPairing(uint32_t sc, ...);

//const t_menu _pairingMenu[] = {
//{"Etat porte 1",	_modesMenu,			NULL,				_ },
////Affichage LCD,	Menu précédent,		Sous-menu,			Fonction de ce champ//
////////////////////////////////////////////////////////////////////////////////////
//{ NULL, NULL, NULL	}
//};

const t_menu _doorNumberMenu[] = {
	{"1 porte",			_mainMenu,			NULL,				NULL},
	{"2 portes",		_mainMenu,			NULL,				NULL},
	{"3 portes ou plus",_mainMenu,			NULL,				NULL},
	//Affichage LCD,	Menu précédent,		Sous-menu,			Fonction de ce champ//
	//////////////////////////////////////////////////////////////////////////////////
	{ NULL, NULL, NULL	}
};

const t_menu _technicianMenu[] = {
	{"Nombre de porte :",_modesMenu,		NULL,				_menuDoorNumber },
	{"Appairage :",		_modesMenu,			NULL,				_menuPairing },
	//Affichage LCD,	Menu précédent,		Sous-menu,			Fonction de ce champ//
	//////////////////////////////////////////////////////////////////////////////////
	{ NULL, NULL, NULL	}
};

const t_menu _modesMenu[] = {
	{"Mode actuel :",	_mainMenu,			NULL,				_menuActualMode },
	//Affichage LCD,	Menu précédent,		Sous-menu,			Fonction de ce champ//
	//////////////////////////////////////////////////////////////////////////////////
	{ NULL, NULL, NULL	}
};

const t_menu _mainMenu[] = {
	{"Mode technicien",	NULL,				_doorNumberMenu,	NULL },
	{"Mode normal",		NULL,				_modesMenu,			NULL },
	//Affichage LCD,	Menu précédent,		Sous-menu,			Fonction de ce champ//
	//////////////////////////////////////////////////////////////////////////////////
	{ NULL, NULL, NULL	}
};

/////////////////////////////////INTERRUPT TEST//////////////
void ButtonSwitch_ISR_Handler(void);
void ButtonSelect_ISR_Handler(void);
void ButtonBack_ISR_Handler(void);

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
		//Init ButtonBack
		pio_set_input(PIOA, PIN_BUTTON_BACK, PIO_PULLUP); //A1 (Bouton Droite)
		pio_handler_set(PIOA, ID_PIOA, PIN_BUTTON_BACK, PIO_IT_FALL_EDGE, ButtonBack_ISR_Handler);
		pio_enable_interrupt(PIOA, PIN_BUTTON_BACK);
		
		NVIC_EnableIRQ(PIOA_IRQn);

		sprintf(buf, "MENU NEW FINISHED \r\n");
		Putstr(buf);
		menu.iIndexMenu = 0;
		menu.pCurrentMenu = _mainMenu;
		break;
		case MENU_SWITCH_BUTTON:			//Quand appui bouton Gauche
		menu.iIndexMenu++;
		if(menu.pCurrentMenu[menu.iIndexMenu].pMenuLabel == NULL) menu.iIndexMenu = 0;
		Menu(MENU_PROMPT);
		break;
		case MENU_SELECT_BUTTON:			//Quand appui bouton Milieu
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
		case MENU_BACK_BUTTON:				//Quand appui bouton Droite
		if(menu.pCurrentMenu[menu.iIndexMenu].pParentMenu)
		{
			menu.pCurrentMenu = menu.pCurrentMenu[menu.iIndexMenu].pParentMenu;
			menu.iIndexMenu = 0;
			Menu(MENU_PROMPT);
		}
		break;

		case MENU_PROMPT:
		SetButtonTimeout();
		Putstr(menu.pCurrentMenu[menu.iIndexMenu].pMenuLabel);
		LcdPutstr(menu.pCurrentMenu[menu.iIndexMenu].pMenuLabel,2,LcdFindCenter(strlen(menu.pCurrentMenu[menu.iIndexMenu].pMenuLabel)));
		setShellStatus(ST_MENU_MENU);
		break;
		case MENU_INIT:
		
		break;
		
		case MENU_RETURN_HOME:
		LcdPutstr("CPU.ACX  ATSAM3U4C",2,1);
		LcdPutstr("www.a-2-s.net",3,4);
		break;

		default:
		Error(ERROR_MENU_SWITCH_BAD_SC, sc);
	}
	return 0;
}

void ButtonSwitch_ISR_Handler()
{
	PushTask(Menu, MENU_SWITCH_BUTTON,0,0);
}

void ButtonSelect_ISR_Handler()
{
	PushTask(Menu, MENU_SELECT_BUTTON,0,0);
}

void ButtonBack_ISR_Handler()
{
	PushTask(Menu, MENU_BACK_BUTTON,0,0);
}

////////////////////////////////////////////////////////////////////////////////////////////////
///////		FONCTIONS DU MENU	////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

uint32_t _menuActualMode(uint32_t sc, ...)
{
	switch(sc)
	{
		default:
		Putstr("_menuActualMode: BAD SC");
		LcdPutstr("MODE PLACEHOLDER",
		3,
		LcdFindCenter(strlen("MODE PLACEHOLDER"))
		);
		break;
	}
	return 0;
}

uint32_t _menuDoorNumber(uint32_t sc, ...)
{
	switch(sc)
	{
		default:
		Putstr("_menuDoorNumber: BAD SC");
		LcdPutstr("DOOR NUMBER PLACEHOLDER",
		3,
		LcdFindCenter(strlen("DOOR NUMBER PLACEHOLDER"))
		);
		break;
	}
	return 0;
}

uint32_t _menuPairing(uint32_t sc, ...)
{
	switch(sc)
	{
		default:
		Putstr("_menuPairing: BAD SC");
		LcdPutstr("PAIRING PLACEHOLDER",
		3,
		LcdFindCenter(strlen("PAIRING PLACEHOLDER"))
		);
		break;
	}
	return 0;
}