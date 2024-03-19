#define NUM_LEDS 8

typedef enum{
	RED,
	GREEN,
	BLUE
}color_t;

void initLEDs(void);
void startPIT(void);
void stopPIT(void);
void ledControl(color_t color, int turnOn, int position);
