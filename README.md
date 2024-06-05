# Calculator

Simple calculator based on Shunting-Yard algorithm implemented in two nights.

It is capable of calculating simple numeric equations:

```
2 + (2 * 3) / 4 - (2 ** 5)
```

It has a lot of build in functions that implemented both as operators and functions:

```
sin(2) + min(10, 3) ** 2
```

```
sin 2 + (10 min 3) ** 2
```

Some boolean operators supported:

```
(10 <= sin(10) + 1) and (cos(3 ** -sin(3.1415926)))
```

Has support for variables and some built-in variables:

```
(10 <= sin(tau) + 1) and (cos(3 ** -sin(pi))) * x
```

_x is passed as a command line argument_

# List of supported functions

| Area                       | Functions                                                                                          |
| -------------------------- | -------------------------------------------------------------------------------------------------- |
| roots                      | sqrt, cbrt                                                                                         |
| exponentiation             | ln, lg, exp                                                                                        |
| rounding                   | ceil, floor, round, trunc                                                                          |
| trigonometry               | sin, asin, sinh, asinh, cos, acos, cosh, acosh, tan, atan, tanh, atanh, ctan, actan, ctanh, actanh |
| arithmetic operators       | `+`, `-`, `/`, `*`, `%`,`^`(`**`),                                                                 |
| logical operators          | `<`, `>`, `<=`, `>=`, `==`, `and`, `or`,`xor`                                                      |
| other (operator functions) | abs, min, max, lcm, gcd, add, sub, div, mul, mod, pow                                              |

# TODO

- Fix variable - function confusion in equations like this `tau min pi`
- Add command line arguments for variables
- Add variadic support for most binary functions (`+(1, 2, 3)`, `min(1, 2, 3)`) [hint](https://blog.kallisti.net.nz/2008/02/extension-to-the-shunting-yard-algorithm-to-allow-variable-numbers-of-arguments-to-functions/)
