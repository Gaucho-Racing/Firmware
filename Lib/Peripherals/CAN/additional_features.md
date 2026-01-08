

- TX buffering policy, do we spread them out over multiple TX buffers
- 


- Redefining interrupt handlers: 
- FIFO vs Queue policy
- Add support for RXFifo1
- can_read_rx_buffer, linked to RXFifo0 callback

- Allow circular buffer of size 0 - right now, whether circular buffer is allocated is used to test whether CAN1 instance is running

- Smaller can headers for tx (right now its just use the TXHeaderTypeDef)