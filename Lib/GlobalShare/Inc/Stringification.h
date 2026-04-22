#ifndef _STRINGIFICATION_H_
#define _STRINGIFICATION_H_

/**
 * @brief Macro to concatenate two tokens without expanding them first.
 *
 * This macro uses the C preprocessor's token-pasting operator (##) to concatenate two tokens into a single token without expanding them first. This is useful when you want to concatenate macro
 * arguments that are themselves macros, and you want to prevent them from being expanded before concatenation.
 *
 * @param a The first token to concatenate.
 * @param b The second token to concatenate.
 * @return The concatenated token resulting from joining a and b without expansion.
 */
#define CONCAT_RAW(a, b) a##b

/**
 * @brief Macro to concatenate two tokens.
 *
 * This macro uses the C preprocessor's token-pasting operator (##) to concatenate two tokens into a single token.
 *
 * @param a The first token to concatenate.
 * @param b The second token to concatenate.
 * @return The concatenated token resulting from joining a and b.
 */
#define CONCAT(a, b) CONCAT_RAW(a, b)

/**
 * @brief Macro to convert a macro argument into a string literal, with expansion.
 *
 * This macro first expands the argument x if it is a macro, and then converts the result into a string literal using the STRINGIFY macro. This allows you to get the string representation of a macro's
 * value rather than its name.
 *
 * @param x The macro argument to be converted into a string literal.
 * @return A string literal representing the expanded value of the argument.
 */
#define STRINGIFY_MACRO(x) STRINGIFY(x)

/**
 * @brief Macro to convert a macro argument into a string literal.
 *
 * This macro uses the C preprocessor's stringification operator (#) to
 * convert the provided argument into a string literal.
 *
 * @param x The macro argument to be converted into a string literal.
 * @return A string literal representing the argument.
 */
#define STRINGIFY(x) #x

#endif
