# Convenience Macros

Any macro implemented must not contain any anti-patterns.

Prefer using macros that are "well known" to be safe.

Relevant macros should follow proper `do { /* ... */ } while(0)` formatting if appropriate.

## Testing

All macros should be tested using a variety of runtime checks.

These runtime checks should include strange arguments like function calls, other macros, and pre/post-fix operators; basically anything that someone trying to break it would throw at it.

Sadly, CTest limits us from verifying that a specific program will not compile.

## Anti-Pattern (Do Not Do)

An example of an anti-pattern (something to not do) would be the following

```c
#define UNSAFE_MAX(x, y) (((x) > (y)) ? (x) : (y))
```

If I ever call `UNSAFE_MAX(++a, b)` you would expect to evaluate `++a`'s value against `b`'s value. Instead it gets evaluated twice during expansion, effectively becoming `(++a > b ? ++a : b)` and double incrementing, which is not fun to debug.
