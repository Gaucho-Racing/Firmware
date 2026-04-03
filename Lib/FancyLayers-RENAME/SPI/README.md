# SPI

This library establishes an easy interface to implement SPI in any GR Project (that utilizes the STM32G4 board). Users simply  have to define SPI and pin configurations in their project and call `GR_SPI_Initialize()`, which will return a `handler` that can be used to send and receive data.

To use this library with CMake, one must link the library `SPI_Lib` as an `INTERFACE` to their targets with `target_link_libraries()`.

When using SPI, it is important to understand how the protocol works and what each pin does. For Nucleo STM32 boards, each pin has a port (clock channel) and a pin number (i.e. B13 is port B and pin 13). Common SPI-related acronyms are as follows:

1. COPI - Controller Out Peripheral In (same as MOSI)
2. CIPO - Controller In Peripheral Out (same as MISO)
3. SCLK - Serial (SPI) Clock (same as SCK)
4. NCS - Negative Chip Select (same as NSS and the complement of CS/SS)

Other acronyms:

1. AFN - Alternate Function Number (specific to STM32 architecture)
2. GND - Electrical Ground (reference voltage)

When you are wiring SPI pins together, always connect COPI to COPI, CIPO to CIPO, GND to GND, SCLK to SCLK, and NCS to NCS.

Make sure to read through the SPI library header file and check which functions return an error value.
The error name to integer value mappings are as follows:

```c
#define GR_SPI_ERR_NONE 0
#define GR_SPI_ERR_FRE -1
#define GR_SPI_ERR_OVR -2
#define GR_SPI_ERR_MODF -3
#define GR_SPI_ERR_CRCERR -4
#define GR_SPI_ERR_RXFULL -5
#define GR_SPI_ERR_BAD_INIT_NVIC -6
#define GR_SPI_ERR_BAD_INIT_RXBUF -7
#define GR_SPI_ERR_BAD_INIT_TXBUF -8
#define GR_SPI_ERR_BAD_SPIX -9
#define GR_SPI_ERR_BAD_ADD -10
#define GR_SPI_ERR_FULL_TRANSMIT -11
#define GR_SPI_ERR_BAD_INIT_CONFIG -12
```

## How To Use

How to use GR_SPI library:

1. Make GR_SPI_Handler and GR_SPI_Pins structs (1 of each, both designed by GR Firmware)
2. Make a LL_SPI_InitTypeDef struct (requires non-GR LL API and library)
3. Populate the LL_SPI_InitTypeDef struct with values for:

• uint32_t TransferDirection
• uint32_t Mode (very important! - really the only thing you need to change from example code)
• uint32_t DataWidth
• uint32_t ClockPolarity
• uint32_t ClockPhase
• uint32_t NSS (should always be LL_SPI_NSS_SOFT unless the GR_SPI library is updated to support hardware control)
• uint32_t BaudRate (increase DIV# to decrease communication rate for better stability)
• uint32_t BitOrder
• uint32_t CRCCalculation
• uint32_t CRCPoly

4. Set the GR_SPI_Pins struct values:

• SPI_TypeDef *SPIx
• GPIO_TypeDef *COPI_port
• GPIO_TypeDef *CIPO_port
• GPIO_TypeDef *SCLK_port
• GPIO_TypeDef *NCS_port
• uint32_t COPI_pin
• uint32_t CIPO_pin
• uint32_t CLK_pin
• uint32_t NCS_pin
• uint32_t AFN

5. Call GR_SPI_Initialize() with handle, config, and pins arguments. This function sets the handle struct you created earlier with the values in the config and enables all necessary hardware configuration and clocks.
6. Make a GR_SPI_Message struct and populate the values:

• uint8_t * data (this is an array of bytes!)
• uint16_t size (this has a max size of GR_SPI_MAX_MSG_BYTE_SIZE)

7. Either call GR_SPI_Send(handle, msg) or while(GR_SPI_IsRxEmpty(handle)) and GR_SPI_Receive(handle, msg). Note that when you are receiving a message, make sure to check GR_SPI_Get_RxMsgSize(handle) to get the size of the message on the top of the RX buffer.
8. Make sure to call free() on any msg->data arrays you malloc'd.
9. Teardown the SPI connection via GR_SPI_Close(handle). This will free any memory allocated within the handle struct, but it will NOT free the handle struct you may have declared yourself. The LL_SPI_InitTypeDef config and GR_SPI_Pins pins structs must also be freed manually if necessary.

## Limitations

The GR_SPI library currently only supports a single controller and a single peripheral P2P connection. The GR_SPI library initializes a decently sized chunk of byte arrays on initialization (GR_SPI_Initialize), but it does not cause any heap memory fragmentation during message transaction (internally). You can still fragment heap memory by constantly allocating and deallocating your own GR_SPI_Message structs or their byte arrays (do not do that). The library's support for slave mode pushes a new message on the rx circular buffer for every byte received (it can't predict message length at the moment). This causes the rx circular buffer to have an effective 50% usable capacity based on internal implementation. Future updates should support rx message size predictions (possibly by using timers or a configurable predefined receive message size parameter).

## EXAMPLE CODE

```c
// ========== SENDER (MASTER) ==========
// Instantiate a GR_SPI_Handler, LL_SPI config struct, and GR_SPI_Pins struct
GR_SPI_Handler handle_spi3;
LL_SPI_InitTypeDef config_spi3;
GR_SPI_Pins pins_spi3;

// Configure LL_SPI configuration struct
config_spi3.Mode = LL_SPI_MODE_MASTER;
config_spi3.NSS = LL_SPI_NSS_SOFT;
config_spi3.TransferDirection = LL_SPI_FULL_DUPLEX;
config_spi3.DataWidth = LL_SPI_DATAWIDTH_8BIT;
config_spi3.ClockPolarity = LL_SPI_POLARITY_LOW;
config_spi3.ClockPhase = LL_SPI_PHASE_1EDGE;
config_spi3.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV32;
config_spi3.BitOrder = LL_SPI_MSB_FIRST;
config_spi3.CRCCalculation = LL_SPI_CRCCALCULATION_ENABLE;
config_spi3.CRCPoly = 0x1D;

// Configure GR_SPI_Pins struct
pins_spi3.SPIx = SPI3;
pins_spi3.COPI_port = GPIOC;
pins_spi3.CIPO_port = GPIOC;
pins_spi3.SCLK_port = GPIOC;
pins_spi3.NCS_port = GPIOA;
pins_spi3.COPI_pin = LL_GPIO_PIN_12; // COPI
pins_spi3.CIPO_pin = LL_GPIO_PIN_11; // CIPO
pins_spi3.SCLK_pin = LL_GPIO_PIN_10; // SCK
pins_spi3.NCS_pin = LL_GPIO_PIN_4;	// NSS
pins_spi3.AFN = 6;

// Initialize SPI
GR_SPI_Initialize(&handle_spi3, &config_spi3, &pins_spi3);

// Instantiate the message object
GR_SPI_Message msg = {0};
msg.data = (uint8_t *)malloc(32 * sizeof(uint8_t));
msg.size = 1;
// Initialize msg byte array with values
for (int i = 0; i < msg.size; i++) {
    msg.data[i] = 'A' + i;
}

// Send a message
GR_SPI_Send(&handle_spi3, &msg);

// Wait until receive is no longer empty
while (GR_SPI_IsRxEmpty(&handle_spi3)) {}

// Clean everything up (note: GR_SPI_Msg_Free(msg) should be used if msg object was on the heap)
free(msg.data);
GR_SPI_Close(&handle_spi3);

// ========== RECEIVER (SLAVE) ==========
GR_SPI_Handler handle_spi2;
LL_SPI_InitTypeDef config_spi2;
GR_SPI_Pins pins_spi2;

// Config values
config_spi2.Mode = LL_SPI_MODE_SLAVE;
config_spi2.NSS = LL_SPI_NSS_SOFT;
config_spi2.TransferDirection = LL_SPI_FULL_DUPLEX;
config_spi2.DataWidth = LL_SPI_DATAWIDTH_8BIT;
config_spi2.ClockPolarity = LL_SPI_POLARITY_LOW;
config_spi2.ClockPhase = LL_SPI_PHASE_1EDGE;
config_spi2.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV32;
config_spi2.BitOrder = LL_SPI_MSB_FIRST;
config_spi2.CRCCalculation = LL_SPI_CRCCALCULATION_ENABLE;
config_spi2.CRCPoly = 0x1D;

// Pins values
pins_spi2.SPIx = SPI2;
pins_spi2.COPI_port = GPIOB;
pins_spi2.CIPO_port = GPIOB;
pins_spi2.SCLK_port = GPIOB;
pins_spi2.NCS_port = GPIOB;
pins_spi2.COPI_pin = LL_GPIO_PIN_15; // COPI
pins_spi2.CIPO_pin = LL_GPIO_PIN_14; // CIPO
pins_spi2.SCLK_pin = LL_GPIO_PIN_13; // SCK
pins_spi2.NCS_pin = LL_GPIO_PIN_12;	// NSS
pins_spi2.AFN = 5;

GR_SPI_Initialize(&handle_spi2, &config_spi2, &pins_spi2);

// Create message struct
GR_SPI_Message msg = {0};
msg.size = 1;
msg.data = (uint8_t *)malloc(msg.size * sizeof(uint8_t));
// Set default values (to check if nothing was changed on receive)
for (int i = 0; i < msg.size; i++) {
    msg.data[i] = '#';
}

// Wait until something is received
while (GR_SPI_IsRxEmpty(&handle_spi2)) {}

// Parse the message (this will be 1 byte in current implementation since it was unsolicited)
GR_SPI_Receive(&handle_spi2, &msg);

// Make a c-string to print the message byte array
char str[33];
memcpy(str, msg.data, msg.size * sizeof(uint8_t));
str[msg.size] = '\0';

// Print the byte array
LOGOMATIC("Received: %s\n", str);

// Clean up
free(msg.data);
GR_SPI_Close(&handle_spi2);

```

## Future Plans

1. Add support multiple boards (STM32U5, STM32L4)
2. Conduct robust race condition testing
3. Support more complex SPI features such as SIMPLEX RX only
4. Update the RX message buffer to utilize predictive message sizing on unsoliticed messages (slave mode only)
