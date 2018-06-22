/*
 * command.c
 *
 * Created: 11/03/2018 13:15:55
 *  Author: Thierry
 */

#include "appli.h"
#include "command.h"
#include "commandMap.h"
#include "./kernel/utils.h"
#include "./kernel/CRC16MODBUS.h"
#include <string.h>

//ToDo: insert your command function implementations below ////////////
//
//      void MyCommandFunc(uint32_t sc, void*pParam)
//      {
//          //Your implementation
//
//          //Must not contain infinite loop nor system delay calls!
//
//      }
///////////////////////////////////////////////////////////////////////

char buf[64];

void set(uint32_t sc, void*pParam)
{
	Putstr("\r\n\t\tOverload the default set command!\r\n");
	_cmd_set(sc,pParam);
}

#define TEST_NEW		100
#define TEST_RUN		102


void Test(uint32_t sc, uint32_t phase);

enum{
	PH_CMD_MOSFET=1,
	PH_CMD_MOT_SERRURE,
	PH_CMD_RELAIS,
	PH_CMD_RELAIS_1,
	PH_CMD_RELAIS_2
};

void Test(uint32_t sc, uint32_t phase)
{
	switch(sc)
	{
	case TEST_NEW:
		Putstr("\r\nTest started...\r\n");
		phase<<=8;
		phase&=0xFFFF00;
		phase|=PH_CMD_MOSFET;
		//no break;
	case TEST_RUN:
		sprintf(buf,"\r\nPH:%d--%d\r\n",*((uint16_t*)((uint8_t*)&phase+1)),*((char*)&phase));
		Putstr(buf);

		Putstr(PENE_OUVERT_NAME);			Putstr("\t: ");		gpio_pin_is_low(PENE_OUVERT)?Putstr("ON"):Putstr("OFF");		Putstr("\r\n");
		Putstr(PENE_FERME_NAME);			Putstr("\t: ");		gpio_pin_is_low(PENE_FERME)?Putstr("ON"):Putstr("OFF");			Putstr("\r\n");
		Putstr(IN_ILS_NAME);				Putstr("\t\t: ");	gpio_pin_is_low(IN_ILS)?Putstr("ON"):Putstr("OFF");				Putstr("\r\n");
		Putstr(IN_SV_NAME);					Putstr("\t\t: ");	gpio_pin_is_low(IN_SV)?Putstr("ON"):Putstr("OFF");				Putstr("\r\n");
		Putstr(IN_RADAR_NAME);				Putstr("\t\t: ");	gpio_pin_is_low(IN_RADAR)?Putstr("ON"):Putstr("OFF");			Putstr("\r\n");
		Putstr(IN_DVU_NAME);				Putstr("\t\t: ");	gpio_pin_is_low(IN_DVU)?Putstr("ON"):Putstr("OFF");				Putstr("\r\n");
		Putstr(IN_PREMIERE_ENTREE_NAME);	Putstr("\t: ");		gpio_pin_is_low(IN_PREMIERE_ENTREE)?Putstr("ON"):Putstr("OFF");	Putstr("\r\n");
		Putstr(IN_DVU_GENERAL_NAME);		Putstr("\t: ");		gpio_pin_is_low(IN_DVU_GENERAL)?Putstr("ON"):Putstr("OFF");		Putstr("\r\n");
		Putstr(IN_ENTREE_LIBRE_NAME);		Putstr("\t: ");		gpio_pin_is_low(IN_ENTREE_LIBRE)?Putstr("ON"):Putstr("OFF");	Putstr("\r\n");
		Putstr(IN_ENTREE_OPTION_NAME);		Putstr("\t: ");		gpio_pin_is_low(IN_ENTREE_OPTION)?Putstr("ON"):Putstr("OFF");	Putstr("\r\n");

		switch(mBitsMsk(phase,0xFF))
		{
		case PH_CMD_MOSFET:
			gpio_set_pin_high(CMD_MOSFET);
			break;
		case PH_CMD_MOT_SERRURE:
			gpio_set_pin_low(CMD_MOSFET);
			gpio_set_pin_high(CMD_MOT_SERRURE);
			break;
		case PH_CMD_RELAIS:
			gpio_set_pin_low(CMD_MOT_SERRURE);
			gpio_set_pin_high(CMD_RELAIS);
			break;
		case PH_CMD_RELAIS_1:
			gpio_set_pin_low(CMD_RELAIS);
			gpio_set_pin_high(CMD_RELAIS_1);
			break;
		case PH_CMD_RELAIS_2:
			gpio_set_pin_low(CMD_RELAIS_1);
			gpio_set_pin_high(CMD_RELAIS_2);
			break;
		default:
			gpio_set_pin_low(CMD_MOSFET);
			gpio_set_pin_low(CMD_MOT_SERRURE);
			gpio_set_pin_low(CMD_RELAIS);
			gpio_set_pin_low(CMD_RELAIS_1);
			gpio_set_pin_low(CMD_RELAIS_2);

			if(*((uint16_t*)((uint8_t*)&phase+1)))
			{
				if(--(*((uint16_t*)((uint8_t*)&phase+1))))
				{
					PushTask(Test,TEST_NEW,phase>>8,0);
				}
			}
			Putstr("\r\nTest terminated...\r\n");
			return;
		}
		break;
	default:
		return;
	}
	PushTask(Test,TEST_RUN,++phase,1000);
}



void start(uint32_t sc, void*pParam)
{
	char *pToken;
	pToken = mstrtok(NULL," \r\n");
	if(pToken)
	{
		switch(CRC16MODBUSFRAME((unsigned char*)pToken,strlen(pToken)))
		{
		case 0x51DB:		//attach
			

			break;
		case 0xC698:	//lcd
			Lcd(LCD_DEL);
			Lcd(LCD_NEW);
			break;
		case 0x0634:	//backlight
			LcdBacklight(true);
			break;

		case 0xDC2E:	//test
			mstrtokRestore();
			pToken = mstrtok(NULL," \r\n");
			if(pToken)
			{
				 sc=atoi((char*)pToken);
				 sprintf(buf,"\r\nNbTest: %d\r\n",(int)sc);
				 Putstr(buf);
			}
			else
			{
				sc=1;
			}
			//sc=0xD3C2B1A0;
			//sprintf(buf,"\r\n\t%02X %02X %02X %02X\r\n",
					//*(char*)((char*)&sc+0),
					//*(char*)((char*)&sc+1),
					//*(char*)((char*)&sc+2),
					//*(char*)((char*)&sc+3)
			//);
			//Putstr(buf);
			PushTask(Test,TEST_NEW,sc,0);
			break;

		case 0x312B:    //bluetooth
			gpio_configure_pin(BLUETOOTH_PWR,BLUETOOTH_FLAGS);
#if BLUETOOTH_ACTIVE_LEVEL==1
			gpio_set_pin_high(BLUETOOTH_PWR);
#else
			gpio_set_pin_low(BLUETOOTH_PWR);
#endif
			break;

		case 0x2616:	//buzzer
			mstrtokRestore();
			pToken = mstrtok(NULL," \r\n");
			pParam=atoi((char*)pToken);
			if(pParam)
			{
				Putstr("\r\nBuzzer on for ");
				sprintf(buf,"%d ms...\r\n",(int)pParam);
				Putstr(buf);
				gpio_configure_pin(BUZZER_PWR,BUZZER_FLAGS);
//#if BUZZER_ACTIVE_LEVEL==1
				//gpio_set_pin_high(BUZZER_PWR);
//#else
				//gpio_set_pin_low(BUZZER_PWR);
//#endif
				{
					pwm_channel_t	pwmChannelInstance;
					const pwm_clock_t clockSetting ={
						.ul_clka = 1000*100,
						.ul_clkb = 1000*100,
						.ul_mck = 12000000
					};
					pmc_enable_periph_clk(ID_PWM);
					pwm_channel_disable(PWM,PWM_CHANNEL_3);
					pwm_init(PWM,(pwm_clock_t*)&clockSetting);
					pwmChannelInstance.ul_prescaler=PWM_CMR_CPRE_CLKB;
					pwmChannelInstance.ul_period=100;
					pwmChannelInstance.ul_duty=50;
					pwmChannelInstance.channel=PWM_CHANNEL_3;
					pwm_channel_init(PWM,&pwmChannelInstance);
					pwm_channel_enable(PWM,PWM_CHANNEL_3);
				}
				PushTask(stop,0x2616,0,(uint32_t)pParam);
			}
			break;
		default:
			break;
		}
	}
	mstrtokRestore();
}

void stop(uint32_t sc, void*pParam)
{
	char *pToken;
	switch(sc)
	{
	case 0x51DB:		//attach
			

		break;
	case 0x2616:	//buzzer
		Putstr("\r\nBuzzer off...\r\n");
//#if BUZZER_ACTIVE_LEVEL==1
		//gpio_set_pin_low(BUZZER_PWR);
//#else
		//gpio_set_pin_high(BUZZER_PWR);
//#endif
		break;
	default:
		pToken = mstrtok(NULL," \r\n");
		if(pToken)
		{
			switch(CRC16MODBUSFRAME((unsigned char*)pToken,strlen(pToken)))
			{
			case 0xC698:	//lcd
				Lcd(LCD_DEL);
				break;
			case 0x0634:	//backlight
				LcdBacklight(false);
				break;

			case 0x312B:    //bluetooth
#if BLUETOOTH_ACTIVE_LEVEL==1
				gpio_set_pin_low(BLUETOOTH_PWR);
#else
				gpio_set_pin_high(BLUETOOTH_PWR);
#endif
				break;
			case 0x2616:	//buzzer
				Putstr("\r\nBuzzer off...\r\n");
#if BUZZER_ACTIVE_LEVEL==1
				gpio_set_pin_low(BUZZER_PWR);
#else
				gpio_set_pin_high(BUZZER_PWR);
#endif
				break;
			default:
				break;
			}
		}
		mstrtokRestore();
		break;
	}
}

uint32_t registerValue[4];

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
		struct{
			unsigned SEC_QL:4;
			unsigned SEC_QH:3;
			unsigned :1;
			unsigned MIN_QL:4;
			unsigned MIN_QH:3;
			unsigned :1;
			unsigned HOUR_QL:4;
			unsigned HOUR_QH:2;
			unsigned :1;
			unsigned :9;
		};
	};
}t_RTC_TIMR;


void send(uint32_t sc, void*pParam)
{
	char *pToken;
	pToken = mstrtok(NULL," \r\n");
	if(pToken)
	{
		switch(CRC16MODBUSFRAME((unsigned char*)pToken,strlen(pToken)))
		{
		case 0xB340:		//synchro
			Putstr("\r\n\tBroadcast synchronization token\r\n");
			registerValue[0]=RTC->RTC_TIMR;
			if((((t_RTC_TIMR*)&registerValue[0])->SEC_QL+=2)>9)
			{
				((t_RTC_TIMR*)&registerValue[0])->SEC_QL=0;
				if((((t_RTC_TIMR*)&registerValue[0])->SEC_QH++)>5)
				{
					((t_RTC_TIMR*)&registerValue[0])->SEC_QH=0;
					if((((t_RTC_TIMR*)&registerValue[0])->MIN_QL++)>9)
					{
						((t_RTC_TIMR*)&registerValue[0])->MIN_QL=0;
						if((((t_RTC_TIMR*)&registerValue[0])->MIN_QH++)>5)
						{
							((t_RTC_TIMR*)&registerValue[0])->MIN_QH=0;
							if((((t_RTC_TIMR*)&registerValue[0])->HOUR_QL++)>9)
							{
								((t_RTC_TIMR*)&registerValue[0])->HOUR_QL=0;
								if((((t_RTC_TIMR*)&registerValue[0])->HOUR_QH++)>2)
								{
									((t_RTC_TIMR*)&registerValue[0])->HOUR_QH=0;
								}
							}
						}
					}
				}
			}
			registerValue[1]=RTC->RTC_CALR;
			WriteMultipleRegisters(MODBUS_BROADCAST_ADDRESS,0x1000,4,registerValue); 
			break;
			case 0x51DB:
				
				break;
		default:
			break;
		}
	}
	mstrtokRestore();
}
