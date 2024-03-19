// Device header
#include "motor.h"
#include "uart.h"
#include "led.h"
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

void tLED(void *argument)
{
	int greenCounter = 0;
	int turnOff = 0;
	int turnOn = 1;
	for (;;)
	{
		if (motorSpeed.leftSpeed != 0)
		{
			// Moving
			// Turn on 1 green LED at a time
			for (int i = 0; i < NUM_LEDS; i++)
			{
				ledControl(GREEN, turnOff, i);
			}
			ledControl(GREEN, turnOn, greenCounter);
			greenCounter++;
			// Toggle all red LEDs
			if (redPeriod <= 1)
			{
				ledControl(RED, turnOn, 0);
			}
			else
			{
				ledControl(RED, turnOff, 0);
			}
		}
		else
		{
			// Stationary
			// Turn on all green LEDs
			for (int i = 0; i < NUM_LEDS; i++)
			{
				ledControl(GREEN, turnOn, i);
			}
			// Turn on all red LEDs
			if (redPeriod % 2)
			{
				ledControl(RED, turnOn, 0);
			}
			else
			{
				ledControl(RED, turnOff, 0);
			}
		}
	}
}

void tAudio(void *argument)
{
}

int main(void)
{
	// Initialization
	SystemCoreClockUpdate();
	initPWM();
	initUART2(BAUD_RATE);
	osKernelInitialize(); // Initialize CMSIS-RTOS

	// Threads
	osThreadNew(tBrain, NULL, NULL);
	osThreadNew(tMotorControl, NULL, NULL);
	osThreadNew(tLED, NULL, NULL);
	osThreadNew(tAudio, NULL, NULL);
	osKernelStart();
}
