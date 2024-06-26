#include "MKL25Z4.h" // Device header

// Using PortB for Motors
#define LEFT_FRONT_W 0
#define LEFT_BACK_W 1
#define RIGHT_FRONT_W 2
#define RIGHT_BACK_W 3

void initPWM(void);

// Movements
void stopMotor(void);
void move(uint16_t speed);
void reverse(uint16_t speed);
void goLeft(uint16_t speed);
void goRight(uint16_t speed);
void moveAll(int16_t leftSpeed, int16_t rightSpeed);

struct MotorSpeed
{
  int16_t leftSpeed;
  int16_t rightSpeed;
};

struct MotorSpeed calculateSpeed(uint8_t x_axis, uint8_t y_axis);