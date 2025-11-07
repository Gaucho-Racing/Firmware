#include <stdbool.h>
#include <stdint.h>

#ifndef _GETBITS_H_
#define _GETBITS_H_

/**
 * @brief Get the value of a single bit within a byte.
 *
 * Bits are indexed MSB-first (bit 0 = MSB, bit 7 = LSB) to align with message
 * protocol conventions where message data is sent MSB (bit) first.
 *
 * @param number The byte to read from
 * @param index Bit index (0-7), where 0 is the MSB (most significant bit)
 *              and 7 is the LSB (least significant bit)
 * @return true if the bit is set (1), false if clear (0)
 *
 * @note Index out of range (>7) results in undefined behavior
 *
 * Example:
 *   uint8_t data = 0b10110100;  // 0xB4
 *   GetBitInByte(data, 0);  // returns true  (MSB is 1)
 *   GetBitInByte(data, 1);  // returns false (bit 1 is 0)
 *   GetBitInByte(data, 7);  // returns false (LSB is 0)
 */
bool GetBitInByte(uint8_t number, uint8_t index);

/**
 * @brief Set or clear a single bit within a byte.
 *
 * Bits are indexed MSB-first (bit 0 = MSB, bit 7 = LSB) to align with message
 * protocol conventions where message data is sent MSB (bit) first.
 *
 * @param byte Pointer to the byte to modify
 * @param index Bit index (0-7), where 0 is the MSB and 7 is the LSB
 * @param value true to set the bit to 1, false to clear it to 0
 *
 * @note Index out of range (>7) results in undefined behavior
 * @note Modifies the byte in place
 *
 * Example:
 *   uint8_t data = 0b00000000;
 *   SetBitInByte(&data, 0, true);   // data becomes 0b10000000 (set MSB)
 *   SetBitInByte(&data, 7, true);   // data becomes 0b10000001 (set LSB)
 *   SetBitInByte(&data, 0, false);  // data becomes 0b00000001 (clear MSB)
 */
void SetBitInByte(uint8_t *byte, uint8_t index, bool value);

/**
 * @brief Extract a multi-bit field from a byte.
 *
 * Extracts a contiguous sequence of bits and returns them right-aligned in the
 * result. Bits are indexed MSB-first (bit 0 = MSB, bit 7 = LSB) to align with
 * message protocol conventions where message data is sent MSB (bit) first.
 *
 * @param number The byte to extract bits from
 * @param index Starting bit index (0-7), where 0 is the MSB
 * @param length Number of bits to extract (1-8)
 * @return Extracted bits, right-aligned (e.g., extracting 3 bits returns a
 * value 0-7)
 *
 * @note If index + length > 8, behavior is undefined
 * @note Result is always right-aligned regardless of the field position
 *
 * Example:
 *   uint8_t data = 0b10110100;  // 0xB4
 *   GetBitsInByte(data, 0, 3);  // returns 0b00000101 (bits 0-2: 101)
 *   GetBitsInByte(data, 2, 4);  // returns 0b00001101 (bits 2-5: 1101)
 *   GetBitsInByte(data, 5, 3);  // returns 0b00000100 (bits 5-7: 100)
 */
uint8_t GetBitsInByte(uint8_t number, uint8_t index, uint8_t length);

/**
 * @brief Set a multi-bit field within a byte.
 *
 * Writes a contiguous sequence of bits into the specified position within a
 * byte. Only the bits in the specified range are modified; other bits remain
 * unchanged. Bits are indexed MSB-first (bit 0 = MSB, bit 7 = LSB) to align
 * with message protocol conventions where message data is sent MSB (bit) first.
 *
 * @param byte Pointer to the byte to modify
 * @param index Starting bit index (0-7), where 0 is the MSB
 * @param length Number of bits to write (1-8)
 * @param value Value to write (only the lower 'length' bits are used)
 *
 * @note If index + length > 8, behavior is undefined
 * @note If value has bits set beyond 'length', they are masked off
 * @note Modifies the byte in place, preserving bits outside the specified range
 *
 * Example:
 *   uint8_t data = 0b00000000;
 *   SetBitsInByte(&data, 0, 3, 0b101);  // data becomes 0b10100000 (set bits
 * 0-2) SetBitsInByte(&data, 3, 2, 0b11);   // data becomes 0b10111000 (set bits
 * 3-4) SetBitsInByte(&data, 0, 3, 0b010);  // data becomes 0b01011000 (update
 * bits 0-2)
 */
void SetBitsInByte(uint8_t *byte, uint8_t index, uint8_t length, uint8_t value);

/**
 * @brief Get the value of a single bit within a byte array.
 *
 * Treats a byte array as a contiguous bit stream and extracts a single bit.
 * Within each byte, bits are indexed MSB-first (bit 0 = MSB, bit 7 = LSB).
 * Bytes are ordered in little endian (per protocol: "bytes of larger integers
 * are sent in little endian order"), meaning byte[0] contains bits 0-7, byte[1]
 * contains bits 8-15, etc.
 *
 * @param byteArray Pointer to the byte array to read from
 * @param bitIndex Absolute bit index across the entire array:
 *                 - Bits 0-7 are in byteArray[0] (bit 0 = MSB of byte 0)
 *                 - Bits 8-15 are in byteArray[1] (bit 8 = MSB of byte 1)
 *                 - And so on...
 * @return true if the bit is set (1), false if clear (0)
 *
 * @note Caller must ensure bitIndex is within array bounds
 * @note No bounds checking is performed
 *
 * Example (16-bit value 0x1234 stored as {0x34, 0x12} in little endian):
 *   uint8_t data[] = {0x34, 0x12};  // 0x1234 in little endian
 *   // Byte 0 (0x34 = 0b00110100):
 *   GetBitInByteArray(data, 0);  // returns false (MSB of byte 0)
 *   GetBitInByteArray(data, 2);  // returns true  (bit 2 of byte 0)
 *   // Byte 1 (0x12 = 0b00010010):
 *   GetBitInByteArray(data, 8);  // returns false (MSB of byte 1)
 *   GetBitInByteArray(data, 11); // returns true  (bit 3 of byte 1)
 */
bool GetBitInByteArray(const uint8_t *byteArray, uint8_t bitIndex);

/**
 * @brief Set or clear a single bit within a byte array.
 *
 * Treats a byte array as a contiguous bit stream and modifies a single bit.
 * Within each byte, bits are indexed MSB-first (bit 0 = MSB, bit 7 = LSB).
 * Bytes are ordered in little endian (per protocol: "bytes of larger integers
 * are sent in little endian order"), meaning byte[0] contains bits 0-7, byte[1]
 * contains bits 8-15, etc.
 *
 * @param byteArray Pointer to the byte array to modify
 * @param bitIndex Absolute bit index across the entire array:
 *                 - Bits 0-7 are in byteArray[0] (bit 0 = MSB of byte 0)
 *                 - Bits 8-15 are in byteArray[1] (bit 8 = MSB of byte 1)
 *                 - And so on...
 * @param value true to set the bit to 1, false to clear it to 0
 *
 * @note Caller must ensure bitIndex is within array bounds
 * @note No bounds checking is performed
 * @note Modifies the array in place
 *
 * Example (setting bits in a 16-bit value stored in little endian):
 *   uint8_t data[] = {0x00, 0x00};  // Two bytes, all zeros
 *   SetBitInByteArray(data, 0, true);   // data becomes {0x80, 0x00} (set MSB
 * of byte 0) SetBitInByteArray(data, 7, true);   // data becomes {0x81, 0x00}
 * (set LSB of byte 0) SetBitInByteArray(data, 8, true);   // data becomes
 * {0x81, 0x80} (set MSB of byte 1) SetBitInByteArray(data, 15, true);  // data
 * becomes {0x81, 0x81} (set LSB of byte 1)
 */
void SetBitInByteArray(uint8_t *byteArray, uint8_t bitIndex, bool value);

#endif
