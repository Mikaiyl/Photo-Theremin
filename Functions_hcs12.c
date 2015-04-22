//  Functions for hcs12
#include <hidef.h>      /* common defines and macros */
#include <mc9s12dp256.h>     /* registers & ports definitions */

void delayms(unsigned int n){ // delay loop 
    unsigned long delay;    // Long is better than int because int fills up faster                
     delay = 0x000000FF * n ;// multiply by input 
     while(delay--);         // count down
  }


void delayus(unsigned int n)
  {  unsigned int delay;                    

     delay = 0x0005 * n ; 
     while(delay--);
  }

void LCDcmd(char c) { //send cmd byte c to LCD control reg
  char lownib = (0x0f & c) <<2; //shift into bits 2:5)
  char hinib = (0xf0 & c) >>2;  //shift into bits 2:5)
 
  PORTK = hinib; 		//Set up data while EN=0
  PORTK = 0x02 | hinib; 	//turn on EN bit, with RS=0
  PORTK = hinib;      	//turn off EN bit
  
  PORTK = lownib; 		//Set up data while EN=0
  PORTK = 0x02 | lownib; //turn on EN bit
  PORTK = lownib;      	//turn off EN bit
 
  delayus(1800);    //wait for command completion
  }

void LCDinit(void) {   

  const int INITLENGTH = 6;//initialize LCD controller on PORTK
                         // send this sequence of bytes to control reg
  char initseq[] = { 0x33, 0x32,0x28,0x06,0x0F,0x01}; 	// 0x0F=cursor on,blink;  0x01=Clear
  int i=0; 
  DDRK = 0xFF; // enable Port K to send the stuff
    for (i=0; i<INITLENGTH; i++){  // send each char to LCD 
      LCDcmd(initseq[i]);  // iterate and execute each commnd
     delayms(5);
    }
  }		
 
void LCDchar(char c) { //send cmd byte c to LCD control reg
  char lownib = ((0x0f & c) <<2); //shift into bits 2:5) 
  char hinib = ((0xf0 & c) >>2);  //shift into bits 2:5)
 
  PORTK = hinib  ; 		//Set up data while EN=0
  PORTK = 0x02 | hinib | 0x01; 	//turn on EN bit, with RS=01
  PORTK = hinib;      	//turn off EN bit
  
  PORTK = lownib; 		//Set up data while EN=0
  PORTK = 0x02 | lownib | 0x01; //turn on EN bit with RS=1
  PORTK = lownib;      	//turn off EN bit
 
  delayus(51);    //wait for command completion
  }

void LCDstring(char i[]){  // iterate through array of chars and send to LCD

    int count = 0;       // count iterations
    LCDcmd(0x02);
  
    for(;i[count]!='\0';){ // loop through each char
        
        LCDchar(i[count++]);     // from var to LCD print function
        if(i[count]=='\n'){
		    LCDcmd(0xc0);   // skip to second line
	      count++;
	      }
    } 
} 

 
 void ATDinit(void) 
//init ATD0 for continuous conversions of PAD07 input
{
  ATD0CTL2 |= 0b10000000;  //ADPU=1
  delayms(10); //wait 10 us for power-up
  ATD0CTL3 = 0b00000000; 
  ATD0CTL4 = 0b10000000; 
//set SRES8=1 for 8-bit conversion (simpler than 10)
	ATD0CTL5 = 0b00100111; //pin 7, SCAN=1, MULT=0
//ATD0CTL5 = 0b00110000; //pin 0 start,SCAN=1,MULT=1
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
void displayTime(int a, int b, int c, int d)
{   
    DDRP = 0xf7;
    PTP = 0xf7;
    PORTB=seg7[a];
    
    DDRP = 0xfb;
    PTP = 0xfb;
    PORTB=seg7[b];
     
	DDRP = 0xfd;
	PTP = 0xfd; 
    PORTB=seg7[c];
    
    DDRP = 0xfe;
    PTP = 0xfe; 
    PORTB=seg7[d];
 }
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
 
 
