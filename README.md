# lasagna

Very small stack based language.

## Compiling

```
git submodule update --init --recursive
make
```
The program has no dependencies, and is made in pure C.

## Running

This language is based entirely on the stack and file streams. Some examples:

### hello world

```
(hello world) printstr
```
This prints hello world. Instead of `"`, `(` and `)` are used to represent strings, and can be nested. The string is pushed onto the stack character by character, with a leading and terminating null byte. printstr is one of a few built in functions defined in src/functions.c

### if statement

```
1 ((if the following condition is true, this will print) println) evalif
```

The evalif function takes 2 parameters, a string to be evaluated, and a single number. The string will be evaluated if the number is not equal to zero. Since strings can be nested, it's not a problem that we have another one inside. In this case, since the 2nd parameter of evalif is 1, the string previous is evaluated. This pushes the string to the stack and then drops it.

### math
```
1 2 +u8 print8
```
This program pushes the bytes 1 and 2, sums them, and prints the result. The order is left to right. For the most part, math is pretty self explanatory. Numeric literals must be less than 255.

### defining a function

In order to define a new word, you'll have to use the mkfun function
```
((hello) println) (say-hello) mkfun
```
Make sure that this file is in the current directory. The word definition is instantly available to you with no reload. Type
```
say-hello
```
within the interactive prompt. It should print out `hello world`.

### loops

Loops loop forever as long as the value on the stack is not 0. They pop the value of the stack before executing the body.

To print a word 10 times:
```
10
(
  (a word) println
  1 -u8 dupu8
) loop
