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
