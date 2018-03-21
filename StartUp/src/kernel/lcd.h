/*
 * lcd.h
 *
 * Created: 14/03/2018 08:23:12
 *  Author: Thierry
 */



#define LCD_USE_8_BIT_INTERFACE


uint32_t Lcd(uint32_t sc,...);

#define LCD_NEW				100
#define LCD_DEL				101
#define LCD_BACKLIGHT_PWR	102
#define LCD_PUTSTR			103
#define LCD_IS_ON			104


#define LcdPutstr(str,line,col)		Lcd(LCD_PUTSTR,(uint32_t)str,(uint32_t)line,(uint32_t)col)
#define LcdBacklight(onOff)			Lcd(LCD_BACKLIGHT_PWR,(uint32_t)onOff)
#define IsLcdOn()					Lcd(LCD_IS_ON)


#define LCD_BUSY_TRY		20

uint8_t	LcdFindHalf(uint32_t len);