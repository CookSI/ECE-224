/*
* "Hello World" example.
 *
 * This example prints 'Hello from Nios II' to the STDOUT stream. It runs on
 * the Nios II 'standard', 'full_featured', 'fast', and 'low_cost' example
 * designs. It runs with or without the MicroC/OS-II RTOS and requires a STDOUT
 * device in your system's hardware.
 * The memory footprint of this hosted application is ~69 kbytes by default
 * using the standard reference design.
 *
 * For a reduced footprint version of this template, and an explanation of how
 * to reduce the memory footprint for a given application, see the
 * "small_hello_world" template.
 *
 */

//Includes
#include <stdio.h>
#include <system.h>
#include <altera_avalon_pio_regs.h>
#include "altera_up_avalon_audio.h"
#include "sys/alt_irq.h"
//#define LED0_ON_MASK 1
//#define LED0_OFF_MASK 14
//#define LED1_ON_MASK 2
//#define LED1_OFF_MASK 13
//#define LED2_ON_MASK 4
//#define LED2_OFF_MASK 11

//Defines

//Structs

//Global Variables
int flag;
int counter = 0;

//Function Declaration
void interrupt();
void polling();
int background();
static void interrupt_init(void* context, alt_u32 id);
void egm_init(int period);

int main(){

	//Initial Registers & Vars


	int mode = IORD(SWITCH_PIO_BASE, 0) & 0x01;
	printf("Mode is: %d\n", mode);

	//Read Switch PIO (0 - Interrupt, 1 - Polling)

	if (mode == 1){
		printf("Polling Mode Selected\n");
		//push button
		printf("Press PB0 To Continue\n");
		while (IORD(BUTTON_PIO_BASE,0) & 1 == 1);

		polling();
	}
	else if (mode == 0) {
		printf("Interrupt Mode Selected\n");

		//push button
		printf("Press PB0 To Continue\n");
		while (IORD(BUTTON_PIO_BASE,0) & 1 == 1);

		interrupt();
	}


	return 0;
}

//ISR
static void interrupt_init(void* context, alt_u32 id){
	// LED 2
     //led 1 on start of test
    //int led = IORD(LED_PIO_BASE, 0) | 4;

    //IOWR(LED_PIO_BASE, 0, led);



    IOWR( RESPONSE_OUT_BASE, 0, 1);
	IOWR( RESPONSE_OUT_BASE, 0, 0);
	IOWR( STIMULUS_IN_BASE, 3, 0x0);

    //led = IORD(LED_PIO_BASE, 0) & 11;
    //led 1 off

	//IOWR(LED_PIO_BASE, 0, led);

}


//Interrupt Code
void interrupt(){

	//led 1 on start of test
	int led = IORD(LED_PIO_BASE, 0) | 2;

	IOWR(LED_PIO_BASE, 0, led);


	led = IORD(LED_PIO_BASE, 0) & 13;
	//led 1 off

	IOWR(LED_PIO_BASE, 0, led);


	// Clear Pre-existing interrupt
//	IOWR(STIMULUS_IN_BASE, 3, 0);
	//Interrupt initialize
	alt_irq_register( STIMULUS_IN_IRQ, (void *)0, interrupt_init);

	IOWR(LED_PIO_BASE, 0, 0x01);
	IOWR(LED_PIO_BASE, 0, 0x00);


	//ENABLE the Button_PIO_IR
	IOWR( STIMULUS_IN_BASE, 2, 1);

	int egm_average_latency;
	int egm_missed;
	int egm_multi;

	int period = 2;
	//for (period = 2; period < 5000; period+=2) {
	while (period <= 5000){
		int counter = 0;
		IOWR(EGM_BASE, 0, 0);
		IOWR(EGM_BASE, 2, period);
		IOWR(EGM_BASE, 3, period/2);
		IOWR(EGM_BASE, 0, 1);
		//egm_init(period, egm_period, egm_pulse_width);

		while(IORD(EGM_BASE, 1)) {
			//egm_busy = IORD(EGM_BASE, 1);
			//led 0 on
			background();
			//led 0 off
			counter++;
		}
		egm_average_latency = IORD(EGM_BASE, 4);
		egm_missed = IORD(EGM_BASE, 5);
		egm_multi = IORD(EGM_BASE, 6);
		printf("Average Latency: %d, Missed: %d, Multi: %d\n", egm_average_latency, egm_missed, egm_multi);
		printf("Period: %d\n", period);
		printf("Tasks: %d\n", counter);

		IOWR(EGM_BASE, 0, 1);
		period += 2;
	}

	printf("Done");

}

void egm_init(int period){
	//Disable EGM
	IOWR(EGM_BASE, 0, 0);
	//egm_period
	IOWR(EGM_BASE, 2, period);
	//egm_pulse_width
	IOWR(EGM_BASE, 3, period/2);
	//egm_enable
	IOWR(EGM_BASE, 0, 1);
}

int background() {
	int j;
	int x = 0;
	int grainsize = 4;
	int g_taskProcessed = 0;

	//IOWR(LED_PIO_BASE, 0, 0x01);

    //led 0 on
    int led = IORD(LED_PIO_BASE, 0) | 1;

	IOWR(LED_PIO_BASE, 0, led);


	for(j = 0; j < grainsize; j++) {
		g_taskProcessed++;
	}

    led = IORD(LED_PIO_BASE, 0) & 14;
    //led 0 off

	IOWR(LED_PIO_BASE, 0, led);


	return x;
}

void polling(){

    //led 1 on start of test
    int led = IORD(LED_PIO_BASE, 0) | 2;

    IOWR(LED_PIO_BASE, 0, led);


    led = IORD(LED_PIO_BASE, 0) & 13;
    //led 1 off

	IOWR(LED_PIO_BASE, 0, led);


	//polling test for loop
	int period = 2;

	for(period = 2; period <= 5000; period += 2){
		int initial = 1;
		int tasks = 0;
		//int char_cycle = 1;
		//int curr = 0;
		int prev = 3;

		int egm_average_latency1;
		int egm_missed1;
		int egm_multi1;

		egm_init(period);

		IOWR(RESPONSE_OUT_BASE, 0, 0);
		// prev is 1, curr is 0
		while (1){
			if ((initial == 1) && ((IORD(STIMULUS_IN_BASE, 0) == 1))){
				initial--;
				IOWR(RESPONSE_OUT_BASE, 0, 1);
				IOWR(RESPONSE_OUT_BASE, 0, 0);
				prev = 1;
				break;
			}
		}
		// prev is 0, curr is 1
		while(IORD(EGM_BASE, 1)){
			background();
			tasks++;
			if ((IORD(STIMULUS_IN_BASE, 0) != 1) && (prev == 1)){
				prev = 0;
				break;
			}
		}
		// prev is 1, curr is 0
		while (IORD(EGM_BASE, 1)){
			if (IORD(STIMULUS_IN_BASE, 0) == 0){
				background();
				prev = 1;
				tasks++;
			}
			if (((IORD(STIMULUS_IN_BASE, 0) == 1) || (IORD(EGM_BASE, 1) == 0)) && (prev == 1)){
				break;
			}
		}

		//After characterization
		while(IORD(EGM_BASE, 1)){
			IOWR(RESPONSE_OUT_BASE, 0, 1);
			IOWR(RESPONSE_OUT_BASE, 0, 0);
			int x;
			for(x = 0; x < tasks; x++){
				background();
			}
			while(1){
				//conditions for next period to continue
				if ((IORD(STIMULUS_IN_BASE, 0) == 1) || (IORD(EGM_BASE, 1) == 0)){
					break;
				}
			}
		}
		egm_average_latency1 = IORD(EGM_BASE, 4);
		egm_missed1 = IORD(EGM_BASE, 5);
		egm_multi1 = IORD(EGM_BASE, 6);
		printf("Average Latency: %d, Missed: %d, Multi: %d\n", egm_average_latency1, egm_missed1, egm_multi1);
		printf("Period: %d\n", period);

		IOWR(EGM_BASE, 0, 1);
	}
}


//Polling Code
//void polling(){
//
//	//polling test for loop
//	int period = 2;
//
//	for(period = 2; period < 5000; period += 2){
//		//int initial = 0;
//		int tasks = 0;
//		int char_cycle = 1;
//		int curr = 0;
//		int prev = 0;
//		int test =0;
//		int overall_tasks = 0;
//
//
//		int egm_average_latency1;
//		int egm_missed1;
//		int egm_multi1;
//
//		egm_init(period);
//
//		IOWR(RESPONSE_OUT_BASE, 0, 0);
//
//		while((IORD(EGM_BASE, 1))){
//				prev = curr;
//				curr = IORD(STIMULUS_IN_BASE, 0);
//				//characterization cycle
//			  		if ((prev == 0) && (curr == 1)) {
//			  			printf("Made it here");
//						IOWR(RESPONSE_OUT_BASE, 0, 1);
//						IOWR(RESPONSE_OUT_BASE, 0, 0);
//						while(1 == 1){
//							background();
//							if(IORD(STIMULUS_IN_BASE, 0) == 1){
//								break;
//							}
//							overall_tasks++;
//							tasks++;
//						}
//						//printf("number of tasks we can execute: %d \n", tasks);
//						char_cycle = 0;
//					}
//
//			//The others
//
//
//		}
//
//			egm_average_latency1 = IORD(EGM_BASE, 4);
//			egm_missed1 = IORD(EGM_BASE, 5);
//			egm_multi1 = IORD(EGM_BASE, 6);
//			//printf("Average Latency: %d, Missed: %d, Multi: %d\n", egm_average_latency1, egm_missed1, egm_multi1);
//			//printf("Period: %d\n", period);
//
//			IOWR(EGM_BASE, 0, 1);
//	}
//}
//void polling(){
//
//	//polling test for loop
//	int period = 2;
//
//	for(period = 2; period < 5000; period += 2){
//		//int initial = 0;
//		int tasks = 0;
//		int char_cycle = 1;
//		int curr = 0;
//		int prev = 0;
//		int test =0;
//		int overall_tasks = 0;
//
//
//		int egm_average_latency1;
//		int egm_missed1;
//		int egm_multi1;
//
//		egm_init(period);
//
//		IOWR(RESPONSE_OUT_BASE, 0, 0);
//
//		while((IORD(EGM_BASE, 1))){
//				prev = curr;
//				curr = IORD(STIMULUS_IN_BASE, 0);
//				//characterization cycle
//			  	if(char_cycle == 1){
//			  		if ((prev == 0) && (curr == 1)) {
//			  			printf("Made it here");
//						IOWR(RESPONSE_OUT_BASE, 0, 1);
//						IOWR(RESPONSE_OUT_BASE, 0, 0);
//						while(1 == 1){
//							background();
//							if(IORD(STIMULUS_IN_BASE, 0) == 1){
//								break;
//							}
//							overall_tasks++;
//							tasks++;
//						}
//						//printf("number of tasks we can execute: %d \n", tasks);
//						char_cycle = 0;
//					}
//			  	}else{ // Test Run
//			  		//printf("number of tasks we can execute: %d \n", tasks);
//			  		if ((prev == 0) && (curr == 1)) {
//			  			printf("Made it here");
//						IOWR(RESPONSE_OUT_BASE, 0, 1);
//						IOWR(RESPONSE_OUT_BASE, 0, 0);
//						int x;
//						for(x = 0; x < tasks; x++){
//							background();
//							overall_tasks++;
//						}
//					}
//			  		//break;
//			  	}
//			  	printf("number of tasks we can execute: %d \n", overall_tasks);
//
//			//The others
//
//
//		}
//
//			egm_average_latency1 = IORD(EGM_BASE, 4);
//			egm_missed1 = IORD(EGM_BASE, 5);
//			egm_multi1 = IORD(EGM_BASE, 6);
//			//printf("Average Latency: %d, Missed: %d, Multi: %d\n", egm_average_latency1, egm_missed1, egm_multi1);
//			//printf("Period: %d\n", period);
//
//			IOWR(EGM_BASE, 0, 1);
//	}
//}

