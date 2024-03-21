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

void tAudio(void* argument){
   uint32_t melody[] = {
     E4, G4, A4, A4, 0,
    A4, B4, C5, C5, 0,
    C5, D5, B4, B4, 0,
    A4, G4, A4, 0,

    E4, G4, A4, A4, 0,
    A4, B4, C5, C5, 0,
    C5, D5, B4, B4, 0,
    A4, G4, A4, 0,

    E4, G4, A4, A4, 0,
    A4, C5, D5, D5, 0,
    D5, E5, F5, F5, 0,
    E5, D5, E5, A4, 0,

    A4, B4, C5, C5, 0,
    D5, E5, A4, 0,
    A4, C5, B4, B4, 0,
    C5, A4, B4, 0,

    A4, A4,
    //Repeat of first part
    A4, B4, C5, C5, 0,
    C5, D5, B4, B4, 0,
    A4, G4, A4, 0,

    E4, G4, A4, A4, 0,
    A4, B4, C5, C5, 0,
    C5, D5, B4, B4, 0,
    A4, G4, A4, 0,

    E4, G4, A4, A4, 0,
    A4, C5, D5, D5, 0,
    D5, E5, F5, F5, 0,
    E5, D5, E5, A4, 0,

    A4, B4, C5, C5, 0,
    D5, E5, A4, 0,
    A4, C5, B4, B4, 0,
    C5, A4, B4, 0,
    //End of Repeat

    E5, 0, 0, F5, 0, 0,
    E5, E5, 0, G5, 0, E5, D5, 0, 0,
    D5, 0, 0, C5, 0, 0,
    B4, C5, 0, B4, 0, A4,

    E5, 0, 0, F5, 0, 0,
    E5, E5, 0, G5, 0, E5, D5, 0, 0,
    D5, 0, 0, C5, 0, 0,
    B4, C5, 0, B4, 0, A4
    };

    // Note durations: 1 = quarter note, 2 = half note, etc.
    // For a faster tempo, you may adjust these durations
    uint32_t noteDurations[] = {
          125, 125, 250, 125, 125,
    125, 125, 250, 125, 125,
    125, 125, 250, 125, 125,
    125, 125, 375, 125,

    125, 125, 250, 125, 125,
    125, 125, 250, 125, 125,
    125, 125, 250, 125, 125,
    125, 125, 375, 125,

    125, 125, 250, 125, 125,
    125, 125, 250, 125, 125,
    125, 125, 250, 125, 125,
    125, 125, 125, 250, 125,

    125, 125, 250, 125, 125,
    250, 125, 250, 125,
    125, 125, 250, 125, 125,
    125, 125, 375, 375,

    250, 125,
    //Rpeat of First Part
    125, 125, 250, 125, 125,
    125, 125, 250, 125, 125,
    125, 125, 375, 125,

    125, 125, 250, 125, 125,
    125, 125, 250, 125, 125,
    125, 125, 250, 125, 125,
    125, 125, 375, 125,

    125, 125, 250, 125, 125,
    125, 125, 250, 125, 125,
    125, 125, 250, 125, 125,
    125, 125, 125, 250, 125,

    125, 125, 250, 125, 125,
    250, 125, 250, 125,
    125, 125, 250, 125, 125,
    125, 125, 375, 375,
    //End of Repeat

    250, 125, 375, 250, 125, 375,
    125, 125, 125, 125, 125, 125, 125, 125, 375,
    250, 125, 375, 250, 125, 375,
    125, 125, 125, 125, 125, 500,

    250, 125, 375, 250, 125, 375,
    125, 125, 125, 125, 125, 125, 125, 125, 375,
    250, 125, 375, 250, 125, 375,
    125, 125, 125, 125, 125, 500
    };

    while (1) {
        int notes = sizeof(melody) / sizeof(melody[0]);

        for (int thisNote = 0; thisNote < notes; thisNote++) {
            // To play a note, set the PWM frequency to the note's frequency
					if (melody[thisNote] != 0) {
						TPM0_C5V = 0; //melody[thisNote];
					} else {
						TPM0_C5V = 0; // Stop the notef
					}

            // To simulate the note's duration, wait for the duration, then stop.
            delay(noteDurations[thisNote]);
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
