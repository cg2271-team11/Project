// Device header
#include "motor.h"
#include "uart.h"
#include "audio.h"
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
		int shouldStop = 0;
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
				shouldStop = 1;
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
				if(shouldStop){
					stopMotor();
				}
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