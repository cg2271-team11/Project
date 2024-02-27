                  // Device header
#include "motor.h"
#include "uart.h"

static void delay(uint32_t milliseconds) {
	volatile uint32_t nof = milliseconds *4800;
	while(nof!=0) {
		__asm("NOP");
		nof--;
	}
}

struct AxisValues axisValues;

int main(void)
{
	SystemCoreClockUpdate();
	initPWM();
	initUART2(BAUD_RATE);
	
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