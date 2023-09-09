#include "test.h"


void testUART(void)
{
    static uint8_t initialised = 0;
    if (initialised == 0)
    {
        OPENBL_USART_Configuration();
        initialised = 1;
    }
    OPENBL_USART_SendByte('H');
    OPENBL_USART_SendByte('e');
    OPENBL_USART_SendByte('l');
    OPENBL_USART_SendByte('l');
    OPENBL_USART_SendByte('o');
    OPENBL_USART_SendByte(' ');
    OPENBL_USART_SendByte('!');
}