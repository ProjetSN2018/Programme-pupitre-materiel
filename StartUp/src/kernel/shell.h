/*
 * shell.h
 *
 * Created: 10/03/2018 11:09:40
 *  Author: Thierry
 */
 
uint32_t Shell(uint32_t sc, ...);

uint16_t getShellStatus(void);
void	 setShellStatus(uint16_t sstatus);

#define SHELL_NEW			100