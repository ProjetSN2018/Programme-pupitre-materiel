/*
 * flash.h
 *
 * Created: 04/06/2018 18:44:39
 *  Author: lavip
 */ 


uint32_t Flash(uint32_t sc, ...);



#define FLASH_NEW		101
#define FLASH_LOAD		103
#define FLASH_SAVE		104
#define FLASH_ERASE		105


#define TEST_PAGE_ADDRESS (IFLASH_ADDR + IFLASH_SIZE - IFLASH_PAGE_SIZE*8)


#define IS_CONFIG_SAVED				0
#define NB_SLAVES					1

#define START_ID_SLAVE_REGISTER		2