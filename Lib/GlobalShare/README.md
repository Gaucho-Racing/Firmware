# Global Share

## Purpose

This directory contains shared definitions and interfaces used across multiple projects. The code here is platform-agnostic and provides common types, enums, and structures that ensure consistency across everything.

Please do not add source files to this directory, this is intended for only header files.

**Important**: All files in this directory are **manually maintained and hand-written**. Nothing here is auto-generated. For auto-generated files please see [Autogen](/Autogen).

Please ensure everything is clearly doc-commented and a markdown section is present for each file as relevant.

## Adding New Shared Definitions

When adding new shared types or constants:

1. Create a new header file in `Inc/` with a descriptive name
2. Use include guards and follow the existing naming conventions
3. Document everything with clear doc-comments
4. Consider backward compatibility if modifying existing files

## State Machine

This is an enum corresponding to [`STATE_MACHINE.md`](/STATE_MACHINE.md) exactly, no change should be made to this enum without modifying the state machine diagram and vice versa.

It is critical that every board runs the same version of this enum so as to have correct state machine numerical definitions.

## Unused

Sometimes the `UNUSED` macro is accessible as it is defined inside of the STM32 HAL/LL library. Other times it is not. This file can _always_ be included and will either

- Provide the macro
- Silently do nothing and let STM32 HAL/LL provide the macro

Since we compile with many warnings enabled then it is a problem to not use a variable, but for testing you may want to have variables you do not use and then you can use this macro.

### Example

No warnings:

```c
{
    int thing = 42;
    UNUSED(thing);
}
```

Will have a warning that will become promoted to an error for release builds:

```c
{
    int thing = 42;
}
```

## Stringification

This macro converts the name of any type or variable into the corresponding quote-surronded label.

This is helpful for cases where you need the explicit name of a variable

### String Example

```c
int input = 42;
LOGOMATIC("%s reads %d\n", STRINGIFY(input), input);
// input reads 42\n
```

### Macro Caveat

Note that if `input` in that case was a macro you would be printing the value represented by that macro, this is an understood edge case. If that is needed then another macro can be created using `STRINGIFY(STRINGIFY(MACRO))`.

See the GNU GCC documentation for more information on this behavior.

## Comparator Macros

Any macro implemented must not contain any anti-patterns.

Prefer using macros that are "well known" to be safe.

Relevant macros should follow proper `do { /* ... */ } while(0)` when needed, and should keep internal variables named in a manner to prevent collisions.

### Testing

All macros should be tested using a variety of runtime checks.

These runtime checks should include strange arguments like function calls, other macros, and pre/post-fix operators; basically anything that someone trying to break it would throw at it.

Sadly, CTest limits us from verifying that a specific program will not compile.

### Anti-Pattern (Do Not Do)

An example of an anti-pattern (something to not do) would be the following

```c
#define UNSAFE_MAX(x, y) (((x) > (y)) ? (x) : (y))
```

If I ever call `UNSAFE_MAX(++a, b)` you would expect to evaluate `++a`'s value against `b`'s value. Instead it gets evaluated twice during expansion, effectively becoming `(++a > b ? ++a : b)` and double incrementing, which is not fun to debug.
