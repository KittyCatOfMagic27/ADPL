#include <iostream>
#include <cstdlib>
#include <fstream>
#include <cstring>
#include <vector>
#include <cassert>
#include <sstream>
#include <unordered_map>
#include <algorithm>
#include <chrono>

using namespace std;

/*
  HOW TO USE:
  usr@penguin:~$adpl --help
*/

/*
  [NOTE]:
  COMPILATION TEMPORARILY REMOVED TO WORK ON
  IMPLEMENTING TOKENIZATION SO FUNCTIONS CAN
  ALSO BE IMPLEMENTED.
  
  FOR COMPILATION GO TO S2.
  
  [NOTE]:
  ALSO WORKING ON PARSING INTO A TREE
  
  DIAGRAM OF TREE:
  
        args ----> fn_node
                      |
                      |
                     expr
                      |
                      |
   conditional ----> loop ----> expr...
                      |
                      |
 conditional ----> if_node ----> expr...
                      |
                      |
   conditional ----> elif ----> expr...
                      |
                      |
                     else ----> expr...
                      |
                      |
                     ret
  
  TIME TO REWORK THE TREE FOR THE THIRD TIME
  MOVING TO S4
  
  [TODO]:
  S7 - VER 0.0.3
  IMPLEMENT IF AND MAYBE ELSE AND ELIF ASWELL
  
  [TODO]:
  S8 - VER 0.0.4
  GIVE FUNCTIONS ARGS AND IMPLEMENT ELSE AND ELIF
  
  [TODO]:
  S9 - VER 0.0.5
  IMPLEMENT ELSE AND ELIF INSTEAD OF GOING OFF ON A
  SIDE TANGENT DUMBASS.
  FIX RECURSION BUG WHERE VARS SET BEFORE RECURSIVE CALL
  AREN'T CORRUPTED. THIS COULD BE DONE BY POPPING THE
  STACK BEFORE A RECURSIVE CALL.
  
  TYPE CHECKING, BOOLEAN TYPE, REVAMP PTR_MAP SYSTEM.
  
  PTR_MAP SYSTEM:
  RUN THROUGH ALL OF PTR_MAPS IN BRANCH TO CHECK FOR
  VARIABLES.

  BOOLEAN:
  THIS IS A TYPE THAT IS STILL DEFINED WITH LET,
  WE CHECK IT BY THE INPUT EXPRESSION/VALUE.
  THE PTR IS A BYTE POINTER, NOT DWORD LIKE INT.
  IT IS SET 1 OR 0 (true/false).

  TYPE CHECKING:
  MAKE REVAMP CURRENT VAR MAP TO BE DATA ORIENTED.
  IT WILL BE A VECTOR OF STRINGS.
  DATA[0] = NAME
  DATA[1] = TYPE
  DATA[2] = ID/LITERAL
  DATA[3...n] = DATA

  MAKE HELPER FUNCTIONS FOR HANDING VAR MAP.
  
  MAKE MODULAR!
  
  
  NEW IDEA TO OVERHAUL:
  ONLY NODES ARE SCOPES.
  TO PARSE THE CODE WE WILL USE THE NEW SYNTAX
  STRUCTURE.
  IT IS HIGHLY MODULAR AND GIVES A CHANCE IN THE
  FUTURE FOR THE IMPLEMENTATION OF OPERATOR OVERLOADING
  AND CUSTOM SYNTAXES AND STATEMENTS.
  
  NEW DIAGRAM:

            function <---- arguments (In data field)
               |
        code in function
               |
              \|/
               if <---- conditional (In data field)
               |
          code in scope
               |
              \|/
              elif <---- conditional (In data field)
               .
               .
               .
              ret
  
  COMPILATION DIAGRAM:
  
          Tokenization
               |
               |
             Define
             Blocks
               |
               |
             Block
              Dec
               &
              Type
             Tables
               |
               |
             Syntax
            Matching
               |
               |
             Syntax
           Processing
          
  [TODO]:
  S10 - VER 0.0.6
  REIMPLEMENT FUNCTIONS AND IF STATEMNETS. ELIF,
  LOOPS, AND RECURSIVE FUNCTIONS ARE ON THE AGENDA.
  AND MAYBE SEPERATE FILE FOR THE SYNTAX DECLARATIONS,
  OR MAYBE EVEN A MINI LANG FOR ADPL SYNTAX DEFINING.
  
  POSSIBLE SYNTAX FOR MINI LANG:
  
  START
  NAME(UINT+UINT)
  STRUCTURE(UINT PLUS UINT -> EAX UINT)
  KEY(A + B)
  ASM_CODE(
    mov eax, @A \n
    add eax, @B \n
  )
  END
  
  [TODO]:
  S11 - VER 0.0.7
  FIX ELSE STATEMNETS. --DONE
  ADD RETURN AND BREAK.
  ADD BOOL LOGIC GATES.
  ADD OPTIMIZATION STEP:
    **CHECK IF A VALUE IS ALREADY IN A
      REGISTAR
    **DELETE REDUNDENT IF/ELIF/ELSE/LOOP
      STATEMENTS
  INTRODUCE CHAR TYPE AND WAY TO PRINT THEM.
  
  [NOTE*]: LONGER TERM GOAL OF BEING ABLE TO ADD
    SYNTAXES LIVE
    IDEA OF HOW THIS WOULD WORK:
    
    @SYN start
      @.name "UINT+UINT"
      @.struct "UINT PLUS UINT" "EAX UINT"
      @.key "A" "+" "B"
      @.asm start
        "mov eax, @A"
        "add eax, @B"
      end
    end
    
    This is a basic definition of a syntax.
    To access a field of the syntax do @.[NAME]
    Then to put info into it either do start end block
    or just put strings.
    
    The name is just the internal name of the syntax.
    
    struct is the structure, the types, symbols, ect.
    Then in struct we have a second argument,
    this the out register in ASM and the output type.
    
    The key is the symbols that we use to access those
    values in the psuedo nasm.
    
    @.asm is the psuedo nasm, in here we put the nasm
    code and use @[KEY_SYM] to access the values that
    we are provided with.
    
    This would be done during the parse stage, we would
    add this to the SYM_TABLE and then carry on parsing.
    
*/

struct SYNTAX{
  vector<string> KEY;
  vector<string> STRUCTURE;
  vector<string> OUTPUT;
  string NAME;
  string OUT_REG;
  string OUT_TYPE;
};

struct NODE{
  string TYPE;
  vector<string> DATA;
  string OTHER;
};

unordered_map<string, SYNTAX> SYNTAX_MAP;
unordered_map<string, string> tk_table;
unordered_map<string, string> dec_table;
unordered_map<int, NODE> TREE;
unordered_map<int, int> BYTE_LIST;
unordered_map<int, vector<int>> TREE_CONECTIONS;
unordered_map<int, int> INV_TREE_CONNECTIONS;
unordered_map<string, int> FN_LIST;
unordered_map<string, string> INV_JUMP;
unordered_map<string, int> SYM_TABLE;
unordered_map<int, unordered_map<string, int>> PTR_MAPS;
unordered_map<int, unordered_map<string, string>> TYPE_MAPS;
vector<string> SYNTAX_LIST;
vector<string> FUNC_ARG_REGISTER_LIST64;
vector<string> FUNC_ARG_REGISTER_LIST32;
vector<string> FUNC_ARG_REGISTER_LIST8;
vector<string> SYMBOL_LIST;
vector<string> SYSCALL_ARGS;

stringstream ss;
string WHITESPACE = " \n\r\t\f\v";
bool DEBUG_MODE;
int L_COUNTER = 0;
int NODE_INDEX = 0;
int STRING_COUNTER = 0;
auto START_TIME = chrono::high_resolution_clock::now();
auto STOP_TIME = chrono::high_resolution_clock::now();
auto DURATION_TIME = chrono::duration_cast<chrono::microseconds>(STOP_TIME - START_TIME);


void start_timer(){
  START_TIME = chrono::high_resolution_clock::now();
}

void stop_timer(string timer_name){
  STOP_TIME = chrono::high_resolution_clock::now();
  DURATION_TIME = chrono::duration_cast<chrono::microseconds>(STOP_TIME - START_TIME);
  float to_seconds = static_cast<float>(DURATION_TIME.count())/1000000;
  cout << timer_name <<" TIME: " << to_seconds << " seconds\n";
}

void print(NODE &x, int index){
  if (x.TYPE != "") cout << "TYPE: "<<x.TYPE<<"\n";
  if (x.DATA.size() > 0){
    cout << "DATA: ";
    for(string &str: x.DATA){
      cout << str << " ";
    }
    cout << "\n";
  }
  if(TREE_CONECTIONS[index].size()>0){
    cout << "CHILDREN: \n";
    for(int y: TREE_CONECTIONS[index]){
      print(TREE[y],y);
    }
    cout << "\n";
  }
}

SYNTAX define_syntax(string &name, vector<string> &structure, vector<string> &key, vector<string> &output, string out_reg, string out_type){
  SYNTAX new_syntax;
  new_syntax.KEY = key;
  new_syntax.STRUCTURE = structure;
  new_syntax.OUTPUT = output;
  new_syntax.NAME = name;
  new_syntax.OUT_REG = out_reg;
  new_syntax.OUT_TYPE = out_type;
  SYNTAX_MAP[name] = new_syntax;
  SYNTAX_LIST.push_back(name);
  return new_syntax;
}

bool isNumber(const string& str){
  for (char const &c : str) {
    if (isdigit(c) == 0) return false;
  }
  return true;
}

string literal_type(string &literal){
  if(literal=="TRUE" || literal=="FALSE")return "BOOL";
  if(isNumber(literal))return "UINT";
  if(literal[0]=='\''&&literal[literal.size()-1]=='\'')
    if(literal.size()<=3)return "CHAR";
    else return "STRING";
  cerr << "'" <<literal<<"' IS OF UNKNOWN TYPE\n";
  assert(false);
}

string literal_or_var(string &val){
  if(val=="TRUE" || val=="FALSE")return "BOOL";
  if(isNumber(val))return "UINT";
  if(val[0]=='\''&&val[val.size()-1]=='\'')
    if(val.size()<=3) return "CHAR";
    else return "STRING";
  return "ID";
}

bool pattern_match_syntax(SYNTAX &x, vector<string> &input, unordered_map<string, string> &type_map){
  if(x.STRUCTURE.size() != input.size())return false;
  for(int i = 0;i < x.STRUCTURE.size();i++){
    if(input[i]==x.STRUCTURE[i]) continue;
    if(find(SYMBOL_LIST.begin(), SYMBOL_LIST.end(), input[i]) != SYMBOL_LIST.end()) return false;
    if(x.STRUCTURE[i] == "ANY_ID"){
      if(literal_or_var(input[i]) == "ID")continue;
      return false;
    }
    else if(x.STRUCTURE[i] == "UINT_ID"){
      string type = literal_or_var(input[i]);
      if(literal_or_var(input[i]) != "ID")return false;
      if(type_map[input[i]]!="UINT")return false;
      continue;
    }
    else if(x.STRUCTURE[i] == "BOOL_ID"){
      string type = literal_or_var(input[i]);
      if(literal_or_var(input[i]) != "ID")return false;
      if(type_map[input[i]]!="BOOL")return false;
      continue;
    }
    else if(x.STRUCTURE[i] == "CHAR_ID"){
      string type = literal_or_var(input[i]);
      if(literal_or_var(input[i]) != "ID")return false;
      if(type_map[input[i]]!="BOOL")return false;
      continue;
    }
    if(x.STRUCTURE[i] == "ANY") continue;
    string type = literal_or_var(input[i]);
    if(type == "ID") if(type_map[input[i]]==x.STRUCTURE[i]) continue;
    if(type==x.STRUCTURE[i])continue;
    return false;
  }
  return true;
}

string get_asm_value(string &var_or_lit, unordered_map<string, int> &ptr_table, string &type){
  string ID = literal_or_var(var_or_lit);
  if(ID == "ID"){
    int ptr=ptr_table[var_or_lit];
    if(type == "UINT"){
      ss << "dword [rbp+" << ptr << "]";
    }
    else if(type == "BOOL" || type == "CHAR"){
      ss << "byte [rbp+" << ptr << "]";
    }
    else if(type == "PTR"){
      ss << "qword [rbp+" << ptr << "]";
    }
    else if(type == "STRING"){
      ss << "__STRING" << ptr;
    }
    string output = ss.str();
    ss.str("");
    return output;
  }
  else{
    string output;
    if(type == "UINT" || type == "CHAR" || type == "PTR"){
      output = var_or_lit;
    }
    else if(type == "STRING"){
      // string label = "__STRING";
      // label += to_string(STRING_COUNTER);
      // STRING_COUNTER++;
      // STRING_CONSTS[name] = label;
      
      // output = STRING_LITS[var_or_lit]; //get label from value
      cerr << "[TODO!] String literals are not implemented!";
      assert(false);
    }
    else if(type == "BOOL"){
      if(var_or_lit=="TRUE"){
        output = "1";
      }
      else{
        output = "0";
      }
    }
    return output;
  }
}

vector<string> processs_syntax(SYNTAX &x, vector<string> &input, int &y){
  vector<string> code;
  for(int i = 0; i < x.OUTPUT.size(); i++){
    auto j = find(x.KEY.begin(), x.KEY.end(), x.OUTPUT[i]);
    if(j != x.KEY.end()){
      int k = j - x.KEY.begin();
      string type = x.STRUCTURE[k];
      if(type == "ANY_ID"){
        type = TYPE_MAPS[y][input[k]];
      }
      else if(type == "ANY"){
        type = literal_or_var(input[k]);
        if(type == "ID"){
          type = TYPE_MAPS[y][input[k]];
        }
      }
      else if(type == "UINT_ID") type = "UINT";
      code.push_back(get_asm_value(input[k], PTR_MAPS[y], type));
    }
    else{
      code.push_back(x.OUTPUT[i]);
    }
  }
  return code;
}

string rtrim(const string &s){
  size_t start = s.find_last_not_of(WHITESPACE);
  return (start == string::npos) ? "" : s.substr(start);
}

string ltrim(const string &s){
  size_t start = s.find_first_not_of(WHITESPACE);
  return (start == string::npos) ? "" : s.substr(start);
}

string trim(string &s){
  return rtrim(ltrim(s));
}

string convertToString(char* &a, int size){
  string s = "";
  for (int i = 0; i < size; i++) {
    s = s + a[i];
  }
  return s;
}

string convertToString(char a){
  string s = "";
  return s + a;
}

vector<string> split(string &x, char delim = ' '){
  x = ltrim(x);
  x += delim;
  vector<string> splitted;
  string temp = "";
  bool in_string = false;
  for (int i = 0; i < x.length(); i++){
    if (x[i] == delim){
      splitted.push_back(temp);
      temp = "";
      i++;
    }
    char y;
    while(true){
      y = x[i];
      if(x[i]=='\''){
        if(temp != "") splitted.push_back(temp);
        temp = "";
        temp += x[i];
        i++;
        while(true){
          if(i>=x.length()){
            cerr << "CAN'T FIND END OF STRING.\n";
            assert(false);
          }
          temp+=x[i];
          if(x[i]=='\''){
            i++;
            break;
          }
          i++;
        }
        splitted.push_back(temp);
        temp = "";
      }
      else if(SYM_TABLE.find(convertToString(y)+x[i+1]) != SYM_TABLE.end()){
        if(temp!="")splitted.push_back(temp);
        temp = "";
        temp += y;
        temp += x[i+1];
        splitted.push_back(temp);
        temp = "";
        i+=2;
      }
      else if(SYM_TABLE.find(convertToString(y)) != SYM_TABLE.end()){
        if(temp!="")splitted.push_back(temp);
        temp = "";
        temp += x[i];
        splitted.push_back(temp);
        temp = "";
        i++;
      }
      else{
        break;
      }
    }
    y = x[i];
    if(y=='#'){
      if(temp != "") splitted.push_back(temp);
      return splitted;
    }
    if(y!=' '||in_string) temp += x[i];
  }
  return splitted;
}

vector<string> shift(vector<string> &v, string &x){
  v.insert(v.begin(), x);
  return v;
}

void DECLARE_VAR(string &name, string &type, int block_node_index){
  if(type != "STRING"){
    int size;
    if(type == "UINT"){
      size = 4;
    }
    else if(type == "BOOL" || type == "CHAR"){
      size = 1;
    }
    else if(type == "PTR"){
      size = 8;
    }
    BYTE_LIST[block_node_index]+=size;
    PTR_MAPS[block_node_index][name]=-BYTE_LIST[block_node_index];
    TYPE_MAPS[block_node_index][name]=type;
  }
  else{
    STRING_COUNTER++;
    TYPE_MAPS[block_node_index][name] = type;
    PTR_MAPS[block_node_index][name] = STRING_COUNTER;
  }
}

void INIT_TABLE(){
  tk_table["let"]="LET";
  tk_table["+"]="PLUS";
  tk_table["-"]="MINUS";
  tk_table["*"]="MULT";
  tk_table["/"]="DIV";
  tk_table["write"]="WRITE";
  tk_table["new_line"]="PNL";
  tk_table["syscall"]="SYSCALL";
  tk_table["ptr"]="GET_PTR";
  tk_table["fn"]="FUNCDEC";
  tk_table["start"]="START";
  tk_table["end"]="END";
  tk_table["("]="OP";
  tk_table[")"]="CP";
  tk_table["main"]="MAIN";
  tk_table["%"]="MOD";
  tk_table["if"]="IF";
  tk_table["<<"]="WRITE_TO";
  tk_table["=="]="EQL";
  tk_table["!="]="NEQL";
  tk_table["<="]="LESS_EQL";
  tk_table[">="]="GREATER_EQL";
  tk_table[">"]="GREATER";
  tk_table["<"]="LESS";
  tk_table["ret"]="RET";
  tk_table["break"]="BREAK";
  tk_table["true"]="TRUE";
  tk_table["false"]="FALSE";
  tk_table["else"]="ELSE";
  tk_table["!"]="!";
  tk_table["loop"]="LOOP";
  tk_table["--"]="DEC";
  tk_table["++"]="INC";
  tk_table["elif"]="ELIF";
  tk_table["+="]="+=";
  tk_table["-="]="-=";
  tk_table["@"]="@";
  
  SYM_TABLE["+"]=1;
  SYM_TABLE["-"]=1;
  SYM_TABLE["*"]=1;
  SYM_TABLE["/"]=1;
  SYM_TABLE["("]=1;
  SYM_TABLE[")"]=1;
  SYM_TABLE["%"]=1;
  SYM_TABLE["<<"]=1;
  SYM_TABLE["=="]=1;
  SYM_TABLE["!="]=1;
  SYM_TABLE["<="]=1;
  SYM_TABLE[">="]=1;
  SYM_TABLE[">"]=1;
  SYM_TABLE["<"]=1;
  SYM_TABLE["!"]=1;
  SYM_TABLE["--"]=1;
  SYM_TABLE["++"]=1;
  SYM_TABLE["+="]=1;
  SYM_TABLE["@"]=1;
  
  SYMBOL_LIST.push_back("PLUS");
  SYMBOL_LIST.push_back("MINUS");
  SYMBOL_LIST.push_back("MULT");
  SYMBOL_LIST.push_back("DIV");
  SYMBOL_LIST.push_back("OP");
  SYMBOL_LIST.push_back("CP");
  SYMBOL_LIST.push_back("MOD");
  SYMBOL_LIST.push_back("EQL");
  SYMBOL_LIST.push_back("NEQL");
  SYMBOL_LIST.push_back("LESS_EQL");
  SYMBOL_LIST.push_back("GREATER_EQL");
  SYMBOL_LIST.push_back("GREATER");
  SYMBOL_LIST.push_back("LESS");
  SYMBOL_LIST.push_back("WRITE_TO");
}

void INIT_FUNC_ARG_REGISTER_LIST(){
  FUNC_ARG_REGISTER_LIST64.push_back("rdi");
  FUNC_ARG_REGISTER_LIST64.push_back("rsi");
  FUNC_ARG_REGISTER_LIST64.push_back("rdx");
  FUNC_ARG_REGISTER_LIST64.push_back("rcx");
  FUNC_ARG_REGISTER_LIST64.push_back("r8");
  FUNC_ARG_REGISTER_LIST64.push_back("r9");
  
  FUNC_ARG_REGISTER_LIST32.push_back("edi");
  FUNC_ARG_REGISTER_LIST32.push_back("esi");
  FUNC_ARG_REGISTER_LIST32.push_back("edx");
  FUNC_ARG_REGISTER_LIST32.push_back("ecx");
  FUNC_ARG_REGISTER_LIST32.push_back("r8d");
  FUNC_ARG_REGISTER_LIST32.push_back("r9d");
  
  FUNC_ARG_REGISTER_LIST8.push_back("al");
  FUNC_ARG_REGISTER_LIST8.push_back("dl");
  
  SYSCALL_ARGS.push_back("rax");
  SYSCALL_ARGS.push_back("rdi");
  SYSCALL_ARGS.push_back("rsi");
  SYSCALL_ARGS.push_back("rdx");
  SYSCALL_ARGS.push_back("r10");
  SYSCALL_ARGS.push_back("r8");
  SYSCALL_ARGS.push_back("r9");
  
  INV_JUMP["je"]="jne";
  INV_JUMP["jne"]="je";
  INV_JUMP["jg"]="jle";
  INV_JUMP["jl"]="jge";
  INV_JUMP["jge"]="jl";
  INV_JUMP["jle"]="jg";
  INV_JUMP[""]="jmp";
  INV_JUMP["jmp"]="";
}

void INIT_SYNTAXES(){
  string name = "UINT+UINT";
  
  vector<string> structure;
  structure.push_back("UINT");
  structure.push_back("PLUS");
  structure.push_back("UINT");
  
  vector<string> key;
  key.push_back("A");
  key.push_back("+");
  key.push_back("B");
  
  vector<string> asm_code;
  asm_code.push_back("    mov eax, ");
  asm_code.push_back("A");
  asm_code.push_back("\n");
  asm_code.push_back("    add eax, ");
  asm_code.push_back("B");
  asm_code.push_back("\n");
  
  define_syntax(name,structure,key,asm_code,"eax","UINT");
  structure.clear();
  key.clear();
  asm_code.clear();
  
  
  name = "UINT-UINT";
  
  structure.push_back("UINT");
  structure.push_back("MINUS");
  structure.push_back("UINT");
  
  key.push_back("A");
  key.push_back("-");
  key.push_back("B");
  
  asm_code.push_back("    mov eax, ");
  asm_code.push_back("A");
  asm_code.push_back("\n");
  asm_code.push_back("    sub eax, ");
  asm_code.push_back("B");
  asm_code.push_back("\n");
  
  define_syntax(name,structure,key,asm_code,"eax","UINT");
  structure.clear();
  key.clear();
  asm_code.clear();
  
  
  name = "UINT*UINT";
  
  structure.push_back("UINT");
  structure.push_back("MULT");
  structure.push_back("UINT");
  
  key.push_back("A");
  key.push_back("*");
  key.push_back("B");
  
  asm_code.push_back("    mov eax, ");
  asm_code.push_back("A");
  asm_code.push_back("\n");
  asm_code.push_back("    mul ");
  asm_code.push_back("B");
  asm_code.push_back("\n");
  
  define_syntax(name,structure,key,asm_code,"eax","UINT");
  structure.clear();
  key.clear();
  asm_code.clear();
  
  
  name = "UINT/UINT";
  
  structure.push_back("UINT");
  structure.push_back("DIV");
  structure.push_back("UINT");
  
  key.push_back("A");
  key.push_back("/");
  key.push_back("B");
  
  asm_code.push_back("    mov eax, ");
  asm_code.push_back("A");
  asm_code.push_back("\n");
  asm_code.push_back("    div ");
  asm_code.push_back("B");
  asm_code.push_back("\n");
  
  define_syntax(name,structure,key,asm_code,"eax","UINT");
  structure.clear();
  key.clear();
  asm_code.clear();
  
  
  name = "UINT%UINT";
  
  structure.push_back("UINT");
  structure.push_back("DIV");
  structure.push_back("UINT");
  
  key.push_back("A");
  key.push_back("%");
  key.push_back("B");
  
  asm_code.push_back("    mov eax, ");
  asm_code.push_back("A");
  asm_code.push_back("\n");
  asm_code.push_back("    div ");
  asm_code.push_back("B");
  asm_code.push_back("\n");
  
  define_syntax(name,structure,key,asm_code,"edx","UINT");
  structure.clear();
  key.clear();
  asm_code.clear();
  
  
  name = "UINT>UINT";
  
  structure.push_back("UINT");
  structure.push_back("GREATER");
  structure.push_back("UINT");
  
  key.push_back("A");
  key.push_back(">");
  key.push_back("B");
  
  asm_code.push_back("    mov eax, ");
  asm_code.push_back("A");
  asm_code.push_back("\n");
  asm_code.push_back("    cmp eax, ");
  asm_code.push_back("B");
  asm_code.push_back("\n");
  asm_code.push_back("    setg al\n");
  
  define_syntax(name,structure,key,asm_code,"al","BOOL");
  structure.clear();
  key.clear();
  asm_code.clear();
  
  
  name = "UINT<UINT";
  
  structure.push_back("UINT");
  structure.push_back("LESS");
  structure.push_back("UINT");
  
  key.push_back("A");
  key.push_back("<");
  key.push_back("B");
  
  asm_code.push_back("    mov eax, ");
  asm_code.push_back("A");
  asm_code.push_back("\n");
  asm_code.push_back("    cmp eax, ");
  asm_code.push_back("B");
  asm_code.push_back("\n");
  asm_code.push_back("    setl al\n");
  
  define_syntax(name,structure,key,asm_code,"al","BOOL");
  structure.clear();
  key.clear();
  asm_code.clear();
  
  
  name = "UINT==UINT";
  
  structure.push_back("UINT");
  structure.push_back("EQL");
  structure.push_back("UINT");
  
  key.push_back("A");
  key.push_back("==");
  key.push_back("B");
  
  asm_code.push_back("    mov eax, ");
  asm_code.push_back("A");
  asm_code.push_back("\n");
  asm_code.push_back("    cmp eax, ");
  asm_code.push_back("B");
  asm_code.push_back("\n");
  asm_code.push_back("    sete al\n");
  
  define_syntax(name,structure,key,asm_code,"al","BOOL");
  structure.clear();
  key.clear();
  asm_code.clear();
  
  
  name = "UINT<=UINT";
  
  structure.push_back("UINT");
  structure.push_back("LESS_EQL");
  structure.push_back("UINT");
  
  key.push_back("A");
  key.push_back("<=");
  key.push_back("B");
  
  asm_code.push_back("    mov eax, ");
  asm_code.push_back("A");
  asm_code.push_back("\n");
  asm_code.push_back("    cmp eax, ");
  asm_code.push_back("B");
  asm_code.push_back("\n");
  asm_code.push_back("    setle al\n");
  
  define_syntax(name,structure,key,asm_code,"al","BOOL");
  structure.clear();
  key.clear();
  asm_code.clear();
  
  
  name = "UINT>=UINT";
  
  structure.push_back("UINT");
  structure.push_back("GREATER_EQL");
  structure.push_back("UINT");
  
  key.push_back("A");
  key.push_back(">=");
  key.push_back("B");
  
  asm_code.push_back("    mov eax, ");
  asm_code.push_back("A");
  asm_code.push_back("\n");
  asm_code.push_back("    cmp eax, ");
  asm_code.push_back("B");
  asm_code.push_back("\n");
  asm_code.push_back("    setge al\n");
  
  define_syntax(name,structure,key,asm_code,"al","BOOL");
  structure.clear();
  key.clear();
  asm_code.clear();
  
  
  name = "REASSIGN";
  
  structure.push_back("ANY_ID");
  structure.push_back("WRITE_TO");
  structure.push_back("ANY");
  
  key.push_back("A");
  key.push_back("<<");
  key.push_back("B");
  
  asm_code.push_back("    mov eax, ");
  asm_code.push_back("B");
  asm_code.push_back("\n");
  asm_code.push_back("    mov ");
  asm_code.push_back("A");
  asm_code.push_back(", eax \n");
  
  define_syntax(name,structure,key,asm_code,"NA","ANY");
  structure.clear();
  key.clear();
  asm_code.clear();
  
  
  name = "INC";
  
  structure.push_back("UINT_ID");
  structure.push_back("INC");
  
  key.push_back("A");
  key.push_back("++");
  
  asm_code.push_back("    add ");
  asm_code.push_back("A");
  asm_code.push_back(", 1");
  asm_code.push_back("\n");
  
  define_syntax(name,structure,key,asm_code,"NA","UINT");
  structure.clear();
  key.clear();
  asm_code.clear();
  
  
  name = "DEC";
  
  structure.push_back("UINT_ID");
  structure.push_back("DEC");
  
  key.push_back("A");
  key.push_back("--");
  
  asm_code.push_back("    sub ");
  asm_code.push_back("A");
  asm_code.push_back(", 1");
  asm_code.push_back("\n");
  
  define_syntax(name,structure,key,asm_code,"NA","UINT");
  structure.clear();
  key.clear();
  asm_code.clear();
  
  
  name = "PLUS_EQUALS";
  
  structure.push_back("UINT_ID");
  structure.push_back("+=");
  structure.push_back("UINT");
  
  key.push_back("A");
  key.push_back("+=");
  key.push_back("B");
  
  asm_code.push_back("    add ");
  asm_code.push_back("A");
  asm_code.push_back(", ");
  asm_code.push_back("B");
  asm_code.push_back("\n");
  
  define_syntax(name,structure,key,asm_code,"NA","UINT");
  structure.clear();
  key.clear();
  asm_code.clear();
  
  
  name = "MIN_EQUALS";
  
  structure.push_back("UINT_ID");
  structure.push_back("-=");
  structure.push_back("UINT");
  
  key.push_back("A");
  key.push_back("-=");
  key.push_back("B");
  
  asm_code.push_back("    sub ");
  asm_code.push_back("A");
  asm_code.push_back(", ");
  asm_code.push_back("B");
  asm_code.push_back("\n");
  
  define_syntax(name,structure,key,asm_code,"NA","UINT");
  structure.clear();
  key.clear();
  asm_code.clear();
  
  
  name = "NOT_OP";
  
  structure.push_back("!");
  structure.push_back("BOOL_ID");
  
  key.push_back("!");
  key.push_back("A");
  
  asm_code.push_back("    xor ");
  asm_code.push_back("A");
  asm_code.push_back(", 1\n");
  
  define_syntax(name,structure,key,asm_code,"NA","BOOL");
  structure.clear();
  key.clear();
  asm_code.clear();
}

void process_line(string &line, vector<string> &token_stream){
  vector<string> ops = split(line);
  if(DEBUG_MODE){
    for(string op: ops){
      cout << op << " ";
    }
    cout << "\n";
  }
  string STATE = "NONE";
  for(string& op: ops){
    if(tk_table.find(op) == tk_table.end()){
      string type = literal_or_var(op);
      if(type!="ID"){
        token_stream.push_back(op);
      }
      else if(dec_table.find(op) == dec_table.end()){
        if(*(&op - 1) == "let" || STATE == "FUNCTION_ARGS"){
          token_stream.push_back(op);
          string next_op = *(&op + 1);
          if(isNumber(next_op)||dec_table[next_op]=="UINT")
            dec_table[op] = "UINT";
          else if(next_op=="true" || next_op=="false" ||dec_table[next_op]=="BOOL")
            dec_table[op] = "BOOL";
          else if(next_op[0]=='\''&&next_op[next_op.size()-1]=='\'')
            if(next_op.size()<=3)
              dec_table[op] = "CHAR";
            else
              dec_table[op] = "STRING";
          else
            dec_table[op] = "ID";
        }
        else if(*(&op - 1) == "fn"){
          token_stream.push_back(op);
          dec_table[op] = "FUNC";
          STATE = "FUNCTION";
        }
        else if(*(&op - 1) == "if"){
          token_stream.push_back(op);
          dec_table[op] = "IF_STATEMENT";
          STATE = "FUNCTION";
        }
      }
      else{
        token_stream.push_back(op);
      }
    }
    else{
      if(STATE == "FUNCTION" && op == "("){
        STATE = "FUNCTION_ARGS";
      }else if(STATE == "FUNCTION_ARGS" && op == ")"){
        STATE = "NONE";
      }
      string tk = tk_table[op];
      token_stream.push_back(tk);
    }
  }
  token_stream.push_back("NL");
}

vector<string> LEXER(const char* fd){
  string line;
  ifstream source_code;
  source_code.open(fd);
  vector<string> token_stream;
  if(source_code.is_open()){
    while(getline(source_code,line)){
      if(line!="")process_line(line, token_stream);
    }
    source_code.close();
    return token_stream;
  }
  else {
    cerr << "Unable to open file.\n";
    cerr << fd << "\n";
    assert(false);
  }
  return token_stream;
}

int TRAVERSE_TREE(int current, string desired_type){
  while(true){
    if(TREE[current].TYPE==desired_type)return current;
    current = INV_TREE_CONNECTIONS[current];
    if(current==0){
      cerr << "COULDN'T FIND THE NODE TYPE '" << desired_type << "' IN TREE TRAVERSE.";
      assert(false);
    }
  }
}

void ATT_NODE(int parent_index, int child_index){
  TREE_CONECTIONS[parent_index].push_back(child_index);
  INV_TREE_CONNECTIONS[child_index]=parent_index;
}

NODE FN_NODE(vector<string> &label_args){
  NODE function_node;
  function_node.TYPE = "FUNC";
  function_node.DATA = label_args;
  for(int i = 1; i < (label_args.size()-1)/2; i++){
    DECLARE_VAR(label_args[i*2], label_args[i*2-1], NODE_INDEX);
  }
  TREE[NODE_INDEX] = function_node;
  NODE_INDEX++;
  return function_node;
}

NODE IF_NODE(vector<string> &conditional){
  NODE if_node;
  if_node.TYPE = "IF";
  if_node.DATA = conditional;
  TREE[NODE_INDEX] = if_node;
  NODE_INDEX++;
  return if_node;
}

NODE VAR_DEC_NODE(string &name, string type = "", string value = ""){
  NODE var_dec_node;
  var_dec_node.TYPE = "VAR_DEC";
  if(type == "" || value == ""){
    var_dec_node.DATA = {name};
  }else{
    var_dec_node.DATA = {name, type, value};
  }
  TREE[NODE_INDEX] = var_dec_node;
  NODE_INDEX++;
  return var_dec_node;
}

NODE VAR_EXPR_NODE(string &name, string &type){
  NODE var_dec_node;
  var_dec_node.TYPE = "VAR_EXPR";
  var_dec_node.DATA = {name, type};
  TREE[NODE_INDEX] = var_dec_node;
  NODE_INDEX++;
  return var_dec_node;
}

NODE PROGRAM_NODE(){
  NODE program_node;
  program_node.TYPE = "PROGRAM";
  TREE[NODE_INDEX] = program_node;
  NODE_INDEX++;
  return program_node;
}

NODE CODE_NODE(string &syntax_name, vector<string> &data){
  NODE code_node;
  code_node.TYPE = "CODE";
  code_node.DATA = data;
  code_node.DATA.push_back(syntax_name);
  rotate(code_node.DATA.rbegin(), code_node.DATA.rbegin() + 1, code_node.DATA.rend());
  TREE[NODE_INDEX] = code_node;
  NODE_INDEX++;
  return code_node;
}

NODE FN_CALL_NODE(vector<string> &label_args, string out_type = "UINT"){
  NODE fn_call_node;
  fn_call_node.TYPE = "FN_CALL";
  fn_call_node.DATA = label_args;
  fn_call_node.OTHER = out_type;
  TREE[NODE_INDEX] = fn_call_node;
  NODE_INDEX++;
  return fn_call_node;
}

NODE ELSE_NODE(){
  NODE else_node;
  else_node.TYPE = "ELSE";
  TREE[NODE_INDEX] = else_node;
  NODE_INDEX++;
  return else_node;
}

NODE ELIF_NODE(vector<string> &conditional){
  NODE elif_node;
  elif_node.TYPE = "ELIF";
  elif_node.DATA = conditional;
  TREE[NODE_INDEX] = elif_node;
  NODE_INDEX++;
  return elif_node;
}

NODE LOOP_NODE(vector<string> &conditional){
  NODE loop_node;
  loop_node.TYPE = "LOOP";
  loop_node.DATA = conditional;
  TREE[NODE_INDEX] = loop_node;
  NODE_INDEX++;
  return loop_node;
}

NODE BREAK_NODE(int loop){
  NODE break_node;
  break_node.TYPE = "BREAK";
  break_node.DATA = {to_string(loop)};
  TREE[NODE_INDEX] = break_node;
  NODE_INDEX++;
  return break_node;
}

class PARSER{
  public:
    
    vector<string> token_stream;
    vector<int> BLOCK_STACK;
    string STATE = "NONE";
    vector<string> stack;
    int CURRENT_FN;
    
    PARSER(vector<string> tk_stream){
      token_stream = tk_stream;
      PROGRAM_NODE();
      BLOCK_STACK.push_back(0);
      fn_list_init();
    }
  
    void run(){
      if(DEBUG_MODE) cout << "TOKEN:         STATE:\n";
      for(string& tk : token_stream){
        if(tk == "NL"){STATE = "NONE"; stack.clear(); continue;}
        func_handler(tk);
        loop_handler(tk);
        func_call_handler(tk);
        if_handler(tk);
        var_handler(tk);
        code_handler(tk);
        if(DEBUG_MODE) cout << tk << "         " << STATE << "\n";
      }
    }
    
  private:
    
    void func_call_handler(string &tk){
      if(STATE == "CALLING_FUNC"){
        if(tk == "CP"){
          STATE = "FUNC_CALL_END";
          NODE called_fn = FN_CALL_NODE(stack);
          ATT_NODE(BLOCK_STACK[BLOCK_STACK.size()-1], NODE_INDEX-1);
          stack.clear();
        }
        else if(tk != "OP"){
          stack.push_back(tk);
        }
      }
      else if(STATE == "NONE" && FN_LIST[tk]!=0){
        stack.push_back(tk);
        STATE = "CALLING_FUNC";
      }
    }
    
    void func_handler(string &tk){
      if(STATE == "FUNCDEC"){
        if(tk == "CP"){
          NODE func_node = FN_NODE(stack);
          int arg_count = (stack.size()-1)/2;
          for(int i = 0; i < arg_count; i++){
            string var = stack[(i*2)+2];
            string type = stack[(i*2)+1];
            DECLARE_VAR(var, type, NODE_INDEX-1);
          }
          FN_LIST[stack[0]]=NODE_INDEX-1;
          ATT_NODE(BLOCK_STACK[BLOCK_STACK.size()-1], NODE_INDEX-1);
          CURRENT_FN = NODE_INDEX-1;
          BLOCK_STACK.push_back(NODE_INDEX-1);
          stack.clear();
          STATE = "FINISH_DEC";
        }
        else if(tk != "OP"){
          stack.push_back(tk);
        }
      }
      else if(tk == "FUNCDEC"){
        STATE = "FUNCDEC";
      }
      else if(tk == "START"){
        STATE = "STARTING_BLOCK";
      }
      else if(tk == "END"){
        STATE = "ENDING_BLOCK";
        BLOCK_STACK.pop_back();
      }
    }
    
    void var_handler(string &tk){
      if(STATE == "VAR_START"){
        stack.push_back(tk);
        STATE = "DEC_VAR";
      }
      else if(STATE == "DEC_VAR"){
        if(tk != "OP"){
          string type = literal_or_var(tk);
          if(type == "ID"){
            type = TYPE_MAPS[CURRENT_FN][tk];
            if(DEBUG_MODE) cout << type << "\n";
          }
          DECLARE_VAR(stack[0], type, CURRENT_FN);
          NODE var_node = VAR_DEC_NODE(stack[0], type, tk);
          ATT_NODE(BLOCK_STACK[BLOCK_STACK.size()-1], NODE_INDEX-1);
          STATE = "VAR_END";
          stack.clear();
        }
        else{
          STATE = "VAR_EXPR";
        }
      }
      else if(STATE == "VAR_EXPR"){
        if(FN_LIST[tk]!=0){
          STATE = "VAR_FUNC";
          stack.push_back(tk);
        }
        else if(tk != "CP"){
          stack.push_back(tk);
          vector<string> tokens;
          for(int j = 1; j < stack.size(); j++){
            tokens.push_back(stack[j]);
          }
          for(int i = 0; i < SYNTAX_LIST.size(); i++){
            string name = SYNTAX_LIST[i];
            SYNTAX syntax = SYNTAX_MAP[name];
            if(!pattern_match_syntax(syntax, tokens, TYPE_MAPS[CURRENT_FN]))continue;
            NODE code_node = CODE_NODE(name, tokens);
            tokens.clear();
            break;
          }
        }
        else{
          if(TREE[NODE_INDEX-1].TYPE=="CODE"){
            string type = SYNTAX_MAP[TREE[NODE_INDEX-1].DATA[0]].OUT_TYPE;
            DECLARE_VAR(stack[0], type, CURRENT_FN);
            NODE var_node = VAR_EXPR_NODE(stack[0], type);
            ATT_NODE(NODE_INDEX-1,NODE_INDEX-2);
            ATT_NODE(BLOCK_STACK[BLOCK_STACK.size()-1], NODE_INDEX-1);
            stack.clear();
          }
          else{
            cerr << "VAR ASSIGN TO '" << stack[0] << "', EXPRESSION INVALID!\n";
            cerr << "THE GIVEN EXPRESSION WAS TOKENIZED AS '";
            vector<string> tokens;
            for(int j = 1; j < stack.size(); j++){
              tokens.push_back(stack[j]);
            }
            for(string &s: tokens){
              cerr << s << " ";
            }
            cerr << "'.\n";
            assert(false);
          }
        }
      }
      else if(STATE == "VAR_FUNC"){
        if(tk != "OP")
        stack.push_back(tk);
        if(tk == "CP"){
          STATE = "VAR_FUNC_END";
          string var = stack[0];
          stack.erase(stack.begin());
          NODE called_fn = FN_CALL_NODE(stack, "PTR");
          stack.clear();
          stack.push_back(var);
        }
        else if(tk != "OP") stack.push_back(tk);
      }
      else if(STATE == "VAR_FUNC_END"){
        string type = "PTR";
        DECLARE_VAR(stack[0], type, CURRENT_FN);
        NODE var_node = VAR_EXPR_NODE(stack[0], type);
        ATT_NODE(NODE_INDEX-1,NODE_INDEX-2);
        ATT_NODE(BLOCK_STACK[BLOCK_STACK.size()-1], NODE_INDEX-1);
        stack.clear();
      }
      else if(tk == "LET"){
        STATE = "VAR_START";
      }
    }
    
    void if_handler(string &tk){
      if(STATE == "IF_STATEMENT"){
        if(tk == "CP"){
          IF_NODE(stack);
          ATT_NODE(BLOCK_STACK[BLOCK_STACK.size()-1], NODE_INDEX-1);
          BLOCK_STACK.push_back(NODE_INDEX-1);
          stack.clear();
          STATE = "IF_STATEMENT_END";
        }
        else if(tk != "OP"){
          stack.push_back(tk);
        }
      }
      else if(STATE == "ELIF_STATEMENT"){
        if(tk == "CP"){
          ELIF_NODE(stack);
          ATT_NODE(BLOCK_STACK[BLOCK_STACK.size()-1], NODE_INDEX-1);
          BLOCK_STACK.push_back(NODE_INDEX-1);
          stack.clear();
          STATE = "ELIF_STATEMENT_END";
        }
        else if(tk != "OP"){
          stack.push_back(tk);
        }
      }
      else if(tk == "IF"){
        STATE = "IF_STATEMENT";
      }
      else if(tk == "ELIF"){
        STATE = "ELIF_STATEMENT";
      }
      else if(tk == "ELSE"){
        STATE = "ELSE_STATEMENT";
        ELSE_NODE();
        ATT_NODE(BLOCK_STACK[BLOCK_STACK.size()-1], NODE_INDEX-1);
        BLOCK_STACK.push_back(NODE_INDEX-1);
      }
    }
    
    void loop_handler(string &tk){
      if(STATE == "LOOP"){
        if(tk == "CP"){
          LOOP_NODE(stack);
          ATT_NODE(BLOCK_STACK[BLOCK_STACK.size()-1], NODE_INDEX-1);
          BLOCK_STACK.push_back(NODE_INDEX-1);
          stack.clear();
          STATE = "LOOP_END";
        }
        else if(tk != "OP"){
          stack.push_back(tk);
        }
      }
      else if(tk == "LOOP"){
        STATE = "LOOP";
      }
      else if(tk == "BREAK"){
        STATE = "BREAK";
        int loop = TRAVERSE_TREE(BLOCK_STACK[BLOCK_STACK.size()-1], "LOOP");
        BREAK_NODE(loop);
        ATT_NODE(BLOCK_STACK[BLOCK_STACK.size()-1], NODE_INDEX-1);
        
      }
    }
    
    void code_handler(string &tk){
      if(STATE == "NONE"){
        stack.push_back(tk);
        for(int i = 0; i < SYNTAX_LIST.size(); i++){
          string name = SYNTAX_LIST[i];
          SYNTAX syntax = SYNTAX_MAP[name];
          if(!pattern_match_syntax(syntax, stack, TYPE_MAPS[CURRENT_FN]))continue;
          NODE code_node = CODE_NODE(name, stack);
          ATT_NODE(BLOCK_STACK[BLOCK_STACK.size()-1], NODE_INDEX-1);
          stack.clear();
          break;
        }
      }
    }
    
    void fn_list_init(){
      FN_LIST["WRITE"]=-1;
      FN_LIST["PNL"]=-1;
      FN_LIST["SYSCALL"]=-1;
      FN_LIST["GET_PTR"]=-1;
    }
};

class COMPILER{
  public:
    ofstream out_stream;
    vector<string> PRINT_ON_BLOCK_END;
    NODE BLOCK_NODE;
    int BLOCK_NODE_INDEX;
    int CURRENT_FN = 0;
    int FN_ITER = 0;
    int FN_ITER_MAX = 0;
    int ESC_L = 0;
    int ESC_LOOP = 0;
    vector<NODE> STRING_DEFS;
    vector<int> BLOCK_STACK;
    unordered_map<string, int> PTR_MAP;
    unordered_map<string, string> TYPE_MAP;
    
    COMPILER (string &wf){
      out_stream.open(wf);
    }
    
    void run(){
      BLOCK_STACK.push_back(0);
      init();
      parse_tree();
      _exit();
      out_stream.close();
    }
    
  private:
    void init(){
      out_stream <<
"SECTION .text		; code section\n"
"WRITE_UINT:\n"
"    mov    eax, edi\n"
"    mov    ecx, 0xa\n"
"    push   rcx\n"
"    mov    rsi, rsp\n"
"    sub    rsp, 16\n"
".toascii_digit:\n"
"    xor    edx, edx\n"
"    div    ecx\n"
"    add    edx, 48\n"
"    dec    rsi\n"
"    mov    [rsi], dl\n"
"    test   eax,eax\n"
"    jnz  .toascii_digit\n"
"    mov    eax, 1\n"
"    mov    edi, 1\n"
"    lea    edx, [rsp+16 + 1]\n"
"    sub    edx, esi\n"
"    syscall\n"
"    add  rsp, 24\n"
"    ret\n"
//To make a system call in 64-bit Linux, place the system call number in rax, and its arguments, in order, in rdi, rsi, rdx, r10, r8, and r9, then invoke syscall.
"WRITE_CHAR:\n"
"    push rax\n"
"    ;xor rsi, rsi\n"
"    mov eax, 1    ;write syscall\n"
"    mov edi, 1    ;stdout\n"
"    mov rsi, rsp  ;get char\n"
"    mov edx, 1    ;char amount\n"
"    syscall\n"
"    pop rax\n"
"    ret\n"
"WRITE_NEW_LINE:\n"
"    push 10\n"
"    mov eax, 1    ;write syscall\n"
"    mov edi, 1    ;stdout\n"
"    mov rsi, rsp  ;get char\n"
"    mov edx, 1    ;char amount\n"
"    syscall\n"
"    pop rax\n"
"    ret\n"
"WRITE_STRING:\n"
"    mov eax, 1    ;write syscall\n"
"    mov edi, 1    ;stdout\n"
"    syscall\n"
"    ret\n"
"global _start		; make label available to linker\n";
    }
    
    void func_node(NODE &x, int y){
      if(x.DATA[0]=="MAIN"){
        out_stream <<
"_start:\n";
      }else{
        out_stream <<
x.DATA[0]<<":\n";
      }
      if(TREE_CONECTIONS[0].size()>0){
         out_stream <<
"    push rbp\n"
"    mov rbp, rsp\n";
        int arg_count = (x.DATA.size()-1)/2;
        if(arg_count > FUNC_ARG_REGISTER_LIST32.size()){
          cerr << "NOT ALLOWED MORE THAN " << FUNC_ARG_REGISTER_LIST32.size() << "ARGUMENTS FOR A FUNCTION.\n" <<
          "ERROR AT FUNCTION '" << x.DATA[0] << "'.\n";
          assert(false);
        }
        BLOCK_NODE = x;
        BLOCK_NODE_INDEX = y;
        CURRENT_FN = y;
        string var;
        string type;
        int BOOL_COUNTER;
        for(int i = 0; i < arg_count; i++){
          type = x.DATA[(i*2)+1];
          var = x.DATA[(i*2)+2];
          DECLARE_VAR(var, type, y);
          if(type=="UINT"){
            out_stream <<
"    mov "<< get_asm_value(var, PTR_MAPS[y], type) <<", " << FUNC_ARG_REGISTER_LIST32[(arg_count-1)-i]<<";"<<var<<"\n";
          }
          else if(type=="BOOL"){
            if(FUNC_ARG_REGISTER_LIST32[(arg_count-1)-i] == "edx"){
              out_stream <<
"    mov "<< get_asm_value(var, PTR_MAPS[y], type) <<", dl;"<<var<<"\n";
            }
            else if(FUNC_ARG_REGISTER_LIST32[(arg_count-1)-i] == "eax"){
              out_stream <<
"    mov "<< get_asm_value(var, PTR_MAPS[y], type) <<", dl;"<<var<<"\n";
            }
            else{
              out_stream <<
"    mov eax," << FUNC_ARG_REGISTER_LIST32[(arg_count-1)-i] << "\n"
"    mov " << get_asm_value(var, PTR_MAPS[y], type) << ", al;"<<var<<"\n";
            }
          }
          else if(type=="PTR"||type=="STRING"){
            out_stream <<
"    mov " << get_asm_value(var, PTR_MAPS[y], type) << ", " << FUNC_ARG_REGISTER_LIST64[(arg_count-1)-i]<<";"<<var<<"\n";
          }
        }
        FN_ITER_MAX = TREE_CONECTIONS[y].size();
        for(int i = 0; i < TREE_CONECTIONS[y].size(); i++){
          int z = TREE_CONECTIONS[y][i];
          FN_ITER = i;
          process_node(TREE[z],z);
        }
        CURRENT_FN = 0;
        out_stream <<
"    pop rbp\n";
        if(x.DATA[0]=="MAIN"){
          out_stream <<
"    mov rax, 60\n"
"    mov rdi, 0\n"
"    syscall\n";
        }
        else{
          out_stream << "    ret\n";
        }
      }else{
        cerr << "Um... there is no code in the function."<<x.DATA[0]<<"\n";
        assert(false);
      }
    }
    
    void var_node(NODE &x, int y){
      string name = x.DATA[0];
      string type = x.DATA[1];
      string value = x.DATA[2];
      string type2 = literal_or_var(x.DATA[2]);
      if(type != "STRING"){
        if(type2=="ID"){
          type2 = TYPE_MAPS[CURRENT_FN][value];
          out_stream << "    mov eax, " << get_asm_value(value,PTR_MAPS[CURRENT_FN],type2) << "\n" <<
          "    mov " << get_asm_value(name,PTR_MAPS[CURRENT_FN],type) << ", eax\n";
        }
        else{
          out_stream << "    mov " << get_asm_value(name,PTR_MAPS[CURRENT_FN],type) << ", " << get_asm_value(value,PTR_MAPS[CURRENT_FN],type2) << "\n";
        }
      }
      else{
        x.DATA.push_back(to_string(CURRENT_FN));
        STRING_DEFS.push_back(x);
      }
    }
    
    void var_expr_node(NODE &x, int y){
      string name = x.DATA[0];
      int z = TREE_CONECTIONS[y][0];
      string in_reg = process_node(TREE[z],z);
      string type = x.DATA[1];
      out_stream << "    mov " << get_asm_value(name,PTR_MAPS[CURRENT_FN],type) << ", " << in_reg <<";"<<name<<"\n";
    }
    
    string code_node(NODE &x, int y){
      string syntax_name = x.DATA[0];
      vector<string> tokens;
      for(int i = 1; i < x.DATA.size();i++){
        tokens.push_back(x.DATA[i]);
      }
      SYNTAX syntax = SYNTAX_MAP[syntax_name];
      vector<string> code = processs_syntax(syntax, tokens, BLOCK_NODE_INDEX);
      for(string &s : code){
        out_stream << s;
      }
      return syntax.OUT_REG;
    }
    
    string fn_call_node(NODE &x, int y){
      //PUSH ARGS TO STACK THEN DO CALL OPERATOR
      string label = x.DATA[0];
      if(label == "WRITE"){
        string type = literal_or_var(x.DATA[1]);
        if(type=="ID"){
          type = TYPE_MAPS[CURRENT_FN][x.DATA[1]];
        }
        label+="_"+type;
        if(type == "BOOL" || type == "CHAR"){
          out_stream <<
    "    mov al, " << get_asm_value(x.DATA[1], PTR_MAPS[CURRENT_FN], type) << "\n" <<
    "    call " << label << "\n";
        }
        else if(type == "PTR"){
          out_stream <<
    "    mov rsi, " << get_asm_value(x.DATA[1], PTR_MAPS[CURRENT_FN], type) << "\n";
        }
        else if(type == "STRING"){
          string ID = get_asm_value(x.DATA[1], PTR_MAPS[CURRENT_FN], type);
          out_stream <<
    "    mov rsi, " << ID << "\n"
    "    mov edx, " << ID << "_LEN\n"
    "    call " << label << "\n";
        }
        else{
          out_stream <<
    "    mov " << FUNC_ARG_REGISTER_LIST32[0]<<", " << get_asm_value(x.DATA[1], PTR_MAPS[CURRENT_FN], type) << "\n"
    "    call " << label << "\n";
        }
      }
      else if(label == "PNL"){
        out_stream <<"    call WRITE_NEW_LINE\n";
      }
      else if(label == "SYSCALL"){
        for(int i = 1; i < x.DATA.size(); i++){
          string value = x.DATA[i];
          string type = literal_or_var(value);
          bool ID = false;
          if(type=="ID"){
            type = TYPE_MAPS[CURRENT_FN][value];
            ID = true;
          }
          if(type=="UINT"&&ID){
            out_stream <<
            "    movsx " << SYSCALL_ARGS[i-1] << ", "<< get_asm_value(value, PTR_MAPS[CURRENT_FN], type) << "\n";
          }
          else if(type=="PTR"||!ID||type=="STRING"){
            out_stream <<
            "    mov " << SYSCALL_ARGS[i-1] << ", "<< get_asm_value(value, PTR_MAPS[CURRENT_FN], type) << "\n";
          }
        }
        out_stream <<
        "    syscall\n";
      }
      else if(label == "GET_PTR"){
        //GET PTR TO VAR
        //AND IMPLEMENT VAR_DEC ACCEPTING FUNC_CALLS
        if(PTR_MAPS[CURRENT_FN][x.DATA[1]]<=0){
          out_stream <<
          "    lea rax, [rbp+" << PTR_MAPS[CURRENT_FN][x.DATA[1]] << "]\n";
        }
        else{
          out_stream <<
          "    mov rax, [__STRING"<<PTR_MAPS[CURRENT_FN][x.DATA[1]]<<"]\n";
        }
        return "rax";
      }
      else{
        for(int i = 1; i < x.DATA.size(); i++){
          string value = x.DATA[i];
          string type = literal_or_var(value);
          if(type=="ID"){
            type = TYPE_MAPS[CURRENT_FN][value];
          }
          if(DEBUG_MODE){
            for(auto t : PTR_MAPS[CURRENT_FN]){
              cout << "{" << t.first << ", " << t.second << "}\n";
            }
          }
          if(type == "UINT"){
          out_stream <<
  "    mov " << FUNC_ARG_REGISTER_LIST32[i-1]<<", " << get_asm_value(value, PTR_MAPS[CURRENT_FN], type) << "\n";
          }
          else if(type == "PTR" || type == "STRING"){
            out_stream <<
            "    mov " << FUNC_ARG_REGISTER_LIST64[i-1]<<", " << get_asm_value(value, PTR_MAPS[CURRENT_FN], type) << "\n";
          }
        }
        out_stream <<
  "    call " << label << "\n";
      }
      return "NA";
    }
    
    void COMP_JMP(NODE &x, string &value, string &type, string &jump){
      if(value == "!"){
        value = x.DATA[1];
        type = literal_or_var(value);
        if(type=="ID"){
          type = TYPE_MAPS[CURRENT_FN][value];
          out_stream <<
    "    cmp " << get_asm_value(value, PTR_MAPS[CURRENT_FN], type) <<", 1\n";
          jump = "je";
        }
        else{
          if(value == "TRUE"){
            jump = "";
          }
          else{
            jump = "jmp";
          }
        }
        if(type!="BOOL"){
          cerr << "PUT NOT GATE (!) IN FRONT A NON-BOOLEAN IN AN IF STATEMENT, INSTEAD WAS PUT INFRONT OF '"
          << value << "'.\n";
          assert(false);
        }
      }
      else{
        type = literal_or_var(value);
        if(type=="ID"){
          type = TYPE_MAPS[CURRENT_FN][value];
        }
        if(type=="UINT"){
          out_stream <<
    "    mov eax, " << get_asm_value(value, PTR_MAPS[CURRENT_FN], type) << "\n" <<
    "    cmp eax, " << get_asm_value(x.DATA[2], PTR_MAPS[CURRENT_FN], type) << "\n";
          string comparator = x.DATA[1];
          if(comparator == "LESS") jump = "jge";
          else if(comparator == "GREATER") jump = "jle";
          else if(comparator == "EQL") jump = "jne";
          else if(comparator == "NEQL") jump = "je";
          else if(comparator == "LESS_EQL") jump = "jg";
          else if(comparator == "GREATER_EQL") jump = "jl";
        }
        else if(type=="BOOL"){
          if(value=="TRUE"){
            jump = "";
          }
          else if(value=="FALSE"){
            jump = "jmp";
          }
          else{
          out_stream <<
    "    cmp " << get_asm_value(value, PTR_MAPS[CURRENT_FN], type) <<", 0\n";
            jump = "je";
          }
        }
      }
    }
    
    void if_node(NODE &x, int y){
      ESC_L=0;
      L_COUNTER++;
      ss << ".L" << L_COUNTER;
      string label = ss.str();
      ss.str("");
      string value = x.DATA[0];
      string jump;
      string type;
      COMP_JMP(x, value, type, jump);
      if(jump!=""){
        out_stream <<
  "    " << jump << " " << label <<"\n";
      }
      for(int z: TREE_CONECTIONS[y]) process_node(TREE[z],z);
      if(FN_ITER_MAX - FN_ITER != 1){
        int NEXT_BLOCK = TREE_CONECTIONS[CURRENT_FN][FN_ITER+1];
        if(TREE[NEXT_BLOCK].TYPE=="ELSE")else_node(TREE[NEXT_BLOCK], NEXT_BLOCK);
        else if(TREE[NEXT_BLOCK].TYPE=="ELIF")elif_node(TREE[NEXT_BLOCK], NEXT_BLOCK);
      }
      else if(jump!="")out_stream << label << ":\n";
    }
    
    void else_node(NODE &x, int y){
      L_COUNTER++;
      if(ESC_L==0){
        out_stream << "    jmp .L" << L_COUNTER << "\n"
        ".L" << L_COUNTER - 1 << ":\n";
        for(int z: TREE_CONECTIONS[y]) process_node(TREE[z],z);
        out_stream << ".L" << L_COUNTER << ":\n";
      }
      else{
        out_stream << "    jmp .L" << ESC_L << "\n"
        ".L" << L_COUNTER - 1 << ":\n";
        for(int z: TREE_CONECTIONS[y]) process_node(TREE[z],z);
        out_stream << ".L" << ESC_L << ":\n";
      }
    }
    
    void loop_node(NODE &x, int y){
      L_COUNTER+=2;
      out_stream <<
"    jmp .L" << L_COUNTER - 1 << "\n"
".L" << L_COUNTER << ":\n";
      string value = x.DATA[0];
      string jump;
      string type;
      int RES = L_COUNTER;
      for(int z: TREE_CONECTIONS[y]) process_node(TREE[z],z);
      out_stream << ".L" << RES - 1 << ":\n";
      COMP_JMP(x, value, type, jump);
      jump = INV_JUMP[jump];
      if(jump!=""){
        out_stream <<
  "    " << jump << " .L" << RES << "\n";
      }
      if(x.TYPE!="LOOP"){
        out_stream <<
    ".L" << x.TYPE << ":\n";
      }
    }
    
    void elif_node(NODE &x, int y){
      L_COUNTER++;
      string value = x.DATA[0];
      string jump;
      string type;
      int NEXT_BLOCK=-1;
      NODE w;
      if(FN_ITER_MAX - FN_ITER != 1){
        NEXT_BLOCK = TREE_CONECTIONS[CURRENT_FN][++FN_ITER];
        w = TREE[NEXT_BLOCK];
      }
      if(ESC_L==0)ESC_L = L_COUNTER;
      out_stream <<
  "    jmp .L" << ESC_L <<"\n";
      out_stream << ".L" << L_COUNTER - 1 << ":\n";
      COMP_JMP(x, value, type, jump);
      if(jump != ""){
        if(ESC_L==L_COUNTER) out_stream << "    " << jump << " .L" << L_COUNTER+1 << "\n";
        else if(w.TYPE!="ELIF" && w.TYPE!="ELSE") out_stream << "    " << jump << " .L" << ESC_L << "\n";
        else out_stream << "    " << jump << " .L" << L_COUNTER << "\n";
      }
      if(ESC_L == L_COUNTER)L_COUNTER++;
      for(int z: TREE_CONECTIONS[y]) process_node(TREE[z],z);
      if(NEXT_BLOCK != -1){
        if(w.TYPE=="ELSE")else_node(w, NEXT_BLOCK);
        else if(w.TYPE=="ELIF")elif_node(w, NEXT_BLOCK);
        else out_stream << ".L" << ESC_L << ":\n";
      }
      else out_stream << ".L" << ESC_L << ":\n";
    }
    
    void break_node(NODE &x, int y){
      int loop = stoi(x.DATA[0]);
      if(TREE[loop].TYPE!="LOOP"){
        string esc_L = TREE[loop].TYPE;
        out_stream << "    jmp .L" << esc_L << "\n";
      }
      else{
        TREE[loop].TYPE=to_string(++L_COUNTER);
        out_stream << "    jmp .L" << L_COUNTER << "\n";
      }
    }
    
    string process_node(NODE &x, int y){
      if(x.TYPE=="FUNC"){
        func_node(x,y);
      }
      else if(x.TYPE=="VAR_DEC"){
        var_node(x,y);
      }
      else if(x.TYPE=="VAR_EXPR"){
        var_expr_node(x,y);
      }
      else if(x.TYPE=="IF"){
        if_node(x,y);
      }
      else if(x.TYPE=="CODE"){
        return code_node(x,y);
      }
      else if(x.TYPE=="FN_CALL"){
        return fn_call_node(x,y);
      }
      else if(x.TYPE=="LOOP"){
        loop_node(x,y);
      }
      else if(x.TYPE=="BREAK"){
        break_node(x,y);
      }
      return "NA";
    }
    
    void parse_tree(){
      if(TREE_CONECTIONS[0].size()>0){
        for(int y: TREE_CONECTIONS[0]){
          process_node(TREE[y],y);
        }
      }else{
        cerr << "Did you not even write a program...?\nLAZY ASS!!!\n";
        assert(false);
      }
    }
    
    void _exit(){
      out_stream <<
      "\n\nSECTION .data\n";
      for(NODE &x : STRING_DEFS){
        int ID = PTR_MAPS[stoi(x.DATA[x.DATA.size()-1])][x.DATA[0]];
        out_stream <<
        "__STRING" << ID << " db " << x.DATA[2] << "\n"
        "__STRING"<< ID << "_LEN equ $ - __STRING" << ID << "\n";
      }
    }
    
};

int main(int argc, char** argv){
  assert(argc > 0);
  cout << "\n";
  start_timer();
  int length = 0;
  while(argv[1][++length]);
  int file_name_size = length / sizeof(char);
  string basename;
  string argv1 = convertToString(argv[1], file_name_size);
  string directory(argv[2]);
  char debug_flag = argv[3][0];
  if(debug_flag == '1'){
    DEBUG_MODE = true;
  }else{
    DEBUG_MODE = false;
  }
  
  if(argv1.find(".adpl")){
    argv1.erase(argv1.length()-5);
    basename = argv1;
  }else{
    basename = argv1;
  }
  
  //INIT OF ADPL AND ASM FILES
  ss << basename << ".adpl";
  string adpl_file = directory+'/'+ss.str();
  ss.str("");
  if(DEBUG_MODE) cout << "ADPL FILE:\n" << adpl_file << "\n";
  
  ss << basename << ".asm";
  string asm_file = directory+'/'+ss.str();
  ss.str("");
  if(DEBUG_MODE) cout << "\nASM FILE:\n" << asm_file << "\n";
  
  //COMPILATION OF ADPL
  INIT_TABLE();
  if(DEBUG_MODE){
    cout << "\nTOKEN TABLE:\n";
    for(auto const &pair: tk_table){
      cout << "{" <<pair.first<<":"<<pair.second<<"}\n";
    }
  }
  
  stop_timer("START_TIME");
  start_timer();
  
  if(DEBUG_MODE) cout << "\nTOKEN STREAM:\n";
  vector<string> token_stream = LEXER(adpl_file.c_str());
  if(DEBUG_MODE){
    for(string tk : token_stream){
      cout<<tk<<" ";
      if(tk=="NL"){
        cout<<"\n";
      }
    }
  }
  start_timer();
  
  INIT_SYNTAXES();
  
  stop_timer("INIT_SYNTAXES");
  start_timer();
  //if(DEBUG_MODE) cout << "\nPARSE TREE STATE DEBUG:\n";
  PARSER parser(token_stream);
  parser.run();
  if(DEBUG_MODE){
    cout << "\nPARSE TREE:\n";
    int i = 0;
    while(true){
      if(i >= NODE_INDEX)break;
      if(TREE[i].TYPE=="PROGRAM"){
        print(TREE[i],i);
      }
      i++;
    }
    cout << "\nPARSE TREE CONNECTIONS:\n";
    i = 0;
    while(true){
      if(i >= NODE_INDEX)break;
      for (int x : TREE_CONECTIONS[i]){
        cout << x <<" ";
      }
      if(TREE_CONECTIONS[i].size()==0){
        cout << "-";
      }
      cout << "    " << TREE[i].TYPE << "\n";
      i++;
    }
    
    cout << "\nINVERSE PARSE TREE CONNECTIONS:\n";
    i = 0;
    while(true){
      if(i >= NODE_INDEX)break;
      cout << INV_TREE_CONNECTIONS[i];
      cout << "    " << TREE[i].TYPE << "\n";
      i++;
    }
  }
  stop_timer("PARSE");
  
  INIT_FUNC_ARG_REGISTER_LIST();
  
  start_timer();
  COMPILER compiler(asm_file);
  compiler.run();
  stop_timer("COMPILER");
  
  start_timer();
  
  //COMPILATION OF ASM CODE
  ss << "nasm -f elf64 -o "<< basename << ".o " << asm_file;
  string cmd_line = ss.str();
  const char* char_line = cmd_line.c_str();
  ss.str("");
  system(char_line);
  
  //ss << "ld " << basename << ".o";
  ss << "ld -o " << basename << " " << basename << ".o";
  cmd_line = ss.str();
  char_line = cmd_line.c_str();
  ss.str("");
  system(char_line);
  
  stop_timer("\nCOMPILATION OF ASM");
  start_timer();
  
  cout << "\nPROGRAM OUTPUT:\n";
  
  //ss << "./a.out";
  ss << "./" << basename;
  cmd_line = ss.str();
  char_line = cmd_line.c_str();
  ss.str("");
  system(char_line);
  
  stop_timer("RUN");
  
  cout << "\n";
  
  start_timer();
  
  //DELETES .O FILE
  if(DEBUG_MODE) cout << "DELETING "<< basename << ".o...\n";
  ss << "shred -u " << basename << ".o";
  cmd_line = ss.str();
  char_line = cmd_line.c_str();
  ss.str("");
  system(char_line);
  if(DEBUG_MODE) cout << "DONE!\n";
  
  // //DELETES .ASM FILE
  // if(DEBUG_MODE) cout << "DELETING "<< asm_file << "...\n";
  // ss << "shred -u " << asm_file;
  // cmd_line = ss.str();
  // char_line = cmd_line.c_str();
  // ss.str("");
  // system(char_line);
  // if(DEBUG_MODE) cout << "DONE!\n";
  
  stop_timer("DELETION");
  
  return 0;
}