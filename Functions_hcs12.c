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
 void displayByte()
{
    volatile unsigned int temp = 0;
    volatile unsigned int dip = PTH;  // input from dip switches
    volatile unsigned char temp1 = 0;
    unsigned char seg7[] ={0x3f, 0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f, 0x77,0x7c,0x39,
                           0x5e,0x79,0x71}; //Array of Hex table
    
    while(1){
    
    dip = PTH;
    temp = dip & 0x0f;
    PTP = 0xf7;
    temp1=seg7[temp];
    PORTB=temp1;
    delayms(1);
    
    
    PTP = 0xfb; 
    temp =  (dip & 0xf0) >> 4;
    PORTB=seg7[temp];
    delayms(1);
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
void play(int delaytime, long duration)
{ 
int i;
//assume inputs in usec
int nloops = duration / delaytime;
for(i=0;i<nloops;i++) { 
	PTT ^=0b00100000; // toggle speaker
  delayus(delaytime); 
 }
}


