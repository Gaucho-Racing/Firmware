Introduction
==============================================
This abstraction acts as an intermediary between the application and platform layers, allowing programmers to communicate with SPI peripherals using simple setup and message APIs.
The application layer is used by programmers to send data to peripherals over SPI. The platform layer is the bare-metal programming of STM32 board registers to enable interrupts and SPI communication.


SPI Protocol Rundown
==============================================
SPI is a synchronous duplex communication protocol. This means data transmission is clocked and that transmitters + receivers can communicate with one another simultaneously.
SPI has a controller (STM32 board) and support for any number of peripherals in either a parallel or serial configuration.
There are four main types of lines that connect a controller and peripheral: COPI (controller out, peripheral in), CIPO (controller in, peripheral out), SCLK (clock), CS (chip select).
Chip select is active low and determines which peripheral line can communicate with the controller over the shared CIPO and COPI buses.


Application Interface
==============================================
The SPI abstraction offers the following interface:
-Setup()
    +Takes in a configuration struct and returns a pointer to a SPI handler struct, which stores data for other abstraction functions to use.
    +Optionally attach a callback function in the SPI configuration struct to be called when new data is received.
-Send()
    +Takes in a handler struct and an SPI message struct.
    +Sends a message over SPI with the relevant data contents.
-Receive()
    +Takes in an SPI message struct and parses the oldest data available on the circular buffer.
    +Overwrites data in the SPI message struct with the requested contents.
-Close()
    +Acts as a destructor for the SPI handler struct that was created during setup.


Assumptions and Internal Structure
==============================================
The SPI abstraction uses a circular buffer data structure to queue received data onto a buffer in the event that data can not be processed at the same frequency as it is being generated.
The abstraction interfaces with a low-level language (LL) to collect and send data over the SPI GPIO pins on the STM32 board.


Limitations
==============================================
In its current implementation, this abstraction only allows for one peripheral to be connected to one controller (the handler). The abstraction will need to be updated in future implementations to allow for multiple peripherals to communicate to one controller.
In that scenario, the plan is to manage an array of circular buffers (one per peripheral), an array of chip selects (one per peripheral), and a state variable for active buffer/peripheral.


