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

int main(void)
{
	SystemCoreClockUpdate();
	initPWM();
	initUART2(BAUD_RATE);
	
	while(1) {
		struct AxisValues axisValues = extractAxisValues();
		
		switch(axisValues.y_axis) {
			case 1:
				reverse(3750);
			break;
			case 2:
				reverse(1600);
			break;
			case 3:
				reverse(800);
			break;
			case 4:
				stopMotor();
			break;
			case 5:
				move(800);
			break;
			case 6:
				move(1600);
			break;
			case 7:
				move(3750);
			break;
												
		}
		
		move(axisValues.y_axis);
	}
	
}