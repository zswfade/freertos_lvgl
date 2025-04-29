#include "SysTick.h"



//延时nus
//nus为要延时的us数.		    								   
void delay_us(u32 nus)
{		
	u32 temp;	    	 
  for(temp=0;temp<nus*5;temp++) 
	__NOP();
}

void delay_ms(u16 nms)
{	 		  	  
	u32 temp;		   
   for(temp=0;temp<nms*5000;temp++) 
	__NOP();   					  	    
} 


