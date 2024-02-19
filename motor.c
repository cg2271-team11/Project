#include "motor.h"

void initPWM(void) {
	//Enable Clock Gating for PORTB
	SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
	
	// Configure Mode 3 for the PWM pin operation, because of alt3
	PORTB->PCR[LEFT_FRONT_W] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[LEFT_FRONT_W] |= PORT_PCR_MUX(3);
	PORTB->PCR[LEFT_BACK_W] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[LEFT_BACK_W] |= PORT_PCR_MUX(3);
	
	PORTB->PCR[RIGHT_FRONT_W] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[RIGHT_FRONT_W] |= PORT_PCR_MUX(3);
	PORTB->PCR[RIGHT_BACK_W] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[RIGHT_BACK_W] |= PORT_PCR_MUX(3);
	
	// Enable Clock Gating for the Timer 0 and 1
	SIM->SCGC6 |= (SIM_SCGC6_TPM1_MASK) | (SIM_SCGC6_TPM2_MASK);
	
	// Select Clock for PWM module, using internal clock
	SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
	SIM_SOPT2 |= SIM_SOPT2_TPMSRC(1); // MCGFLLCLK or MCGPLLCLK/2

	// Set Modulo Value 48000000 / 128 = 375000 / 7500 = 50 Hz
	TPM1->MOD = 3750;
	TPM2->MOD = 3750;
	
	// Enabling of Center Aligned PWM - Lower EMI and Reduced current ripple compared to edge-aligned
	TPM1->SC |= TPM_SC_CPWMS_MASK;
	TPM2->SC |= TPM_SC_CPWMS_MASK;
	
	// Set PWM Duty cycle to 50%
	TPM1_C0V = 0; 	// LEFT_FRONT_W
	TPM1_C1V = 0; 		// LEFT_BACK_W
	TPM2_C0V = 0;  // RIGHT_FRONT_W
	TPM2_C1V = 0; 		// RIGHT_BACK_W
	
	// Update SnC register: CMOD = 01, PS=111 (128) // upcounting only, divide by 128
	TPM1->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
	TPM1->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7));

	TPM2->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
	TPM2->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7));
	
	// Enable PWM on TPM1 Channel 0 -> PTB0
	TPM1_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM1_C0SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
	
	// Enable PWM on TPM1 Channel 1 -> PTB1
	TPM1_C1SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM1_C1SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
	
	// Enable PWM on TPM2 Channel 0 -> PTB2
	TPM2_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM2_C0SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
	
	// Enable PWM on TPM2 Channel 1 -> PTB3
	TPM2_C1SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM2_C1SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
	
}

void stopMotor(void) {
	TPM1_C0V = 0; 	// LEFT_FRONT_W
	TPM1_C1V = 0; 		// LEFT_BACK_W
	TPM2_C0V = 0;  // RIGHT_FRONT_W
	TPM2_C1V = 0; 		// RIGHT_BACK_W
}
void move(uint16_t speed) {
	TPM1_C0V = speed; 	// LEFT_FRONT_W
	TPM1_C1V = 0; 		// LEFT_BACK_W
	TPM2_C0V = speed;  // RIGHT_FRONT_W
	TPM2_C1V = 0; 		// RIGHT_BACK_W
}
void turnLeft(uint16_t speed) {
	TPM1_C0V = speed; 	// LEFT_FRONT_W
	TPM1_C1V = 0; 		// LEFT_BACK_W
	TPM2_C0V = 0;  // RIGHT_FRONT_W
	TPM2_C1V = 0; 		// RIGHT_BACK_W
}
void turnRight(uint16_t speed) {
	TPM1_C0V = 0; 	// LEFT_FRONT_W
	TPM1_C1V = 0; 		// LEFT_BACK_W
	TPM2_C0V = speed;  // RIGHT_FRONT_W
	TPM2_C1V = 0; 		// RIGHT_BACK_W
}