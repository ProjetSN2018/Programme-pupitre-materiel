/*
 * lcd.c
 *
 * Created: 14/03/2018 08:22:59
 *  Author: Thierry
 */
#include "kernel.h"
#define LCD_PIN_OUT_FLAGS			(PIO_TYPE_PIO_OUTPUT_0 | PIO_DEFAULT)
#define LCD_PIN_ACTIVE_LEVEL		1

#define LCD_PIN_IN_FLAGS			(PIO_TYPE_PIO_INPUT | PIO_DEFAULT)


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

#ifdef LCD_USE_8_BIT_INTERFACE
#define LCD_DB0						(PIO_PB8_IDX)
#define LCD_DB0_NAME				"LCD_DB0"

#define LCD_DB1						(PIO_PB9_IDX)
#define LCD_DB1_NAME				"LCD_DB1"

#define LCD_DB2						(PIO_PB10_IDX)
#define LCD_DB2_NAME				"LCD_DB2"

#define LCD_DB3						(PIO_PB11_IDX)
#define LCD_DB3_NAME				"LCD_DB3"

#endif


///LCD PRIVATE SERVICE CODES////////////////////////////////////////
enum{
	_LCD_WRITE_BYTE = 1,
	_LCD_WRITE_INST,
	_LCD_WRITE_DATA,
	_LCD_E_STROBE,
	_LCD_E_UP,
	_LCD_E_DOWN,
	_LCD_SET_RAM_ADDRESS,
	_LCD_IS_BUSY,
	_LCD_SET_DATA_LINE_OUT,
	_LCD_SET_DATA_LINE_IN
};

#define _LcdWriteByte(inst)			Lcd(_LCD_WRITE_BYTE,(uint32_t)inst)
#define _LcdWriteInst(inst)			Lcd(_LCD_WRITE_INST,(uint32_t)inst)
#define _LcdWriteData(data)			Lcd(_LCD_WRITE_DATA,(uint32_t)data)
#define _LcdEstrobe()				Lcd(_LCD_E_STROBE)
#define _LcdEup()					Lcd(_LCD_E_UP)
#define _LcdEdown()					Lcd(_LCD_E_DOWN)
#define _LcdSetCursor(line,col)		Lcd(_LCD_SET_RAM_ADDRESS,(uint32_t)line,(uint32_t)col)
#define _LcdIsBusy()				Lcd(_LCD_IS_BUSY)
#define _LcdSetDataLineOut()		Lcd(_LCD_SET_DATA_LINE_OUT)
#define _LcdSetDataLineIn()			Lcd(_LCD_SET_DATA_LINE_IN)


const char line2addr[]={ 0x00, 0x40, 0x14, 0x54 };
char buf[16];

struct {
	uint32_t status;
}lcd;

#define ST_LCD_ON		0x0000001


uint32_t Lcd(uint32_t sc, ...)
{
	switch(sc)
	{
	case LCD_NEW:
		_LcdSetDataLineOut();
		gpio_configure_pin(LCD_E,			LCD_PIN_OUT_FLAGS);
		gpio_configure_pin(LCD_RWB,			LCD_PIN_OUT_FLAGS);
		gpio_configure_pin(LCD_RS,			LCD_PIN_OUT_FLAGS);
		gpio_configure_pin(LCD_BACKLIGHT,	LCD_PIN_OUT_FLAGS);

		gpio_set_pin_low(LCD_E);
		gpio_set_pin_low(LCD_RS);
		gpio_set_pin_low(LCD_RWB);
		gpio_set_pin_high(LCD_BACKLIGHT);

		delay_ms(500);

#ifdef LCD_USE_8_BIT_INTERFACE
		//8 bit data interface/////////////////////////////
		_LcdWriteInst(0x30);
		delay_ms(6);
		_LcdWriteInst(0x30);
		delay_ms(2);
		_LcdWriteInst(0x30);
		delay_ms(2);
		_LcdWriteInst(0x38);	//SET FUNCTION : 8 bits interface/2 line display/5x7 dots
#else
		//4 bit data interface/////////////////////////////
		_LcdWriteByte(0x30);
		delay_ms(10);
		_LcdWriteByte(0x30);
		delay_ms(10);
		_LcdWriteByte(0x30);
		delay_ms(10);
		_LcdWriteByte(0x20);
		delay_ms(10);
		_LcdWriteInst(0x28);	//SET FUNCTION : 4 bits interface/2 line display/5x7 dots
#endif
		_LcdWriteInst(0x08);	//DISPLAY OFF
		_LcdWriteInst(0x01);	//DISPLAY CLEAR
		_LcdWriteInst(0x06);	//ENTRY MODE SET : Cursor Increase/Display is not shift
		_LcdWriteInst(0x0C);	//DISPLAY ON/CURSOR OFF

		lcd.status = ST_LCD_ON;
		break;

	case LCD_DEL:
		_LcdWriteInst(0x08);	//DISPLAY OFF
		_LcdSetDataLineIn();
		gpio_configure_pin(LCD_E,			LCD_PIN_OUT_FLAGS);
		gpio_configure_pin(LCD_RWB,			LCD_PIN_OUT_FLAGS);
		gpio_configure_pin(LCD_RS,			LCD_PIN_OUT_FLAGS);
		gpio_configure_pin(LCD_BACKLIGHT,	LCD_PIN_OUT_FLAGS);
		gpio_set_pin_low(LCD_E);
		gpio_set_pin_low(LCD_RS);
		gpio_set_pin_low(LCD_RWB);
		gpio_set_pin_low(LCD_BACKLIGHT);
		lcd.status=0;
		break;

	case LCD_IS_ON:
		return (uint32_t)(mIsBitsSet(lcd.status,ST_LCD_ON));
		//no break;

	case LCD_PUTCH:
#define _ch			pa1
#define _line		pa2
#define _col		pa3
		_LcdSetCursor(_line,_col);
		_LcdWriteData(_ch);
#undef _ch
#undef _line
#undef _col
break;

	case LCD_PUTSTR:
#define _str		pa1
#define _line		pa2
#define _col		pa3
		_LcdSetCursor(_line,_col);
		while(*(char*)_str)
		{
			_LcdWriteData(*(char*)_str++);
		}
#undef _str
#undef _line
#undef _col
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
	case _LCD_WRITE_BYTE:
#define _byte	((char)pa1)
		_LcdSetDataLineOut();
		gpio_set_pin_low(LCD_RWB);
		_LcdEup();
		(_byte&0x80)?gpio_set_pin_high(LCD_DB7):gpio_set_pin_low(LCD_DB7);
		(_byte&0x40)?gpio_set_pin_high(LCD_DB6):gpio_set_pin_low(LCD_DB6);
		(_byte&0x20)?gpio_set_pin_high(LCD_DB5):gpio_set_pin_low(LCD_DB5);
		(_byte&0x10)?gpio_set_pin_high(LCD_DB4):gpio_set_pin_low(LCD_DB4);
#ifdef LCD_USE_8_BIT_INTERFACE
		(_byte&0x08)?gpio_set_pin_high(LCD_DB3):gpio_set_pin_low(LCD_DB3);
		(_byte&0x04)?gpio_set_pin_high(LCD_DB2):gpio_set_pin_low(LCD_DB2);
		(_byte&0x02)?gpio_set_pin_high(LCD_DB1):gpio_set_pin_low(LCD_DB1);
		(_byte&0x01)?gpio_set_pin_high(LCD_DB0):gpio_set_pin_low(LCD_DB0);
#endif		
#undef _byte		
		_LcdEdown();
		break;

	case _LCD_WRITE_INST:
#define _inst	((char)pa1)
		gpio_set_pin_low(LCD_RS);
		_LcdWriteByte(_inst);
#ifndef LCD_USE_8_BIT_INTERFACE
		_LcdWriteByte(_inst<<4);
#endif
#undef _inst
		break;

	case _LCD_WRITE_DATA:
#define _data	((char)pa1)
		gpio_set_pin_high(LCD_RS);
		_LcdWriteByte(_data);
#ifndef LCD_USE_8_BIT_INTERFACE
		_LcdWriteByte(_data<<4);
#endif
#undef _data
		break;

	case _LCD_E_STROBE:
		delay_us(500);
		gpio_set_pin_high(LCD_E);
		delay_us(100);
		gpio_set_pin_low(LCD_E);
		delay_us(500);
		break;

	case _LCD_E_UP:
		delay_us(500);
		gpio_set_pin_high(LCD_E);
		delay_us(50);
		break;

	case _LCD_E_DOWN:
		delay_us(50);
		gpio_set_pin_low(LCD_E);
		delay_us(500);
		break;

	case _LCD_SET_RAM_ADDRESS:
#define _nLine	pa1
#define _nCol	pa2
		_LcdWriteInst((line2addr[_nLine&0x03]+_nCol)|0x80);
#undef _nLine
#undef _nCol
		break;


	case _LCD_IS_BUSY:
		break;
#define _lcdStatus	sc
		_LcdSetDataLineIn();
		gpio_set_pin_low(LCD_RS);
		gpio_set_pin_high(LCD_RWB);
		delay_us(50);
		gpio_set_pin_high(LCD_E);
		delay_us(50);
		_lcdStatus = (gpio_pin_is_high(LCD_DB7)<<7)|(gpio_pin_is_high(LCD_DB6)<<6)|(gpio_pin_is_high(LCD_DB5)<<5)|(gpio_pin_is_high(LCD_DB4)<<4);
		delay_us(50);
		gpio_set_pin_low(LCD_E);
		delay_us(50);
		gpio_set_pin_high(LCD_E);
		delay_us(50);
		_lcdStatus |= (gpio_pin_is_high(LCD_DB7)<<3)|(gpio_pin_is_high(LCD_DB6)<<2)|(gpio_pin_is_high(LCD_DB5)<<1)|(gpio_pin_is_high(LCD_DB4)<<0);
		delay_us(50);
		gpio_set_pin_low(LCD_E);
		delay_us(50);
		_LcdSetDataLineOut();
		return (uint32_t)((_lcdStatus&0x80)==0x80);
#undef _lcdStatus
		//no break;

	case _LCD_SET_DATA_LINE_OUT:
		gpio_configure_pin(LCD_DB4,			LCD_PIN_OUT_FLAGS);
		gpio_configure_pin(LCD_DB5,			LCD_PIN_OUT_FLAGS);
		gpio_configure_pin(LCD_DB6,			LCD_PIN_OUT_FLAGS);
		gpio_configure_pin(LCD_DB7,			LCD_PIN_OUT_FLAGS);
#ifdef LCD_USE_8_BIT_INTERFACE
		gpio_configure_pin(LCD_DB0,			LCD_PIN_OUT_FLAGS);
		gpio_configure_pin(LCD_DB1,			LCD_PIN_OUT_FLAGS);
		gpio_configure_pin(LCD_DB2,			LCD_PIN_OUT_FLAGS);
		gpio_configure_pin(LCD_DB3,			LCD_PIN_OUT_FLAGS);
#endif
		break;

	case _LCD_SET_DATA_LINE_IN:
		gpio_configure_pin(LCD_DB4,			LCD_PIN_IN_FLAGS);
		gpio_configure_pin(LCD_DB5,			LCD_PIN_IN_FLAGS);
		gpio_configure_pin(LCD_DB6,			LCD_PIN_IN_FLAGS);
		gpio_configure_pin(LCD_DB7,			LCD_PIN_IN_FLAGS);
#ifdef LCD_USE_8_BIT_INTERFACE
		gpio_configure_pin(LCD_DB0,			LCD_PIN_IN_FLAGS);
		gpio_configure_pin(LCD_DB1,			LCD_PIN_IN_FLAGS);
		gpio_configure_pin(LCD_DB2,			LCD_PIN_IN_FLAGS);
		gpio_configure_pin(LCD_DB3,			LCD_PIN_IN_FLAGS);
#endif
		break;

	/////// INVALID SC CODE TRAP ERROR /////////////////////////////////////////////////////////////////
	default:
		Error(ERROR_LCD_SWITCH_BAD_SC,sc);
	}
	return 0;
}

uint8_t LcdFindHalf(uint32_t len)
{
	float col;
	col = 20 - len;
	col = col/2;
	return col;
}