# Bit Manipulations Library

Intended for use with CAN following the standards in [Autogen](/Autogen/CAN/)

## Bit Ordering Convention

This library uses big-endian bit indexing

- Bit Index 0 -> Most Significant Bit (MSB)
- Bit Index 7 -> Least Significant Bit (rightmost bit) for an 8-bit byte

In a CAN message, expect bit fields to be organized as big-endian and bytes of larger integers to be sent in little-endian order.

## Notes

- All functions preserve the input; modified values are returned
- Index bounds are not checked; out-of-range access is undefined behavior
- For multi-byte CAN signals, handle byte ordering in your message parser/packer
- This library is byte-order agnostic for individual integer types
