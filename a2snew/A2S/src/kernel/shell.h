/*
 * shell.h
 *
 * Created: 10/03/2018 11:09:40
 *  Author: Thierry
 */


uint32_t Shell(uint32_t sc, ...);

#define SHELL_NEW					100
#define SHELL_HEART_BEAT			102
#define SHELL_LED_INIT				103
#define SHELL_LED_IDENTIFIED		104
#define SHELL_LED_INIT_MASTER		105
