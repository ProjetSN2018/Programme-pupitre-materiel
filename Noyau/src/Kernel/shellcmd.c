/*
 * shellcmd.c
 *
 * Created: 07/02/2018 17:29:14
 *  Author: Mod_loc
 */ 

#include "./Headers/appli.h"
#include "shellcmd.h"

const t_cmdEntry _cmdEntries[] ={
	{ 0x6CAB, _cmd_set		},
	

	//////////////////LIST TERMINATOR ////////////////////////////
	{	0,	NULL			}	
};

void _cmd_set(uint32_t sc, void* pParam)
{
	Putstr(pParam);
}