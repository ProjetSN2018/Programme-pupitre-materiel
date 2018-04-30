/*
 * lcd.c
 *
 * Created: 14/03/2018 08:22:59
 *  Author: Thierry
 */
#include "kernel.h"
#define LCD_PIN_FLAGS				(PIO_TYPE_PIO_OUTPUT_0 | PIO_DEFAULT)
#define LCD_PIN_ACTIVE_LEVEL		1

#define LCD_DB4						(PIO_PA0_IDX)
#define LCD_DB4_NAME				"LCD_DB4"

#define LCD_DB5						(PIO_PA1_IDX)
#define LCD_DB5_NAME				"LCD_DB5"

#define LCD_DB6						(PIO_PA2_IDX)
#define LCD_DB6_NAME				"LCD_DB5"

#define LCD_DB7						(PIO_PA3_IDX)
#define LCD_DB7_NAME				"LCD_DB7"

#define LCD_E						(PIO_PA4_IDX)
#define LCD_E_NAME					"LCD_E"

#define LCD_RWB						(PIO_PA5_IDX)
#define LCD_RWB_NAME				"LCD_RWB"

#define LCD_RS						(PIO_PA6_IDX)
#define LCD_RS_NAME					"LCD_RS"

#define LCD_BACKLIGHT				(PIO_PA7_IDX)
#define LCD_BACKLIGHT_NAME			"LCD_BACKLIGHT"

///LCD PRIVATE SERVICE CODES////////////////////////////////////////
enum{
	_LCD_WRITE_HALF = 1,
	_LCD_WRITE_INST,
	_LCD_WRITE_DATA,
	_LCD_E_STROBE
};

#define _LcdWriteHalf(inst)		Lcd(_LCD_WRITE_HALF,(uint32_t)inst)
#define _LcdWriteInst(inst)		Lcd(_LCD_WRITE_INST,(uint32_t)inst)
#define _LcdWriteData(data)		Lcd(_LCD_WRITE_DATA,(uint32_t)data)
#define _LcdEstrobe()			Lcd(_LCD_E_STROBE)


uint32_t Lcd(uint32_t sc, ...)
{
	switch(sc)
	{
	case LCD_NEW:
		gpio_configure_pin(LCD_DB4,LCD_PIN_FLAGS);
		gpio_configure_pin(LCD_DB5,LCD_PIN_FLAGS);
		gpio_configure_pin(LCD_DB6,LCD_PIN_FLAGS);
		gpio_configure_pin(LCD_DB7,LCD_PIN_FLAGS);
		gpio_configure_pin(LCD_E,LCD_PIN_FLAGS);
		gpio_configure_pin(LCD_RWB,LCD_PIN_FLAGS);
		gpio_configure_pin(LCD_RS,LCD_PIN_FLAGS);
		gpio_configure_pin(LCD_BACKLIGHT,LCD_PIN_FLAGS);
		gpio_set_pin_low(LCD_E);
		gpio_set_pin_low(LCD_RS);
		gpio_set_pin_low(LCD_RWB);

		delay_ms(100);
		_LcdWriteHalf(0x30);
		delay_ms(10);
		_LcdWriteHalf(0x30);
		delay_ms(2);
		_LcdWriteHalf(0x30);
		delay_ms(2);
		_LcdWriteHalf(0x20);
		delay_ms(2);
		_LcdWriteInst(0x28);	//SET FUNCTION : 4 bits interface/2 line display/5x7 dots
		_LcdWriteInst(0x08);	//DISPLAY OFF
		_LcdWriteInst(0x01);	//DISPLAY CLEAR
		_LcdWriteInst(0x06);	//ENTRY MODE SET : Cursor Increase/Display is not shift
		_LcdWriteInst(0x0F);	//DISPLAY ON/Cursor on
		break;

	case LCD_BACKLIGHT_PWR:
#define _onOff	(unsigned)pa1
		if(_onOff)
#if LCD_PIN_ACTIVE_LEVEL==1
			gpio_set_pin_high(LCD_BACKLIGHT);
#else
			gpio_set_pin_low(LCD_BACKLIGHT);
#endif
		else
#if LCD_PIN_ACTIVE_LEVEL==1
			gpio_set_pin_low(LCD_BACKLIGHT);
#else
			gpio_set_pin_high(LCD_BACKLIGHT);
#endif

#undef _onOff
		break;

	///LCD PRIVATE SERVICE IMPLEMENTATION //////////////////////////////////////////
#define _inst		pa1
	case _LCD_WRITE_HALF:
		gpio_set_pin_low(LCD_RS);
		(_inst&0x80)?gpio_set_pin_high(LCD_DB7):gpio_set_pin_low(LCD_DB7);
		(_inst&0x40)?gpio_set_pin_high(LCD_DB6):gpio_set_pin_low(LCD_DB6);
		(_inst&0x20)?gpio_set_pin_high(LCD_DB5):gpio_set_pin_low(LCD_DB5);
		(_inst&0x10)?gpio_set_pin_high(LCD_DB4):gpio_set_pin_low(LCD_DB4);
		Lcd(_LCD_E_STROBE);
		break;
	case _LCD_WRITE_INST:
		gpio_set_pin_low(LCD_RS);
		_LcdWriteHalf(_inst);
		_LcdWriteHalf(_inst<<4);
		break;
#undef _inst

#define _data pa1
	case _LCD_WRITE_DATA:
		gpio_set_pin_high(LCD_RS);
		(_data&0x80)?gpio_set_pin_high(LCD_DB7):gpio_set_pin_low(LCD_DB7);
		(_data&0x40)?gpio_set_pin_high(LCD_DB6):gpio_set_pin_low(LCD_DB6);
		(_data&0x20)?gpio_set_pin_high(LCD_DB5):gpio_set_pin_low(LCD_DB5);
		(_data&0x10)?gpio_set_pin_high(LCD_DB4):gpio_set_pin_low(LCD_DB4);
		Lcd(_LCD_E_STROBE);
		(_data&0x08)?gpio_set_pin_high(LCD_DB7):gpio_set_pin_low(LCD_DB7);
		(_data&0x04)?gpio_set_pin_high(LCD_DB6):gpio_set_pin_low(LCD_DB6);
		(_data&0x02)?gpio_set_pin_high(LCD_DB5):gpio_set_pin_low(LCD_DB5);
		(_data&0x01)?gpio_set_pin_high(LCD_DB4):gpio_set_pin_low(LCD_DB4);
		Lcd(_LCD_E_STROBE);
		gpio_set_pin_low(LCD_DB7);
		gpio_set_pin_low(LCD_DB6);
		gpio_set_pin_low(LCD_DB5);
		gpio_set_pin_low(LCD_DB4);
		break;
#undef _data

	case _LCD_E_STROBE:
		gpio_set_pin_high(LCD_E);
		delay_us(300);
		gpio_set_pin_low(LCD_E);
		delay_us(300);
		break;

	/////// INVALID SC CODE TRAP ERROR /////////////////////////////////////////////////////////////////
	default:
		Error(ERROR_LCD_SWITCH_BAD_SC,sc);
	}
	return 0;
}