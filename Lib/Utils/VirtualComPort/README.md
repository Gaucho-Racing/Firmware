# Virtual COM Port
> [!WARNING]
> Not all ST-Link devices are physically wired to support VCP!
>
> This will only work if you plug it in and already see an available serial port!

A wrapper around USART to provide easier control of the VCP.

Does not conflict with Logomatic using SWO.

## What it does
- Initializes `USART2` or `USART3` for simple serial TX/RX.
- Provides lightweight polling helpers for byte and line-based input.
- Intended for quick debug/CLI-style communication over ST-Link VCP.

## Quick setup
1. Include `vcp.h`.
2. Fill a `VCP_Config` with your USART, GPIO port/pins, baud, and framing settings.
3. Call `VCP_Init(&config)` once during startup.
4. Use `VCP_Send`, `VCP_IsDataAvailable`, `VCP_Receive`, and `VCP_ReceiveLine`.
5. Alternatively, configure [interrupts](#advanced-interrupts)

## Core API
- `VCP_Init(VCP_Config *input_config)` initializes clocks, GPIO, USART, and enables IRQ.
- `VCP_Send(uint8_t *data, uint32_t length)` transmits raw bytes.
- `VCP_IsDataAvailable()` returns `true` if RX data is ready.
- `VCP_Receive()` reads one byte (returns `0` if uninitialized).
- `VCP_ReceiveLine(uint8_t *buffer, uint8_t buffer_size)` reads until newline/NULL or buffer full.

## Advanced Interrupts
STM32 provides a weak reference to `USARTx_IRQHandler`.

To process on interrutps instead of polling simply write that function, for `USART2` that could be:
```c
void USART2_IRQHandler(void)
{
	if (LL_USART_IsActiveFlag_ORE(USART2)) {
		LL_USART_ClearFlag_ORE(USART2);
	}

	while (LL_USART_IsEnabledIT_RXNE(USART2) && LL_USART_IsActiveFlag_RXNE(USART2)) {
		uint8_t receivedData = LL_USART_ReceiveData8(USART2);
		while (!LL_USART_IsActiveFlag_TXE_TXFNF(USART2)) {}
		LL_USART_TransmitData8(USART2, receivedData);
		LOGOMATIC("Received: %c\n", receivedData);
	}
}
```
