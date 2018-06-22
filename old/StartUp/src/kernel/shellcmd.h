/*
 * shellcmd.h
 *
 * Created: 11/03/2018 01:27:24
 *  Author: Thierry
 */ 
#include "commandStub.h"

/////// COMMAND FUNCTIONS PROTOTYPES /////////////////////////////////////////////////////

void _cmd_start(uint32_t sc, void*pParam);
void _cmd_set(uint32_t sc, void*pParam);
void _cmd_cls(uint32_t sc, void*pParam);
void _cmd_show(uint32_t sc, void*pParam);
void _cmd_hide(uint32_t sc, void*pParam);
void _cmd_restart(uint32_t sc, void*pParam);
void _cmd_infos(uint32_t sc, void*pParam);

extern const t_command_entry cmdEntries[];
///////////////////////////////////////////////////////////////////////////////////////////