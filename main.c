// Device header
#include "motor.h"
#include "uart.h"
#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"

static void delay(uint32_t milliseconds) {
	volatile uint32_t nof = milliseconds *4800;
	while(nof!=0) {
		__asm("NOP");
		nof--;
	}
}

// Only accessed by perform movement thread for now
// At global to be seen in debugger
struct AxisValues axisValues;
void tBrain(void* argument){
	// Should instead send some value to motor thread 
	// which handles the movement
	while(1) {
		axisValues = extractAxisValues();
		switch(axisValues.x_axis) {
			case 0x01:
				goRight(3750);
				break;
			case 0x02:
				goRight(1600);
				break;
			case 0x03:
				goRight(800);
				break;
			case 0x04: 
				stopMotor();
				break;
			case 0x05:
				goLeft(800);
				break;
			case 0x06:
				goLeft(1600);
				break;
			case 0x07:
				goLeft(3750);
				break;								
		}
		switch(axisValues.y_axis) {
			case 0x01:
				reverse(3750);
				break;
			case 0x02:
				reverse(1600);
				break;
			case 0x03:
				reverse(800);
				break;
			case 0x04:
				stopMotor();
				break;
			case 0x05:
				move(800);
				break;
			case 0x06:
				move(1600);
				break;
			case 0x07:
				move(3750);
				break;								
		}
	}
}

void tMotorControl(void* argument){
	
}

void tLED(void* argument){
	
}

void tAudio(void* argument){
}

int main(void)
{
	// Initialization
	SystemCoreClockUpdate();
	initPWM();
	initUART2(BAUD_RATE);
	osKernelInitialize();                 // Initialize CMSIS-RTOS
	
	// Threads
  osThreadNew(tBrain, NULL, NULL);
	osThreadNew(tMotorControl, NULL, NULL);
	osThreadNew(tLED, NULL, NULL);
	osThreadNew(tAudio, NULL, NULL);
  osKernelStart();

}