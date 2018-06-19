#include <string.h>																		//On utilise des strings, donc on ajoute string.h
#include "appli.h"																		//appli.h est le coeur de l'applicatif et est include dans chacune des classes
///////////////////////////////ON CREE LE MENU!!!//////////////////////////////////////////

typedef struct tt_menu {																//On créé l'objet t_menu auquel on assigne différents attributs	:		
	const char			 *pMenuLabel;													//Un pointeur sur le menu lui même
	const struct tt_menu *pParentMenu;													//Un pointeur sur le menu-parent du menu
	const struct tt_menu *pSubMenu;														//Un pointeur sur un sous-menu du menu
	const t_pFunc		  pMenuFunc;													//La fonction appelée lors de l'accès au menu
}t_menu;

extern const t_menu _mainMenu[];														//Déclaration de l'objet _mainMenu
extern const t_menu _modesMenu[];														//Déclaration de l'objet _modesMenu
																						
																						//Déclaration des fonctions liées aux menu		
uint32_t _menuActualMode(uint32_t sc, ...);												//Fonction d'affichage du mode actuel
uint32_t _ContrastControl(uint32_t sc, ...);											//Fonction de réglage du contraste
uint32_t _VolumeControl(uint32_t sc, ...);												//Fonction de réglage du volume
uint32_t _TimeSetting(uint32_t sc, ...);												//Fonction de réglage horaire
uint32_t _menuDoorNumber(uint32_t sc, ...);												//Fonction qui gère le choix du nombre de porte
uint32_t _menuPairing(uint32_t sc, ...);												//Fonction qui gère l'appairage

uint32_t _modeLibre(uint32_t sc, ...);													//Fonction qui gère le mode Libre
uint32_t _modeLibreSES(uint32_t sc, ...);												//Fonction qui gère le mode LibreSES
uint32_t _modeSortie(uint32_t sc, ...);													//Fonction qui gère le mode Sortie
uint32_t _modeDeverrouillage(uint32_t sc, ...);											//Fonction qui gère le mode Deverrouillage
uint32_t _modeBlocage(uint32_t sc, ...);												//Fonction qui gère le mode Blocage
uint32_t _modeControle(uint32_t sc, ...);												//Fonction qui gère le mode Controle
uint32_t _modeControleE(uint32_t sc, ...);												//Fonction qui gère le mode ControleE
uint32_t _modeControleES(uint32_t sc, ...);												//Fonction qui gère le mode ControleES
uint32_t _modeCtrlPerso(uint32_t sc, ...);												//Fonction qui gère le mode CtrlPerso

//const t_menu _menuName[] = {
//{"Affichage 1",	_menuParent,		_sousMenu,			_fonctionMenu },			//Template d'une déclaration de menu
////Affichage LCD,	Menu précédent,		Sous-menu,			Fonction de ce champ//		//Certains des champs peuvent être mis a NULL s'il n'y a rien a y mettre
////////////////////////////////////////////////////////////////////////////////////	//Ex : un menu sans fonction aura NULL sur son dernier champ
//{ NULL, NULL, NULL	}																//Tout les menu se terminent par un NULL sur chaque champ
//};

const t_menu _doorNumberMenu[] = {	//Nombre de porte									//Menu permettant de choisir le nombre de porte
	{"1 porte",			_mainMenu,			NULL,				_menuPairing},			//1 porte, amène vers la fonction d'appairage
	{"2 portes",		_mainMenu,			NULL,				_menuPairing},			//2 portes, amène vers la fonction d'appairage
	{"3 portes ou plus",_mainMenu,			NULL,				_menuPairing},			//3 portes ou plus, amène vers la fonction d'appairage
	//Affichage LCD,	Menu précédent,		Sous-menu,			Fonction de ce champ//
	//////////////////////////////////////////////////////////////////////////////////
	{ NULL, NULL, NULL	}																
};

const t_menu _OneDoorMenu[] = {		//Choix des modes 1 porte							//Menu permettant de choisir le mode pour le cas où il n'y a qu'une porte
	{"Libre",			_doorNumberMenu,	NULL,				_modeLibre},			//Mode Libre, amène vers la fonction de mode Libre
	{"Sortie",			_doorNumberMenu,	NULL,				_modeSortie},			//Mode Sortie, amène vers la fonction de mode Sortie
	{"Déverrouillage",	_doorNumberMenu,	NULL,				_modeDeverrouillage},	//Mode Déverrouillage, amène vers la fonction de mode Déverrouillage
	{"Blocage",			_doorNumberMenu,	NULL,				_modeBlocage},			//Mode Blocage, amène vers la fonction de mode Blocage
	{"Contrôle Entrée",	_doorNumberMenu,	NULL,				_modeControleE},		//Mode Contrôle Entrée, amène vers la fonction de mode Contrôle Entrée
	{"Contrôle E/S",	_doorNumberMenu,	NULL,				_modeControleES},		//Mode Contrôle E/S, amène vers la fonction de mode Contrôle E/S
	
	//Affichage LCD,	Menu précédent,		Sous-menu,			Fonction de ce champ//
	//////////////////////////////////////////////////////////////////////////////////
	{ NULL, NULL, NULL	}
};

const t_menu _TwoDoorMenu[] = {		//Choix des modes 2 portes							//Menu permettant de choisir le mode pour le cas où il y deux portes
	{"Libre",			_mainMenu,			NULL,				_modeLibre},			//Mode Libre, amène vers la fonction de mode Libre
	{"LibreSES",		_mainMenu,			NULL,				_modeLibreSES},			//Mode LibreSES, amène vers la fonction de mode LibreSES
	{"Sortie",			_mainMenu,			NULL,				_modeSortie},			//Mode Sortie, amène vers la fonction de mode Sortie
	{"Déverrouillage",	_mainMenu,			NULL,				_modeDeverrouillage},	//Mode Déverrouillage, amène vers la fonction de mode Déverrouillage
	{"Blocage",			_mainMenu,			NULL,				_modeBlocage},			//Mode Blocage, amène vers la fonction de mode Blocage
	{"Ctrl P1/P2 E",	_mainMenu,			NULL,				_modeControleE},		//Mode ControleE, amène vers la fonction de mode ControleE
	{"Ctrl P1/P2 E/S",	_mainMenu,			NULL,				_modeControleES},		//Mode ControleES, amène vers la fonction de mode ControleES
	{"Ctrl P2",			_mainMenu,			NULL,				_modeControle},			//Mode Controle, amène vers la faction de mode Controle P2
	
	//Affichage LCD,	Menu précédent,		Sous-menu,			Fonction de ce champ//
	//////////////////////////////////////////////////////////////////////////////////
	{ NULL, NULL, NULL	}
};

const t_menu _ThreeDoorMenu[] = {	//Choix des modes 3 portes ou plus					//Menu permettant de choisir le mode pour le cas où il y a trois portes ou plus
	{"Libre",					_mainMenu,			NULL,		_modeLibre},			//Mode Libre,
	{"LibreSES",				_mainMenu,			NULL,		_modeLibreSES},			//Mode LibreSES,
	{"Sortie",					_mainMenu,			NULL,		_modeSortie},			//Mode Sortie,
	{"Déverrouillage",			_mainMenu,			NULL,		_modeDeverrouillage},	//Mode Déverrouillage,
	{"Blocage",					_mainMenu,			NULL,		_modeBlocage},			//Mode Blocage,
	{"Contrôle personnalisé",	_mainMenu,			NULL,		_modeCtrlPerso},		//Mode Contrôle personnalisé,
	//Affichage LCD,			Menu précédent,		Sous-menu,	Fonction de ce champ//
	//////////////////////////////////////////////////////////////////////////////////
	{ NULL, NULL, NULL	}
};

const t_menu _technicianMenu[] = {	//Mode technicien
	{"Nombre de porte :",_modesMenu,		NULL,				_menuDoorNumber },		//Menu permettant de choisir le nombre de porte
	{"Appairage :",		_modesMenu,			NULL,				_menuPairing },			//Menu permettant de lancer l'appairage des cartes
	//Affichage LCD,	Menu précédent,		Sous-menu,			Fonction de ce champ//
	//////////////////////////////////////////////////////////////////////////////////
	{ NULL, NULL, NULL	}
};

const t_menu _modesMenu[] = {		//Affichage mode
	{"Mode actuel :",	_mainMenu,			NULL,				_menuActualMode },		//Affichage du mode actuel
	//Affichage LCD,	Menu précédent,		Sous-menu,			Fonction de ce champ//
	//////////////////////////////////////////////////////////////////////////////////
	{ NULL, NULL, NULL	}
};

const t_menu _mainMenu[] = {		//ACCUEIL
	{"Mode technicien",	NULL,				_doorNumberMenu,	NULL },					//Menu permettant de passer en mode technicien
	{"Mode normal",		NULL,				_modesMenu,			NULL },					//Menu permettant de passer en mode normal (non technicien)
	//Affichage LCD,	Menu précédent,		Sous-menu,			Fonction de ce champ//
	//////////////////////////////////////////////////////////////////////////////////
	{ NULL, NULL, NULL	}
};

/////////////////////////////////INTERRUPT TEST//////////////
void ButtonSwitch_ISR_Handler(void);
void ButtonSelect_ISR_Handler(void);
void ButtonBack_ISR_Handler(void);

struct {
	int		iIndexMenu;					//Index de positionnement parmis les menus
	const t_menu	*pCurrentMenu;		//Pointeur sur le menu actuel
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
			menu.iIndexMenu = 0;				//Init de l'index du menu a 0
			menu.pCurrentMenu = _mainMenu;		//Le menu actuel est le menu principal (_mainMenu)
		break;
		
		case MENU_SWITCH_BUTTON:				//Quand appui sur le bouton Gauche
			menu.iIndexMenu++;					//On incrémente l'index pour parcourir les sélections possibles
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
		LcdPutstr("www.A-2-S.fr",2,1);
		LcdPutstr("Naviguer  Select  Back",3,0);
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

/////////////////////////////////////////////////////////////////Règle générale des modes de fonctionnement : - Une seule porte ouverte à la fois, les autres sont fermée et verrouillée
///////		FONCTIONS DU MENU	/////////////////////////////////											  - On refuse les demandes d'entrée si quelqu'un est déjà dans le sas
/////////////////////////////////////////////////////////////////												

uint32_t _menuActualMode(uint32_t sc, ...)		//Cette fonction donne le mode de fonctionnement du système actuel sur la ligne 3 (en partant de 0)
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

uint32_t _menuDoorNumber(uint32_t sc, ...)		//Cette fonction donne le nombre de porte dans le systeme actuel sur la ligne 3 (en partant de 0)
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

uint32_t _menuPairing(uint32_t sc, ...)			//Cette fonction gère l'apparaige des cartes entres elles
{												//Elle devrait être exécutée seulement une fois que le nombre de porte a été choisi
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

uint32_t _modeLibre(uint32_t sc, ...)			//Cette fonction règle le système en mode libre
{												//Mode libre signifie que les portes sont ouvrables et fermable sans contrôle (appui boutons toujours nécessaire)
	switch(sc)									//Cette fonction devrait être exécutée seulement une fois l'appairage effectué
	{
		default:
		Putstr("_modeLibre: BAD SC");
		LcdPutstr("LIBRE PLACEHOLDER",
		3,
		LcdFindCenter(strlen("LIBRE PLACEHOLDER"))
		);
		break;
	}
	return 0;
}

uint32_t _modeLibreSES(uint32_t sc, ...)		//Cette fonction règle le système en mode libreSES
{												//Pareil que pour le mode Libre, sauf que plusieurs portes peuvent être ouvertes en même temps (malgré la contre indication des règles générales)
	switch(sc)									//Cette fonction devrait être exécutée seulement une fois l'appairage effectué
	{
		default:
		Putstr("_menuLibreSES: BAD SC");
		LcdPutstr("LIBRESES PLACEHOLDER",
		3,
		LcdFindCenter(strlen("LIBRESES PLACEHOLDER"))
		);
		break;
	}
	return 0;	
}

uint32_t _modeSortie(uint32_t sc, ...)			//Cette fonction règle le système en mode Sortie
{												//Mode Sortie signifie que les portes sont libre côté sortie, mais elles disposent d'un contrôle en entrée, sauf pour la première
	switch(sc)									//Cette fonction devrait être exécutée seulement une fois l'appairage effectué
	{
		default:
		Putstr("_menuSortie: BAD SC");
		LcdPutstr("SORTIE PLACEHOLDER",
		3,
		LcdFindCenter(strlen("SORTIE PLACEHOLDER"))
		);
		break;
	}
	return 0;
}

uint32_t _modeDeverrouillage(uint32_t sc, ...)	//Cette fonction règle le système en mode Deverrouillage
{												//Mode Deverrouillage signifie que tout est en accès libre (ouverture de toutes les portes quelque soit la situation), ouverture d'urgence
	switch(sc)									//Cette fonction devrait être exécutée seulement une fois l'appairage effectué
	{
		default:
		Putstr("_menuDeverrouillage: BAD SC");
		LcdPutstr("DEVERROUILLAGE PLACEHOLDER",
		3,
		LcdFindCenter(strlen("DEVERROUILLAGE PLACEHOLDER"))
		);
		break;
	}
	return 0;
}		

uint32_t _modeBlocage(uint32_t sc, ...)			//Cette fonction règle le système en mode Blocage
{												//Mode Blocage signifie que personne ne peut rentrer ou sortir dans le SAS, tout est bloqué, pupitre de commande on fonctionnel
	switch(sc)									//Cette fonction devrait être exécutée seulement une fois l'appairage effectué
	{
		default:
		Putstr("_menuBlocage: BAD SC");
		LcdPutstr("BLOCAGE PLACEHOLDER",
		3,
		LcdFindCenter(strlen("BLOCAGE PLACEHOLDER"))
		);
		break;
	}
	return 0;
}			

uint32_t _modeControle(uint32_t sc, ...)		//
{
	switch(sc)
	{
		default:
		Putstr("_menuControle: BAD SC");
		LcdPutstr("CONTROLE PLACEHOLDER",
		3,
		LcdFindCenter(strlen("CONTROLE PLACEHOLDER"))
		);
		break;
	}
	return 0;
}

uint32_t _modeControleE(uint32_t sc, ...)		//Cette fonction règle le système en mode Controle E
{												//Mode Controle E signifie
	switch(sc)
	{
		default:
		Putstr("_menuControleE: BAD SC");
		LcdPutstr("CONTROLE E PLACEHOLDER",
		3,
		LcdFindCenter(strlen("CONTROLE E PLACEHOLDER"))
		);
		break;
	}
	return 0;
}
	
uint32_t _modeControleES(uint32_t sc, ...)
{
	switch(sc)
	{
		default:
		Putstr("_menuControleES: BAD SC");
		LcdPutstr("CONTROLE ES PLACEHOLDER",
		3,
		LcdFindCenter(strlen("CONTROLE ES PLACEHOLDER"))
		);
		break;
	}
	return 0;
}

uint32_t _modeCtrlPerso(uint32_t sc, ...)
{
	switch(sc)
	{
		default:
		Putstr("_menuCtrlPerso: BAD SC");
		LcdPutstr("CTRLPERSO PLACEHOLDER",
		3,
		LcdFindCenter(strlen("CTRLPERSO PLACEHOLDER"))
		);
		break;
	}
	return 0;
}