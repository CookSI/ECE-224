/*=========================================================================*/
/*  Includes                                                               */
/*=========================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "altera_avalon_timer.h"
#include "altera_avalon_timer_regs.h"
#include "alt_types.h"

#define TIME_HIGH 0x0016 
#define TIME_LOW 0xE360
//Buttons 
int  btns;



//Button ISR
static void ISR_BUTTON(void* context, alt_u32 id){
	IOWR(BUTTON_PIO_BASE, 2, 0x0); // Mask interrupt to disable button requests

	// Timer is enabled
	IOWR(TIMER_0_BASE, 0x02, TIME_LOW);
	IOWR(TIMER_0_BASE, 0x03, TIME_HIGH);

	IOWR(TIMER_0_BASE, 1, 5);

}

static void ISR_TIMER(void* context, alt_u8 id){

	btns = IORD(BUTTON_PIO_BASE, 0x0);
	orintf("Buttons as int %d ", btns);
	IOWR(LED_PIO_BASE, 0 ,btns^0xF);

	IOWR(TIMER_0_BASE, 0x0, 0x0);

	IOWR(TIMER_0_BASE, 0x1, 0x0);

	IOWR(BUTTON_PIO_BASE, 0x03, 0x0);

	IOWR(BUTTON_PIO_BASE, 0x02, 0x0F);
}

int main(){

	alt_irq_register(BUTTON_PIO_IRQ, (void *)0, ISR_BUTTON);
	alt_irq_register(TIMER_0_IRQ, (void *)0, ISR_TIMER);

	//LEDs OFF
	IOWR(LED_PIO_BASE,0x0, 0x0);

	//CLEAR TIMER
	IOWR(TIMER_0_BASE, 0x01, 0x0);

	//INTERUPT MASK ENABLE
	IOWR(BUTTON_PIO_BASE, 0x02, 0x0F);
	while(1 == 1);
}
