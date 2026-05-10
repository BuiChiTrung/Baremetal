#include <stdio.h>
#include <string.h>

#include "uart.h"
#include "uart_dma.h"

extern uint8_t g_rx_cmplt;
extern uint8_t g_uart_cmplt;
extern uint8_t g_tx_cmplt;

extern char uart_data_buffer[UART_DATA_BUFF_SIZE];
char msg_buff[150] = {'\0'};

int main(void) {

    /* Initialize UART2 for both Transmit (TX) and Receive (RX) */
    uart2_rx_tx_init();

    /* Enable the clock for the DMA1 controller */
    dma1_init();

    /* Config DMA1 Stream 5 to load receive UART data into uart_data_buffer */
    dma1_stream5_uart_rx_config();

    /* Format an initial boot message */
    sprintf(msg_buff, "Initialization...cmplt\n\r");

    /* Tell DMA1 Stream 6 to transmit the boot message over UART */
    dma1_stream6_uart_tx_config((uint32_t)msg_buff, strlen(msg_buff));

    /* Wait (block) until the DMA and UART hardware finish transmitting the boot message */
    while (!g_tx_cmplt) {}

    while (1) {

        /* Check if the DMA RX stream interrupt fired, indicating a complete message was received */
        if (g_rx_cmplt) {
            /* Format a new message that echoes back the received data */
            sprintf(msg_buff, "Message received : %s \r\n", uart_data_buffer);

            /* Clear all completion flags so they are ready for the next cycle */
            g_rx_cmplt = 0;
            g_tx_cmplt = 0;
            g_uart_cmplt = 0;

            /* Tell DMA1 Stream 6 to transmit our newly formatted echo message */
            dma1_stream6_uart_tx_config((uint32_t)msg_buff, strlen(msg_buff));

            /* Wait until the transmission is 100% complete before going back to listening */
            while (!g_tx_cmplt) {}
        }
    }
}
