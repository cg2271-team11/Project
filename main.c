// Device header
#include "motor.h"
#include "uart.h"
#include "led.h"
#include "audio.h"
#include "RTE_Components.h"
#include CMSIS_device_header
#include "cmsis_os2.h"
#include <stdbool.h>

void tLED(void *argument)
{
}

osThreadId_t tid_movingLEDThread;
osThreadId_t tid_stationaryLEDThread;

osMessageQueueId_t uartValuesMessageQueue;

void tBrain(void *argument)
{
  UartValues_t myUartData;

  while (1)
  {
    myUartData = extractUartValues();

    osMessageQueuePut(uartValuesMessageQueue, &myUartData, NULL, 0);

    if (myUartData.button == 1)
    {
      setCourseEnded(true);
    }

    if (myUartData.brake || myUartData.x_axis != 4 || myUartData.y_axis != 4)
    {
      osThreadFlagsSet(tid_movingLEDThread, 0x0001);
    }
    else
    {
      osThreadFlagsSet(tid_stationaryLEDThread, 0x0001);
    }
  }
}

void tMotorControl(void *argument)
{
  UartValues_t myUartData;
  for (;;)
  {
    osMessageQueueGet(uartValuesMessageQueue, &myUartData, NULL, osWaitForever);
    if (myUartData.brake) {
      brake();
      continue;
    }
    struct MotorSpeed motorSpeed = calculateSpeed(myUartData.x_axis, myUartData.y_axis);
    moveAll(motorSpeed.leftSpeed, motorSpeed.rightSpeed);
  }
}


void tAudio(void *argument)
{
  playBeginningTheme();
}

int main(void)
{
  // Initialization
  SystemCoreClockUpdate();
  initPWM();
  initLEDs();
  initAudioPWM();
  initUART2(BAUD_RATE);
  osKernelInitialize(); // Initialize CMSIS-RTOS

  // Threads
  uartValuesMessageQueue = osMessageQueueNew(1, sizeof(UartValues_t), NULL);
  osThreadNew(tBrain, NULL, NULL);
  osThreadNew(tMotorControl, NULL, NULL);
  osThreadNew(tAudio, NULL, NULL);
  tid_movingLEDThread = osThreadNew(movingLEDThread, NULL, NULL);
  tid_stationaryLEDThread = osThreadNew(stationaryLEDThread, NULL, NULL);
  osKernelStart();
}
