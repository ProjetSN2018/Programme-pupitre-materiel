/*
 * commandStub.h
 *
 * Created: 11/03/2018 01:21:05
 *  Author: Thierry
 */


typedef void(*t_pCmdFunc)(uint32_t sc, void*pParam);
///////////////////////////////////////////////////////////////////////////////////////////
typedef struct tt_command_entry{
	uint16_t    cmdCode;
	t_pCmdFunc  pCmdFunc;
}t_command_entry;

extern const t_command_entry commands[];
///////////////////////////////////////////////////////////////////////////////////////////
