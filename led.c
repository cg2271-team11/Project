#include "led.h"

#define RED_LED 4	   // Port D Pin 4
#define GREEN_LED_0 7  // Port C Pin 7
#define GREEN_LED_1 0  // Port C Pin 0
#define GREEN_LED_2 3  // Port C Pin 3
#define GREEN_LED_3 4  // Port C Pin 4
#define GREEN_LED_4 5  // Port C Pin 5
#define GREEN_LED_5 6  // Port C Pin 6
#define GREEN_LED_6 10 // Port C Pin 10
#define GREEN_LED_7 11 // Port C Pin 11
#define MASK(x) (1 << (x))

const int greenLEDs[] = {GREEN_LED_0, GREEN_LED_1, GREEN_LED_2, GREEN_LED_3, GREEN_LED_4, GREEN_LED_5, GREEN_LED_6, GREEN_LED_7};

/*----------------------------------------------------------------------------
 * Init functions
 *---------------------------------------------------------------------------*/

void initLEDsGPIO(void)
{
	// // IO PINS
	// Enable Clock to PORTD and PORTC
	SIM->SCGC5 |= ((SIM_SCGC5_PORTD_MASK) | (SIM_SCGC5_PORTC_MASK));

	// Configure MUX settings and port data directions
	PORTD->PCR[RED_LED] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[RED_LED] |= PORT_PCR_MUX(1);
	PTD->PDDR |= MASK(RED_LED);

	for (int i = 0; i < NUM_LEDS; i++)
	{
		PORTC->PCR[greenLEDs[i]] &= ~PORT_PCR_MUX_MASK;
		PORTC->PCR[greenLEDs[i]] |= PORT_PCR_MUX(1);
		PTC->PDDR |= MASK(greenLEDs[i]);
	}
}

void initLEDs(void)
{
	initLEDsGPIO();
	for (int i = 0; i < NUM_LEDS; i++)
	{
		ledControl(GREEN, 0, i);
	}
}

// This function activates a single color of the RGB LED
// of the FRDMKL25Z board
// Note that the LEDS are active low
void ledControl(color_t color, int turnOn, int position)
{
	if (turnOn == 1)
	{
		switch (color)
		{
		case RED:
			PTD->PDOR &= ~(unsigned int)MASK(RED_LED);
			break;
		case GREEN:
			PTC->PDOR &= ~(unsigned int)MASK(greenLEDs[position]);
			break;
		case NONE:
			break;
		}
	}
	else
	{
		switch (color)
		{
		case RED:
			PTD->PDOR |= (unsigned int)MASK(RED_LED);
			break;
		case GREEN:
			PTC->PDOR |= (unsigned int)MASK(greenLEDs[position]);
			break;
		case NONE:
			break;
		}
	}
}

/**
 * LED thread
 * Requirements:
 * 1. Moving State
 *    a. The front 8-10 Green LED’s must be in a Running Mode (1 LED at a time)
 *    b. The rear 8-10 Red LED’s must be flashing continuously at a rate of 500ms ON, 500ms OFF
 * 2. Stationary State
 *    a. The front 8-10 Green LED’s must all be lighted up continuously
 *    b. The rear 8-10 Red LED’s must be flashing continuously at a rate of 250ms ON, 250ms OFF
 *
 * Dependencies:
 * State must be updated by the motor thread
 * Clock for timing the flashing
 **/


int turnOff = 0;
int turnOn = 1;
void movingLEDThread(void *argument) {
  int greenCounter = 0;
  int isRedOn = 1;
  for (;;) {
    osThreadFlagsWait(0x0001, osFlagsWaitAny, osWaitForever);
    // Moving
    // Turn on 1 green LED at a time
    for (int i = 0; i < NUM_LEDS; i++) {
      ledControl(GREEN, turnOff, i);
    }
    ledControl(GREEN, turnOn, greenCounter);
    greenCounter = greenCounter >= NUM_LEDS - 1 ? 0 : greenCounter + 1;
    // Toggle all red LEDs
    ledControl(RED, isRedOn, 0);
    isRedOn = isRedOn == 1 ? 0 : 1;
    osDelay(500);
  }
}

void stationaryLEDThread(void *argument) {
  int isRedOn = 1;
  for (;;)
  {
    osThreadFlagsWait(0x0001, osFlagsWaitAny, osWaitForever);
    // Stationary
    // Turn on all green
    for (int i = 0; i < NUM_LEDS; i++)
    {
      ledControl(GREEN, turnOn, i);
    }
    // Toggle all red LEDs
    ledControl(RED, isRedOn, 0);
    isRedOn = isRedOn == 1 ? 0 : 1;
    osDelay(250);
  }
}
