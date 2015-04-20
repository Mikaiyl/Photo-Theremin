#include <hidef.h>      /* common defines and macros */
#include <mc9s12dp256.h>     /* registers & ports definitions */
#pragma LINK_INFO DERIVATIVE "mc9s12dp256b"
#include "Functions_hcs12.h"
#include <stdio.h>
#include "222defs.h"

//interrupt RTI_ISR(), UnimplementedISR(); //OPTIONAL PROTOTYPE STATEMENTS

interrupt RTI_ISR(), UnimplementedISR(); //OPTIONAL PROTOTYPE STATEMENTS

typedef void (*near tIsrFunc)(void); 
// tIsrFunc is a pointer to a function

const tIsrFunc _vect[] @0x3E70 = {   // table in RAM at 0x3E70 
/*  Top 8 vectors of Interrupt table @0xFFF0 in Flash, but DBug12  redirects them to shadow in RAM @ 0x3E70 thru 0x3E7F */
        RTI_ISR,                 /* vector 07: RTI */
        UnimplementedISR,        /* vector 06: IRQ */
        UnimplementedISR,        /* vector 05: XIRQ */
        UnimplementedISR,        /* vector 04: SWI */
        UnimplementedISR,  /* vector 03: unimplemented opcode trap*/
        UnimplementedISR,        /* vector 02: COP */
        UnimplementedISR,        /* vector 01: Clock monitor fail */
        UnimplementedISR		 /* Reset vector */
   };


 void RTIInit(unsigned char rtictlvalue) {
  /* setup RTI interrupt frequency & enable RTI interrupts, assuming 4MHz xtal */
  /* clock divider set in RTICTL reg, and is (N+1)*2^(M+9), where N=bits(3:0), M=bits(6:4).
    But M=0 disables divider.
    f= 4MHz / ((n+1)*2^(M+9)).  
    Useful values:  0x10 = 4kHz, 0x11 = 2kHz, 0x12 = 1.33kHz, 0x13 = 1kHz
   */  
   if (!rtictlvalue) //if zero, set to default
		RTICTL =0x10;
	else
		RTICTL = rtictlvalue; /*set RTI prescaler */ 
  CRGINT = 0b10000000; /* enable RTI interrupts */ 
}
long int absoluteTime=0; // global vaariable to count interrupts. 

interrupt void RTI_ISR(void) 
{  //RTI service routine
		// keep track of total interrupt count
	absoluteTime++; // uses global vaariable. 
  CRGFLG = 0x80; /* clear RTIF bit to enable next interrupt*/
  if(0x80 && PTH){
    
    PTT ^= 0x20;
  }
}



void main(void) 
 {     
    /* Make variables
    *************************/
    char buffer[32]; 
    int count=0; 
    absoluteTime=0;
    /* Initialize ports
    *************************/
    DDRT = 0xff;
    EnableInterrupts; 
    /* Initialization Functions
    *************************/    
    ATDinit();
    LCDinit();
    RTIInit(0x11);
        /* Get Busy
        *************************/
        while(1)
          {
            while (!(0x80 & ATD0STAT0)); 
            sprintf(buffer, "Pot = %d\nTime=%d",ATD0DR7H,absoluteTime);
            LCDstring(buffer);

	          count++;
	        }
	 }