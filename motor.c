#include "motor.h"

void initPWM(void)
{
	// Enable Clock Gating for PORTB
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
	TPM1->MOD = 7499;
	TPM2->MOD = 7499;

	// Enabling of Center Aligned PWM - Lower EMI and Reduced current ripple compared to edge-aligned
	TPM1->SC |= TPM_SC_CPWMS_MASK;
	TPM2->SC |= TPM_SC_CPWMS_MASK;

	// Set PWM Duty cycle to 50%
	TPM1_C0V = 0; // LEFT_FRONT_W
	TPM1_C1V = 0; // LEFT_BACK_W
	TPM2_C0V = 0; // RIGHT_FRONT_W
	TPM2_C1V = 0; // RIGHT_BACK_W

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

void stopMotor(void)
{
	TPM1_C0V = 0; // LEFT_FRONT_W
	TPM1_C1V = 0; // LEFT_BACK_W
	TPM2_C0V = 0; // RIGHT_FRONT_W
	TPM2_C1V = 0; // RIGHT_BACK_W
}

void moveAll(int16_t leftSpeed, int16_t rightSpeed)
{
	if (leftSpeed == 0)
	{
		TPM1_C0V = 0;
		TPM1_C1V = 0;
	}
	else if (leftSpeed > 0)
	{
		TPM1_C0V = leftSpeed;
		TPM1_C1V = 0;
	}
	else
	{
		TPM1_C1V = -leftSpeed;
		TPM1_C0V = 0;
	}

	if (rightSpeed == 0)
	{
		TPM2_C0V = 0;
		TPM2_C1V = 0;
	}
	else if (rightSpeed > 0)
	{
		TPM2_C0V = rightSpeed;
		TPM2_C1V = 0;
	}
	else
	{
		TPM2_C1V = -rightSpeed;
		TPM2_C0V = 0;
	}
}



struct MotorSpeed calculateSpeed(uint8_t x_axis, uint8_t y_axis)
{
	struct MotorSpeed motorSpeed;

	const int16_t modifier = 2200;


	int16_t speed = 0;
	switch (y_axis)
	{
	case 0x01:
		speed = -7500;
		break;
	case 0x02:
		speed = -5000;
		break;
	case 0x03:
		speed = 0;
		break;
	case 0x04:
		speed = 0;
		break;
	case 0x05:
		speed = 5000;
		break;
	case 0x06:
		speed = 7500;
		break;
	case 0x07:
		speed = 7500;
		break;
	}

	motorSpeed.leftSpeed = speed;
	motorSpeed.rightSpeed = speed;

	if (speed > 0)
	{
		switch (x_axis)
		{
		case 0x01:
			motorSpeed.leftSpeed -= 3 * modifier;
			break;
		case 0x02:
			motorSpeed.leftSpeed -= 2 * modifier;
			break;
		case 0x03:
			motorSpeed.leftSpeed -= 1 * modifier;
			break;
		case 0x04:
			break;
		case 0x05:
			motorSpeed.rightSpeed -= 1 * modifier;
			break;
		case 0x06:
			motorSpeed.rightSpeed -= 2 * modifier;
			break;
		case 0x07:
			motorSpeed.rightSpeed -= 3 * modifier;
			break;
		}
	}
	else if (speed < 0)
	{
		switch (x_axis)
		{
		case 0x01:
			motorSpeed.leftSpeed += 3 * modifier;
			break;
		case 0x02:
			motorSpeed.leftSpeed += 2 * modifier;
			break;
		case 0x03:
			motorSpeed.leftSpeed += 1 * modifier;
			break;
		case 0x04:
			break;
		case 0x05:
			motorSpeed.rightSpeed += 1 * modifier;
			break;
		case 0x06:
			motorSpeed.rightSpeed += 2 * modifier;
			break;
		case 0x07:
			motorSpeed.rightSpeed += 3 * modifier;
			break;
		}
	}
	else
	{
		switch (x_axis)
		{
		case 0x01:
			motorSpeed.leftSpeed -= 2 * 2500;
			motorSpeed.rightSpeed += 2* 2500;
			break;
		case 0x02:
			motorSpeed.leftSpeed -= 1.5 * 2500;
			motorSpeed.rightSpeed += 1.5 * 2500;
			break;
		case 0x03:
			motorSpeed.leftSpeed -= 1 * 2500;
			motorSpeed.rightSpeed += 1 * 2500;
			break;
		case 0x04:
			break;
		case 0x05:
			motorSpeed.leftSpeed += 1 * 2500;
			motorSpeed.rightSpeed -= 1 * 2500;
			break;
		case 0x06:
			motorSpeed.leftSpeed += 1.5 * 2500;
			motorSpeed.rightSpeed -= 1.5 * 2500;
			break;
		case 0x07:
			motorSpeed.leftSpeed += 2 * 2500;
			motorSpeed.rightSpeed -= 2 * 2500;
			break;
		}
	}

	return motorSpeed;
}


struct MotorSpeed prototypeCalculateSpeed(uint8_t x_axis, uint8_t y_axis)
{
	struct MotorSpeed motorSpeed;

	const int16_t modifier = 1000;


	int16_t speed = 0;
	switch (y_axis)
	{
	case 0x01:
		speed = -7500;
		break;
	case 0x02:
		speed = -5000;
		break;
	case 0x03:
		speed = -2500;
		break;
	case 0x04:
		speed = 0;
		break;
	case 0x05:
		speed = 2500;
		break;
	case 0x06:
		speed = 5000;
		break;
	case 0x07:
		speed = 7500;
		break;
	}

	motorSpeed.leftSpeed = speed;
	motorSpeed.rightSpeed = speed;

	if (speed > 0)
	{
		switch (x_axis)
		{
		case 0x01:
			motorSpeed.leftSpeed -= 3 * modifier; // max 7500, start from 5500
			break;
		case 0x02:
			motorSpeed.leftSpeed -= 2 * modifier;
			break;
		case 0x03:
			motorSpeed.leftSpeed -= 1 * modifier;
			break;
		case 0x04:
			break;
		case 0x05:
			motorSpeed.rightSpeed -= 1 * modifier; 
			break;
		case 0x06:
			motorSpeed.rightSpeed -= 2 * modifier;
			break;
		case 0x07:
			motorSpeed.rightSpeed -= 3 * modifier;
			break;
		}
	}
	else if (speed < 0)
	{
		switch (x_axis)
		{
		case 0x01:
			motorSpeed.leftSpeed += 3 * modifier;
			break;
		case 0x02:
			motorSpeed.leftSpeed += 2 * modifier;
			break;
		case 0x03:
			motorSpeed.leftSpeed += 1 * modifier;
			break;
		case 0x04:
			break;
		case 0x05:
			motorSpeed.rightSpeed += 1 * modifier;
			break;
		case 0x06:
			motorSpeed.rightSpeed += 2 * modifier;
			break;
		case 0x07:
			motorSpeed.rightSpeed += 3 * modifier;
			break;
		}
	}
	else
	{
		switch (x_axis)
		{
		case 0x01:
			motorSpeed.leftSpeed -= 3 * 2500;
			motorSpeed.rightSpeed += 3 * 2500;
			break;
		case 0x02:
			motorSpeed.leftSpeed -= 2 * 2500;
			motorSpeed.rightSpeed += 2 * 2500;
			break;
		case 0x03:
			motorSpeed.leftSpeed -= 1 * 2500;
			motorSpeed.rightSpeed += 1 * 2500;
			break;
		case 0x04:
			break;
		case 0x05:
			motorSpeed.leftSpeed += 1 * 2500;
			motorSpeed.rightSpeed -= 1 * 2500;
			break;
		case 0x06:
			motorSpeed.leftSpeed += 2 * 2500;
			motorSpeed.rightSpeed -= 2 * 2500;
			break;
		case 0x07:
			motorSpeed.leftSpeed += 3 * 2500;
			motorSpeed.rightSpeed -= 3 * 2500;
			break;
		}
	}

	return motorSpeed;
}
