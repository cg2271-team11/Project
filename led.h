
#include "MKL25Z4.h"
#define NUM_LEDS 8

typedef enum
{
	RED,
	GREEN,
	NONE,
} color_t;

void initLEDs(void);
void ledControl(color_t color, int turnOn, int position);
void movingLEDThread(void *argument);
void stationaryLEDThread(void *argument);
