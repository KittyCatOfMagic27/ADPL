# ADPL
A Dumb Programming Language written in C++
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
All the documentation is in the text files uploaded because
that is where I have been keeping the documentation before 
I had pushed this project to github. I may consider moving
the documentation to this file later on, but I have no idea
how to style in an md file well.


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
