/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "kernel.h"


//Private Service Codes ///////////////////////////////////////////////////////

enum{
	_KRTC_DAY_BY_DATE=0,
	_KRTC_IS_LEAP_YEAR,
	_KRTC_IS_SUMMER_TIME
};

#define  _kRtcDayByDate(date,month,centyear)		kRtc(_KRTC_DAY_BY_DATE,(uint32_t)date,(uint32_t)month,(uint32_t)centyear)
#define  _kRtcIsLeapYear(centyear)					kRtc(_KRTC_IS_LEAP_YEAR,(uint32_t)centyear)
#define  _kRtcIsSummerTime(day,date,month,hour)		kRtc(_KRTC_IS_SUMMER_TIME,(uint32_t)day,(uint32_t)date,(uint32_t)month,(uint32_t)hour)

enum { JANUARY=1, FEBRUARY, MARCH, APRIL, MAY, JUNE, JULY, AUGUST, SEPTEMBER, OCTOBER, NOVEMBER, DECEMBER };
enum { SUNDAY=1, MONDAY, TUESDAY, WEDNESDAY, THURSDAY, FRIDAY, SATURDAY };

const uint8_t nbDaysInMonth[] = {31,28,31,30,31,30,31,31,30,31,30,31};

const char* daysOfWeek[] = {
    "Sunday",
    "Monday",
    "Tuesday",
    "Wednesday",
    "Thursday",
    "Friday",
    "Saturday"
};

const char* months[] ={
    "January",
    "February",
    "March",
    "April",
    "May",
    "June",
    "July",
    "August",
    "September",
    "October",
    "November",
    "December"
};

struct {
    union{
        uint8_t   status;
        struct{
            unsigned started:1;         //1 -> RTC is startded => running   0 -> not started
            unsigned leapYear:1;        //1 -> Leap year					0 -> normal
            unsigned summerTime:1;      //1 -> Summer time                  0 -> winter time
            unsigned show:1;            //1 -> Show time
            unsigned dummy:4;           // NOT USED
        };
    };
}rtc;

///SAM3U RTC REGISTERS TYPEDEFS //////////////////////////////////////////
typedef struct{
	union{
		uint32_t reg32;
		struct{
			unsigned CENT:7;
			unsigned :1;
			unsigned YEAR:8;
			unsigned MONTH:5;
			unsigned DAY:3;
			unsigned DATE:6;
			unsigned :2;
		};
	};
}t_RTC_CALR;

typedef struct{
	union{
		uint32_t reg32;
		struct{
			unsigned SEC:7;
			unsigned :1;
			unsigned MIN:7;
			unsigned :1;
			unsigned HOUR:6;
			unsigned AMPM:1;
			unsigned :9;
		};
	};
}t_RTC_TIMR;

#define _rtcCalr	((t_RTC_CALR*)&RTC->RTC_CALR)
#define _rtcTimr	((t_RTC_TIMR*)&RTC->RTC_TIMR)


uint32_t kRtc(uint32_t sc,...)
{
	uint32_t u1,u2,u3,u4;
	
	switch(sc)
	{
	case KRTC_NEW:
		////////RTC SETUP SECTION////////////////////////////////////////
		pmc_switch_sclk_to_32kxtal(PMC_OSC_XTAL);
		while (!pmc_osc_is_ready_32kxtal());
		//rtc_set_hour_mode(RTC, 0);
		//Enable RTC interrupt //////////////////////////////////////////
		NVIC_DisableIRQ(RTC_IRQn);
		NVIC_ClearPendingIRQ(RTC_IRQn);
		NVIC_SetPriority(RTC_IRQn, 1);
		NVIC_EnableIRQ(RTC_IRQn);
		rtc_enable_interrupt(RTC,RTC_IER_SECEN);	//Second IRQ enabled
		break;

	case KRTC_SET_TIME:
#define _hour	pa1
#define _minute	pa2
#define _second	pa3
#define _res	sc
		_res = rtc_set_time(RTC,_hour,_minute,_second);
		if(_res	== 0)
		{
			rtc.summerTime = _kRtcIsSummerTime(_rtcCalr->DAY,_rtcCalr->DATE,_rtcCalr->MONTH,_rtcTimr->HOUR);
		}
		return _res;
#undef _hour
#undef _minute
#undef _second
		break;

	case KRTC_SET_DATE:
#define _date		pa1
#define _month		pa2
#define _centyear	pa3

		if((_month==0)||(_month)>12) return -1;
		if(_date>nbDaysInMonth[_month-1]+_kRtcIsLeapYear(_centyear)?1:0) return -1;
		if((_centyear/100>20)||(_centyear/100<19)||(_centyear%100>99)) return -1;
#define _day	sc
		_day = _kRtcDayByDate(_date,_month,_centyear);
#define _res		sc
		_res = rtc_set_date(RTC,_centyear%100,_month,_date,_day);
#undef _day
		if(_res == 0)
		{
			rtc.summerTime = _kRtcIsSummerTime(_rtcCalr->DAY,_rtcCalr->DATE,_rtcCalr->MONTH,_rtcTimr->HOUR);
			//_rtcCalr->CENT=_centyear/100;
		}
		return _res;
#undef _date
#undef _month
#undef _centyear
#undef _res
		//no break;

	case KRTC_GETF_DATE:
#define _pStr	pa1
#define _year	u1
#define _month	u2
#define _date	u3
#define _day	u4
		rtc_get_date(RTC,&_year,&_month,&_date,&_day);
		sprintf((char*)_pStr,"%s %d %s %4d",daysOfWeek[_day-1],(int)_date,months[_month-1],(int)_year);
#undef _pStr
#undef _year
#undef _month
#undef _date
#undef _day
		//no break;

	//PRIVATE SERVICES IMPLEMENTATION ////////////////////////////////////////////////////////////////////
	//_RTCDayByDate(uint8_t date, uint8_t month, uint8_t century, uint8_t year)
	case _KRTC_DAY_BY_DATE:
#define _day		sc
#define _date		pa1
#define _month		pa2
#define _centyear	pa3
		//Mike Keith algorithm ----------------------------------------------------------------//
		if(_month>=3)
		{
			_day = ((_month*23)/9 + _date + 4 + _centyear + _centyear/4 - _centyear/100 + _centyear/400 - 2 ) % 7 + 1;
		}
		else
		{
			_day = ((_month*23)/9 + _date + 4 + _centyear + (_centyear-1)/4 - (_centyear-1)/100 + (_centyear-1)/400 ) % 7 + 1;
		}

		return _day; //Normally 1 to 7 !!!!
#undef _day
#undef _date
#undef _month
#undef _centyear
		//no break;

	case _KRTC_IS_LEAP_YEAR:
#define _centyear	pa1
		return (((_centyear%4==0)&&(_centyear%100!=0)) || (_centyear%400==0));
#undef _centyear
		//no break;

	case _KRTC_IS_SUMMER_TIME:
#define _day	pa1
#define _date	pa2
#define _month	pa3
#define _hour	pa4
		if((_month>MARCH)&&(_month<OCTOBER)) return 1;
		else
		{
			if(_month==MARCH)
			{
				if(_date>=25)
				{
					if(_day==SUNDAY)
					{
						if(_hour>=2) return 1;
					}
					else
					{
						return (((int8_t)(_date-24)-(int8_t)_day)>0);
					}
				}
			}
			else if(_month==OCTOBER)
			{
				if(_date>=25)
				{
					if(_day==SUNDAY)
					{
						if(_hour<2) return 1;
					}
					else
					{
						return (((int8_t)(_date-24)-(int8_t)_day)<=0);
					}
				}
			}
		}
		return 0;
#undef _day
#undef _date
#undef _month
#undef _hour
		//no break;


	/////// INVALID SC CODE TRAP ERROR /////////////////////////////////////////////////////////////////
	default:
		Error(ERROR_KRTC_SWITCH_BAD_SC,sc);
}
	return 0;
}

enum{
	_SHELL_PROMPT = 4
};

void RTC_Handler(void)
{
	rtc_clear_status(RTC,RTC_SCCR_SECCLR);
	PushTask(Shell,_SHELL_PROMPT,0,0);
}




//void RTCStart(void)
//{
    //rtc.hours = 23;
    //rtc.minutes = 59;
    //rtc.seconds = 55;
    //rtc.milliseconds = 0;
//
    ////Date Wednesday 4th, October 2017
    ////         4      4      10   20 17
    //rtc.century = 20;
    //rtc.year = 20;
    //rtc.day = 4;  // 1-Sunday 7-Saturday : 4-Wednesday
    //rtc.date = 28;   //
    //rtc.month = 2; //
//
    //rtc.status = 0;
    //rtc.leapYear=1;
//
    ////rtc.summerTime = 1; //
    //rtc.summerTime=_RTCIsSummerTime(rtc.day,rtc.date,rtc.month,rtc.hours);
//
    //rtc.started = 1;
//}
//
//
//
//void RTCTicTac(void)
//{
    //if(!rtc.started) return;    //Si RTC not started CHAO!!!
//
    //if(++rtc.milliseconds < 1000u) return; //Si non totalisé 1000ms -> 1s ; CIAO!!!
    //rtc.milliseconds = 0; //On repart sur une base saine !!!!
    //rtc.summerTime=_RTCIsSummerTime(rtc.day,rtc.date,rtc.month,rtc.hours);
    //if(rtc.show)
    //{
        //sprintf(buf,"\r[%c-%02d:%02d:%02d]>",
            //rtc.summerTime?'E':'H',
            //rtc.hours,
            //rtc.minutes,
            //rtc.seconds
        //);
        //Putstr(buf);
    //}
//
    //if(++rtc.seconds < 60u) return;
    //rtc.seconds = 0;
//
    //if(++rtc.minutes < 60u) return;
    //rtc.minutes = 0;
//
    //if(++rtc.hours < 24u) return;
    //rtc.hours = 0;
//
    //if(++rtc.day>7) rtc.day = 1;
//
    //if(++rtc.date <= (nbDaysInMonth[rtc.month-1] + (rtc.leapYear? 1 : 0))) return;
//
    //rtc.date=1;
//
    //if(++rtc.month<=12) return;
    //rtc.month=1;
//
    //if(++rtc.year<=99) return;
    //rtc.year=0;
//
    //if(++rtc.century<=99) return;
    //rtc.century = 0;
//
//}
//uint8_t RTCShow(uint8_t show)
//{
    //return rtc.show=show;
//}
//uint8_t RTCDisplayTime(void)
//{
    //sprintf(buf,"\e[K\r%c-%02d:%02d:%02d",
        //rtc.summerTime?'E':'H',
        //rtc.hours,
        //rtc.minutes,
        //rtc.seconds
    //);
    //Putstr(buf);
    //return 1;
//}
//
//uint8_t RTCDisplayDate(void)
//{
    //sprintf(buf,"\e[K\r%s %02d %s %02d%02d",
        //daysOfWeek[rtc.day-1],  //jour de la semaine 1-7 1: Sunday
        //rtc.date,               //date dans le mois
        //months[rtc.month-1],    //mois
        //rtc.century,            //siècle
        //rtc.year                //année
    //);
    //Putstr(buf);
    //return 1;
//}
//
//uint8_t RTCSetTime(uint8_t hh, uint8_t mm, uint8_t sec)
//{
    //if((hh>23) || (mm>59) || (sec>59)) return 0;
    //rtc.hours = hh;
    //rtc.minutes = mm;
    //rtc.seconds = sec;
    //rtc.summerTime=_RTCIsSummerTime(rtc.day,rtc.date,rtc.month,rtc.hours);
    //return 1;
//}
//
//uint8_t RTCSetDate(uint8_t date, uint8_t month, uint8_t century, uint8_t year)
//{
    //if((month==0)||(month)>12) return 0;
    ////if(date>nbDaysInMonth[month-1]) return 0;
	//if(date>nbDaysInMonth[month-1]+_RTCIsLeapYear(century,year)?1:0) return 0;
    //if((century>99)||(year>99)) return 0;
//
    //rtc.day = _RTCDayByDate(date,month,century,year);
//
    //rtc.date = date;
    //rtc.month = month;
    //rtc.century = century;
    //rtc.year = year;
    //rtc.summerTime=_RTCIsSummerTime(rtc.day,rtc.date,rtc.month,rtc.hours);
    //return 1;
//}
//
//
//////////////////////////////////////////////////////////////////////////////////
////IMPLEMENTATION DES FONCTIONS PRIVEES
//
//uint8_t _RTCDayByDate(uint8_t date, uint8_t month, uint8_t century, uint8_t year)
//{
    //uint8_t  D; //Le jour de la semaine
    //uint16 y;
//
    //y = century*100 + year;
//
    ////Algorithme de Mike Keith ----------------------------------------------------------------//
    //if(month>=3)
    //{
        //D = ( ((uint16)month*23)/9 + date + 4 + y + y/4 - y/100 + y/400 - 2 ) % 7 + 1;
    //}
    //else
    //{
        //D = ( ((uint16)month*23)/9 + date + 4 + y + (y-1)/4 - (y-1)/100 + (y-1)/400 ) % 7 + 1;
    //}
//
    //return D; //Normaly 1 to 7 !!!!
//}
//
//uint8_t  _RTCIsLeapYear(uint8_t century, uint8_t year)
//{
    //uint16 centYear = ((uint16)century)*100 + (uint16)year;
    //return (((centYear%4==0)&&(centYear%100!=0)) || (centYear%400==0));
//}
//
//uint8_t  _RTCIsSummerTime(uint8_t day, uint8_t date, uint8_t month, uint8_t hours)
//{
    //if((month>MARCH)&&(month<OCTOBER))
        //return 1;
    //else
    //{
        //if(month==MARCH)
        //{
            //if(date>=25)
            //{
                //if(day==SUNDAY)
                //{
                    //if(hours>=2) return 1;
                //}
                //else
                //{
                    //return (((int8)(date-24)-(int8)day)>0);
                //}
            //}
        //}
        //else if(month==OCTOBER)
        //{
            //if(date>=25)
            //{
                //if(day==SUNDAY)
                //{
                    //if(hours<2) return 1;
                //}
                //else
                //{
                    //return (((int8)(date-24)-(int8)day)<=0);
                //}
            //}
        //}
    //}
    //return 0;
//}

/* [] END OF FILE */
