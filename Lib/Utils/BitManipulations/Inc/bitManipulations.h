/**
 * @file bitManipulations.h
 * @brief Bit manipulation utilities for embedded message protocol handling
 *
 * This library provides utilities for reading and writing individual bits and
 * bit fields within bytes and byte arrays, following the convention:
 *   - **Bits:** MSB-first indexing (bit 0 = most significant bit, bit 7 = least
 * significant bit)
 *   - **Bytes:** LSB-first ordering in multi-byte values (byte 0 is the least
 * significant byte)
 *
 * This aligns with message protocols where:
 *   - Message data bits are transmitted MSB-first within each byte
 *   - Multi-byte integers are transmitted in little-endian byte order
 *
 * @note All bit indices are 0-based
 * @note Index out-of-bounds behavior is undefined; caller must validate
 */

#include <stdbool.h>
#include <stdint.h>

#ifndef _BIT_MANIPULATIONS_H_
#define _BIT_MANIPULATIONS_H_

// ============================================================================
// BIT INDEXING CONVENTION
// ============================================================================
// Bits have MSB at index 0 and bytes have LSB at index 0
//
// For a byte 0b10110100 (0xB4):
//   Bit index:  0  1  2  3  4  5  6  7
//   Bit value:  1  0  1  1  0  1  0  0
//               ↑MSB                ↑LSB
//
// For a 16-bit value 0x1234 stored as bytes {0x34, 0x12} (little endian):
//   Byte[0] = 0x34 (LSB of the 16-bit value)
//   Byte[1] = 0x12 (MSB of the 16-bit value)
// ============================================================================

/**
 * @brief Read a single bit from an integer value (LSB-first bit indexing).
 *
 * Extracts a single bit from the specified position using LSB-first indexing,
 * where bit_position 0 refers to the least significant bit (2^0).
 *
 * @param value The integer value to read from (any unsigned integer type)
 * @param bit_position Bit position (0 = LSB, increasing toward MSB)
 * @return 1 if the bit is set, 0 if clear
 *
 * @note This macro uses LSB-first indexing (bit 0 = LSB), which is standard
 *       for register/memory operations but differs from the MSB-first
 * convention used by SetBitInByte and SetBitsInByte functions.
 * @note No bounds checking; bit_position must be < width of value's type
 *
 * @warning For MSB-first indexing within bytes, manually translate:
 *          msb_index = 7 - lsb_index (for 8-bit values)
 *
 * Example (LSB-first):
 *   uint8_t data = 0b10110100;  // 0xB4
 *   GETBIT(data, 0);  // returns 0 (LSB is 0)
 *   GETBIT(data, 2);  // returns 1 (bit 2 is 1)
 *   GETBIT(data, 7);  // returns 1 (MSB is 1)
 */
#define GETBIT(value, bit_position) (((value) >> (bit_position)) & 1u)

/**
 * @brief Extract a multi-bit field from an integer value (LSB-first indexing).
 *
 * Extracts a contiguous sequence of bits starting at bit_index and returns
 * them right-aligned. Uses LSB-first indexing where bit_index 0 is the LSB.
 *
 * @param value The integer value to extract bits from (any unsigned integer
 * type)
 * @param bit_index Starting bit position (0 = LSB)
 * @param length Number of bits to extract (1-32 for safety)
 * @return Extracted bits, right-aligned (e.g., 3 bits → value 0-7)
 *
 * @note This macro uses LSB-first indexing (standard for hardware registers)
 * @note For length >= 64, mask calculation may overflow; use with care
 * @note If bit_index + length exceeds the type width, behavior is undefined
 * @note Result is always right-aligned regardless of field position
 *
 * @warning For MSB-first indexing within bytes, translate the bit_index:
 *          lsb_start = 7 - msb_index - (length - 1)
 *
 * Example (LSB-first, extracting from uint8_t):
 *   uint8_t data = 0b10110100;  // 0xB4
 *   GETBITS(data, 0, 3);  // returns 0b100 (bits 0-2: LSB side)
 *   GETBITS(data, 2, 4);  // returns 0b1101 (bits 2-5)
 *   GETBITS(data, 5, 3);  // returns 0b101 (bits 5-7: MSB side)
 */
#define GETBITS(value, bit_index, length)                                      \
	((value >> bit_index) & ((1uLL << length) - 1))

/**
 * @brief Set or clear a single bit within a byte (MSB-first indexing).
 *
 * Modifies a single bit at the specified index using MSB-first convention,
 * where index 0 refers to the most significant bit (bit 7 in hardware).
 * Returns the modified byte value.
 *
 * @param number The byte value to modify
 * @param index Bit index (0-7), where:
 *              - 0 = MSB (most significant bit, hardware bit 7)
 *              - 7 = LSB (least significant bit, hardware bit 0)
 * @param value true to set the bit to 1, false to clear it to 0
 * @return The modified byte with the bit set or cleared
 *
 * @note Uses MSB-first indexing to align with message protocol conventions
 * @note Index must be in range [0, 7]; out-of-range causes undefined behavior
 * @note Does not modify the input; returns a new value
 *
 * @warning Index must be in range [0, 7]; out-of-range causes undefined
 * behavior
 *
 * Example:
 *   uint8_t data = 0b00000000;
 *   data = SetBitInByte(data, 0, true);   // Sets MSB → data = 0b10000000
 *   data = SetBitInByte(data, 7, true);   // Sets LSB → data = 0b10000001
 *   data = SetBitInByte(data, 0, false);  // Clears MSB → data = 0b00000001
 */
uint8_t SetBitInByte(uint8_t number, uint8_t index, bool value);

/**
 * @brief Set a multi-bit field within a byte (MSB-first indexing).
 *
 * Writes a contiguous sequence of bits into the specified position within a
 * byte, using MSB-first indexing where index 0 refers to the most significant
 * bit. Only the bits in the specified range are modified; other bits remain
 * unchanged. Returns the modified byte value.
 *
 * @param number The byte value to modify
 * @param index Starting bit index (0-7), where:
 *              - 0 = MSB position (hardware bit 7)
 *              - 7 = LSB position (hardware bit 0)
 * @param length Number of bits to write (1-8)
 * @param value Value to write (only the lower 'length' bits are used)
 * @return The modified byte with the bit field set
 *
 * @note Uses MSB-first indexing to align with message protocol conventions
 * @note If index + length > 8, behavior is undefined
 * @note If value has bits set beyond 'length', they are automatically masked
 * off
 * @note Does not modify the input; returns a new value preserving bits outside
 *       the specified range
 *
 * @warning If index + length > 8, behavior is undefined
 *
 * Example:
 *   uint8_t data = 0b00000000;
 *   // Set 3 bits starting at index 0 (MSB side) to 0b101:
 *   data = SetBitsInByte(data, 0, 3, 0b101);  // data becomes 0b10100000
 *
 *   // Set 2 bits starting at index 3 to 0b11:
 *   data = SetBitsInByte(data, 3, 2, 0b11);   // data becomes 0b10111000
 *
 *   // Update the MSB-side 3 bits to 0b010:
 *   data = SetBitsInByte(data, 0, 3, 0b010);  // data becomes 0b01011000
 *
 * MSB-first mapping for an 8-bit byte:
 *   Index:     0   1   2   3   4   5   6   7
 *   HW bit:    7   6   5   4   3   2   1   0
 *   Position: MSB                         LSB
 */
uint8_t SetBitsInByte(uint8_t number, uint8_t index, uint8_t length,
		      uint8_t value);

#endif
