#include <string.h>


#include "appli.h"

#include "./kernel/modbus_private.h"
///////////////////////////////ON CREE LE MENU!!!//////////////////////////////////////////

 typedef struct tt_menu {
	const char				*pMenuLabel;
	const struct tt_menu	*pParentMenu;
	const struct tt_menu	*pSubMenu;
	const t_pFunc			pMenuFunc;
 }t_menu;

 extern const t_menu _mainMenu[];
 extern const t_menu _porteMenu[];
 extern const t_menu _optionSas[];
 uint32_t _ouvrirPorte(uint32_t sc, ...);
 uint32_t _enregistrerConfig(uint32_t sc, ...);
 uint32_t _effacerConfig(uint32_t sc, ...);
 uint32_t _mode1(uint32_t sc, ...);
 uint32_t _mode2(uint32_t sc, ...);
 uint32_t _mode3(uint32_t sc, ...);
 uint32_t _mode4(uint32_t sc, ...);

 


const t_menu _modeSas[]={
	{"Mode 1",	_optionSas,		NULL,	_mode1 },
	{"Mode 2",	_optionSas,		NULL,	_mode2 },
	{"Mode 3",	_optionSas,		NULL,	_mode3 },
	{"Mode 4",	_optionSas,		NULL,	_mode4 },
};


const t_menu _optionSas[] = {
	{"Sauvegarder config",	_mainMenu,	NULL,		_enregistrerConfig },
	{"Effacer Config",		_mainMenu,	NULL,		_effacerConfig },
	{"Changer mode",		_mainMenu,	_modeSas,	 NULL},
	//////////////////////////////////////////////////////
	{ NULL, NULL, NULL	}
};

const t_menu _mainMenu[]={
	{"Ouvrir porte",	NULL,	NULL,	_ouvrirPorte},
	{"Option sas",		NULL,	_optionSas,	NULL },
	
	//////////////////////////////////////////////////////
	{ NULL, NULL, NULL	}
};


/////////////////////////////////INTERRUPT TEST//////////////
void ButtonSwitch_ISR_Handler(void);	
void ButtonSelect_ISR_Handler(void);
void ButtonClear_ISR_Handler(void);

struct {
	int				iIndexMenu;
	const t_menu	*pCurrentMenu;
	uint32_t				status;
}menuElem,menus[MENU_DEPTH_VALUE];

 #define	ST_MENU_INIT		0x01
 #define	IsMenuInIinit()			mIsBitsSet(menuElem.status,ST_MENU_INIT)
 #define	PutMenuInInit()			mBitsSet(menuElem.status,ST_MENU_INIT)
 #define	MenuInitDone()			mBitsClr(menuElem.status,ST_MENU_INIT)
 
t_stack menuStack;


uint32_t Menu(uint32_t sc, ...)
{
	switch(sc)
	{
		case MENU_NEW:
			Putstr("\r\nMENU_NEW\r\n");

			//Init ButtonSwitch
			pio_set_input(PIOA, PIN_BUTTON_SWITCH, PIO_PULLUP); //A0 (Bouton Gauche)
			pio_handler_set(PIOA, ID_PIOA, PIN_BUTTON_SWITCH, PIO_IT_FALL_EDGE, ButtonSwitch_ISR_Handler);
			
			//Init ButtonSelect
			pio_set_input(PIOA, PIN_BUTTON_SELECT, PIO_PULLUP); //A1 (Bouton Milieu)
			pio_handler_set(PIOA, ID_PIOA, PIN_BUTTON_SELECT, PIO_IT_FALL_EDGE, ButtonSelect_ISR_Handler);
			
			//Init ButtonClear
			pio_set_input(PIOA, PIN_BUTTON_CLEAR, PIO_PULLUP); //A1 (Bouton Droite)
			pio_handler_set(PIOA, ID_PIOA, PIN_BUTTON_CLEAR, PIO_IT_FALL_EDGE, ButtonClear_ISR_Handler);
			
			NVIC_ClearPendingIRQ(PIOA_IRQn);
			NVIC_EnableIRQ(PIOA_IRQn);
			pio_get_interrupt_status(PIOA);
			pio_enable_interrupt(PIOA, PIN_BUTTON_SWITCH);
			pio_enable_interrupt(PIOA, PIN_BUTTON_SELECT);
			pio_enable_interrupt(PIOA, PIN_BUTTON_CLEAR);

			stackNew(&menuStack, menus, MENU_DEPTH_VALUE, MENU_ELEM_SIZE);
			menuElem.iIndexMenu = 0;
			menuElem.pCurrentMenu = _mainMenu;
			int i = 0;
			break;
		case MENU_SWITCH_BUTTON:			//Quand press bouton Gauche
			if((!IsNetworkInit()) && (IsAttachReady()))
			{
				Modbus(SLAVE_ADD_REQUEST);
			}
		
			else if (IsNetworkInit())
			{
				if(IsModulePlatine())
				{
					Putstr("\r\n\tMODULE PLAT PUSh\r\n\n");
					
				}
				menuElem.iIndexMenu++;
				if(menuElem.pCurrentMenu[menuElem.iIndexMenu].pMenuLabel == NULL) menuElem.iIndexMenu = 0;
				Menu(MENU_PROMPT);
			}

			break;
		case MENU_SELECT_BUTTON:			//Quand press bouton Milieu
			if(menuElem.pCurrentMenu[menuElem.iIndexMenu].pMenuFunc)
			{
				menuElem.pCurrentMenu[menuElem.iIndexMenu].pMenuFunc(0);
			}
			else if(menuElem.pCurrentMenu[menuElem.iIndexMenu].pSubMenu)
			{
				stackPush(&menuStack, &menuElem);
				menuElem.pCurrentMenu = menuElem.pCurrentMenu[menuElem.iIndexMenu].pSubMenu;
				menuElem.iIndexMenu = 0;
				Menu(MENU_PROMPT);
			}
			break;

		case MENU_CLEAR_BUTTON:			//Quand press bouton Droite
			if(!IsNetworkInit() && IsMaster())
			{	
				PushTask(Modbus, MODBUS_NETWORK_VALIDATION, 0, 100);	
			}	
			if(menuElem.pCurrentMenu[menuElem.iIndexMenu].pParentMenu)
			{
				stackPop(&menuStack, &menuElem);
				Menu(MENU_PROMPT);
			}
			break;

		case MENU_PROMPT:
			LcdPutstr("                    ", 2, 0);
			LcdPutstr((menuElem.pCurrentMenu[menuElem.iIndexMenu].pMenuLabel),2,LcdFindHalf(strlen(menuElem.pCurrentMenu[menuElem.iIndexMenu].pMenuLabel)));
			break;
		case MENU_INIT:

			break;
			
		case MENU_SET_STATE:
#define STATE	pa1
			mBitsSet(menuElem.status, STATE);
#undef STATE
			break;
		case MENU_CLR_STATE:
#define STATE pa1
			mBitsClr(menuElem.status, STATE);
#undef STATE
		case MENU_GET_STATE:
			return menuElem.status;
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

uint32_t _enregistrerConfig(uint32_t sc, ...)
{
	Flash(FLASH_SAVE);
	LcdPutstr("                     ", 3, 0);
	LcdPutstr("CONFIG SAVED",3,0);
	return 0;
}

uint32_t _effacerConfig(uint32_t sc, ...)
{
	Flash(FLASH_ERASE);
	LcdPutstr("                     ", 3, 0);
	LcdPutstr("CONFIG ERASED",3,0);
	return 0;
}

uint32_t _ouvrirPorte(uint32_t sc, ...)
{
	if(IsModulePlatine())
	{
		ServicesBufferPush(WRITE_SINGLE_COIL);
		LcdPutstr("                     ", 3, 0);
		LcdPutstr("OPEN DOOR 1",3,0);
	}
	
	return 0;
}

uint32_t _mode1(uint32_t sc, ...)
{
	LcdPutstr("                     ", 3, 0);
	LcdPutstr("mode1",3,0);
	return 0;
}

uint32_t _mode2(uint32_t sc, ...)
{
	LcdPutstr("                     ", 3, 0);
	LcdPutstr("mode2",3,0);
	return 0;
}

uint32_t _mode3(uint32_t sc, ...)
{
	LcdPutstr("                     ", 3, 0);
	LcdPutstr("mode3",3,0);
	return 0;
}

uint32_t _mode4(uint32_t sc, ...)
{
	LcdPutstr("                     ", 3, 0);
	LcdPutstr("mode4",3,0);
	return 0;
}

