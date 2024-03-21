#include "audio.h"

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