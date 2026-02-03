# SPI

This library establishes an easy interface to implement SPI in any GR Project (that utilizes the STM32G4 board). Users simply  have to define SPI configurations and pin configurations in their project and call `GR_SPI_Initialize()` which will return a `handler` which can be used to send and receive data.

To use this library with CMake, one must link the library `SPI_Lib` as an `INTERFACE` to their targets with `target_link_libraries()`.

## How To Use

How to use GR_SPI library(FIX):

1. The SPI config has to have
2. The SPI_Initialize function creates a circular buffer that either leads to populating the tx or rx buffer. The SPI protocol is configured and the messages

## Limitations

The GR_SPI library currently only supports a single controller and a single peripheral P2P connection. The GR_SPI significantly fragments microcontroller memory over time from dynamic message allocation. The library only supports controller/master behavior and does not support peripheral/slave behavior.

## EXAMPLE CODE

```c
// Instantiate a GR_SPI_Handler, LL_SPI config struct, and GR_SPI_Pins sttruct
GR_SPI_Handler ex_handler;
LL_SPI_InitTypeDef ex_config;
GR_SPI_Pins ex_pins;

// Configure LL_SPI configuration struct
ex_config.TransferDirection = LL_SPI_FULL_DUPLEX;
ex_config.Mode = LL_SPI_MODE_MASTER;
ex_config.DataWidth = LL_SPI_DATAWIDTH_8BIT;
ex_config.ClockPolarity = LL_SPI_POLARITY_LOW;
ex_config.ClockPhase = LL_SPI_PHASE_1EDGE;
ex_config.NSS = LL_SPI_NSS_SOFT;
ex_config.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV2;
ex_config.BitOrder = LL_SPI_LSB_FIRST;
ex_config.CRCCalculation = LL_SPI_CRCCALCULATION_ENABLE;
ex_config.CRCPoly = 0x1D;

// Configure GR_SPI_Pins struct
ex_pins.SPIx = SPI3;
ex_pins.GPIOx = (GPIO_TypeDef **)(malloc(4 * sizeof(GPIO_TypeDef *)));
// All pins are in the A clock port
for (int i = 0; i < 3; i++) {
    *(ex_pins.GPIOx + i) = GPIOC;
}
ex_pins.GPIOx[3] = GPIOA;
ex_pins.num_pins = 4;
ex_pins.pin_nums = (uint32_t *)malloc(4 * sizeof(int));
ex_pins.pin_nums[0] = LL_GPIO_PIN_12; // COPI
ex_pins.pin_nums[1] = LL_GPIO_PIN_11; // CIPO
ex_pins.pin_nums[2] = LL_GPIO_PIN_10; // SCK
ex_pins.pin_nums[3] = LL_GPIO_PIN_4; // NSS
ex_pins.alternate_function_number = 6;

// Initialize SPI
GR_SPI_Initialize(&ex_handler, &ex_config, &ex_pins);

// Instantiate the message object
GR_SPI_Message msg;
msg.data = (uint8_t *)malloc(32 * sizeof(uint8_t));
msg.size = 32;

// Initialize msg byte array with values
for(int i = 0; i < msg.size; i++) {
    msg.data[i] = 'A' + i;
}

// Send a message
GR_SPI_Send(&ex_handler, &msg);

// Wait until receive is no longer empty
while (GR_SPI_IsRxEmpty(&ex_handler)) {}

// Receive a message
GR_SPI_Receive(&ex_handler, &msg);

// Now msg contains the received content

// Clean everything up (note: GR_SPI_Msg_Free(msg) should be used if msg object was on the heap)
free(msg.data);
GR_SPI_Close(&ex_handler);

```

## Future Plans

1. Add support multiple boards (STM32U5, STM32L4)
2. Prevent minor race conditions
3. Replace CircularBuffer with a custom byte array buffer
4. Support more complex SPI features such as SIMPLEX RX only
5. Remove all dynamic message heap allocation and stick to byte arrays
