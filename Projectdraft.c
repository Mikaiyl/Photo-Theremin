#include <hidef.h>      /* common defines and macros */
#include <mc9s12dp256.h>     /* registers & ports definitions */
#pragma LINK_INFO DERIVATIVE "mc9s12dp256b"
#include <stdio.h>
#define pitch ATD0DR3H
#define volume ATD0DR4H

long int absoluteTime=0; // global vaariable to count interrupts. 


int bogusISRFlag= 0;


interrupt void RTI_ISR(void) 
  {   
  
      unsigned char y = PTH>>2;
      static int x=0;
      absoluteTime++; // uses global vaariable
      
      
      if(PTH & 0x01){
      
+          PWMDTY5 = ((PWMPER5/2)/(25500/(volume * 100)));
      }else{
          PWMDTY5 = 0;
          }
      /*if(!x){
       
        x=(y*25)+1;
        CRGFLG = 0x80;
      } 
        x--;*/
         PWMPER5 = pitch;
         CRGFLG = 0x80;
  }

interrupt void UnimplementedISR(void) 
{  

   bogusISRFlag=1; // handle bogus interrupts
}

typedef void (*near tIsrFunc)(void); // tIsrFunc is a pointer to a function


const tIsrFunc _vect[] @0x3E70 = 
{   // table in RAM at 0x3E70 
    /*  Top 8 vectors of Interrupt table @0xFFF0 in Flash, but DBug12  redirects them to shadow in RAM @ 0x3E70 thru 0x3E7F */
        RTI_ISR,             /* vector 07: RTI */
        UnimplementedISR,    /* vector 06: IRQ */
        UnimplementedISR,    /* vector 05: XIRQ */
        UnimplementedISR,    /* vector 04: SWI */
        UnimplementedISR,    /* vector 03: unimplemented opcode trap*/
        UnimplementedISR,    /* vector 02: COP */
        UnimplementedISR,    /* vector 01: Clock monitor fail */
        UnimplementedISR		 /* Reset vector */
 };
     
void Welcome (void){
     
     LCDstring("Light Teramin \nProject 2015");
     
     delayms(3000);
}
void lights(){
    int i;
     
     i = volume / 32;
     PORTB = 0xff << i;                       
    
}
void init(){
 
  
  LCDinit();
  ATDinit();
  RTIInit(0x11);
  
  DDRP = 0x0f;
  DDRB = 0xFF;    //PORTB as output
  DDRT = 0xff;
  DDRJ = 0xFF;    //PTJ as output for Dragon12+ LEDs
  PTJ=0x0;        //display data on PORTB pins
  PTP = 0xFF;
     
  PWMPRCLK = 0x03;   // 4MHz / 8 = 500kHz
  PWMSCLA = 0x0A;    // 500kHz / (2*10) =25kHz
  PWMCLK = 0x20;     // PWM source channel 5
  PWMPOL = 0x20;     // High then low
  PWMCAE = 0x00;     // Left aligned
  PWMCTL = 0x0F;     // 8-bit, 15 cycle wait
  PWME = 0x20;       // Enable PWM5
  
   
  EnableInterrupts;
  Welcome();

};

void main(void) 
 {
 
  char buffer[32]; //convert binary value to character string written to buffer
  init();
 
  while(1)
  {
    
  
		
	(void)sprintf(buffer, "Frequency: %4d \n L1: %3d L2: %3d ", 25000/pitch , pitch , volume);
	
	LCDstring(buffer); //print the buffer string
    lights();

  
  }
 }