# STM32G474 CCU

Charging Control Unit

Compile and flash CCU.elf

FIXME For testing on a Nucleo, use HSI; otherwise use HSE whenever possible

GR26 CCU is the GR25 ECU (aka GR25 Big Bird)

Requires using the serial port to send `C` to toggle charging and any other input to disable it. Send '?' for state data
