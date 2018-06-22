#include <string.h>																		//On utilise des strings, donc on ajoute string.h
#include "appli.h"																		//appli.h est le coeur de l'applicatif et est include dans chacune des classes
///////////////////////////////ON CREE LE MENU!!!//////////////////////////////////////////

typedef struct tt_menu {																//On cr�� l'objet t_menu auquel on assigne diff�rents attributs	:
	const char			 *pMenuLabel;													//Un pointeur sur le menu lui m�me
	const struct tt_menu *pParentMenu;													//Un pointeur sur le menu-parent du menu
	const struct tt_menu *pSubMenu;														//Un pointeur sur un sous-menu du menu
	const t_pFunc		  pMenuFunc;													//La fonction appel�e lors de l'acc�s au menu
}t_menu;

extern const t_menu _mainMenu[];														//D�claration de l'objet _mainMenu
extern const t_menu _modesMenu[];														//D�claration de l'objet _modesMenu
extern const t_menu _menuPairingUneP[];
extern const t_menu _menuPairingDeuxP[];
extern const t_menu _menuPairingTroisP[];
extern const t_menu _OneDoorMenu[];
extern const t_menu _TwoDoorMenu[];
extern const t_menu _ThreeDoorMenu[];
extern const t_menu _doorNumberMenu[];
extern const t_menu _technicianMenu[];

//D�claration des fonctions li�es aux menu
uint32_t _menuActualMode(uint32_t sc, ...);												//Fonction d'affichage du mode actuel
uint32_t _ContrastControl(uint32_t sc, ...);											//Fonction de r�glage du contraste
uint32_t _VolumeControl(uint32_t sc, ...);												//Fonction de r�glage du volume
uint32_t _TimeSetting(uint32_t sc, ...);												//Fonction de r�glage horaire
uint32_t _menuDoorNumber(uint32_t sc, ...);												//Fonction qui g�re le choix du nombre de porte
uint32_t _menuPairing(uint32_t sc, ...);												//Fonction qui g�re l'appairage

uint32_t _modeLibre(uint32_t sc, ...);													//Fonction qui g�re le mode Libre
uint32_t _modeLibreSES(uint32_t sc, ...);												//Fonction qui g�re le mode LibreSES
uint32_t _modeSortie(uint32_t sc, ...);													//Fonction qui g�re le mode Sortie
uint32_t _modeDeverrouillage(uint32_t sc, ...);											//Fonction qui g�re le mode Deverrouillage
uint32_t _modeBlocage(uint32_t sc, ...);												//Fonction qui g�re le mode Blocage
uint32_t _modeControle(uint32_t sc, ...);												//Fonction qui g�re le mode Controle
uint32_t _modeControleE(uint32_t sc, ...);												//Fonction qui g�re le mode ControleE
uint32_t _modeControleES(uint32_t sc, ...);												//Fonction qui g�re le mode ControleES
uint32_t _modeCtrlPerso(uint32_t sc, ...);												//Fonction qui g�re le mode CtrlPerso

//const t_menu _menuName[] = {
//{"Affichage 1",	_menuParent,		_sousMenu,			_fonctionMenu },			//Template d'une d�claration de menu
////Affichage LCD,	Menu pr�c�dent,		Sous-menu,			Fonction de ce champ//		//Certains des champs peuvent �tre mis a NULL s'il n'y a rien a y mettre
////////////////////////////////////////////////////////////////////////////////////	//Ex : un menu sans fonction aura NULL sur son dernier champ
//{ NULL, NULL, NULL	}																//Tout les menu se terminent par un NULL sur chaque champ
//};

const t_menu _menuPairingUneP[] = {		//menu d'appairage une porte
	{"menu d'appairage",_doorNumberMenu,	_OneDoorMenu,		NULL},
	//Affichage LCD,	Menu pr�c�dent,		Sous-menu,			Fonction de ce champ//
	//////////////////////////////////////////////////////////////////////////////////
	{ NULL, NULL, NULL	}
};

const t_menu _menuPairingDeuxP[] = {		//menu d'appairage deux portes
	{"menu d'appairage",_doorNumberMenu,	_TwoDoorMenu,		NULL},
	//Affichage LCD,	Menu pr�c�dent,		Sous-menu,			Fonction de ce champ//
	//////////////////////////////////////////////////////////////////////////////////
	{ NULL, NULL, NULL	}
};

const t_menu _menuPairingTroisP[] = {		//menu d'appairage trois portes
	{"menu d'appairage",_doorNumberMenu,	_ThreeDoorMenu,		NULL},
	//Affichage LCD,	Menu pr�c�dent,		Sous-menu,			Fonction de ce champ//
	//////////////////////////////////////////////////////////////////////////////////
	{ NULL, NULL, NULL	}
};

const t_menu _doorNumberMenu[] = {	//Nombre de porte									//Menu permettant de choisir le nombre de porte
	{"1 porte",			_mainMenu,			_menuPairingUneP,	NULL},					//1 porte, am�ne vers la fonction d'appairage
	{"2 portes",		_mainMenu,			_menuPairingDeuxP,	NULL},					//2 portes, am�ne vers la fonction d'appairage
	{"3 portes ou plus",_mainMenu,			_menuPairingTroisP,	NULL},					//3 portes ou plus, am�ne vers la fonction d'appairage
	//Affichage LCD,	Menu pr�c�dent,		Sous-menu,			Fonction de ce champ//
	//////////////////////////////////////////////////////////////////////////////////
	{ NULL, NULL, NULL	}
};

const t_menu _OneDoorMenu[] = {		//Choix des modes 1 porte							//Menu permettant de choisir le mode pour le cas o� il n'y a qu'une porte
	{"Libre",			_menuPairingUneP,	NULL,				_modeLibre},			//Mode Libre, am�ne vers la fonction de mode Libre
	{"Sortie",			_menuPairingUneP,	NULL,				_modeSortie},			//Mode Sortie, am�ne vers la fonction de mode Sortie
	{"D�verrouillage",	_menuPairingUneP,	NULL,				_modeDeverrouillage},	//Mode D�verrouillage, am�ne vers la fonction de mode D�verrouillage
	{"Blocage",			_menuPairingUneP,	NULL,				_modeBlocage},			//Mode Blocage, am�ne vers la fonction de mode Blocage
	{"Contr�le Entr�e",	_menuPairingUneP,	NULL,				_modeControleE},		//Mode Contr�le Entr�e, am�ne vers la fonction de mode Contr�le Entr�e
	{"Contr�le E/S",	_menuPairingUneP,	NULL,				_modeControleES},		//Mode Contr�le E/S, am�ne vers la fonction de mode Contr�le E/S
	
	//Affichage LCD,	Menu pr�c�dent,		Sous-menu,			Fonction de ce champ//
	//////////////////////////////////////////////////////////////////////////////////
	{ NULL, NULL, NULL	}
};

const t_menu _TwoDoorMenu[] = {		//Choix des modes 2 portes							//Menu permettant de choisir le mode pour le cas o� il y deux portes
	{"Libre",			_menuPairingDeuxP,	NULL,				_modeLibre},			//Mode Libre, am�ne vers la fonction de mode Libre
	{"LibreSES",		_menuPairingDeuxP,	NULL,				_modeLibreSES},			//Mode LibreSES, am�ne vers la fonction de mode LibreSES
	{"Sortie",			_menuPairingDeuxP,	NULL,				_modeSortie},			//Mode Sortie, am�ne vers la fonction de mode Sortie
	{"D�verrouillage",	_menuPairingDeuxP,	NULL,				_modeDeverrouillage},	//Mode D�verrouillage, am�ne vers la fonction de mode D�verrouillage
	{"Blocage",			_menuPairingDeuxP,	NULL,				_modeBlocage},			//Mode Blocage, am�ne vers la fonction de mode Blocage
	{"Ctrl P1/P2 E",	_menuPairingDeuxP,	NULL,				_modeControleE},		//Mode ControleE, am�ne vers la fonction de mode ControleE
	{"Ctrl P1/P2 E/S",	_menuPairingDeuxP,	NULL,				_modeControleES},		//Mode ControleES, am�ne vers la fonction de mode ControleES
	{"Ctrl P2",			_menuPairingDeuxP,	NULL,				_modeControle},			//Mode Controle, am�ne vers la faction de mode Controle P2
	
	//Affichage LCD,	Menu pr�c�dent,		Sous-menu,			Fonction de ce champ//
	//////////////////////////////////////////////////////////////////////////////////
	{ NULL, NULL, NULL	}
};

const t_menu _ThreeDoorMenu[] = {	//Choix des modes 3 portes ou plus					//Menu permettant de choisir le mode pour le cas o� il y a trois portes ou plus
	{"Libre",					_menuPairingTroisP,	NULL,		_modeLibre},			//Mode Libre,
	{"LibreSES",				_menuPairingTroisP,	NULL,		_modeLibreSES},			//Mode LibreSES,
	{"Sortie",					_menuPairingTroisP,	NULL,		_modeSortie},			//Mode Sortie,
	{"D�verrouillage",			_menuPairingTroisP,	NULL,		_modeDeverrouillage},	//Mode D�verrouillage,
	{"Blocage",					_menuPairingTroisP,	NULL,		_modeBlocage},			//Mode Blocage,
	{"Contr�le personnalis�",	_menuPairingTroisP,	NULL,		_modeCtrlPerso},		//Mode Contr�le personnalis�,
	//Affichage LCD,			Menu pr�c�dent,		Sous-menu,	Fonction de ce champ//
	//////////////////////////////////////////////////////////////////////////////////
	{ NULL, NULL, NULL	}
};

const t_menu _modesMenu[] = {		//Affichage mode
	{"Mode actuel :",	_mainMenu,			NULL,				_menuActualMode },		//Affichage du mode actuel
	//Affichage LCD,	Menu pr�c�dent,		Sous-menu,			Fonction de ce champ//
	//////////////////////////////////////////////////////////////////////////////////
	{ NULL, NULL, NULL	}
};

const t_menu _mainMenu[] = {		//ACCUEIL
	{"Mode technicien",	NULL,				_doorNumberMenu,	NULL },					//Menu permettant de passer en mode technicien
	{"Mode normal",		NULL,				_modesMenu,			NULL },					//Menu permettant de passer en mode normal (non technicien)
	//Affichage LCD,	Menu pr�c�dent,		Sous-menu,			Fonction de ce champ//
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

		//sprintf(buf, "MENU NEW FINISHED \r\n");
		//Putstr(buf);
		menu.iIndexMenu = 0;				//Init de l'index du menu a 0
		menu.pCurrentMenu = _mainMenu;		//Le menu actuel est le menu principal (_mainMenu)
		break;
		
		case MENU_SWITCH_BUTTON:			//Quand appui bouton de gauche
		menu.iIndexMenu++;					//On incr�mente l'index pour parcourir les s�lections possibles
		if(menu.pCurrentMenu[menu.iIndexMenu].pMenuLabel == NULL) menu.iIndexMenu = 0;	//Une fois qu'on arrive a la fin des s�lections possibles, on remet l'index � 0
		Menu(MENU_PROMPT);					//On affiche le menu avec les changements apport�s
		break;
		
		case MENU_SELECT_BUTTON:			//Quand appui bouton du milieu
		if(menu.pCurrentMenu[menu.iIndexMenu].pMenuFunc)
		{
			menu.pCurrentMenu[menu.iIndexMenu].pMenuFunc(0);		//Si une fonction est disponible pour le menu actuel, on lance cette fonction
		}
		else if(menu.pCurrentMenu[menu.iIndexMenu].pSubMenu)		//Si un sous-menu est disponible, on va sur ce sous-menu
		{
			menu.pCurrentMenu = menu.pCurrentMenu[menu.iIndexMenu].pSubMenu;
			menu.iIndexMenu = 0;									//On remet l'index � 0
			Menu(MENU_PROMPT);										//On affiche le menu avec les changements apport�s
		}
		break;
		
		case MENU_BACK_BUTTON:				//Quand appui bouton de droite
		if(menu.pCurrentMenu[menu.iIndexMenu].pParentMenu)						//Si le menu poss�de un menu parent,
		{
			menu.pCurrentMenu = menu.pCurrentMenu[menu.iIndexMenu].pParentMenu;	//On ram�ne l'index sur ce menu parent
			menu.iIndexMenu = 0;												//On remet l'index � 0
			Menu(MENU_PROMPT);													//On affiche le menu avec les changements apport�s
		}
		break;

		case MENU_PROMPT:
		//Putstr(menu.pCurrentMenu[menu.iIndexMenu].pMenuLabel);	//Affichage du menu actuel pour la console
		LcdPutstr(menu.pCurrentMenu[menu.iIndexMenu].pMenuLabel,2,LcdFindCenter(strlen(menu.pCurrentMenu[menu.iIndexMenu].pMenuLabel))); //Affichage centr� des lignes du menu
		//setShellStatus(ST_MENU_MENU);
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

/////////////////////////////////////////////////////////////////R�gles g�n�rales des modes de fonctionnement : - Une seule porte ouverte � la fois, les autres sont ferm�e et verrouill�e
///////		FONCTIONS DU MENU	/////////////////////////////////											  - On refuse les demandes d'entr�e si quelqu'un est d�j� dans le sas
/////////////////////////////////////////////////////////////////

uint32_t _menuActualMode(uint32_t sc, ...)		//Cette fonction donne le mode de fonctionnement du syst�me actuel sur la ligne 3 (en partant de 0)
{
	switch(sc)
	{
		default:
		("_menuActualMode: BAD SC");
		LcdPutstr("MODE",
		3,
		LcdFindCenter(strlen("MODE"))
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
		LcdPutstr("DOOR NUMBER",
		3,
		LcdFindCenter(strlen("DOOR NUMBER"))
		);
		break;
	}
	return 0;
}

uint32_t _menuPairing(uint32_t sc, ...)			//Cette fonction g�re l'apparaige des cartes entres elles
{												//Elle devrait �tre ex�cut�e seulement une fois que le nombre de porte a �t� choisi
	switch(sc)
	{
		default:
		Putstr("_menuPairing: BAD SC");
		LcdPutstr("PAIRING PLACEHOLDER",
		3,
		LcdFindCenter(strlen("PAIRING"))
		);
		break;
	}
	return 0;
}

uint32_t _modeLibre(uint32_t sc, ...)			//Cette fonction r�gle le syst�me en mode libre
{												//Mode libre signifie que les portes sont ouvrables et fermable sans contr�le (appui boutons toujours n�cessaire)
	switch(sc)									//Cette fonction devrait �tre ex�cut�e seulement une fois l'appairage effectu�
	{
		default:
		Putstr("_modeLibre: BAD SC");
		LcdPutstr("LIBRE",
		3,
		LcdFindCenter(strlen("LIBRE"))
		);
		break;
	}
	return 0;
}

uint32_t _modeLibreSES(uint32_t sc, ...)		//Cette fonction r�gle le syst�me en mode libreSES
{												//Pareil que pour le mode Libre, sauf que plusieurs portes peuvent �tre ouvertes en m�me temps (malgr� la contre indication des r�gles g�n�rales)
	switch(sc)									//Cette fonction devrait �tre ex�cut�e seulement une fois l'appairage effectu�
	{
		default:
		Putstr("_menuLibreSES: BAD SC");
		LcdPutstr("LIBRE SES",
		3,
		LcdFindCenter(strlen("LIBRE SES"))
		);
		break;
	}
	return 0;
}

uint32_t _modeSortie(uint32_t sc, ...)			//Cette fonction r�gle le syst�me en mode Sortie
{												//Mode Sortie signifie que les portes sont libre c�t� sortie, mais elles disposent d'un contr�le en entr�e, sauf pour la premi�re
	switch(sc)									//Cette fonction devrait �tre ex�cut�e seulement une fois l'appairage effectu�
	{
		default:
		Putstr("_menuSortie: BAD SC");
		LcdPutstr("SORTIE ",
		3,
		LcdFindCenter(strlen("SORTIE"))
		);
		break;
	}
	return 0;
}

uint32_t _modeDeverrouillage(uint32_t sc, ...)	//Cette fonction r�gle le syst�me en mode Deverrouillage
{												//Mode Deverrouillage signifie que tout est en acc�s libre (ouverture de toutes les portes quelque soit la situation), ouverture d'urgence
	switch(sc)									//Cette fonction devrait �tre ex�cut�e seulement une fois l'appairage effectu�
	{
		default:
		Putstr("_menuDeverrouillage: BAD SC");
		LcdPutstr("DEVERROUILLAGE",
		3,
		LcdFindCenter(strlen("DEVERROUILLAGE"))
		);
		break;
	}
	return 0;
}

uint32_t _modeBlocage(uint32_t sc, ...)			//Cette fonction r�gle le syst�me en mode Blocage
{												//Mode Blocage signifie que personne ne peut rentrer ou sortir dans le SAS, tout est bloqu�, pupitre de commande on fonctionnel
	switch(sc)									//Cette fonction devrait �tre ex�cut�e seulement une fois l'appairage effectu�
	{
		default:
		Putstr("_menuBlocage: BAD SC");
		LcdPutstr("BLOCAGE",
		3,
		LcdFindCenter(strlen("BLOCAGE"))
		);
		break;
	}
	return 0;
}

uint32_t _modeControle(uint32_t sc, ...)		
{
	switch(sc)
	{
		default:
		Putstr("_menuControle: BAD SC");
		LcdPutstr("CONTROLE",
		3,
		LcdFindCenter(strlen("CONTROLE"))
		);
		break;
	}
	return 0;
}

uint32_t _modeControleE(uint32_t sc, ...)		//Mode Contr�le Entr�e
{												
	switch(sc)
	{
		default:
		Putstr("_menuControleE: BAD SC");
		LcdPutstr("CONTROLE E",
		3,
		LcdFindCenter(strlen("CONTROLE E"))
		);
		break;
	}
	return 0;
}

uint32_t _modeControleES(uint32_t sc, ...)		//Mod Contr�le Entr�e Sortie
{
	switch(sc)
	{
		default:
		Putstr("_menuControleES: BAD SC");
		LcdPutstr("CONTROLE E/S",
		3,
		LcdFindCenter(strlen("CONTROLE E/S"))
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
		LcdPutstr("CTRLPERSO",
		3,
		LcdFindCenter(strlen("CTRLPERSO"))
		);
		break;
	}
	return 0;
}