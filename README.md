# ADPL VERSION 0.0.7
#### A Dumb Programming Language written in C++ #####
       
INSPIRED BY TSODING ON YOUTUBE:    
-https://www.youtube.com/c/TsodingDaily    
      
Also incase it wasn't obvious this language is **MEGA UNSAFE**.
So use this thing recreationly for now, change whatever you'd
like, and please give suggestions and issues with the language.
I know about a lot of the bugs and I am and will work to fix them,
but if you do give this litte project the time of day I'd love it 
if you'd help with bug reporting or ways to fix very unorganized and
faulty systems *(aka this thing as a whole)*. Anyway have fun fiddling
with this little project and thank you for checking out my work!!!


## Setup ##
Copy the shell commands in the file ADPL.sh into your bash
profile file to use the ADPL commands and put these files in
the directory /home/$USR/ADPL/.

```
usr@penguin:~$ adpl --help
usr@penguin:~$ adpl <file>.adpl
usr@penguin:~$ ./<file>
```

For more information on the adpl command use the help flag.
**Note** that the build and debug flags are for development.


## Documentation ##
The entry point is the main function and additional documentation is in the files SYNTAX_STRUCT.txt and FUNCTIONS.txt.
    
### Functions ###
To define a function you put the keyword 'fn', the function name, and then the arguments.   
```
fn foo(UINT x UINT y BOOL z)
  # code
end
```
Currently you can only have 6 arguments and you need to specify the types of the arguments. 
To close a block you need to put the end keyword.  
   
To call a function you put the name of the function and then the args wrapped in parentheses.
```
fn main()
  let x 'Hello ADPL!'
  write(x)
end 
```
     
### Variables ###
Variables are defined with the keyword 'let' and then the to value assign to the vaiable. This value can be a literal value or another variable.
```
let a 69
let b 420
```
Variables can also have the value of expressions and function calls** by putting the expression within parentheses. Reassignment can be done with the write to operator '<<'.
```
let a 34
let b 35
let c (34 + 35)
a << c
```
They are stored within the function scope, which means that a variable defined in an if or loop can be acessed anywhere the function, this is subject to change though. This maybe considered a feature or a lazy design choice *of which it's both...* either way it's how variables work in this language.    
****The only functions that can be used to assign a value are builtins because of the lack of a return keyword. Return will be implemented next patch.**
    
### Comments ### 
A hash tag is the comment symbol, no multi-line comments yet because of the scuffed way comments are implemeted.
```
fn bar()
  let x 69  # nice
  let y 420 # dank
end
```
      
### Unsigned Intergers ###
They have the type keyword of 'UINT'. They are just postive whole numbers, currently they are the most supported type. 
They are 32-bit intergers; nothing special.
```
let x 69
let y 420
let z 13
```
#### Operators #####
   
   Name   |  OP Symbol  |       Use
:---:|:---:|:---
Add | + | Adds two uints.
Subtract | - | Subracts two uints.
Multiply | * | Multiplies two uints.
Divide | / | Divides two uints.
Mod | % | Outputs the remainder of two uints.
Decrement | -- | Subracts by 1.
Increment | ++ | Adds by 1.
Add to | += | Adds one uint to the other.
Sub from | -= | Subtract one uint from another.
Equal | == | True if a equals b.
Less Eql. | <= | True if a is less than or equal to b.
Great Eql. | >= | True if a is greater than or equal to b.
Greater | > | True if a is greater than b.
Less | < | True if a is less than b.
Not Equal | != | True if a does not equal b.

### Boolean ### 
Stored in a 8-bit register, have the type keyword of 'BOOL', and can be true or false. 
They are the output of all comparison operators and are also a standalone type. 
The kewords 'true' and 'false' are also used to set their value.  
Currently the only operator is not '!' which goes infront of a boolean variable and flips the value.

```
let a true
let b false
```
    
### Character ###
These are ascii chacters, use the type keyword of 'CHAR', and they are stored in an 8-bit register. The are inbetween single quotes and are only a single character.
  
```
fn main()
  write('H')
  write('e')
  write('l')
  write('l')
  write('o')
  write(' ')
  write('W')
  write('o')
  write('r')
  write('l')
  write('d')
  write('!')
  new_line()
end
```

### Pointer ###
Uses the 'PTR' type keyword and stored in a 64-bit register. These are from using the 'ptr()' function. The current use is in syscalls.  
```
fn main()
  let NR 1                     # Write ID
  let stdout 1                 # Stdout directory
  let size 1                   # String size
  let char 'H'                 # String
  let char_ptr(ptr(char))      # Pointer to String

  syscall(NR stdout char size) # Write String to stdout
end
```
This is a mock-up of the internal write function using a syscall and a pointer.

### String ###
These are constants and cannot be literals or reassigned, right now they are simply used to write to the terminal easier. Despite this I am planning to expand upon this type by adding a length function and making a dynamic string or a char array.
```
fn main()
  let string0 'Hello World!'
  write(string0)
  new_line()
  let string1 'Finally a normal hello world...'
  write(string1)
  new_line()
end
```

### If ###
If only runs the code within its block if the condition within the parentheses is met.
```
let x 10
let y 10
if(x==y)
  let msg 'X equals Y'
  write(msg)
  new_line()
end
```
### Else ##
Else only activates if the previous ifs and elifs were not met.
```
let x 49
if(x>50)
  x++
end
else
  write(69)
end
```

### Elif ###
Elif is short for else if, so it is a combination of the operators else and if. It runs if the previous if and/or elifs conditions were not met and this one was.
```
if(false)
  x++
end
elif(true)
  write(69)
end
```
### Loop ###
Loop reruns the code within the loop until the condition is false.  
Here is a program that writes the nth fibonacci number.
```
fn main()
  let n 10
  let x 1
  let y 1
  let i 0
  loop(i<n)
    let z(x + y)
    x << y
    y << z
    i++
  end
  write(x)
end
```

## OS ##
This language is built for Debian Linux and compiles to NASM.   
Sorry for any inconvenience this may cause.


## Goals ##
These are goals for this language, the long and short term.
I can't promise these things because of life and school,
but these are my hopes and aspirations for this project.

### Short Term ###
- Make strings more dynamic instead of just a constant.
- Add arrays.
- Add structs (maybe).
- Bug fix and refactor how types are handled.
- Move documentation here.
- Fix loop and elif/bug.
- Rework branches in general.
- Give bools, chars, pointers, and strings more operations.

### Long Term ###
- Add live syntax adding.
- Linking.
- Self-Hosting.
- A standard library.
- Examples other than Hello_World.adpl:
  - Rule 110
  - Game of Life
  - Simple Web Server
  - MAYBE A graphical interface and window library
- Additional libraries.
- Deadcode removal and Optimization step.
