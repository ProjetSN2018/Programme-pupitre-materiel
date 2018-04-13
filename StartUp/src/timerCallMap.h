/*
 * imerCallMap.h
 *
 * Created: 27/03/2018 15:10:40
 *  Author: Mod_loc
 */ 
#include <asf.h>
typedef uint32_t(*t_pFunc)(uint32_t,...);
extern t_pFunc timerCallMap[];
#define __TIMER_CALL_SERVICE		1