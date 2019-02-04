# stack-lang

Very small stack based language.

## Compiling

```
cc stack.c
```
That is it. There are no dependencies or makefiles. Everything will be included.

## Running

This language is based entirely on the stack and file streams. Some examples:

### hello world

```
(hello world) printstr
```
This prints hello world. Instead of `"`, `(` and `)` are used to represent strings, and can be nested. The string is pushed onto the stack character by character, with a leading and terminating null byte. printstr is one of a few built in functions defined in functions.h 

### if statement

```
((if the following condition is true, this will print) printstr) 1 evalif
```

The evalif function takes 2 parameters, a string to be evaluated, and a single number. The string will be evaluated if the number is not equal to zero. Since strings can be nested, it's not a problem that we have another one inside. 



