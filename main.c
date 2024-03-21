// Device header
#include "motor.h"
#include "uart.h"
#include "audio.h"
#include "RTE_Components.h"
#include CMSIS_device_header
#include "cmsis_os2.h"
#include <stdbool.h>

// Only accessed by perform movement thread for now
// At global to be seen in debugger
struct AxisValues axisValues;
// Only accessed by perform movement thread for now
// At global to be seen in debugger
struct UartValues uartValues;
struct MotorSpeed motorSpeed;
volatile uint8_t button;


bool courseEnded = false;

void tBrain(void* argument){
	// Should instead send some value to motor thread 
	// which handles the movement
	while (1)
    {
      uartValues = extractUartValues();
      button = uartValues.button;
      motorSpeed = calculateSpeed(uartValues.x_axis, uartValues.y_axis);
      moveAll(motorSpeed.leftSpeed, motorSpeed.rightSpeed);
    }
}

void tMotorControl(void *argument)
{
}

void tLED(void *argument)
{
}


void tAudio(void* argument){
	playBeginningTheme(courseEnded);
}

int main(void)
{
	// Initialization
	SystemCoreClockUpdate();
	initPWM();
	initAudioPWM();
	initUART2(BAUD_RATE);
	osKernelInitialize();                 // Initialize CMSIS-RTOS
	
	// Threads
  osThreadNew(tBrain, NULL, NULL);
	osThreadNew(tMotorControl, NULL, NULL);
	osThreadNew(tLED, NULL, NULL);
	osThreadNew(tAudio, NULL, NULL);
  osKernelStart();
	
}
