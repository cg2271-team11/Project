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