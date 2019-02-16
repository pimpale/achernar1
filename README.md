#stack - lang

Very small stack based language
        .

    ##Compiling

``` cc stack.c
``` That is it.There are no dependencies or
    makefiles.Everything will be included
        .

    ##Running

        This language is based entirely on the stack and file streams
        .Some examples :

    ## #hello world

``` (hello world)printstr
``` This prints hello world.Instead
    of `"`, `(` and `)` are used to represent strings, and can be nested. The string is pushed onto the stack character by character, with a leading and terminating null byte. printstr is one of a few built in functions defined in functions.h 

    ## #if statement

``` ((if the following condition is true, this will print)printstr)1 evalif
```

    The evalif function takes 2 parameters,
    a string to be evaluated,
    and a single number
            .The string will be evaluated if the number is
        not equal to zero.Since strings can be nested,
    it's not a problem that we have another one inside. In this case, since the 2nd parameter of evalif is 1, the string previous is evaluated. This pushes the string to the stack and then drops it.

            ## #math
``` 1 2 + print8
``` This program adds 1 and
        2,
    and prints the result.The order is left to right.For the most part,
    math is pretty self explanatory.Numeric literals must be less than
    255. To construct longer ones,
    you'll have to list the bytes, and then reinterpet them as a longer number.

        ## #math with larger numbers
``` 1 1 128 255 +
        16 
``` This pushes 4 bytes to the stack,
    which are interpreted as 2 16 bit integers by the operand.stack
        - lang utilizes absolutely no typing.Everything is just bytes.

          ## #defining a word

          In order to define a new word,
    you'll have to create a new file. For this example, create a new file and name it `print-hello-world` Within it, type:
``` (hello world)printstr
``` Make sure that this file is in the current directory
            .The word
        definition is instantly available to you with no reload.Type 
``` print
        - hello -
        world
``` within the interactive prompt.It should print out `hello world`
            .Most forth implementations have a dictionary.With stack
        - lang,
    the
    dictionary is the file system.Most words are defined in terms of other words
    ,
    and only a few are actually hardcoded in
            .These can clutter up the file system.
