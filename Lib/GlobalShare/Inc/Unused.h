#ifndef _UNUSED_H_
#define _UNUSED_H_

/**
 * @brief Macro to mark a variable or parameter as unused.
 *
 * This macro helps to avoid compiler warnings about unused variables or
 * parameters. It casts the variable to void, indicating that it is
 * intentionally unused.
 *
 * @param x The variable or parameter to be marked as unused.
 */
#define UNUSED(x) (void)(x)
#endif
