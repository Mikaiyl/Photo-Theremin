// Functions_hcs12.h


#ifndef _FUNCTIONS_HCS12
	#define _FUNCTIONS_HCS12 
	
//	#include "Functions_hcs12.c"
	
		extern void delayms(unsigned int n);
		extern void delayus(unsigned int n);
		extern void LCDcmd(char c);
		extern void LCDinit(void);
		extern void LCDchar(char c);
		extern void LCDstring(char i[]);
		extern void displayByte();
		extern void ATDinit(void);
    extern void play(int dt, int dur);

#endif
