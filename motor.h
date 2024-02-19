#include "MKL25Z4.h"                    // Device header

// Using PortB for Motors
#define LEFT_FRONT_W 0
#define LEFT_BACK_W 1
#define RIGHT_FRONT_W 1
#define RIGHT_BACK_W 2

void initPWM(void);

// Movements
void stopMotor(void);
void move(uint16_t speed);
void turnLeft(uint16_t speed);
void turnRight(uint16_t speed);
