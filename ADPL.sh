

function adpl() {
  if [ $1 = "--help" ]
  then
    echo ""
    echo "adpl --help             brings up this help page."
    echo "adpl -b <file>.adpl     Recompiles the compiler and then compiles adpl code from adpl file."
    echo "adpl -d <file>.adpl     Prints debug text while compiling."
    echo ""
    echo ""
    echo ""
    echo "adpl(1)                 Compiles adpl code from adpl file."
    echo ""
    echo "For additional information check INFO.txt in the ADPL folder."
 	elif [ $1 = "-b" ]
  then
    g++ /home/$USER/ADPL/adpl.cpp -o /home/$USER/ADPL/adpl_compiler
    if [ $1 = "-d" ]
    then
      /home/$USER/ADPL/./adpl_compiler $3 $PWD 1 # disfunctional, don't know why and don't care enough to fix
    else
      /home/$USER/ADPL/./adpl_compiler $2 $PWD 0 
    fi
  elif [ $1 = "-d" ]
  then
    /home/$USER/ADPL/./adpl_compiler $2 $PWD 1
  else
    /home/$USER/ADPL/./adpl_compiler $1 $PWD 0
  fi
  return 69
}
