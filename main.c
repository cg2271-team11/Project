#include "MKL25Z4.h"                    // Device header
#include "motor.h"

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
	
	while(1) {
		move(3700);
		delay(100);
		stopMotor();
		delay(1000);
		turnLeft(3750);
		delay(1000);
		turnRight(3750);
		delay(1000);
	}
	
}