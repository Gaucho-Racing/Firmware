# Lib/Peripherals

These abstract peripheral interfaces intend on satisfying the following goals:

1. Reduce boilerplate in application code
2. Share similar APIs across different peripherals, so we can (relatively) easily swap between two
3. Support interrupt-based TX/RX so we don't need to busy-wait while transferring large packets
4. Queue outgoing and incoming messages in a [CircularBuffer](/Lib/Utils/CircularBuffer/README.md)

See each subdirectory for specific details about that peripheral.
