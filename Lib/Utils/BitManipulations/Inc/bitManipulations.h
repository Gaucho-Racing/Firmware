/**
 * @file bitManipulations.h
 * @brief Bit manipulation utilities using MSB-first bit indexing
 *
 * This library provides utilities for reading and writing individual bits and
 * bit fields with a single, consistent convention:
 *   - Bit indices are MSB-first: index 0 refers to the most significant bit.
 *
 * @note All bit indices are 0-based.
 * @note Index out-of-bounds behavior is undefined; the caller must validate.
 */

#include <stdbool.h>
#include <stdint.h>

#ifndef _BIT_MANIPULATIONS_H_
#define _BIT_MANIPULATIONS_H_

// ============================================================================
// BIT INDEXING CONVENTION (MSB-FIRST)
// ============================================================================
// Bits are indexed MSB-first within a value.
//
// For a byte 0b10110100 (0xB4):
//   Bit index:  0  1  2  3  4  5  6  7
//   Bit value:  1  0  1  1  0  1  0  0
//               ↑most-significant          rightmost
// ============================================================================

/**
 * @brief Read a single bit using MSB-first indexing.
 *
 * Extracts a single bit from the specified position using MSB-first indexing,
 * where bit_position 0 refers to the most significant bit of the value.
 *
 * @param value The integer value to read from (any unsigned integer type)
 * @param bit_position Bit position (0 = most significant)
 * @return 1 if the bit is set, 0 if clear
 *
 * @note No bounds checking; bit_position must be < width of value's type.
 *
 * Example (MSB-first on a byte):
 *   uint8_t data = 0b10110100;  // 0xB4
 *   GETBIT(data, 0);  // 1
 *   GETBIT(data, 2);  // 1
 *   GETBIT(data, 7);  // 0
 */
#define GETBIT(value, bit_position) (((value) >> (sizeof(value) * 8 - 1 - (bit_position))) & 1u)

/**
 * @brief Extract a multi-bit field using MSB-first indexing.
 *
 * Extracts a contiguous sequence of bits starting at bit_index (MSB-first)
 * and returns them right-aligned.
 *
 * @param value The integer value to extract bits from (any unsigned integer
 * type)
 * @param bit_index Starting bit position (0 = most significant)
 * @param length Number of bits to extract (1..width)
 * @return Extracted bits, right-aligned (e.g., 3 bits → 0..7)
 *
 * @note For length >= 64, mask calculation may overflow; use with care
 * @note If bit_index + length exceeds the type width, behavior is undefined
 *
 * Example (MSB-first on a byte):
 *   uint8_t data = 0b10110100;  // 0xB4
 *   GETBITS(data, 0, 3);  // 0b101 (top three bits)
 *   GETBITS(data, 2, 4);  // 0b1101 (bits 2..5 by MSB index)
 *   GETBITS(data, 5, 3);  // 0b100 (bits 5..7 by MSB index)
 */
#define GETBITS(value, bit_index, length)                                                                              \
	(((value) >> ((sizeof(value) * 8) - (bit_index) - (length))) & ((1uLL << (length)) - 1))

/**
 * @brief Set or clear a single bit within a byte (MSB-first indexing).
 *
 * Modifies a single bit at the specified index using MSB-first convention,
 * where index 0 refers to the most significant bit (bit 7 in hardware).
 * Returns the modified byte value.
 *
 * @param number The byte value to modify
 * @param index Bit index (0-7), where 0 is the most significant bit.
 * @param value true to set the bit to 1, false to clear it to 0
 * @return The modified byte with the bit set or cleared
 *
 * @note Uses MSB-first indexing to align with message protocol conventions
 * @note Index must be in range [0, 7]; out-of-range causes undefined behavior
 * @note Does not modify the input; returns a new value
 *

 *
 * Example:
 *   uint8_t data = 0b00000000;
 *   data = SetBitInByte(data, 0, true);   // Sets MSB → data = 0b10000000
 *   data = SetBitInByte(data, 7, true);   // Sets rightmost bit → 0b10000001
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
 * @param index Starting bit index (0-7), where 0 is the most significant bit.
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
 *   Position: MSB                         rightmost
 */
uint8_t SetBitsInByte(uint8_t number, uint8_t index, uint8_t length, uint8_t value);

#endif
