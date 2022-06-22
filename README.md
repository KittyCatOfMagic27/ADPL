# ADPL VERSION 0.0.8
#### A Dumb Programming Language written in C++ #####
       
INSPIRED BY TSODING ON YOUTUBE:    
-https://www.youtube.com/c/TsodingDaily    
      
CHANGE LOG:   
- Complete Rework
- ADIL (A Dumb Intermediate Language)
- Returns
- New 'const' keyword 
- Arrays
- Address Minipulation
- Compiler Commands
- Lots of syscall wrappers
- And much more!
     
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
The entry point is the main function and additional documentation is in the file FUNCTIONS.txt.
    
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
  const x 'Hello ADPL!'
  writeln(x)
end 
```

You simply put the keyword ret, then the variable/value you'd like to return, the return type after the function arguments!!!
```
fn foo() UINT
  ret 69
end

fn main()  
  let a foo()
  writeln(a)
end
```
     
### Variables ###
Variables are defined with the keyword 'let' and then the to value assign to the vaiable. This value can be a literal value or another variable.
```
let a 69
let b 420
```
Variables can also have the value of expressions and function calls by putting the expression after the vaiable, reassigning is putting the value after the variable.

```
let a 34
let b 35
let c 34 + 35
a c
```
They are stored within the function scope, which means that a variable defined in an if or loop can be acessed anywhere the function, this is subject to change though. This maybe considered a feature or a lazy design choice *of which it's both...* either way it's how variables work in this language. Also be aware that variables of the same name have to hav ethe same type, this will be patched out soon!
    
### Constants ###
These are stored gloablly in the .data and .bss sections of the asm. 
```
const string 'Hello World!'

fn main()
  writeln(string)
  new_line()
end
```
Note constants can also be a way of memory allocation. To allocate memory with a constant use the const key word with the amount of bytes you want to allocate in square brackets. Like this. 
```
const[100] buffer
fn main()
  let buffer.ptr ptr(buffer)
end
```
And then you can get the begining of the memory allocated with taking that name and making it a pointer.
    
### Comments ### 
A hash tag is the comment symbol, no multi-line comments yet because of the lack of a need for them.
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
Add | + | Adds two uints. (a + b)
Subtract | - | Subracts two uints. (a - b)
Multiply | * | Multiplies two uints. (a * b)
Divide | / | Divides two uints. (a / b)
Mod | % | Outputs the remainder of two uints. (a % b)
Decrement | -- | Subracts by 1. (a--)
Increment | ++ | Adds by 1. (a++)
Equal | == | True if a equals b. (a == b)
Less Eql. | <= | True if a is less than or equal to b. (a <= b)
Great Eql. | >= | True if a is greater than or equal to b. (a >=b)
Greater | > | True if a is greater than b. (a > b)
Less | < | True if a is less than b. (a < b)
Not Equal | != | True if a does not equal b. (a != b)

### Boolean ### 
Stored in a 8-bit register, have the type keyword of 'BOOL', and can be true or false. 
They are the output of all comparison operators and are also a standalone type. 
The kewords 'true' and 'false' are also used to set their value.  

```
let a true
let b false
```

   Name   |  OP Symbol  |       Use
:---:|:---:|:---
And | && | True if both are true. (a && b)
Or | || | True if one or both of them are true. (a || b)
Xor | -| | True if ONLY one of them is true. (a -| b)
Not | ! | Flips the value of a boolen. (a!)
    
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
  writeln('!')
end
```
Name   |  OP Symbol  |       Use
:---:|:---:|:---
Equal | == | True if a equals b. (a == b)
Not Equal | != | True if a does not equal b. (a != b)

### Pointer ###
Uses the 'PTR' type keyword and stored in a 64-bit register. These are from using the 'ptr()' function. The current use is in syscalls and memory accessing.  
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

Name   |  OP Symbol  |       Use
:---:|:---:|:---
Decrement | -- | Subracts by 1. (a--)
Increment | ++ | Adds by 1. (a++)
Add | + | Adds a uint to a pointer (a+b)
Sub | - | Subtracts uint from pointer (a-b)
At Pointer | @ | Used to get the value at the address of a pointer (@TYPE a)

### String ###
These can only be constants and cannot be literals or reassigned, right now they are simply used to write to the terminal easier. Despite this I am planning to expand upon this type by making a dynamic string or a char array.   
```
const string0 'Hello World!'
const string1 'Okay, this is a bit more normal...'
fn main()
  let string_ptr ptr(string0)
  write(string_ptr string0.len)
  new_line()
  string_ptr ptr(string1)
  write(string_ptr string1.len)
end
```

### Arrays ###
An array is defined with as a const and cannot be initialized with any data. To define one type const, the amount of bits that you want to allocate in square brackets, and then the variable name. Once you do this you can make it a pointer and it can be interfaced with as an address!
```
fn main()
  # allocate 4x the length because UINT is 4 bytes
  const[100] fibnumbers
  let array_length 25
  let pointer ptr(fibnumbers)
  let current pointer
  let index 2
  let bytes_per 4
  
  # Fill first two values with ones
  @UINT current 1
  current pointer + bytes_per
  @UINT current 1
  
  loop(index < array_length)
    # get value 1 element back
    let pos index - 1
    pos pos * bytes_per
    current pointer + pos
    let a @UINT current
    
    # get value 2 elements back
    pos index - 2
    pos pos * bytes_per
    current pointer + pos
    let b @UINT current
    
    # save value
    let c a + b
    pos index * bytes_per
    current pointer + pos
    @UINT current c
    
    index++
  end
end
```
Using arrays is very verbose for now, but that will change soon!

### If ###
If only runs the code within its block if the condition within the parentheses is met.
```
let x 10
let y 10
if(x==y)
  const msg 'X equals Y'
  writeln(msg)
end
```
### Else ##
Else only activates if the previous ifs and else ifs were not met.
```
let x 49
if(x>50)
  x++
else
  writeln(69)
end
```

### Else If ###
Else if only activates if the previous ifs and else ifs were not met and the condition in the else if is met.
```
if(false)
  x++
end
else if(true)
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
    let z x + y
    x y
    y z
    i++
  end
  writeln(x)
end
```
### Comp. Commands ###
These are just like pre-processor directives in C. The current cones are more for the asm compilation than anything, but in the future I hope to expand upon them more.  

Name|Use
:---:|:---
global | Used to make functions (and possibly consts) available in linked files. NO USE CASE CURRENTLY
include | Links .asm or .adpl file to current file. NO USE CASE CURRENTLY
extern | Tells the compiler a function was called that was from a link file. NO USE CASE CURRENTLY
define | Replaces as all instances of the defined var with the value. EX: ;;define STDOUT 1
     
The two ones that are currently defined are for linking files which is not fully implemented yet, BUT they are still in the source code in this branch so they are getting documentation.  
To use them you put the symbol ';;' in the global scope and then the arguements after it.
```
;;include 'stdlib.adpl'
```
Again linking and the standard library are not implemented yet, but this is a sneak peak of what all of this will look like!

## OS ##
This language is built for Debian Linux and compiles to NASM.   
Sorry for any inconvenience this may cause.


## Goals ##
These are goals for this language, the long and short term.
I can't promise these things because of life and school,
but these are my hopes and aspirations for this project.

### Short Term ###
- Make strings more dynamic instead of just a constant.
- Add structs (maybe).
- Bug fix and refactor how types are handled.
- Give bools, chars, pointers, and strings more operations.

### Long Term ###
- Linking.
- Self-Hosting.
- A standard library.
- Examples other than Hello_World.adpl:
  - Game of Life
  - Simple Web Server
  - MAYBE A graphical interface and window library
- Additional libraries.
- Deadcode removal and Optimization step.
