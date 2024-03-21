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

		int16_t y_speed = 0;
		switch(axisValues.y_axis) {
			case 0x01:
				y_speed = -3750;
				break;
			case 0x02:
				y_speed = -1600;
				break;
			case 0x03:
				y_speed = -800;
				break;
			case 0x04:
				y_speed = 0;
				break;
			case 0x05:
				y_speed = 800;
				break;
			case 0x06:
				y_speed = 1600;
				break;
			case 0x07:
				y_speed = 3750;
				break;								
		}
		int16_t leftSpeed = y_speed;
		int16_t rightSpeed = y_speed;
		int direction = 0;
		if(y_speed >= 0){
			direction = 1;
		}else{
			direction = -1;
		}
		switch(axisValues.x_axis) {
			case 0x01:
				// Most right
				rightSpeed -= direction * 3750;
				break;
			case 0x02:
				rightSpeed -= direction * 1600;
				break;
			case 0x03:
				rightSpeed -= direction * 800;
				break;
			case 0x04: 
				// Do not change status of right and left speed at all
				break;
			case 0x05:
				leftSpeed -= direction * 800;
				break;
			case 0x06:
				leftSpeed -= direction * 1600;
				break;
			case 0x07:
				// Most left
				leftSpeed -= direction * 3750;
				break;								
		}
		moveAll(leftSpeed, rightSpeed);
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