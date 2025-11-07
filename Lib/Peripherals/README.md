# Lib/Peripherals

These abstract peripheral interfaces intend on satisfying the following goals:

1. Reduce boilerplate in application code
2. Share similar APIs across different peripherals, so we can (relatively) easily swap between two
3. Support interrupt-based TX/RX so we don't need to busy-wait while transferring large packets.

See each subdirectory for more details.
