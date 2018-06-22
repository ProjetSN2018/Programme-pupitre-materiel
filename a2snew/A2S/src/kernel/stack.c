/*
 * stack.c
 *
 * Created: 23/03/2018 13:02:31
 *  Author: eleve
 */ 

 #include "kernel.h"
 #define STACK_SIGNATURE		0xabcd
 uint32_t Stack(uint32_t sc, ...)
 {
	switch(sc)
	{
#define _stack		((t_stack*)pa1)
#define _org		((char*)pa2)
#define _size		((uint16_t)pa3)
#define _elemSize	((uint8_t)pa4)

	case STACK_NEW:
		_stack->size = _size;
		_stack->org = _org;
		_stack->sp = _org;
		_stack->elemSize = _elemSize;
		_stack->status = STACK_SIGNATURE; 
		
	
		break;
#undef _org
#undef _size
#undef _elemSize
	case STACK_PUSH:
#define _elem	pa2	
		
		if(_stack->status != STACK_SIGNATURE) Error(ERROR_STACK_NOT_INIT, sc);
		if(_stack->sp == (_stack->org + (_stack->size * _stack->elemSize))) Error(ERROR_STACK_PUSH_FULL, sc);
	
#define _k	sc
		
		for(_k = 0; _k < _stack->elemSize; _k++)
		{
			_stack->sp[_k] = ((char*)_elem)[_k];
		}
		_stack->sp+=_stack->elemSize;
		
		break;

	case STACK_POP:
		if(_stack->status != STACK_SIGNATURE) Error(ERROR_STACK_NOT_INIT, sc);
		if(_stack->sp == _stack->org) Error(ERROR_STACK_UNDERFLOW, sc);
		_stack->sp-=_stack->elemSize;
		
		for(_k = 0; _k < _stack->elemSize; _k++)
		{
			((char*)_elem)[_k] = _stack->sp[_k];
		}
	
		break;
#undef _k
#undef _elem

	case STACK_FLUSH:
		if(_stack->status != STACK_SIGNATURE) Error(ERROR_STACK_NOT_INIT, sc);
		_stack->sp = _stack->org;
		break;
#undef _stack

	default:
		Error(ERROR_STACK_SWITCH_BAD_SC, sc);
		
	}
	return 0;
 }