// Device header
#include "motor.h"
#include "uart.h"
#include "led.h"
#include "audio.h"
#include "RTE_Components.h"
#include CMSIS_device_header
#include "cmsis_os2.h"
#include <stdbool.h>

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
struct UartValues uartValues;
struct MotorSpeed motorSpeed;
volatile uint8_t button;

void tBrain(void *argument)
{
  // Should instead send some value to motor thread
  // which handles the movement
  while (1)
  {
    uartValues = extractUartValues();
		button = uartValues.button;
		if (button == 1)
		{
			setCourseEnded(true);
		}
    motorSpeed = calculateSpeed(uartValues.x_axis, uartValues.y_axis);
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
	int isRedOn = 1;
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
		if (motorSpeed.leftSpeed != 0 || motorSpeed.rightSpeed != 0)
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
	playBeginningTheme();
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
