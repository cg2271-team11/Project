#include "audio.h"

static void delay(uint32_t milliseconds) {
	volatile uint32_t nof = milliseconds *4800;
	while(nof!=0) {
		__asm("NOP");
		nof--;
	}
}

volatile bool courseEnded = false;

void setCourseEnded(bool updatedCourseEnded){
	courseEnded = updatedCourseEnded;
}
void initAudioPWM(void) {
	// Enable clock gating to port C module
	SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;

	// Configure Mode 3 for PWM pin operation
	PORTD->PCR[BUZZER_PIN] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[BUZZER_PIN] |= PORT_PCR_MUX(4);

	// Enable clock gating for Timer0
	SIM->SCGC6 |= (SIM_SCGC6_TPM0_MASK);

	// Select clock for TPM module
	SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1); // MCGFLLCLK OR MCGPLLCLK/2

	// set modulo value 48000000/128 = 375000, 375000Hz/(50Hz) = 7500 Edge aligned
	TPM0->MOD = 7500;

	// Set Edge aligned PWM mode
	TPM0->SC &= ~(TPM_SC_CPWMS_MASK);

	// Set prescaler and counting mode
	TPM0->SC &= ~((TPM_SC_PS_MASK) | (TPM_SC_CMOD_MASK));
	TPM0->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7));

	// enable PWM on the TPM and channel
	TPM0_C5SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM0_C5SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));

	// off buzzer first
	TPM0_C5V = 0;
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

void playEndingTheme(void) {
		 
const int CD = 600;
	uint32_t endingMelody[] = {
			C4,E4,G4,C5,E5,G5,E5,0,C4,DS4,GS4,C5,DS5,GS5,E5,0,
			D4,F4,AS4,D5,F5,AS5,AS5,AS5,AS5,C6,0
	};

	uint32_t endingNoteDurations[] = {
		CD/3,CD/3,CD/3,CD/3,CD/3,CD,CD/2,CD,CD/3,CD/3,CD/3,CD/3,CD/3,CD,CD/2,CD,CD/3,CD/3,CD/3,CD/3,CD/3,CD,CD/3,CD/3,CD/3,CD * 4,CD
	};
	int endingNotes = sizeof(endingMelody) / sizeof(endingMelody[0]);
	for (int thisNote = 0; thisNote < endingNotes; thisNote++) {
			// To play a note, set the PWM frequency to the note's frequency
			setNote(endingMelody[thisNote]);
			// To simulate the note's duration, wait for the duration, then stop.
			delay(endingNoteDurations[thisNote] / 4);
	}
}

void playBeginningTheme() {
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
	
	while (!courseEnded) {
		int beginningNotes = sizeof(beginningMelody) / sizeof(beginningMelody[0]);
		for (int thisNote = 0; thisNote < beginningNotes; thisNote++) {
			if (courseEnded)
			{
				break;
			}
			// To play a note, set the PWM frequency to the note's frequency
			setNote(beginningMelody[thisNote]);
			// To simulate the note's duration, wait for the duration, then stop.
			delay(beginningNoteDurations[thisNote] / 4);
		}
	}
	playEndingTheme();
}