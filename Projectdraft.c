#include <hidef.h>      /* common defines and macros */
#include <mc9s12dp256.h>     /* registers & ports definitions */
#pragma LINK_INFO DERIVATIVE "mc9s12dp256b"
#include <stdio.h>
#define ENABLE 0b00000010//ENable line of LCDC is bit 1 of PORTK
#define DATASEL 0b00000001 //DATASEL will set reg select (bit0 of PORTK) to 1
#define pitch ATD0DR3H
#define volume ATD0DR4H

long int absoluteTime=0; // global vaariable to count interrupts. 
int ms=0, sec=0, min=0, hr=0;

int bogusISRFlag= 0;
unsigned char seg7[] ={0x3f, 0x06,0x5b,0x4f,0x66, 
  0x6d,0x7d,0x07,0x7f,0x6f};  // 0-9


interrupt void RTI_ISR(void) 
{   absoluteTime++; // uses global vaariable
    
    CRGFLG = 0x80; /* clear RTIF bit to enable next interrupt*/
    
    if ((absoluteTime % (ms/50)) == 0) {
        
	  PTT^= 0b00100000;// toggle  PTT to crate halp an audio wave on speaker.
} 
     if(absoluteTime % 4 == 0){
       ms++;
       if(ms == 999){
          sec++;
          ms=0;
          if(sec == 59){
            min++;
            sec=0;
            if(min == 59){
              hr++;
              min=0;
            }
          }
       }
    }

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
  
     
void Welcome (void){
     
     LCDstring("Light Teramin \nProject 2015");
     
     delayms(3000);
}

void main(void) 
 {
 
  char buffer[32]; //convert binary value to character string written to buffer
  
  LCDinit();
  
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
  
  ms=0, sec=0, min=0, hr=0;
  
  RTIInit(0x11);
  
  EnableInterrupts;
  
  Welcome();
  
  for(;;)
  {
    

		
	sprintf(buffer, "Frequency: %d \n L1: %d L2: %d%d \n", hr, min, sec, ms);
	
	LCDstring(buffer); //print the buffer string

  
  }
 }