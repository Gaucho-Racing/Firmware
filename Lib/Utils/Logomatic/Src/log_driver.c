#include "Logomatic.h"
#include "main.h"

int __io_putchar(int ch)
{
	if (global_logomatic_driver != NULL) {
		return global_logomatic_driver(ch);
	}
	return ch;
}

#if defined(ITM)
int Logomatic_Driver_ITM(int ch)
{
	ITM_SendChar(ch);
	return ch;
}
#endif

#if defined(LPUART1) || defined(LPUART2) || defined(LPUART3) || defined(USART1) || defined(USART2) || defined(USART3) || defined(USART4) || defined(USART5) || defined(USART6) || defined(USART7) ||   \
    defined(USART8) || defined(UART1) || defined(UART2) || defined(UART3) || defined(UART4) || defined(UART5) || defined(UART6) || defined(UART7) || defined(UART8)
int Logomatic_Private_UART(USART_TypeDef *uart, int ch)
{
#if defined(USART_ISR_TXE_TXFNF)
	while (!(uart->ISR & USART_ISR_TXE_TXFNF)) { /* Wait*/
	}
	uart->TDR = (uint8_t)ch;
#elif defined(USART_ISR_TXE)
	while (!(uart->ISR & USART_ISR_TXE)) { /* Wait */
	}
	uart->TDR = (uint8_t)ch;
#else
	while (!(uart->SR & USART_SR_TXE)) { /* Wait */
	}
	uart->DR = (uint8_t)ch;
#endif
	return ch;
}
#endif

#if defined(LPUART1)
int Logomatic_Driver_LPUART1(int ch)
{
	return Logomatic_Private_UART(LPUART1, ch);
}
#endif

#if defined(LPUART2)
int Logomatic_Driver_LPUART2(int ch)
{
	return Logomatic_Private_UART(LPUART2, ch);
}
#endif

#if defined(LPUART3)
int Logomatic_Driver_LPUART3(int ch)
{
	return Logomatic_Private_UART(LPUART3, ch);
}
#endif

#if defined(USART1)
int Logomatic_Driver_USART1(int ch)
{
	return Logomatic_Private_UART(USART1, ch);
}
#endif

#if defined(USART2)
int Logomatic_Driver_USART2(int ch)
{
	return Logomatic_Private_UART(USART2, ch);
}
#endif

#if defined(USART3)
int Logomatic_Driver_USART3(int ch)
{
	return Logomatic_Private_UART(USART3, ch);
}
#endif

#if defined(USART4)
int Logomatic_Driver_USART4(int ch)
{
	return Logomatic_Private_UART(USART4, ch);
}
#endif

#if defined(USART5)
int Logomatic_Driver_USART5(int ch)
{
	return Logomatic_Private_UART(USART5, ch);
}
#endif

#if defined(USART6)
int Logomatic_Driver_USART6(int ch)
{
	return Logomatic_Private_UART(USART6, ch);
}
#endif

#if defined(USART7)
int Logomatic_Driver_USART7(int ch)
{
	return Logomatic_Private_UART(USART7, ch);
}
#endif

#if defined(USART8)
int Logomatic_Driver_USART8(int ch)
{
	return Logomatic_Private_UART(USART8, ch);
}
#endif

#if defined(UART1)
int Logomatic_Driver_UART1(int ch)
{
	return Logomatic_Private_UART(UART1, ch);
}
#endif

#if defined(UART2)
int Logomatic_Driver_UART2(int ch)
{
	return Logomatic_Private_UART(UART2, ch);
}
#endif

#if defined(UART3)
int Logomatic_Driver_UART3(int ch)
{
	return Logomatic_Private_UART(UART3, ch);
}
#endif

#if defined(UART4)
int Logomatic_Driver_UART4(int ch)
{
	return Logomatic_Private_UART(UART4, ch);
}
#endif

#if defined(UART5)
int Logomatic_Driver_UART5(int ch)
{
	return Logomatic_Private_UART(UART5, ch);
}
#endif

#if defined(UART6)
int Logomatic_Driver_UART6(int ch)
{
	return Logomatic_Private_UART(UART6, ch);
}
#endif

#if defined(UART7)
int Logomatic_Driver_UART7(int ch)
{
	return Logomatic_Private_UART(UART7, ch);
}
#endif

#if defined(UART8)
int Logomatic_Driver_UART8(int ch)
{
	return Logomatic_Private_UART(UART8, ch);
}
#endif
