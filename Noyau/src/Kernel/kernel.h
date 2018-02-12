/*
 * kernel.h
 *
 * Created: 07/02/2018 17:21:01
 *  Author: Mod_loc
 */ 


typedef uint32_t(*t_ptf32) (uint32_t, ...);
//application task function pointer type
typedef void (*t_ptf)(uint32_t, void*);

#include "error.h"
#include "taskman.h"
#include "shell.h"

