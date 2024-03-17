// Device header
#include "motor.h"
#include "uart.h"
#include "RTE_Components.h"
#include CMSIS_device_header
#include "cmsis_os2.h"

static void delay(uint32_t milliseconds)
{
  volatile uint32_t nof = milliseconds * 4800;
  while (nof != 0)
  {
    __asm("NOP");
    nof--;
  }
}

// Only accessed by perform movement thread for now
// At global to be seen in debugger
struct AxisValues axisValues;
void tBrain(void *argument)
{
  // Should instead send some value to motor thread
  // which handles the movement
  while (1)
  {
    axisValues = extractAxisValues();

    uint16_t speed = 0;
    switch (axisValues.y_axis)
    {
    case 0x01:
      speed = -3750;
      break;
    case 0x02:
      speed = -2500;
      break;
    case 0x03:
      speed = -1250;
      break;
    case 0x04:
      speed = 0;
      break;
    case 0x05:
      speed = 1250;
      break;
    case 0x06:
      speed = 2500;
      break;
    case 0x07:
      speed = 3750;
      break;
    }

    uint16_t leftSpeed = speed;
    uint16_t rightSpeed = speed;

    if (speed > 0)
    {
      switch (axisValues.x_axis)
      {
      case 0x01:
        leftSpeed -= 3 * 1250;
        break;
      case 0x02:
        leftSpeed -= 2 * 1250;
        break;
      case 0x03:
        leftSpeed -= 1 * 1250;
        break;
      case 0x04:
        break;
      case 0x05:
        rightSpeed -= 1 * 1250;
        break;
      case 0x06:
        rightSpeed -= 2 * 1250;
        break;
      case 0x07:
        rightSpeed -= 3 * 1250;
        break;
      }
    }
    else if (speed < 0)
    {
      switch (axisValues.x_axis)
      {
      case 0x01:
        leftSpeed += 3 * 1250;
        break;
      case 0x02:
        leftSpeed += 2 * 1250;
        break;
      case 0x03:
        leftSpeed += 1 * 1250;
        break;
      case 0x04:
        break;
      case 0x05:
        rightSpeed += 1 * 1250;
        break;
      case 0x06:
        rightSpeed += 2 * 1250;
        break;
      case 0x07:
        rightSpeed += 3 * 1250;
        break;
      }
    }
    else
    {
      switch (axisValues.x_axis)
      {
      case 0x01:
        leftSpeed -= 3 * 1250;
        rightSpeed += 3 * 1250;
        break;
      case 0x02:
        leftSpeed -= 2 * 1250;
        rightSpeed += 2 * 1250;
        break;
      case 0x03:
        leftSpeed -= 1 * 1250;
        rightSpeed += 1 * 1250;
        break;
      case 0x04:
        break;
      case 0x05:
        leftSpeed += 1 * 1250;
        rightSpeed -= 1 * 1250;
        break;
      case 0x06:
        leftSpeed += 2 * 1250;
        rightSpeed -= 2 * 1250;
        break;
      case 0x07:
        leftSpeed += 3 * 1250;
        rightSpeed -= 3 * 1250;
        break;
      }
    }
    moveAll(leftSpeed, rightSpeed);
  }
}

void tMotorControl(void *argument)
{
}

void tLED(void *argument)
{
}

void tAudio(void *argument)
{
}

int main(void)
{
  // Initialization
  SystemCoreClockUpdate();
  initPWM();
  initUART2(BAUD_RATE);
  osKernelInitialize(); // Initialize CMSIS-RTOS

  // Threads
  osThreadNew(tBrain, NULL, NULL);
  osThreadNew(tMotorControl, NULL, NULL);
  osThreadNew(tLED, NULL, NULL);
  osThreadNew(tAudio, NULL, NULL);
  osKernelStart();
}
