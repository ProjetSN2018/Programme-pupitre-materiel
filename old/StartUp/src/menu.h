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
 #define MENU_RETURN_HOME		108

 /////////Status/////////////////////////////
 #define ST_MENU_IDLE		0x000001
 #define ST_MENU_MENU		0x000002

 /////////PIN BUTTON/////////////////////////
 #define PIN_BUTTON_SWITCH		PIO_PA10
 #define PIN_BUTTON_SELECT		PIO_PA9
 #define PIN_BUTTON_BACK		PIO_PA8

 #define MENU_DEPTH_VALUE		10
 #define MENU_ELEM_SIZE			(sizeof(void*)+sizeof(int))
 uint32_t Menu(uint32_t sc, ...);