/*
 * LCD.c
 *
 * Created: 12/02/2018 17:20:12
 *  Author: Mod_loc
 */ 

#include "./Headers/appli.h"

enum{
	_LCD_PUT_DATA=1,
	_LCD_NYBBLE
};

#define _LcdPutData(data)		Lcd(_LCD_PUT_DATA,(uint32_t)data)
#define _LcdNybble()			Lcd(_LCD_NYBBLE)

 uint32_t Lcd(uint32_t sc, ...)
 {
	switch (sc)
	{
	case LCD_NEW:
		gpio_configure_pin(LCD_DATA4, LCD_PIN_FLAGS);
		gpio_configure_pin(LCD_DATA5, LCD_PIN_FLAGS);
		gpio_configure_pin(LCD_DATA6, LCD_PIN_FLAGS);
		gpio_configure_pin(LCD_DATA7, LCD_PIN_FLAGS);
		gpio_configure_pin(LCD_E,	  LCD_PIN_FLAGS);
		gpio_configure_pin(LCD_RS,    LCD_PIN_FLAGS);

		gpio_set_pin_low(LCD_DATA4);
		gpio_set_pin_low(LCD_DATA5);
		gpio_set_pin_low(LCD_DATA6);
		gpio_set_pin_low(LCD_DATA7);
		gpio_set_pin_low(LCD_E);
		gpio_set_pin_low(LCD_RS);

		delay_ms(100);
		_LcdPutData(0x30);
		delay_ms(30);
		_LcdNybble();
		delay_ms(10);
		_LcdNybble();
		delay_ms(10);
		_LcdNybble();
		delay_ms(10);
		_LcdPutData(0x20);
		_LcdNybble();

		break;


	//////// Private Services Implementation //////////////////////////////////////
	case _LCD_PUT_DATA:
#define data ((char)pa1)
		if(mIsBitsSet(data,LCD_DATA7_MSK)) gpio_set_pin_high(LCD_DATA7);
		else gpio_set_pin_low(LCD_DATA7);
		if(mIsBitsSet(data,LCD_DATA6_MSK)) gpio_set_pin_high(LCD_DATA6);
		else gpio_set_pin_low(LCD_DATA6);
		if(mIsBitsSet(data,LCD_DATA5_MSK)) gpio_set_pin_high(LCD_DATA5);
		else gpio_set_pin_low(LCD_DATA5);
		if(mIsBitsSet(data,LCD_DATA4_MSK)) gpio_set_pin_high(LCD_DATA4);
		else gpio_set_pin_low(LCD_DATA4);
#undef data
		break;

	case _LCD_NYBBLE:
		gpio_set_pin_high(LCD_E);
		delay_ms(1);
		gpio_set_pin_low(LCD_E);
		break;

	default:
		Error(ERR_LCD_SWITCH_BAD_SC, sc);
	}
 }