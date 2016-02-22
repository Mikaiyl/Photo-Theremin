#include <hidef.h>      /* common defines and macros */
#include <mc9s12dp256.h>     /* registers & ports definitions */
#pragma LINK_INFO DERIVATIVE "mc9s12dp256b"
#include "Functions_hcs12.h"
#include <stdio.h>
#include "222defs.h"

/****************************
 * Global vars 
 ****************************/
long int absoluteTime=0; 	// global variable to count interrupts. 
int ms=0, sec=0, min=0, hr=0;
int bogusISRFlag= 0; 

void init(){    
	/* Make variables
    *************************/
    char buffer[33]; 
    int count=0; 
    absoluteTime=0;
    /* Initialize ports
    *************************/
    DDRT = 0xff;
    DDRJ = 0xff;
    DDRB = 0xff;
    EnableInterrupts; 
    
    /* Initialization Functions
    *************************/    
    ATDinit();
    LCDinit();
    RTIInit(0x11);
    PTJ=0x0;        		//display data on PORTB pins
}

long int absoluteTime=0; 
				// global variable to count interrupt iterations. 

interrupt void RTI_ISR(void) 
{  				// RTI service routine 
	absoluteTime++; 	// uses global vaariable. 
	CRGFLG = 0x80; 		// clear RTIF bit to enable next interrupt
	if(0x80 && PTH){
    		PTT ^= 0x20;
  	}
}

void main(void) 
 {     
	init();
        while(1){		//print the buffer string
		sprintf(buffer, "Time: %d:%d:%d:%d \n", hr, min, sec, ms);
		LCDstring(buffer); 
	}
}
