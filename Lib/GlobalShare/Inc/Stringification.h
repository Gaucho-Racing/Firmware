#ifndef _STRINGIFICATION_H_
#define _STRINGIFICATION_H_

/**
 * @brief Macro to convert a macro argument into a string literal.
 *
 * This macro uses the C preprocessor's stringification operator (#) to
 * convert the provided argument into a string literal.
 *
 * @param x The macro argument to be converted into a string literal.
 * @return A string literal representing the argument.
 */
#define STRINGIFY(x) (#x)

#endif
