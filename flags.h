#ifndef FLAGS_H
#define FLAGS_H

#include <stdbool.h>
#include <string>
#include <vector>
#include <cassert>
#include <iostream>

namespace kf{
  typedef void (*flagfn)(std::string);
  
  class FlagProxy{
    public:
      FlagProxy(std::string help_text){
        assert(help_text!="");
        HELP.push_back(help_text);
      }
      
      void AddFlag(std::string flag, flagfn fn, std::string desc, int inputs = 0){
        assert(flag!="");
        assert(desc!="");
        FNPTRS.push_back(fn);
        FLAGS.push_back(flag);
        INPUTS.push_back(inputs);
        HELP.push_back(flag);
        HELP.push_back(desc);
        if(flag.size() > MAX_FLAG_LEN) MAX_FLAG_LEN = flag.size();
      }
      
      void AddHelp(std::string external_flag, std::string desc){
        HELP.push_back(external_flag);
        HELP.push_back(desc);
      }
      
      void AddHelp(std::string desc){
        HELP_END.push_back(desc);
      }
      
      void Parse(int argc, char** argv){
        for(int i = 1; i < argc; i++){
          std::string s(argv[i]);
          if(s == "--help"){
            PrintHelpDesc();
          }
          else if(s == "help"){
            std::cout << "Please use --help instead of help.\n";
            PrintHelpDesc();
          }
          else{
            bool recognized = false;
            for(int j = 0; j < FLAGS.size(); j++){
              if(FLAGS[j]==s){
                std::string input;
                for(int k = 0; k < INPUTS[j]; k++){
                  if(++i==argc){
                    std::cerr << "\033[1;31mFLAG '"<<s<<"' NEEDS "<< INPUTS[j] <<" INPUT, ONLY GIVEN "<< k <<" INPUTS!\n"
                    "HELP DESC FOR '" <<s<< "':\033[0m\n" <<
                    HELP[j*2+2] << "\n";
                    assert(false);
                  }
                  std::string cinput(argv[i]);
                  input+=cinput+' ';
                }
                FNPTRS[j](input);
                recognized=true;
                break;
              }
            }
            if(recognized==false){
              std::cerr << "\033[1;31mFLAG '"<< s <<"' NOT RECORGINZED!\033[0m\n";
              PrintHelpDesc();
              assert(false);
            }
          }
        }
      }
      
    private:
      void PrintHelpDesc(void){
        std::cout << "\n" <<HELP[0] << "\n\n";
        for(int i = 1; i < HELP.size(); i+=2){
          std::cout << HELP[i];
          std::string offset;
          for(int j = 0; j < MAX_FLAG_LEN - HELP[i].size(); j++){
            offset+=" ";
          }
          std::cout << offset << "       " << HELP[i+1] << "\n";
        }
        std::cout << "\n\n\n";
        for(std::string &s: HELP_END){
          std::cout << s <<"\n";
        }
      }
    
      std::vector<flagfn> FNPTRS;
      std::vector<std::string> FLAGS;
      std::vector<int> INPUTS;
      std::vector<std::string> HELP;
      std::vector<std::string> HELP_END;
      int MAX_FLAG_LEN = 0;
  };
};

#endif
