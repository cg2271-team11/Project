// Device header
#include "motor.h"
#include "uart.h"
#include "led.h"
#include "audio.h"
#include "RTE_Components.h"
#include CMSIS_device_header
#include "cmsis_os2.h"

static void delay(uint32_t milliseconds)
{
	volatile uint32_t nof = milliseconds * 4800;
	while (nof != 0)
	{
		__asm("NOP");
		nof--;
	}
}

// 0 = Stationary, 1 = Moving
volatile int isMoving = 0;

// Only accessed by perform movement thread for now
// At global to be seen in debugger
struct AxisValues axisValues;
struct MotorSpeed motorSpeed;
void tBrain(void *argument)
{
	// Should instead send some value to motor thread
	// which handles the movement
	while (1)
	{
		axisValues = extractAxisValues();
		motorSpeed = calculateSpeed(axisValues.x_axis, axisValues.y_axis);
		moveAll(motorSpeed.leftSpeed, motorSpeed.rightSpeed);
	}
}

void tMotorControl(void *argument)
{
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

osThreadId_t tid_movingLEDThread;
osThreadId_t tid_stationaryLEDThread;

int turnOff = 0;
int turnOn = 1;
void movingLEDThread(void *argument)
{
	int greenCounter = 0;
	int onRed = 1;
	for (;;)
	{
		osThreadFlagsWait(0x0001, osFlagsWaitAny, osWaitForever);
		// Moving
		for (int i = 0; i < NUM_LEDS; i++)
		// Turn on 1 green LED at a time
		{
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

void stationaryLEDThread(void *argument)
{
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

void tLED(void *argument)
{
	tid_movingLEDThread = osThreadNew(movingLEDThread, NULL, NULL);
	tid_stationaryLEDThread = osThreadNew(stationaryLEDThread, NULL, NULL);
	for (;;)
	{
		if (isMoving)
		{
			osThreadFlagsSet(tid_movingLEDThread, 0x0001);
		}
		else
		{
			osThreadFlagsSet(tid_stationaryLEDThread, 0x0001);
		}
	}
}

void setNote(uint32_t freq)
{
    if (freq == 0)
    {
        TPM0->MOD = 0;
				TPM0_C5V = 0;
        return;
    }
    uint32_t mod = (375000 / (freq));
    TPM0->MOD = mod;
    TPM0_C5V = mod / 2; // 50% duty cycle
}

void tAudio(void* argument){
   uint32_t beginningMelody[] = {
    E4, E4, 0, E4, 0, C4, E4, 0, G4, 0, 0,  G3, 0, 0, 0,
    C4, 0, 0, G3, 0, 0, E3, 0, 0, A3, 0, B3, 0, AS3, A3, 0,
    G3, E4, G4, A4, 0, F4, G4, 0, E4, 0, C4, D4, B3, 0, 0,
    C4, 0, 0, G3, 0, 0, E3, 0, 0, A3, 0, B3, 0, AS3, A3, 0,
    G3, E4, G4, A4, 0, F4, G4, 0, E4, 0, C4, D4, B3, 0, 0
    // This is a simplified and partial representation.
	};
	 uint32_t beginningNoteDurations[] = {
    150, 150, 100, 150, 100, 300, 300, 100, 300, 100, 100, 300, 100, 100, 100,
    300, 100, 100, 300, 100, 100, 300, 100, 100, 300, 100, 300, 100, 300, 100,
    300, 300, 300, 300, 100, 300, 300, 100, 300, 100, 300, 300, 300, 100, 100,
    300, 100, 100, 300, 100, 100, 300, 100, 100, 300, 100, 300, 100, 300, 100,
    300, 300, 300, 300, 100, 300, 300, 100, 300, 100, 300, 300, 300, 100, 100
};
	 
const int CD = 600;
uint32_t endingMelody[] = {
    C4,E4,G4,C5,E5,G5,E5,0,C4,DS4,GS4,C5,DS5,GS5,E5,0,
		D4,F4,AS4,D5,F5,AS5,AS5,AS5,AS5,C6
};

// Corresponding note durations in milliseconds (ms)
uint32_t endingNoteDurations[] = {
	CD/3,CD/3,CD/3,CD/3,CD/3,CD,CD/2,CD,CD/3,CD/3,CD/3,CD/3,CD/3,CD,CD/2,CD,CD/3,CD/3,CD/3,CD/3,CD/3,CD,CD/3,CD/3,CD/3,CD * 4
};

    while (1) {
        int beginningNotes = sizeof(beginningMelody) / sizeof(beginningMelody[0]);
        int endingNotes = sizeof(endingMelody) / sizeof(endingMelody[0]);
			//setNote(E4);
        for (int thisNote = 0; thisNote < beginningNotes; thisNote++) {
            // To play a note, set the PWM frequency to the note's frequency
					setNote(beginningMelody[thisNote]);

            // To simulate the note's duration, wait for the duration, then stop.
          delay(beginningNoteDurations[thisNote] / 4);
            //TPM0_C5V = 0; // Stop the note
            //delay(50); // Delay between notes
        }
    }
}

int main(void)
{
	// Initialization
	SystemCoreClockUpdate();
	initPWM();
	initLEDs();
	initAudioPWM();
	initUART2(BAUD_RATE);
	osKernelInitialize(); // Initialize CMSIS-RTOS

	// Threads
	osThreadNew(tBrain, NULL, NULL);
	osThreadNew(tMotorControl, NULL, NULL);
	osThreadNew(tLED, NULL, NULL);
	osThreadNew(tAudio, NULL, NULL);
	osKernelStart();
}
