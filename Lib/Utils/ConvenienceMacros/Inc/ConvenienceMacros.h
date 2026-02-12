#ifndef _CONVENIENCE_MACROS_H_
#define _CONVENIENCE_MACROS_H_

/**
 * @brief Macro to compute the maximum of two values.
 * @param x First value.
 * @param y Second value.
 * @return The maximum of x and y.
 */
#define MAX(x, y)                                                                                                                                                                                      \
	__extension__({                                                                                                                                                                                \
		__typeof__(x) __internal_x = (x);                                                                                                                                                      \
		__typeof__(y) __internal_y = (y);                                                                                                                                                      \
		__internal_x > __internal_y ? __internal_x : __internal_y;                                                                                                                             \
	})

/**
 * @brief Macro to compute the minimum of two values.
 * @param x First value.
 * @param y Second value.
 * @return The minimum of x and y.
 */
#define MIN(x, y)                                                                                                                                                                                      \
	__extension__({                                                                                                                                                                                \
		__typeof__(x) __internal_x = (x);                                                                                                                                                      \
		__typeof__(y) __internal_y = (y);                                                                                                                                                      \
		__internal_x < __internal_y ? __internal_x : __internal_y;                                                                                                                             \
	})

/**
 * @brief Macro to compute the maximum of two values with type checking.
 * @param x First value.
 * @param y Second value.
 * @return The maximum of x and y.
 * @note This macro includes a type check to ensure that x and y are of the same type, which can help prevent unintended behavior.
 */
#define MAX_WITH_TYPES(x, y)                                                                                                                                                                           \
	__extension__({                                                                                                                                                                                \
		__typeof__(x) __internal_x = (x);                                                                                                                                                      \
		__typeof__(y) __internal_y = (y);                                                                                                                                                      \
		(void)(&__internal_x == &__internal_y);                                                                                                                                                \
		__internal_x > __internal_y ? __internal_x : __internal_y;                                                                                                                             \
	})

/**
 * @brief Macro to compute the minimum of two values with type checking.
 * @param x First value.
 * @param y Second value.
 * @return The minimum of x and y.
 * @note This macro includes a type check to ensure that x and y are of the same type, which can help prevent unintended behavior.
 */
#define MIN_WITH_TYPES(x, y)                                                                                                                                                                           \
	__extension__({                                                                                                                                                                                \
		__typeof__(x) __internal_x = (x);                                                                                                                                                      \
		__typeof__(y) __internal_y = (y);                                                                                                                                                      \
		(void)(&__internal_x == &__internal_y);                                                                                                                                                \
		__internal_x < __internal_y ? __internal_x : __internal_y;                                                                                                                             \
	})

#endif
