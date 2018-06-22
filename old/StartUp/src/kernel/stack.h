/*
 * stack.h
 *
 * Created: 21/03/2018 22:31:13
 *  Author: Thierry
 */
typedef struct tt_stack{
	char		*sp;
	char		*org;
	uint16_t	size;
	uint8_t		elemSize;
	uint16_t	status;
}t_stack;

uint32_t Stack(uint32_t sc,...);
#define STACK_NEW		100
#define STACK_PUSH		102
#define STACK_POP		103
#define STACK_FLUSH		104

#define stackNew(stack, org, size, elemSize)	Stack(STACK_NEW, (uint32_t)stack,(uint32_t) org, (uint32_t)size, (uint32_t)elemSize)
#define stackPush(stack, elem)					Stack(STACK_PUSH, (uint32_t)stack,(uint32_t) elem)
#define stackPop(stack, elem)					Stack(STACK_POP, (uint32_t)stack, (uint32_t) elem)
#define stackFlush(stack)						Stack(STACK_FLUSH, (uint32_t)stack)