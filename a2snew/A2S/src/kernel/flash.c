/*
 * flash.c
 *
 * Created: 04/06/2018 18:44:46
 *  Author: lavip
 */ 

#include "kernel.h"
extern char * buf[];
struct {
	uint32_t flashInit;
	uint32_t testPageAddr;
	uint32_t *pul_test_page;
	uint32_t ul_idx;
	uint8_t uc_key;
	uint32_t ul_page_buffer[6];
}flash;

uint32_t * flashLoadBuffer[15];
uint32_t Flash(uint32_t sc, ...)
{
	switch(sc)
	{
		case FLASH_NEW:
			Putstr("\r\nshell\r\n");
			
			break;
		case FLASH_LOAD:

			if(nvm_read(INT_FLASH, TEST_PAGE_ADDRESS, (void *)flashLoadBuffer, sizeof(flashLoadBuffer))
			== STATUS_OK)
			{
				Putstr("\r\n\tLOAD OK NOT EMPTY\r\n");
			}
			if(flashLoadBuffer[0] != 1) return 0;
			return 1;
			break;
			
		case FLASH_SAVE:
			flash.ul_page_buffer[IS_CONFIG_SAVED] = 1;
#ifdef MASTER
		
			flash.ul_page_buffer[NB_SLAVES] = Modbus(MODBUS_GET_NB_SLAVES);
			//sprintf(buf, "\r\n\tFONCTION::: %d", Modbus(MODBUS_GET_NB_SLAVES));
			//Putstr(buf);
			for(int i = 0; i < flash.ul_page_buffer[NB_SLAVES]; i++)
			{
				flash.ul_page_buffer[START_ID_SLAVE_REGISTER + i] = Modbus(MODBUS_GET_ID_SLAVE, i);
			}
			
#else
			
			flash.ul_page_buffer[NB_SLAVES] = 0;

			flash.ul_page_buffer[START_ID_SLAVE_REGISTER] = GetModuleAddr();
#endif
			if(nvm_write(INT_FLASH, TEST_PAGE_ADDRESS, (void *)flash.ul_page_buffer, sizeof(flash.ul_page_buffer)) ==
			STATUS_OK)
			{
				sprintf(buf, "\r\n\tSAVE OK\r\n");
				Putstr(buf);
			}
			break;

		case FLASH_ERASE:
			flash.ul_page_buffer[IS_CONFIG_SAVED] = 0;
#ifdef MASTER

			flash.ul_page_buffer[NB_SLAVES] = Modbus(MODBUS_GET_NB_SLAVES);
			//sprintf(buf, "\r\n\tFONCTION::: %d", Modbus(MODBUS_GET_NB_SLAVES));
			//Putstr(buf);
			for(int i = 0; i < flash.ul_page_buffer[NB_SLAVES]; i++)
			{
				flash.ul_page_buffer[START_ID_SLAVE_REGISTER + i] = 0;
			}

#else

			flash.ul_page_buffer[NB_SLAVES] = 0;

			flash.ul_page_buffer[START_ID_SLAVE_REGISTER] = 0xFF;
#endif
			if(nvm_write(INT_FLASH, TEST_PAGE_ADDRESS, (void *)flash.ul_page_buffer, sizeof(flash.ul_page_buffer)) ==
			STATUS_OK)
			{
				sprintf(buf, "\r\n\tSAVE OK\r\n");
				Putstr(buf);
			}
			break;
		default:
			Error(ERROR_FLASH_SWITCH_BAD_SC, sc);
			break;
	}
	return 0;
}