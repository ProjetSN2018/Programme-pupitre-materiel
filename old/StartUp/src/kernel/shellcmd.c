/*
 * shellcmd.c
 *
 * Created: 11/03/2018 01:27:13
 *  Author: Thierry
 */
#include "kernel.h"
#include "CRC16MODBUS.h"
#include "utils.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

const t_command_entry cmdEntries[]={

	{0x624F, (t_pCmdFunc)_cmd_start		},
	{0x6CAB, (t_pCmdFunc)_cmd_set		},
	{0x3BED, (t_pCmdFunc)_cmd_cls		},
	{0xAAF6, (t_pCmdFunc)_cmd_show		},
	{0xB326, (t_pCmdFunc)_cmd_hide		},
	{0x902B, (t_pCmdFunc)_cmd_restart	},
	{0xB344, (t_pCmdFunc)_cmd_restart	}, //0xB344 => reboot
	{0xEBE8, (t_pCmdFunc)_cmd_infos		},

	//----------------------//
	{0x0000, NULL       }
};

#define ST_SHELL_SHOW_UNKNOW_CRC		0x000002
#define ST_SHELL_PROMPT_DISABLED        0x000004

char buf[256];

void _cmd_show(uint32_t sc, void*pParam)
{
	char *pToken;
	pToken = strtok(pParam," \r\n");
	if(pToken)
	{
		switch(CRC16MODBUSFRAME((unsigned char*)pToken,strlen(pToken)))
		{
		case 0x57E5:    //crc
			mBitsSet(*(uint16_t*)sc,ST_SHELL_SHOW_UNKNOW_CRC);
			break;
		default:
			break;
		}
	}
}

void _cmd_hide(uint32_t sc, void*pParam)
{
	char *pToken;
	pToken = strtok(pParam," \r\n");
	if(pToken)
	{
		switch(CRC16MODBUSFRAME((unsigned char*)pToken,strlen(pToken)))
		{
		case 0x57E5:    //crc
			mBitsClr(*(uint16_t*)sc,ST_SHELL_SHOW_UNKNOW_CRC);
			break;
		default:
			break;
		}
	}
}

void _cmd_start(uint32_t sc, void*pParam)
{
	Putstr(" cmd start : ");
	Putstr(pParam);
}

void _cmd_set(uint32_t sc, void*pParam)
{
	char *pToken;
	uint8_t u1=0,u2=0,u3=0;
	uint16_t u4=0;
	//pToken = mstrtok(pParam," \r\n");

	pToken = mstrtok(NULL," \r\n");
	if(pToken)
	{
		switch(CRC16MODBUSFRAME((unsigned char*)pToken,strlen(pToken)))
		{
		case 0x7327:    //time
#define hour    u1
#define minute  u2
#define second  u3
			//pToken=strtok(pToken+strlen(pToken)+1," :./\r\n");
			pToken=mstrtok(NULL," :./\r\n");
			if(pToken) hour=atoi((char*)pToken);
			//pToken=strtok(pToken+strlen(pToken)+1," :./\r\n");
			pToken=mstrtok(NULL," :./\r\n");
			if(pToken) minute=atoi((char*)pToken);
			//pToken=strtok(pToken+strlen(pToken)+1," :./\r\n");
			pToken=mstrtok(NULL," :./\r\n");
			if(pToken) second=atoi((char*)pToken);
			kRtc(KRTC_SET_TIME,(uint32_t)hour,(uint32_t)minute,(uint32_t)second);
#undef hour
#undef minute
#undef second
			break;
		case 0xE1A9:    //date
#define date    u1
#define month   u2
#define centYear u4
			pToken=mstrtok(NULL," :./\r\n");
			if(pToken) date=atoi((char*)pToken);
			pToken=mstrtok(NULL," :./\r\n");
			if(pToken) month=atoi((char*)pToken);
			pToken=mstrtok(NULL," :./\r\n");
			if(pToken) centYear=atoi((char*)pToken);
			
			Putstr("\e[2K"); //Erase current line on terminal screen
			if(kRtc(KRTC_SET_DATE,(uint32_t)date,(uint32_t)month,(uint32_t)centYear)==0)
			{
				Putstr("\r\n");
				kRtc(KRTC_GETF_DATE,(uint32_t)buf);
				Putstr(buf);
				Putstr("\r\n");
			}
#undef date
#undef month
#undef centYear
			break;
		}
	}
}

void _cmd_restart_proc(uint32_t sc, uint32_t timeout);

void _cmd_restart_proc(uint32_t sc, uint32_t timeout)
{
	mBitsSet(*(uint16_t*)sc,ST_SHELL_PROMPT_DISABLED);
	sprintf(buf,"\rSystem will restart in %i second...",(int)timeout); Putstr(buf);
	if(timeout==0)
	{	
		Putstr("\r\n\n");
		for(sc=500000;sc;sc--);
		rstc_start_software_reset(RSTC);
		for(;;);
	}
	else
	{
		PushTask(_cmd_restart_proc,sc,--timeout,1000);	
	}
}

void _cmd_restart(uint32_t sc, void*pParam)
{
	mBitsSet(*(uint16_t*)sc,ST_SHELL_PROMPT_DISABLED);
	pParam=atoi((char*)pParam);
	PushTask(_cmd_restart_proc,sc,pParam,0);
}

void _cmd_cls(uint32_t sc, void*pParam)
{
	Putstr("\e[2J");
}

const char*eprocs[]={
	"UNDEFINED!",
	"ARM946S",
	"ARM7TDMI",
	"Cortex-M3",
	"ARM920T",
	"ARM926EJS",
	"Cortex-A5",
	"Cortex-M4"
};

const char*nvpsiz[]={
	"NONE",
	"8K bytes",
	"16K bytes",
	"32K bytes",
	NULL,
	"64K bytes",
	NULL,
	"128K bytes",
	NULL,
	"256K bytes",
	"512K bytes",
	NULL,
	"1024K bytes",
	NULL,
	"2048K bytes",
	NULL
};

const char*sramsiz[]={
	"48K bytes",
	"1K bytes",
	"2K bytes",
	"6K bytes",
	"24K bytes",
	"4K bytes",
	"80K bytes",
	"160K bytes",
	"8K bytes",
	"16K bytes",
	"32K bytes",
	"64K bytes",
	"128K bytes",
	"256K bytes",
	"96K bytes",
	"512K bytes",
};
typedef struct{
	const uint8_t	value;
	const char*		name;
	const char*		description;
}t_arch;

const t_arch arch[]={
	{0x19, "AT91SAM9xx",		"AT91SAM9xx Series"								},
	{0x29, "AT91SAM9XExx",		"AT91SAM9XExx Series"							},
	{0x34, "AT91x34",			"AT91x34 Series"								},
	{0x37, "CAP7",				"CAP7 Series"									},
	{0x39, "CAP9",				"CAP9 Series"									},
	{0x3B, "CAP11",				"CAP11 Series"									},
	{0x40, "AT91x40",			"AT91x40 Series"								},
	{0x42, "AT91x42",			"AT91x42 Series"								},
	{0x55, "AT91x55",			"AT91x55 Series"								},
	{0x60, "AT91SAM7Axx",		"AT91SAM7Axx Series"							},
	{0x61, "AT91SAM7AQxx",		"AT91SAM7AQxx Series"							},
	{0x63, "AT91x63",			"AT91x63 Series"								},
	{0x70, "AT91SAM7Sxx",		"AT91SAM7Sxx Series"							},
	{0x71, "AT91SAM7XCxx",		"AT91SAM7XCxx Series"							},
	{0x72, "AT91SAM7SExx",		"AT91SAM7SExx Series"							},
	{0x73, "AT91SAM7Lxx",		"AT91SAM7Lxx Series"							},
	{0x75, "AT91SAM7Xxx",		"AT91SAM7Xxx Series"							},
	{0x76, "AT91SAM7SLxx",		"AT91SAM7SLxx Series"							},
	{0x80, "SAM3UxC",			"SAM3UxC Series (100-pin version)"				},
	{0x81, "SAM3UxE",			"SAM3UxE Series (144-pin version)"				},
	{0x83, "SAM3AxC/SAM3AxC",	"SAM3AxC/SAM4AxC Series (100-pin version)"		},
	{0x84, "SAM3XxC/SAM4XxC",	"SAM3XxC/SAM4XxC Series (100-pin version)"		},
	{0x85, "SAM3XxE/SAM4XxE",	"SAM3XxE/SAM4XxE Series (144-pin version)"		},
	{0x86, "SAM3XxG/SAM4XxG",	"SAM3XxG/SAM4XxG Series (208/217-pin version)"	},
	{0x88, "SAM3SxA/SAM4SxA",	"SAM3SxA/SAM4SxA Series (48-pin version)"		},
	{0x89, "SAM3SxB/SAM4SxB",	"SAM3SxB/SAM4SxB Series (64-pin version)"		},
	{0x8A, "SAM3SxC/SAM4SxC",	"SAM3SxC/SAM4SxC Series (100-pin version)"		},
	{0x92, "AT91x92",			"AT91x92 Series"								},
	{0x93, "SAM3NxA",			"SAM3NxA Series (48-pin version)"				},
	{0x94, "SAM3NxB",			"SAM3NxB Series (64-pin version)"				},
	{0x95, "SAM3NxC",			"SAM3NxC Series (100-pin version)"				},
	{0x99, "SAM3SDxB",			"SAM3SDxB Series (64-pin version)"				},
	{0x9A, "SAM3SDxC",			"SAM3SDxC Series (100-pin version)"				},
	{0xA5, "SAM5A",				"SAM5A"											},
	{0xF0, "AT75Cxx",			"AT75Cxx Series"								},
/////////////////////////////////////////////////////////////////////////////////
	{0x00, "NO ARCH!",			NULL											}
};

const char*nvptyp[]={
	"ROM --- ROM",
	"ROMLESS --- ROMless or on-chip Flash",
	"FLASH --- Embedded Flash Memory",
	"ROM_FLASH --- ROM and Embedded Flash Memory [NVPSIZ is ROM size / NVPSIZ2 is Flash size]",
	"SRAM --- SRAM emulating ROM"
};

void _cmd_infos(uint32_t sc, void*pParam)
{
	chipid_data_t chipData;

	chipid_read(CHIPID,&chipData);
	Putstr("\r\n\t=============== CHIP IDENTIFIER INFOS ===============\r\n\n");
	sprintf(buf,"\tVERSION: %c\r\n",(char)(chipData.ul_version-1)+'A');	Putstr(buf);
		Putstr("\t  EPROC: "),Putstr(eprocs[chipData.ul_eproc]);	Putstr("\r\n");
		Putstr("\t NVPSIZ: "),Putstr(nvpsiz[chipData.ul_nvpsiz]);	Putstr("\r\n");
		Putstr("\tNVPSIZ2: "),Putstr(nvpsiz[chipData.ul_nvpsiz2]);	Putstr("\r\n");
		Putstr("\tSRAMSIZ: "),Putstr(sramsiz[chipData.ul_sramsiz]);	Putstr("\r\n");
#define k sc
		k=0;
		while(arch[k].value && (arch[k].value!=chipData.ul_arch)) k++;
		Putstr("\t   ARCH: "),Putstr(arch[k].name); Putstr(" --- "); Putstr(arch[k].description); Putstr("\r\n");
#undef k
		Putstr("\t NVPTYP: "),Putstr(nvptyp[chipData.ul_nvptyp]);	Putstr("\r\n");
	if(chipData.ul_extflag)
	{
		sprintf(buf,"\t   EXID: %08X\r\n",(unsigned int)chipData.ul_extid); Putstr(buf);
	}
	Putstr("\r\n\t=====================================================\r\n\n");
}