

function adpl() {
  if [ $1 = "--build" ]
  then
    echo "Building..."
    g++ /home/$USER/ADPL/adpl.cpp -o /home/$USER/ADPL/adpl_compiler
    echo "Done Building!"
  else
    /home/$USER/ADPL/./adpl_compiler "$@"
  fi
  return 69
}
