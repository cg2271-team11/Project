#include "MKL25Z4.h"
#define BAUD_RATE 9600
// #define UART_TX_PORTE22 22
#define UART_RX_PORTE23 23
#define UART2_INT_PRIO 128

void initUART2(uint32_t baud_rate);
// void UART2_Transmit_Poll(uint8_t data);
uint8_t UART2_Receive_Poll(void);

// struct: 0x(x-axis)(y-axis)
// 1-3: left/down
// 4: nothing
// 5-7: right/up
// 1/7 fastest mode
struct UartValues
{
	uint8_t button;
  uint8_t x_axis;
  uint8_t y_axis;
};

struct UartValues extractUartValues();
