/*
 * LCD.h
 *
 * Created: 12/02/2018 17:20:30
 *  Author: Mod_loc
 */ 

//LCD I/O MAPPING ////////////////////////////////////////////////
#define LCD_DATA4			(PIO_PC25_IDX)
#define LCD_DATA5			(PIO_PC26_IDX)
#define LCD_DATA6			(PIO_PC28_IDX)
#define LCD_DATA7			(PIO_PB25_IDX)

#define LCD_E				(PIO_PD7_IDX)
#define LCD_RS				(PIO_PD8_IDX)

#define LCD_DATA4_MSK		0x10
#define LCD_DATA5_MSK		0x20
#define LCD_DATA6_MSK		0x40
#define LCD_DATA7_MSK		0x80


//////////////////////////////////////////////////////////////////
#define LCD_PIN_FLAGS		(PIO_TYPE_PIO_OUTPUT_1 | PIO_DEFAULT)
#define LCD_DATA4_FLAGS		LCD_PIN_FLAGS
#define LCD_DATA5_FLAGS		LCD_PIN_FLAGS
#define LCD_DATA6_FLAGS		LCD_PIN_FLAGS
#define LCD_DATA7_FLAGS		LCD_PIN_FLAGS
#define LCD_E_FLAGS			LCD_PIN_FLAGS
#define LCD_RS_FLAGS		LCD_PIN_FLAGS
//////////////////////////////////////////////////////////////////

 uint32_t Lcd(uint32_t sc, ...);
 
 #define LCD_NEW		100
