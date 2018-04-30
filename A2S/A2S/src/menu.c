#include <string.h>


#include "appli.h"

#include "./kernel/modbus_private.h"
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
	int				iIndexMenu;
	const t_menu	*pCurrentMenu;
}menuElem,menus[MENU_DEPTH_VALUE];

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
			//ReadCoils(0x16, 0x01, 1);
			//ModbusAtach();
			ServicesBufferPush(WRITE_SINGLE_COIL);
			//WriteMultipleRegisters(0x20, READ_COIL, 0, 0);
			menuElem.iIndexMenu++;
			//gpio_toggle_pin(CMD_MOSFET);
			//gpio_toggle_pin(CMD_MOT_SERRURE);
			//WriteMultipleRegisters(1, READ_COILS, 6, "Hello there!");
			if(menuElem.pCurrentMenu[menuElem.iIndexMenu].pMenuLabel == NULL) menuElem.iIndexMenu = 0;
			Menu(MENU_PROMPT);
			break;
		case MENU_SELECT_BUTTON:			//Quand press bouton Milieu
			//WriteSingleCoil(0x20, 0x01, 0x01);
			ServicesBufferPush(READ_COILS);
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
			if(menuElem.pCurrentMenu[menuElem.iIndexMenu].pParentMenu)
			{
				stackPop(&menuStack, &menuElem);
				Menu(MENU_PROMPT);
				

			}
			Modbus(MODBUS_SLAVE_SEND);
			//WriteSingleCoil(0x16, 0x01, 0x01);
			//WriteSingleCoil(MODBUS_BROADCAST_ADDRESS, 0x01, 0x01);
			
			break;
		case MENU_PROMPT:
			LcdPutstr("                    ", 2, 0);
			LcdPutstr((menuElem.pCurrentMenu[menuElem.iIndexMenu].pMenuLabel),2,LcdFindHalf(strlen(menuElem.pCurrentMenu[menuElem.iIndexMenu].pMenuLabel)));
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