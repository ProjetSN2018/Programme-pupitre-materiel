/*
 * kernel.h
 *
 * Created: 10/03/2018 07:13:28
 *  Author: Thierry
 */
 extern char buf[];
/// TASK MANAGER PARAMETERS //////////////////////////////////////////////////////////////////
#define TASKMAN_TASK_LIST_LEN				20
#define TASKMAN_DELAYED_TASK_LIST_LEN		20

/// SHELL PARAMETERS /////////////////////////////////////////////////////////////////////////
#define UART_BAUDRATE						115200ul
#define SHELL_ESC_TIMEOUT					20
#define SHELL_EDIT_TIMEOUT					0

#define SHELL_TXBUF_LEN						512
#define SHELL_EDITBUF_LEN					256
#define SHELL_ESCSEQBUF_LEN					128


#define HEART_LED					(PIO_PB20_IDX)
#define HEART_LED_FLAGS				(PIO_TYPE_PIO_OUTPUT_1 | PIO_DEFAULT)
#define HEART_LED_ACTIVE_LEVEL		0
#define HEART_LED_NAME				"Heart Yellow Led"

#define ERROR_LED					(PIO_PB19_IDX)
#define ERROR_LED_FLAGS				(PIO_TYPE_PIO_OUTPUT_1 | PIO_DEFAULT)
#define ERROR_LED_ACTIVE_LEVEL		1
#define ERROR_LED_NAME				"Error Red Led (alias Power Red Led)"

#define POWER_LED					(PIO_PB19_IDX)
#define POWER_LED_FLAGS				(PIO_TYPE_PIO_OUTPUT_1 | PIO_DEFAULT)
#define POWER_LED_ACTIVE_LEVEL		1
#define POWER_LED_NAME				"Power Red Led"

#include <asf.h>

#define pa1					(*(uint32_t*)(&sc+1))
#define pa2					(*(uint32_t*)(&sc+2))
#define pa3					(*(uint32_t*)(&sc+3))
#define pa4					(*(uint32_t*)(&sc+4))
#define pa5					(*(uint32_t*)(&sc+5))
#define pa6					(*(uint32_t*)(&sc+6))

#define mBitsSet(f,m)		((f)|=(m))
#define mBitsClr(f,m)		((f)&=(~(m)))
#define mBitsTgl(f,m)		((f)^=(m))
#define mBitsMsk(f,m)		((f)&(m))
#define mIsBitsSet(f,m)		(((f)&(m))==(m))
#define mIsBitsClr(f,m)		(((~(f))&(m))==(m))


typedef void(*t_pTaskFunc)(uint32_t,...);
typedef uint32_t(*t_pFunc)(uint32_t,...);

#include "error.h"
#include "taskman.h"
#include "kRtc.h"
#include "lcd.h"
#include "shellcom.h"
#include "shellcmd.h"
#include "shell.h"

#include "menu.h"
#include "sas.h"



#define KernelNew()				{ Taskman(TASKMAN_NEW); Shell(SHELL_NEW); }
#define KernelRun()				{ for(;;) Taskman(TASKMAN_POP); }

#define PushTask(func,sc,pa,delay)		Taskman(TASKMAN_PUSH,(uint32_t)func,(uint32_t)sc,(uint32_t)pa,(uint32_t)delay)

	