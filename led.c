#include "led.h"

#define RED_LED 			4 // Port D Pin 4
#define GREEN_LED_0 7 // Port C Pin 7
#define GREEN_LED_1 0 // Port C Pin 0
#define GREEN_LED_2 3 // Port C Pin 3
#define GREEN_LED_3 4 // Port C Pin 4
#define GREEN_LED_4 5 // Port C Pin 5
#define GREEN_LED_5 6 // Port C Pin 6
#define GREEN_LED_6 10 // Port C Pin 10
#define GREEN_LED_7 11 // Port C Pin 11
#define MASK(x) 			(1 << (x))


const int greenLEDs[] = {GREEN_LED_0, GREEN_LED_1, GREEN_LED_2, GREEN_LED_3, GREEN_LED_4, GREEN_LED_5, GREEN_LED_6, GREEN_LED_7};


/*----------------------------------------------------------------------------
 * Init functions
 *---------------------------------------------------------------------------*/

 void initPIT(void) {
    // Enable clock to PIT module
    SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
    // Enable module, freeze timers in debug mode
    PIT->MCR &= ~PIT_MCR_MDIS_MASK;
    PIT->MCR |= PIT_MCR_FRZ_MASK;
    // Initialize PIT0 to count down from argument
    // (0.25 * 48,000,000) - 1 = 11,999,999
    PIT->CHANNEL[0].LDVAL = 11999999;
    // No chaining
    PIT->CHANNEL[0].TCTRL &= PIT_TCTRL_CHN_MASK;
    // Generate interrupts
    PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TIE_MASK;
    /* Enable Interrupts */
    NVIC_SetPriority(PIT_IRQn, 128); // 0, 64, 128 or 192
    NVIC_ClearPendingIRQ(PIT_IRQn);
    NVIC_EnableIRQ(PIT_IRQn);
 }

 void startPIT(void) {
    // Enable counter
    PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK;
 }
 void stopPIT(void) {
    // Enable counter
    PIT->CHANNEL[0].TCTRL &= ~PIT_TCTRL_TEN_MASK;
 }
 
 
volatile int redPeriod = 0; // counts from 0-3, increments every 250ms
 
int getRedPeriod(){
	return redPeriod;
}
 
void PIT_IRQHandler(void) {
	
    //clear pending IRQ
    NVIC_ClearPendingIRQ(PIT_IRQn);
    if (PIT->CHANNEL[0].TFLG & PIT_TFLG_TIF_MASK) {
        // clear status flag for timer channel 0
        PIT->CHANNEL[0].TFLG &= PIT_TFLG_TIF_MASK;
        redPeriod = (redPeriod + 1) % 4;
    }
}


 void initLEDsGPIO(void){
    // // IO PINS
	// Enable Clock to PORTD and PORTC
	SIM->SCGC5 |= ((SIM_SCGC5_PORTD_MASK) | (SIM_SCGC5_PORTC_MASK));
	
	 // Configure MUX settings and port data directions
	PORTD->PCR[RED_LED] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[RED_LED] |= PORT_PCR_MUX(1);
	PTD->PDDR |= MASK(RED_LED);
	
	 for (int i = 0; i < NUM_LEDS; i++){
     PORTC->PCR[greenLEDs[i]] &= ~PORT_PCR_MUX_MASK;
     PORTC->PCR[greenLEDs[i]] |= PORT_PCR_MUX(1);
		 PTC->PDDR |= MASK(greenLEDs[i]);
   }
	
 }

void initLEDs(void){
    initPIT();
    initLEDsGPIO();
}

// This function activates a single color of the RGB LED
// of the FRDMKL25Z board
// Note that the LEDS are active low
void ledControl(color_t color, int turnOn, int position){
	if(turnOn){
		switch(color){
			case RED:
				PTB->PDOR &= ~(unsigned int)MASK(RED_LED);
				break;
			case GREEN:
				PTB->PDOR &= ~(unsigned int)MASK(greenLEDs[position]);
				break;
			case NONE:
				break;
		}
	}else{
		switch(color){
			case RED:
				PTB->PDOR |= (unsigned int)MASK(RED_LED);
				break;
			case GREEN:
				PTB->PDOR |= (unsigned int)MASK(greenLEDs[position]);
				break;
			case NONE:
				break;
		}
	}
}
