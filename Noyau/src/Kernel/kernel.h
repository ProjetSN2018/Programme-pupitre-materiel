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

//Accès aux contenus des paramètres optionnels des fonctions
#define pa1         (*(uint32_t*)(&sc+1))
#define pa2         (*(uint32_t*)(&sc+2))
#define pa3         (*(uint32_t*)(&sc+3))
#define pa4         (*(uint32_t*)(&sc+4))
#define pa5         (*(uint32_t*)(&sc+5))

//Accès aux adresses des paramètres optionnels des fonctions
#define pPa1         ((uint32_t*)(&sc+1))
#define pPa2         ((uint32_t*)(&sc+2))
#define pPa3         ((uint32_t*)(&sc+3))
#define pPa4         ((uint32_t*)(&sc+4))
#define pPa5         ((uint32_t*)(&sc+5))


#define mBitsSet(flags,mask)     (flags|=(mask))
#define mBitsClr(flags,mask)     (flags&=~(mask))
#define mBitsTgl(flags,mask)     (flags^=(mask))
#define mBitsMsk(flags,mask)     (flags&(mask))
#define mIsBitsSet(flags,mask)   (( (flags&(mask)))==(mask))
#define mIsBitsClr(flags,mask)   (((~flags)&(mask))==(mask))