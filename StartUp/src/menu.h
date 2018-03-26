/* * menu.h
 *
 * Created: 16/02/2018 14:29:26
 *  Author: eleve
 */ 
 #define MENU_NEW				100
 #define MENU_SWITCH_BUTTON		101
 #define MENU_SELECT_BUTTON		102
 #define MENU_BACK_BUTTON		103
 #define MENU_SELECTED			104
 #define MENU_PROMPT			105
 #define MENU_LANGUAGE			106
 #define MENU_INIT				107

 /////////Status/////////////////////////////
 #define ST_MENU_IDLE		0x000001
 #define ST_MENU_MENU		0x000002

 /////////PIN BUTTON/////////////////////////
 #define PIN_BUTTON_SWITCH		PIO_PA10
 #define PIN_BUTTON_SELECT		PIO_PA9
 #define PIN_BUTTON_BACK		PIO_PA8

 uint32_t Menu(uint32_t sc, ...);

 #define NB_MENU		3
 #define NB_SUB_MENU	3