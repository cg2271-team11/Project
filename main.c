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
struct MotorSpeed motorSpeed;
void tBrain(void *argument)
{
  // Should instead send some value to motor thread
  // which handles the movement
  while (1)
  {
    axisValues = extractAxisValues();
    motorSpeed = calculateSpeed(axisValues.x_axis, axisValues.y_axis);
    moveAll(motorSpeed.leftSpeed, motorSpeed.rightSpeed);
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
