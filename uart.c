#include "uart.h"
#include <stdbool.h>

void initUART2(uint32_t baud_rate)
{
  uint32_t divisor, bus_clock;

  SIM->SCGC4 |= SIM_SCGC4_UART2_MASK;
  SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;

  // PORTE->PCR[UART_TX_PORTE22] &= ~PORT_PCR_MUX_MASK;
  // PORTE->PCR[UART_TX_PORTE22] |= PORT_PCR_MUX(4);
  PORTE->PCR[UART_RX_PORTE23] &= ~PORT_PCR_MUX_MASK;
  PORTE->PCR[UART_RX_PORTE23] |= PORT_PCR_MUX(4);

  UART2->C2 &= ~((UART_C2_TE_MASK) | (UART_C2_RE_MASK));

  bus_clock = (DEFAULT_SYSTEM_CLOCK) / 2;
  divisor = bus_clock / (baud_rate * 16);
  UART2->BDH = UART_BDH_SBR(divisor >> 8);
  UART2->BDL = UART_BDL_SBR(divisor);

  UART2->C1 = 0;
  UART2->S2 = 0;
  UART2->C3 = 0;

  NVIC_SetPriority(UART2_IRQn, 128);
  NVIC_ClearPendingIRQ(UART2_IRQn);
  NVIC_EnableIRQ(UART2_IRQn);

  UART2->C2 |= ((UART_C2_TE_MASK) | (UART_C2_RE_MASK) | (UART_C2_RIE_MASK));
}

volatile uint8_t rx_data = 0;
bool isFirstData = true;

void UART2_IRQHandler(void)
{
  NVIC_ClearPendingIRQ(UART2_IRQn);
  if (UART2->S1 & UART_S1_RDRF_MASK)
  {
		// Ignore first data received as it might be from a prior run
		if(isFirstData){
			uint8_t dummy = UART2->D;
			isFirstData = false;
		}else{
			rx_data = UART2->D;
		}
  }
}

UartValues_t extractUartValues()
{
  UartValues_t values;
	values.button = (rx_data >> 6) & 0x01; // Extract button value
  values.x_axis = (rx_data >> 3) & 0x07; // Extract x-axis value
  values.y_axis = rx_data & 0x07;        // Extract y-axis value

  return values;
}
