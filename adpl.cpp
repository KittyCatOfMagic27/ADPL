#include <iostream>
#include <cstdlib>
#include <fstream>
#include <cstring>
#include <vector>
#include <cassert>
#include <sstream>
#include <unordered_map>
#include <chrono>
#include <utility>
#include <algorithm>
#include "./flags.h"
#include <unistd.h>
#include <limits.h>

using namespace std;

/*
  HOW TO USE:
  usr@penguin:~$adpl --help
*/

unordered_map<string, unordered_map<string, string>> VAR_SIZE_MAPS;
unordered_map<string, unordered_map<string, int>> PTR_MAPS;
unordered_map<string, vector<vector<string>>> OPS;
unordered_map<int, vector<string>> WORD_TABLE;
unordered_map<string, int> CALL_AMOUNT_MAPS;
unordered_map<string, int> PTR_AMOUNT_MAPS;
unordered_map<string, string> DATA_TYPES;
unordered_map<string, int> FN_ARG_COUNT;
vector<string> FUNC_ARG_REGISTER_LIST64;
vector<string> FUNC_ARG_REGISTER_LIST32;
vector<string> FUNC_ARG_REGISTER_LIST8;
unordered_map<string, string> VAR_TYPE;
vector<pair<string, string>> TK_STREAM;
unordered_map<string, string> COMP_JMP;
unordered_map<string, string> INV_JMP;
unordered_map<string, string> RET_FNS;
unordered_map<string, string> RET_REG;
unordered_map<string, bool> SYM_TABLE;
unordered_map<string, int> TYPE_BITS;
unordered_map<string, string> DEFINE;
unordered_map<string, int> BYTE_MAP;
vector<pair<string, string>> DATA;
vector<string> SYSCALL_ARGS;
vector<string> LINKED_FILES;
vector<string> CONDITIONALS;
vector<string> NATIVE_FNS;
vector<string> GLOBAL_FNS;
vector<string> EXTERN_FNS;
vector<string> FN_LIST;
vector<string> BSS;
vector<int> ELSES;

string FILE_BASENAME;
string EXEC_FILE;
bool DEBUG_MODE = false;
bool RUN = false;

stringstream ss;
string WHITESPACE = " \n\r\t\f\v";
int LOOP_COUNT = 0;
int IF_COUNT = 0;
int _START_CALL_AMOUNT = 0;
int PTR_AMOUNT = 0;
int DEBUGGER_COUNT = 0;
auto START_TIME = chrono::high_resolution_clock::now();
auto STOP_TIME = chrono::high_resolution_clock::now();
auto DURATION_TIME = chrono::duration_cast<chrono::microseconds>(STOP_TIME - START_TIME);

void breakpoint(int x = DEBUGGER_COUNT++){
  cout << x << "\n";
}

void INIT_TABLE(){
  TYPE_BITS["UINT"]=4;
  TYPE_BITS["PTR"]=8;
  TYPE_BITS["BOOL"]=1;
  TYPE_BITS["CHAR"]=1;
  
  
  //BYTES
  WORD_TABLE[1].push_back("byte");
  
  WORD_TABLE[2].push_back("word");
  
  WORD_TABLE[3].push_back("byte");
  WORD_TABLE[3].push_back("word");
  
  WORD_TABLE[4].push_back("dword");
  
  WORD_TABLE[5].push_back("byte");
  WORD_TABLE[5].push_back("dword");
  
  WORD_TABLE[6].push_back("dword");
  WORD_TABLE[6].push_back("word");
  
  WORD_TABLE[7].push_back("dword");
  WORD_TABLE[7].push_back("word");
  WORD_TABLE[7].push_back("byte");
  
  WORD_TABLE[8].push_back("qword");
  
  
  SYM_TABLE["+"]=true;
  SYM_TABLE["-"]=true;
  SYM_TABLE["*"]=true;
  SYM_TABLE["/"]=true;
  SYM_TABLE["("]=true;
  SYM_TABLE[")"]=true;
  SYM_TABLE["%"]=true;
  SYM_TABLE["<<"]=true;
  SYM_TABLE["=="]=true;
  SYM_TABLE["!="]=true;
  SYM_TABLE["<="]=true;
  SYM_TABLE[">="]=true;
  SYM_TABLE[">"]=true;
  SYM_TABLE["<"]=true;
  SYM_TABLE["!"]=true;
  SYM_TABLE["--"]=true;
  SYM_TABLE["++"]=true;
  // SYM_TABLE["+="]=true;
  SYM_TABLE["@"]=true;
  SYM_TABLE[";;"]=true;
  SYM_TABLE["["]=true;
  SYM_TABLE["]"]=true;
  SYM_TABLE["&&"]=true;
  SYM_TABLE["||"]=true;
  SYM_TABLE["-|"]=true;
  
  NATIVE_FNS.push_back("write"); // VALUE   (if string PTR/CONST LENGTH)
  NATIVE_FNS.push_back("writeln"); // VALUE   (if string PTR/CONST LENGTH)
  NATIVE_FNS.push_back("syscall"); //
  NATIVE_FNS.push_back("ptr"); // ID
  RET_FNS["ptr"]="PTR";
  RET_REG["ptr"]="rax";
  NATIVE_FNS.push_back("len"); // CONST STRING (DEPRECATED?)
  RET_FNS["len"]="UINT";
  RET_REG["len"]="eax";
  NATIVE_FNS.push_back("new_line"); // NA
  NATIVE_FNS.push_back("array"); // TYPE AMOUNT
  RET_FNS["array"]="PTR";
  RET_REG["array"]="rax";
  NATIVE_FNS.push_back("alloc"); // BYTES
  RET_FNS["alloc"]="PTR";
  RET_REG["alloc"]="rax";
  NATIVE_FNS.push_back("addr_sub"); // PTR BYTES
  RET_FNS["addr_sub"]="PTR";
  RET_REG["addr_sub"]="rax";
  NATIVE_FNS.push_back("addr_add"); // PTR BYTES
  RET_FNS["addr_add"]="PTR";
  RET_REG["addr_add"]="rax";
  NATIVE_FNS.push_back("addr_get"); // TYPE PTR
  RET_FNS["addr_get"]="$1";
  RET_REG["addr_get"]="a";
  NATIVE_FNS.push_back("addr_equ"); // TYPE PTR VALUE
  NATIVE_FNS.push_back("sleep"); // SECONDS NANOSECONDS
  NATIVE_FNS.push_back("exit"); // EXITCODE
  NATIVE_FNS.push_back("error"); // STRING STRING.LEN EXITCODE
  NATIVE_FNS.push_back("cast"); // TYPE ID
  RET_FNS["cast"]="$1";
  RET_REG["cast"]="a";
  NATIVE_FNS.push_back("read_bits"); // VALUE BUFFER
  NATIVE_FNS.push_back("open_fd"); // FILENAME(STRING) FLAGS(UINT) MODE(UINT)
  RET_FNS["open_fd"]="UINT"; // FD(UINT)
  RET_REG["open_fd"]="eax";
  NATIVE_FNS.push_back("close_fd"); // FD(UINT)
  NATIVE_FNS.push_back("read"); // FD(UINT) PTR(BUFFER) UINT(LEN)
  NATIVE_FNS.push_back("fstats"); // FD(UINT) PTR(BUFFER[144])
  
  // SYMBOL              OUT     IN      IN    ...
  OPS["++"].push_back({"UINT", "UINT"});
  OPS["++"].push_back({"PTR", "PTR"});
  OPS["--"].push_back({"UINT", "UINT"});
  OPS["--"].push_back({"PTR", "PTR"});
  
  OPS["+"].push_back({"UINT", "UINT", "UINT"});
  OPS["+"].push_back({"PTR", "PTR", "UINT"});
  OPS["+"].push_back({"PTR", "UINT", "PTR"});
  OPS["-"].push_back({"UINT", "UINT", "UINT"});
  OPS["-"].push_back({"PTR", "PTR", "UINT"});
  
  OPS["*"].push_back({"UINT", "UINT", "UINT"});
  OPS["/"].push_back({"UINT", "UINT", "UINT"});
  OPS["%"].push_back({"UINT", "UINT", "UINT"});
  OPS[">"].push_back({"BOOL", "UINT", "UINT"});
  OPS["<"].push_back({"BOOL", "UINT", "UINT"});
  OPS[">="].push_back({"BOOL", "UINT", "UINT"});
  OPS["<="].push_back({"BOOL", "UINT", "UINT"});
  
  OPS["=="].push_back({"BOOL", "UINT", "UINT"});
  OPS["!="].push_back({"BOOL", "UINT", "UINT"});
  OPS["=="].push_back({"BOOL", "BOOL", "BOOL"});
  OPS["!="].push_back({"BOOL", "BOOL", "BOOL"});
  OPS["=="].push_back({"BOOL", "CHAR", "CHAR"});
  OPS["!="].push_back({"BOOL", "CHAR", "CHAR"});
  
  OPS["!"].push_back({"BOOL", "BOOL"});
  OPS["&&"].push_back({"BOOL", "BOOL", "BOOL"});
  OPS["||"].push_back({"BOOL", "BOOL", "BOOL"});
  OPS["-|"].push_back({"BOOL", "BOOL", "BOOL"}); //XOR BECAUSE THERE IS NO XOR STANDARD OTHER THAT ^ WHICH IS FOR EXPONENTS AS WELL
  
  COMP_JMP[">"]="jg";
  COMP_JMP[">="]="jge";
  COMP_JMP["<"]="jl";
  COMP_JMP["<="]="jle";
  COMP_JMP["!="]="jne";
  COMP_JMP["=="]="je";
  
  INV_JMP["je"]="jne";
  INV_JMP["jne"]="je";
  INV_JMP["jg"]="jle";
  INV_JMP["jl"]="jge";
  INV_JMP["jge"]="jl";
  INV_JMP["jle"]="jg";
  INV_JMP[""]="jmp";
  INV_JMP["jmp"]="";
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
  FUNC_ARG_REGISTER_LIST8.push_back("cl");
  
  SYSCALL_ARGS.push_back("rax");
  SYSCALL_ARGS.push_back("rdi");
  SYSCALL_ARGS.push_back("rsi");
  SYSCALL_ARGS.push_back("rdx");
  SYSCALL_ARGS.push_back("r10");
  SYSCALL_ARGS.push_back("r8");
  SYSCALL_ARGS.push_back("r9");
  
  INV_JMP["je"]="jne";
  INV_JMP["jne"]="je";
  INV_JMP["jg"]="jle";
  INV_JMP["jl"]="jge";
  INV_JMP["jge"]="jl";
  INV_JMP["jle"]="jg";
  INV_JMP[""]="jmp";
  INV_JMP["jmp"]="";
}

void start_timer(){
  START_TIME = chrono::high_resolution_clock::now();
}

void stop_timer(string timer_name){
  STOP_TIME = chrono::high_resolution_clock::now();
  DURATION_TIME = chrono::duration_cast<chrono::microseconds>(STOP_TIME - START_TIME);
  float to_seconds = static_cast<float>(DURATION_TIME.count())/1000000;
  cout << timer_name <<" TIME: " << to_seconds << " seconds\n";
}

bool isNumber(const string& str){
  for (char const &c : str) {
    if (isdigit(c) == 0) return false;
  }
  return true;
}

string literal_or_var(string &val){
  if(val=="true" || val=="false")return "BOOL";
  if(isNumber(val))return "UINT";
  if(val[0]=='\''&&val[val.size()-1]=='\'')
    if(val.size()<=3) return "CHAR";
    else return "STRING";
  return "ID";
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

string asm_var(int ptr, int size){
  string word = WORD_TABLE[size][0];
  ss << word << " [rbp-" << ptr << "]";
  string s = ss.str();
  ss.str("");
  return s;
}

string asm_get_value(string &value, unordered_map<string, int> &ptr_table){
  string type = literal_or_var(value);
  if(type == "ID"){
    int ptr = ptr_table[value];
    type = VAR_TYPE[value];
    if(type == "UINT"){
      ss << "dword [rbp-" << ptr << "]";
    }
    else if(type == "BOOL" || type == "CHAR"){
      ss << "byte [rbp-" << ptr << "]";
    }
    else if(type == "PTR"){
      ss << "qword [rbp-" << ptr << "]";
    }
    else if(type == "STRING"){
      cerr << "\033[1;31m[TODO!] STRING GET VAL NOT IMPLEMENTED YET.\033[0m\n";
      assert(false);
    }
    else if(DATA_TYPES.find(value)!=DATA_TYPES.end()){
      type = DATA_TYPES[value];
      if(type == "STRING" || type == "ARRAY"){
        ss << "[" << value << "]";
      }
      else if(type == "UINT"){
        ss << value ;
      }
      else{
        ss << value ;
      }
    }
    else{
      cerr << "\033[1;31mUNRECOGNIZED VARIABLE '"<<value<<"'.\033[0m\n";
      assert(false);
    }
    string output = ss.str();
    ss.str("");
    return output;
  }
  else{
    if(value == "true"){
      return "1";
    }
    else if(value == "false"){
      return "0";
    }
    return value;
  }
}

void process_line(string &line){
  vector<string> ops = split(line);
  if(DEBUG_MODE){
    for(string op: ops){
      cout << op << " ";
    }
    cout << "\n";
  }
  vector<int> IF_STACK;
  for(int i = 0; i < ops.size(); i++){
    string op = ops[i];
    pair<string,string> result;
    if(op=="let"){
      result.first=op;
      result.second=ops[++i];
      TK_STREAM.push_back(result);
    }
    else if(op=="const"){
      string var = ops[++i];
      string value = ops[++i];
      pair<string, string> buff;
      if(var!="["){
        buff.first = var;
        buff.second = value;
        DATA.push_back(buff);
        DATA_TYPES[var]=literal_or_var(value);
        if(DATA_TYPES[var]=="STRING"){
          string length = var + ".len";
          buff.first = length;
          buff.second = " equ $ - " + var;
          DATA.push_back(buff);
          DATA_TYPES[length] = "UINT";
        }
      }
      else{
        string amount = ops[i++];
        var = ops[++i];
        if(ops[i+1]=="["){
          cerr << "\033[1;31mVALUE: "<< ops[i+1] << "\n"
          "[TODO!] ASSIGNING VALS TO ARRAYS NOT IMPLEMENTED YET.\033[0m\n";
          assert(false);
        }
        BSS.push_back(var + " resb " + amount + "\n");
        DATA_TYPES[var] = "ARRAY";
      }
    }
    else if(op=="fn"){
      result.first=op;
      result.second=ops[++i];
      TK_STREAM.push_back(result);
      FN_LIST.push_back(ops[i]);
    }
    else if(op=="if"){
      result.first=op;
      result.second=to_string(++IF_COUNT);
      IF_STACK.push_back(IF_COUNT);
      TK_STREAM.push_back(result);
    }
    else if(op=="else"){
      result.first=op;
      TK_STREAM.push_back(result);
    }
    else if(op=="loop"){
      result.first=op;
      result.second=to_string(++LOOP_COUNT);
      TK_STREAM.push_back(result);
    }
    else if(OPS.find(op) != OPS.end()){
      TK_STREAM.pop_back();
      result.first=op;
      TK_STREAM.push_back(result);
      result.first=ops[i-1];
      result.second="";
      TK_STREAM.push_back(result);
    }
    else if(TYPE_BITS.find(op) != TYPE_BITS.end()){
      result.first=op;
      result.second=ops[++i];
      TK_STREAM.push_back(result);
    }
    else if(DEFINE.find(op) != DEFINE.end()){
      result.first=DEFINE[op];
      TK_STREAM.push_back(result);
    }
    else if(op==";;"){
      if(ops[++i]=="include"){
        string fd = ops[++i];
        if(fd[0]!='\''){
          cerr << "\033[1;31mPUT THE FILE DIRECTORY AS A STRING.\033[0m\n";
          assert(false);
        }
        string trimmed;
        for(int j = 1; j < fd.size()-1; j++){
          trimmed += fd[j];
        }
        LINKED_FILES.push_back(trimmed);
      }
      else if(ops[i]=="global"){
        GLOBAL_FNS.push_back(ops[++i]);
      }
      else if(ops[i]=="extern"){
        EXTERN_FNS.push_back(ops[++i]);
        FN_LIST.push_back(ops[i]);
      }
      else if(ops[i]=="define"){
        string var = ops[++i];
        string value = ops[++i];
        DEFINE[var] = value;
      }
      else{
        cerr << "\033[1;31mCOMMAND "<< ops[i] <<" DOESN'T EXIST!\033[0m\n";
      }
    }
    else if(op=="@"){
      result.first=op;
      result.second=ops[++i];
      TK_STREAM.push_back(result);
    }
    else if(op!=""){
      result.first=op;
      TK_STREAM.push_back(result);
    }
  }
}

void LEXER(const char* fd){
  string line;
  ifstream source_code;
  source_code.open(fd);
  if(source_code.is_open()){
    while(getline(source_code,line)){
      if(line!="")process_line(line);
    }
    source_code.close();
  }
  else {
    cerr << "Unable to open file.\n";
    cerr << fd << "\n";
    assert(false);
  }
}

void TYPE_CHECK(pair<string, string>& var){
  string type = literal_or_var(var.second);
  if(type == "ID"){
    if(VAR_TYPE.find(var.second) != VAR_TYPE.end()){
      VAR_TYPE[var.first] = VAR_TYPE[var.second];
    }
    else if(RET_FNS.find(var.second) != RET_FNS.end()){
      VAR_TYPE[var.first] = RET_FNS[var.second];
    }
    else if(DATA_TYPES.find(var.second) != DATA_TYPES.end()){
      VAR_TYPE[var.first] = DATA_TYPES[var.second];
    }
    else if(TYPE_BITS.find(var.second) != TYPE_BITS.end()){
      VAR_TYPE[var.first] = var.second;
    }
    else{
      cerr << "\033[1;31m"<<"VARIABLE '"<<var.second<<"' IS UNDEFINED!"<<"\033[0m\n";
      assert(false);
    }
  }
  else{
    VAR_TYPE[var.first] = type;
  }
}

void OP_MATCH(string &sym, int &op_code, int &i, string &type){
  op_code = -1;
  for(int k = 0; k < OPS[sym].size(); k++){
    vector<string> v = OPS[sym][k];
    bool brk = false;
    for(int j = 1; j < v.size(); j++){
      string curv = TK_STREAM[i+j].first;
      string curt = literal_or_var(curv);
      if(curt=="ID"){
        if(VAR_TYPE.find(curv)!=VAR_TYPE.end()) curt = VAR_TYPE[curv];
        else if(DATA_TYPES.find(curv)!=DATA_TYPES.end()) curt = DATA_TYPES[curv];
      }
      if(v[j]!=curt) break;
      if(j == v.size()-1) op_code = k;
      type = v[0];
      brk = true;
    }
    if(brk) break;
  }
}

void CHECK_GRAMMER(){
  vector<string> BLOCK_STACK;
  string CF;
  for(int i = 0; i < TK_STREAM.size(); i++){
    pair<string, string> cur = TK_STREAM[i];
    if(cur.first=="let"){
      string value = TK_STREAM[++i].first;
      pair<string, string> var(cur.second, value);
      if(value=="@"){
        value = TK_STREAM[i].second;
        var.second = value;
      }
      else if(RET_FNS.find(value) != RET_FNS.end()){
        string out_type = RET_FNS[value];
        if(out_type[0]=='$'){
          int arg_n = out_type[1]-'0';
          var.second = TK_STREAM[i+arg_n+1].first;
        }
      }
      else if(OPS.find(value) != OPS.end()){
        int op_code;
        string type;
        OP_MATCH(value, op_code, i, type);
        if(op_code < 0){
          cerr << "\033[1;31m"<<"OPERATION "<< value <<" CANNOT BE MATCHED IN ASSIGNMENT OF '"<< cur.second <<"'!\033[0m\n";
          assert(false);
        }
        var.second = type;
        TK_STREAM[i].second = to_string(op_code);
      }
      TYPE_CHECK(var);
    }
    else if(cur.first=="fn"){
      pair<string, string> fn(cur.second, "");
      BLOCK_STACK.push_back(cur.second);
      CF = cur.second;
      if(CF == "main") CF = "_start";
      pair<string, string> buff = TK_STREAM[i+1];
      int j = 2;
      while(buff.first!=")"){
        buff = TK_STREAM[i+j];
        j++;
      }
      if(TYPE_BITS.find(TK_STREAM[i+j].first) != TYPE_BITS.end()) RET_FNS[CF]=TK_STREAM[i+j].first;
      int size = TYPE_BITS[TK_STREAM[i+j].first];
      string reg;
      if(size == 8) reg = "rax";
      else if(size == 4) reg = "eax";
      else if(size == 1) reg = "al";
      RET_REG[CF] = reg;
    }
    else if(cur.first=="loop"){
      BLOCK_STACK.push_back("loop");
    }
    else if(cur.first=="if"){
      BLOCK_STACK.push_back("if"+cur.second);
    }
    else if(cur.first=="else"){
      string if_label = BLOCK_STACK[BLOCK_STACK.size()-1];
      string _if;
      for(int j = 2; j < if_label.size(); j++){
        _if += if_label[j];
      }
      if(isNumber(_if)){
        BLOCK_STACK.pop_back();
        TK_STREAM[i].second = to_string(ELSES.size());
        BLOCK_STACK.push_back("else"+_if);
        ELSES.push_back(stoi(_if));
      }
      else{
        cerr << "\033[1;31mERROR: ELSE NOT PROCEEDING IF OR ELSE IF!\033[0m\n";
    assert(false);
      }
      string else_label = BLOCK_STACK.at(BLOCK_STACK.size()-2);
      if(else_label.size() > 4){
        if(else_label[0] == 'e' && else_label[1] == 'l' && else_label[2] == 's' && else_label[3] == 'e'){
          string _else;
          for(int j = 4; j < else_label.size(); j++){
            _else += else_label[j];
          }
          int else_id = stoi(_else);
          int replace_with = stoi(_if);
          replace_if(ELSES.begin(), ELSES.end(), [else_id](int &j){
            return j == else_id;
          }, replace_with);
          BLOCK_STACK.pop_back();
          BLOCK_STACK.pop_back();
          BLOCK_STACK.push_back("else"+_if);
        }
      }
    }
    else if(OPS.find(cur.first) != OPS.end()){
      int op_code;
      string type;
      OP_MATCH(cur.first, op_code, i, type);
      if(op_code < 0){
        cerr << "\033[1;31mOPERATION "<< cur.first <<" CANNOT BE MATCHED!\033[0m\n";
        assert(false);
      }
      TK_STREAM[i].second = to_string(op_code);
    }
    else if(cur.first=="end"){
      string block = BLOCK_STACK[BLOCK_STACK.size()-1];
      BLOCK_STACK.pop_back();
      if(block[0]=='i'&&block[1]=='f'){
        block = BLOCK_STACK[BLOCK_STACK.size()-1];
        if(block[0]=='e'&&block[1]=='l'&&block[2]=='s'&&block[3]=='e'){
          BLOCK_STACK.pop_back();
          block = "elif";
        }
        else{
          block = "if";
        }
      }
      else if(block[0]=='e'&&block[1]=='l'&&block[2]=='s'&&block[3]=='e'){
        block = "else";
      }
      TK_STREAM[i].second = block;
    }
  }
  if(BLOCK_STACK.size()>0){
    cout << BLOCK_STACK.size() << "\n";
    cerr << "\033[1;31mERROR: UNCLOSED BLOCK DETECTED!\033[0m\n";
    assert(false);
  }
}

// CRINGE, OUT DATED, AND WHO NEEDS AN ACTUALLY TYPE SAFE LANGUAGE ANYWAY
// void CHECK_GRAMMER_WITH_TYPES(){
//   for(int i = 0; i < TK_STREAM.size(); i++){
//     pair<string, string> cur = TK_STREAM[i];
//     if(cur.first=="+"){
//       string type = literal_or_var(cur.second);
//       if(type == "ID") type = VAR_TYPE[cur.second];
//       if(type != "UINT"){
//         cerr << "\033[1;31mFIRST ARG OF PLUS IS NOT A UINT\033[0m\n";
//         assert(false);
//       }
//       string type2 = literal_or_var(TK_STREAM[++i].first);
//       if(type == "ID") type = VAR_TYPE[TK_STREAM[i].first];
//       if(type != "UINT"){
//         cerr << "\033[1;31mFIRST ARG OF PLUS IS NOT A UINT\033[0m\n";
//         assert(false);
//       }
//     }
//   }
// }

void PRESCAN(vector<string> &INTER){
  string CF = "global";
  for(int i = 0; i < INTER.size(); i++){
    if(INTER[i]=="Label:"){
      string block = INTER[++i];
      if(block!="if"&&block!="else"&&block!="loop") CF = block;
    }
    else if(INTER[i]=="call"){
      CALL_AMOUNT_MAPS[CF]++;
    }
  }
}

void INTER_ASSIGN_FROM_ADDR(string &var, string size, string &ptr, vector<string> &INTER, int &i){
  i+=2;
  INTER.push_back(var);
  INTER.push_back("<--");
  INTER.push_back(";");
  INTER.push_back("addr_get");
  INTER.push_back(size);
  INTER.push_back(ptr);
  INTER.push_back(";");
}

void INTER_OP(string &sym, int op_code, string &var, vector<string> &INTER, int &i){
  i++;
  int arg_length = OPS[sym][op_code].size()-1;
  INTER.push_back(var);
  INTER.push_back("=");
  INTER.push_back(TK_STREAM[i].first);
  INTER.push_back(TK_STREAM[i].second);
  for(int j = 0; j < arg_length; j++){
    INTER.push_back(TK_STREAM[++i].first);
  }
  INTER.push_back(";");
}

vector<string> INTERMEDIATE(){
  vector<string> INTER;
  string CF;
  for(int i = 0; i < TK_STREAM.size(); i++){
    pair<string, string> cur = TK_STREAM[i];
    if(cur.first=="let"){
      string type = VAR_TYPE[cur.second];
      string value = TK_STREAM[i+1].first;
      INTER.push_back("def");
      INTER.push_back(cur.second);
      
      if(type == "UINT"){
        INTER.push_back("4");
        if(OPS.find(value) != OPS.end()){
          INTER.push_back(";");
          INTER_OP(value, stoi(TK_STREAM[i+1].second), cur.second, INTER, i);
        }
        else if(RET_FNS.find(value) != RET_FNS.end()){
          INTER.push_back(";");
          INTER.push_back(cur.second);
          INTER.push_back("<--");
          INTER.push_back(";");
          cur = TK_STREAM[++i];
        }
        else if(TK_STREAM[i+1].first=="@"){
          INTER.push_back(";");
          INTER_ASSIGN_FROM_ADDR(cur.second, "4", TK_STREAM[i+2].first, INTER, i);
        }
        else{
          INTER.push_back(value);
          INTER.push_back(";");
        }
      }
      else if(type == "CHAR"){
        INTER.push_back("1");
        if(OPS.find(value) != OPS.end()){
          INTER.push_back(";");
          INTER_OP(value, stoi(TK_STREAM[i+1].second), cur.second, INTER, i);
        }
        else if(RET_FNS.find(value) != RET_FNS.end()){
          INTER.push_back(";");
          INTER.push_back(cur.second);
          INTER.push_back("<--");
          INTER.push_back(";");
          cur = TK_STREAM[++i];
        }
        else if(TK_STREAM[i+1].first=="@"){
          INTER.push_back(";");
          INTER_ASSIGN_FROM_ADDR(cur.second, "1", TK_STREAM[i+2].first, INTER, i);
        }
        else{
          INTER.push_back(value);
          INTER.push_back(";");
        }
      }
      else if(type == "BOOL"){
        INTER.push_back("1");
        if(OPS.find(value) != OPS.end()){
          INTER.push_back(";");
          INTER_OP(value, stoi(TK_STREAM[i+1].second), cur.second, INTER, i);
        }
        else if(RET_FNS.find(value) != RET_FNS.end()){
          INTER.push_back(";");
          INTER.push_back(cur.second);
          INTER.push_back("<--");
          INTER.push_back(";");
          cur = TK_STREAM[++i];
        }
        else if(TK_STREAM[i+1].first=="@"){
          INTER.push_back(";");
          INTER_ASSIGN_FROM_ADDR(cur.second, "1", TK_STREAM[i+2].first,INTER , i);
        }
        else{
          if(value == "true" || value == "false") INTER.push_back(value);
          INTER.push_back(";");
        }
      }
      else if(type == "PTR"){
        PTR_AMOUNT_MAPS[CF]++;
        BYTE_MAP[CF]+=8;
        PTR_MAPS[CF][cur.second] = BYTE_MAP[CF];
        VAR_SIZE_MAPS[CF][cur.second] = "8";
        INTER.push_back("8");
        if(OPS.find(value) != OPS.end()){
          INTER.push_back(";");
          INTER_OP(value, stoi(TK_STREAM[i+1].second), cur.second, INTER, i);
        }
        else if(RET_FNS.find(value) != RET_FNS.end()){
          INTER.push_back(";");
          INTER.push_back(cur.second);
          INTER.push_back("<--");
          INTER.push_back(";");
          cur = TK_STREAM[++i];
        }
        else if(TK_STREAM[i+1].first=="@"){
          INTER.push_back(";");
          INTER_ASSIGN_FROM_ADDR(cur.second, "8", TK_STREAM[i+2].first,INTER , i);
        }
        else{
          INTER.push_back(value);
          INTER.push_back(";");
          i++;
        }
      }
    }
    else if(cur.first=="fn"){
      string label = cur.second;
      if(label=="main"){
        label = "_start";
      }
      CF = label;
      INTER.push_back("Label:");
      INTER.push_back(label);
      pair<string, string> buff = TK_STREAM[++i];
      if(buff.first != "("){
        cerr << "\033[1;31m"<<"FUNCTION '"<<cur.second<<"' HAS NO OPEN PAREN TO START ARGS LIST!"<<"\033[0m\n";
        assert(false);
      }
      INTER.push_back(";");
      buff = TK_STREAM[++i];
      while(buff.first != ")"){
        // FOR ARGS FIRST WILL BE TYPE
        // SECOND WILL BE NAME
        INTER.push_back("arg");
        INTER.push_back(buff.second);
        if(buff.first=="STRING"){
          // [TODO!] STRING IMP
        }
        else INTER.push_back(to_string(TYPE_BITS[buff.first]));
        VAR_TYPE[buff.second]=buff.first;
        INTER.push_back(";");
        buff = TK_STREAM[++i];
      }
    }
    else if(cur.first=="end"){
      INTER.push_back("end");
      if(cur.second=="loop"||cur.second=="if"||cur.second=="else"||cur.second=="elif"){
        INTER.push_back(cur.second);
      }
      INTER.push_back(";");
    }
    else if(cur.first=="loop" || cur.first=="if"){
      INTER.push_back("Label:");
      INTER.push_back(cur.first);
      INTER.push_back(cur.second);
      INTER.push_back(";");
      if(TK_STREAM[++i].first != "("){
        cerr << "\033[1;31m"<<"LOOP/IF WITH THE ID OF '"<<cur.second<<"' HAS NO OPEN PAREN TO START ARGS LIST!"<<"\033[0m\n";
        assert(false);
      }
      INTER.push_back("con");
      string type = literal_or_var(TK_STREAM[++i].first);
      if(OPS.find(TK_STREAM[i].first) != OPS.end()){
        INTER.push_back(TK_STREAM[i].first);//COMPARISON
        INTER.push_back(TK_STREAM[i].second);//ID
        INTER.push_back(TK_STREAM[++i].first);//A
        INTER.push_back(TK_STREAM[++i].first);//B
      }
      else{
        if(type == "ID"){
          type = VAR_TYPE[TK_STREAM[i].first];
        }
        if(type == "BOOL"){
          INTER.push_back(TK_STREAM[i].first);//BOOL
        }
      }
      INTER.push_back(";");
    }
    else if(cur.first=="else"){
      INTER.push_back("Label:");
      INTER.push_back(cur.first);
      INTER.push_back(cur.second);
      INTER.push_back(";");
    }
    else if(cur.first=="ret"){
      INTER.push_back("out");
      INTER.push_back(TK_STREAM[++i].first);
      INTER.push_back("to");
      string reg;
      string type = literal_or_var(TK_STREAM[i].first);
      if(type=="ID"){
        type = VAR_TYPE[TK_STREAM[i].first];
      }
      int size = TYPE_BITS[type];
      if(size == 8) reg = "rax";
      else if(size == 4) reg = "eax";
      else if(size == 1) reg = "al";
      INTER.push_back(reg);
      INTER.push_back(";");
      INTER.push_back("ret");
      INTER.push_back(";");
    }
    else if(cur.first=="@"){
      string type = cur.second;
      string ptr = TK_STREAM[++i].first;
      string value = TK_STREAM[++i].first;
      INTER.push_back("addr_equ");
      INTER.push_back(to_string(TYPE_BITS[type]));
      INTER.push_back(ptr);
      INTER.push_back(value);
      INTER.push_back(";");
    }
    else if(VAR_TYPE.find(cur.first)!=VAR_TYPE.end()){
      if(find(NATIVE_FNS.begin(), NATIVE_FNS.end(), TK_STREAM[i+1].first) != NATIVE_FNS.end() || find(FN_LIST.begin(), FN_LIST.end(), TK_STREAM[i+1].first) != FN_LIST.end()){
        INTER.push_back(cur.first);
        INTER.push_back("<--");
        INTER.push_back(";");
      }
      else if(TK_STREAM[i+1].first=="@"){
        INTER_ASSIGN_FROM_ADDR(cur.first, to_string(TYPE_BITS[TK_STREAM[i+1].second]), TK_STREAM[i+2].first, INTER, i);
      }
      else{
        string type = VAR_TYPE[cur.first];
        if(OPS.find(TK_STREAM[i+1].first) != OPS.end()){
          INTER_OP(TK_STREAM[i+1].first, stoi(TK_STREAM[i+1].second), cur.first, INTER, i);
        }
        else{
          INTER.push_back(cur.first);
          INTER.push_back("=");
          INTER.push_back(TK_STREAM[++i].first);
          INTER.push_back(";");
        }
      }
    }
    else if(OPS.find(cur.first) != OPS.end()){
      string op = cur.first;
      int op_id = stoi(cur.second);
      INTER.push_back(cur.first);
      INTER.push_back(cur.second);
      for(int j = 0; j < OPS[cur.first][op_id].size()-1; j++){
        INTER.push_back(TK_STREAM[++i].first);
      }
      INTER.push_back(";");
    }
    if(find(FN_LIST.begin(), FN_LIST.end(), cur.first) != FN_LIST.end()){
      if(TK_STREAM[++i].first!="("){
        cerr << "\033[1;31mNO OPEN PAREN IN FN CALL OF '"<<cur.first<<"'.\033[0m\n";
        assert(false);
      }
      pair<string, string> buff = TK_STREAM[++i];
      while(buff.first != ")"){
        // FOR ARGS FIRST WILL BE TYPE
        // SECOND WILL BE NAME
        INTER.push_back("in");
        string type = literal_or_var(buff.first);
        if(type == "ID"){
          type = VAR_TYPE[buff.first];
        }
        INTER.push_back(to_string(TYPE_BITS[type]));
        INTER.push_back(buff.first);
        INTER.push_back(";");
        buff = TK_STREAM[++i];
      }
      INTER.push_back("call");
      INTER.push_back(cur.first);
      INTER.push_back(";");
    }
    else if(find(NATIVE_FNS.begin(), NATIVE_FNS.end(), cur.first) != NATIVE_FNS.end()){
      if(cur.first=="write" || cur.first=="writeln"){
        string label = "WRITE_";
        if(TK_STREAM[++i].first!="("){
          cerr << "\033[1;31mNO OPEN PAREN IN FN CALL OF 'WRITE'.\033[0m\n";
          assert(false);
        }
        string fd = "1";
        string value = TK_STREAM[++i].first;
        string type = literal_or_var(value);
        if(type == "ID"){
          if(VAR_TYPE.find(value)!=VAR_TYPE.end()) type = VAR_TYPE[value];
          else if(DATA_TYPES.find(value)!=DATA_TYPES.end()) type = DATA_TYPES[value];
        }
        if(type == "UINT" && TK_STREAM[i+1].first!=")"){
          fd = value;
          value = TK_STREAM[++i].first;
          type = literal_or_var(value);
          if(type == "ID"){
            if(VAR_TYPE.find(value)!=VAR_TYPE.end()) type = VAR_TYPE[value];
            else if(DATA_TYPES.find(value)!=DATA_TYPES.end()) type = DATA_TYPES[value];
          }
        }
        label+=type;
        if(type == "PTR"){
          INTER.push_back("in");
          INTER.push_back("8");
          INTER.push_back(value);
          INTER.push_back("to");
          INTER.push_back("rsi");
          INTER.push_back(";");
          if(literal_or_var(TK_STREAM[++i].first)=="UINT" || DATA_TYPES.find(TK_STREAM[i].first)!=DATA_TYPES.end()) INTER.push_back("in");
          else INTER.push_back("insx");
          INTER.push_back("4");
          INTER.push_back(TK_STREAM[i].first);
          INTER.push_back("to");
          INTER.push_back("rdx");
          INTER.push_back(";");
          INTER.push_back("in");
          INTER.push_back("4");
          INTER.push_back(fd);
          INTER.push_back("to");
          INTER.push_back("edi");
          INTER.push_back(";");
          INTER.push_back("call");
          INTER.push_back(label);
          INTER.push_back(";");
        }
        else if(type == "STRING"){
          INTER.push_back("asm");
          INTER.push_back("out");
          INTER.push_back("rsi");
          INTER.push_back("    lea rsi, ");
          INTER.push_back("@");
          INTER.push_back(value);
          INTER.push_back("\n");
          INTER.push_back(";");
          INTER.push_back("in");
          INTER.push_back("4");
          INTER.push_back(value+".len");
          INTER.push_back("to");
          INTER.push_back("rdx");
          INTER.push_back(";");
          INTER.push_back("in");
          INTER.push_back("4");
          INTER.push_back(fd);
          INTER.push_back("to");
          INTER.push_back("edi");
          INTER.push_back(";");
          INTER.push_back("call");
          INTER.push_back("WRITE_PTR");
          INTER.push_back(";");
        }
        else{
          INTER.push_back("in");
          INTER.push_back(to_string(TYPE_BITS[type]));
          INTER.push_back(value);
          INTER.push_back(";");
          INTER.push_back("in");
          INTER.push_back("4");
          INTER.push_back(fd);
          INTER.push_back(";");
          INTER.push_back("call");
          INTER.push_back(label);
          INTER.push_back(";");
          i++;
        }
        if(cur.first=="writeln"){
          INTER.push_back("in");
          INTER.push_back("4");
          INTER.push_back(fd);
          INTER.push_back("to");
          INTER.push_back("eax");
          INTER.push_back(";");
          INTER.push_back("call");
          INTER.push_back("WRITE_NEW_LINE");
          INTER.push_back(";");
        }
      }
      else if(cur.first=="new_line"){
        if(TK_STREAM[++i].first!="("){
          cerr << "\033[1;31mNO OPEN PAREN IN FN CALL OF 'new_line'.\033[0m\n";
          assert(false);
        }
        string fd = TK_STREAM[++i].first != ")" ? TK_STREAM[i++].first : "1";
        INTER.push_back("in");
        INTER.push_back("4");
        INTER.push_back(fd);
        INTER.push_back("to");
        INTER.push_back("eax");
        INTER.push_back(";");
        INTER.push_back("call");
        INTER.push_back("WRITE_NEW_LINE");
        INTER.push_back(";");
        if(TK_STREAM[i].first!=")"){
          cerr << "\033[1;31mNO CLOSED PAREN IN FN CALL OF 'new_line'.\033[0m\n";
          assert(false);
        }
      }
      else if(cur.first=="syscall"){
        int arg_count = 0;
        if(TK_STREAM[++i].first!="("){
          cerr << "\033[1;31mNO OPEN PAREN IN FN CALL OF 'syscall'.\033[0m\n";
          assert(false);
        }
        i++;
        while(TK_STREAM[i].first!=")"){
          string value = TK_STREAM[i].first;
          string type = literal_or_var(value);
          if(type == "ID"){
            type = VAR_TYPE[value];
          }
          int size = TYPE_BITS[type];
          if(size != 8){
            INTER.push_back("insx");
          }
          else INTER.push_back("in");
          INTER.push_back(to_string(size));
          INTER.push_back(value);
          INTER.push_back("to");
          INTER.push_back(SYSCALL_ARGS[arg_count]);
          INTER.push_back(";");
          i++;
          arg_count++;
        }
        INTER.push_back("syscall");
        INTER.push_back(";");
      }
      else if(cur.first=="ptr"){
        if(TK_STREAM[++i].first!="("){
          cerr << "\033[1;31mNO OPEN PAREN IN FN CALL OF 'ptr'.\033[0m\n";
          assert(false);
        }
        string value = TK_STREAM[++i].first;
        string type = literal_or_var(value);
        if(type == "ID"){
          type = VAR_TYPE[value];
        }
        else{
          cerr << "\033[1;31mPUT NON-VAR INTO 'ptr' FUNCTION.\033[0m\n";
          assert(false);
        }
        INTER.push_back("asm");
        INTER.push_back("out");
        INTER.push_back("rax");
        INTER.push_back("    lea rax, ");
        INTER.push_back("@");
        INTER.push_back(value);
        INTER.push_back("\n");
        INTER.push_back(";");
        i++;
      }
      else if(cur.first=="len"){
        if(TK_STREAM[++i].first!="("){
          cerr << "\033[1;31mNO OPEN PAREN IN FN CALL OF 'ptr'.\033[0m\n";
          assert(false);
        }
        string value = TK_STREAM[++i].first;
        string type = literal_or_var(value);
        if(DATA_TYPES.find(value)==DATA_TYPES.end()){
          cerr << "\033[1;31mDON'T PUT NON-CONST INTO 'len' FUNCTION.\033[0m\n";
          assert(false);
        }
        INTER.push_back("asm");
        INTER.push_back("out");
        INTER.push_back("eax");
        INTER.push_back("    mov eax, ");
        INTER.push_back(value+".len");
        INTER.push_back("\n");
        INTER.push_back(";");
        i++;
      }
      else if(cur.first=="array"){
        if(TK_STREAM[++i].first != "("){
          cerr << "\033[1;31mNO OPEN PAREN IN FN CALL OF 'array'.\033[0m\n";
          assert(false);
        }
        string type = TK_STREAM[++i].first;
        string amount = TK_STREAM[i].second;
        INTER.push_back("alloc");
        INTER.push_back(to_string(TYPE_BITS[type] * stoi(amount)));
        INTER.push_back(";");
        
        if(TK_STREAM[++i].first != ")"){
          cerr << "\033[1;31mNO CLOSED PAREN IN FN CALL OF 'array'.\033[0m\n";
          assert(false);
        }
      }
      else if(cur.first=="alloc"){
        if(TK_STREAM[++i].first != "("){
          cerr << "\033[1;31mNO OPEN PAREN IN FN CALL OF 'alloc'.\033[0m\n";
          assert(false);
        }
        string bytes = TK_STREAM[++i].first;
        INTER.push_back("alloc");
        INTER.push_back(bytes);
        INTER.push_back(";");
        if(TK_STREAM[++i].first != ")"){
          cerr << "\033[1;31mNO CLOSED PAREN IN FN CALL OF 'alloc'.\033[0m\n";
          assert(false);
        }
      }
      else if(cur.first=="dealloc"){
        if(TK_STREAM[++i].first != "("){
          cerr << "\033[1;31mNO OPEN PAREN IN FN CALL OF 'dealloc'.\033[0m\n";
          assert(false);
        }
        string bytes = TK_STREAM[++i].first;
        INTER.push_back("dealloc");
        INTER.push_back(bytes);
        INTER.push_back(";");
        if(TK_STREAM[++i].first != ")"){
          cerr << "\033[1;31mNO CLOSED PAREN IN FN CALL OF 'dealloc'.\033[0m\n";
          assert(false);
        }
      }
      else if(cur.first=="addr_sub"){
        if(TK_STREAM[++i].first != "("){
          cerr << "\033[1;31mNO OPEN PAREN IN FN CALL OF 'addr_sub'.\033[0m\n";
          assert(false);
        }
        INTER.push_back("addr_sub");
        INTER.push_back(TK_STREAM[++i].first);
        INTER.push_back(TK_STREAM[++i].first);
        INTER.push_back(";");
        if(TK_STREAM[++i].first != ")"){
          cerr << "\033[1;31mNO CLOSED PAREN IN FN CALL OF 'addr_sub'.\033[0m\n";
          assert(false);
        }
      }
      else if(cur.first=="addr_add"){
        if(TK_STREAM[++i].first != "("){
          cerr << "\033[1;31mNO OPEN PAREN IN FN CALL OF 'addr_add'.\033[0m\n";
          assert(false);
        }
        INTER.push_back("addr_add");
        INTER.push_back(TK_STREAM[++i].first);
        INTER.push_back(TK_STREAM[++i].first);
        INTER.push_back(";");
        if(TK_STREAM[++i].first != ")"){
          cerr << "\033[1;31mNO CLOSED PAREN IN FN CALL OF 'addr_add'.\033[0m\n";
          assert(false);
        }
      }
      else if(cur.first=="addr_get"){
        if(TK_STREAM[++i].first != "("){
          cerr << "\033[1;31mNO OPEN PAREN IN FN CALL OF 'addr_get'.\033[0m\n";
          assert(false);
        }
        INTER.push_back("addr_get");
        INTER.push_back(to_string(TYPE_BITS[TK_STREAM[++i].first]));
        INTER.push_back(TK_STREAM[i].second);
        INTER.push_back(";");
        if(TK_STREAM[++i].first != ")"){
          cerr << "\033[1;31mNO CLOSED PAREN IN FN CALL OF 'addr_get'.\033[0m\n";
          assert(false);
        }
      }
      else if(cur.first=="addr_equ"){
        if(TK_STREAM[++i].first != "("){
          cerr << "\033[1;31mNO OPEN PAREN IN FN CALL OF 'addr_equ'.\033[0m\n";
          assert(false);
        }
        INTER.push_back("addr_equ");
        INTER.push_back(to_string(TYPE_BITS[TK_STREAM[++i].first]));
        INTER.push_back(TK_STREAM[i].second);
        INTER.push_back(TK_STREAM[++i].first);
        INTER.push_back(";");
        if(TK_STREAM[++i].first != ")"){
          cerr << "\033[1;31mNO CLOSED PAREN IN FN CALL OF 'addr_equ'.\033[0m\n";
          assert(false);
        }
      }
      else if(cur.first=="sleep"){
        if(TK_STREAM[++i].first != "("){
          cerr << "\033[1;31mNO OPEN PAREN IN FN CALL OF 'sleep'.\033[0m\n";
          assert(false);
        }
        INTER.push_back("sleep");
        INTER.push_back(TK_STREAM[++i].first);
        INTER.push_back(TK_STREAM[++i].first);
        INTER.push_back(";");
        if(TK_STREAM[++i].first != ")"){
          cerr << "\033[1;31mNO CLOSED PAREN IN FN CALL OF 'sleep'.\033[0m\n";
          assert(false);
        }
      }
      else if(cur.first=="error"){
        if(TK_STREAM[++i].first != "("){
          cerr << "\033[1;31mNO OPEN PAREN IN FN CALL OF 'error'.\033[0m\n";
          assert(false);
        }
        string ptr = TK_STREAM[++i].first;
        string length = TK_STREAM[++i].first;
        string fd = "2";
        
        INTER.push_back("in");
        INTER.push_back("8");
        INTER.push_back(ptr);
        INTER.push_back("to");
        INTER.push_back("rsi");
        INTER.push_back(";");
        if(literal_or_var(length)=="UINT" || DATA_TYPES.find(length)!=DATA_TYPES.end()) INTER.push_back("in");
        else INTER.push_back("insx");
        INTER.push_back("4");
        INTER.push_back(length);
        INTER.push_back("to");
        INTER.push_back("rdx");
        INTER.push_back(";");
        INTER.push_back("in");
        INTER.push_back("4");
        INTER.push_back(fd);
        INTER.push_back("to");
        INTER.push_back("edi");
        INTER.push_back(";");
        INTER.push_back("call");
        INTER.push_back("WRITE_PTR");
        INTER.push_back(";");
        INTER.push_back("in");
        INTER.push_back("4");
        INTER.push_back(fd);
        INTER.push_back("to");
        INTER.push_back("eax");
        INTER.push_back(";");
        INTER.push_back("call");
        INTER.push_back("WRITE_NEW_LINE");
        INTER.push_back(";");
        
        string error_code = TK_STREAM[++i].first;
        if(literal_or_var(error_code)=="UINT" || DATA_TYPES.find(error_code)!=DATA_TYPES.end()) INTER.push_back("in");
        else INTER.push_back("insx");
        INTER.push_back("4");
        INTER.push_back(error_code);
        INTER.push_back("to");
        INTER.push_back("rdi");
        INTER.push_back(";");
        INTER.push_back("in");
        INTER.push_back("4");
        INTER.push_back("60"); //EXIT SYSCALL
        INTER.push_back("to");
        INTER.push_back("rax");
        INTER.push_back(";");
        INTER.push_back("syscall");
        INTER.push_back(";");
        if(TK_STREAM[++i].first != ")"){
          cerr << "\033[1;31mNO CLOSED PAREN IN FN CALL OF 'error'.\033[0m\n";
          assert(false);
        }
      }
      else if(cur.first=="exit"){
        if(TK_STREAM[++i].first != "("){
          cerr << "\033[1;31mNO OPEN PAREN IN FN CALL OF 'exit'.\033[0m\n";
          assert(false);
        }
        string error_code = TK_STREAM[++i].first;
        if(literal_or_var(error_code)=="UINT" || DATA_TYPES.find(error_code)!=DATA_TYPES.end()) INTER.push_back("in");
        else INTER.push_back("insx");
        INTER.push_back("4");
        INTER.push_back(error_code);
        INTER.push_back("to");
        INTER.push_back("rdi");
        INTER.push_back(";");
        INTER.push_back("in");
        INTER.push_back("4");
        INTER.push_back("60"); //EXIT SYSCALL
        INTER.push_back("to");
        INTER.push_back("rax");
        INTER.push_back(";");
        INTER.push_back("syscall");
        INTER.push_back(";");
        if(TK_STREAM[++i].first != ")"){
          cerr << "\033[1;31mNO CLOSED PAREN IN FN CALL OF 'exit'.\033[0m\n";
          assert(false);
        }
      }
      else if(cur.first=="cast"){
        if(TK_STREAM[++i].first != "("){
          cerr << "\033[1;31mNO OPEN PAREN IN FN CALL OF 'cast'.\033[0m\n";
          assert(false);
        }
        INTER.push_back("cast");
        INTER.push_back(TK_STREAM[++i].first);
        INTER.push_back(TK_STREAM[i].second);
        INTER.push_back(";");
        if(TK_STREAM[++i].first != ")"){
          cerr << "\033[1;31mNO CLOSED PAREN IN FN CALL OF 'cast'.\033[0m\n";
          assert(false);
        }
      }
      else if(cur.first=="read_bits"){
        if(TK_STREAM[++i].first != "("){
          cerr << "\033[1;31mNO OPEN PAREN IN FN CALL OF 'read_bits'.\033[0m\n";
          assert(false);
        }
        string value = TK_STREAM[++i].first;
        string buffer = TK_STREAM[++i].first;
        string type = literal_or_var(value);
        if(type=="ID"){
          if(VAR_TYPE.find(value)!=VAR_TYPE.end()) type = VAR_TYPE[value];
          else if(DATA_TYPES.find(value)!=DATA_TYPES.end()) type = DATA_TYPES[value];
        }
        INTER.push_back("in");
        INTER.push_back(to_string(TYPE_BITS[type]));
        INTER.push_back(value);
        INTER.push_back(";");
        INTER.push_back("in");
        INTER.push_back("4");
        INTER.push_back(to_string(TYPE_BITS[type]*8));
        INTER.push_back(";");
        INTER.push_back("in");
        INTER.push_back("8");
        INTER.push_back(buffer);
        INTER.push_back(";");
        INTER.push_back("call");
        INTER.push_back("@READ_BITS");
        INTER.push_back(";");
        if(TK_STREAM[++i].first != ")"){
          cerr << "\033[1;31mNO CLOSED PAREN IN FN CALL OF 'read_bits'.\033[0m\n";
          assert(false);
        }
      }
      else if(cur.first=="open_fd"){
        if(TK_STREAM[++i].first!="("){
          cerr << "\033[1;31mNO OPEN PAREN IN FN CALL OF 'open_fd'.\033[0m\n";
          assert(false);
        }
        string filename = TK_STREAM[++i].first;
        string flags = TK_STREAM[++i].first;
        string mode = TK_STREAM[++i].first;
        if(DATA_TYPES.find(filename)==DATA_TYPES.end()){
          if(DATA_TYPES[filename]!="STRING"){
            cerr << "\033[1;31mFILENAME IN FN CALL OF 'open_fd' IS NOT A STRING.\033[0m\n";
          }
        }
        string type = literal_or_var(mode);
        if(type == "ID"){
          if(VAR_TYPE.find(mode)!=DATA_TYPES.end()) type = VAR_TYPE[mode];
          else if(DATA_TYPES.find(mode)!=DATA_TYPES.end()) type = DATA_TYPES[mode];
        }
        if(type != "UINT"){
          cerr << "\033[1;MODE IN FN CALL OF 'open_fd' IS NOT A UINT.\033[0m\n";
        }
        INTER.push_back("open_fd");
        INTER.push_back(filename);
        INTER.push_back(flags);
        INTER.push_back(mode);
        INTER.push_back(";");
        if(TK_STREAM[++i].first!=")"){
          cerr << "\033[1;31mNO CLOSED PAREN IN FN CALL OF 'open_fd'.\033[0m\n";
          assert(false);
        }
      }
      else if(cur.first=="close_fd"){
        if(TK_STREAM[++i].first!="("){
          cerr << "\033[1;31mNO OPEN PAREN IN FN CALL OF 'close_fd'.\033[0m\n";
          assert(false);
        }
        string fd = TK_STREAM[++i].first;
        INTER.push_back("in");
        INTER.push_back("4");
        INTER.push_back("3");
        INTER.push_back("to");
        INTER.push_back("eax");
        INTER.push_back(";");
        INTER.push_back("in");
        INTER.push_back("4");
        INTER.push_back(fd);
        INTER.push_back("to");
        INTER.push_back("edi");
        INTER.push_back(";");
        INTER.push_back("syscall");
        INTER.push_back(";");
        if(TK_STREAM[++i].first!=")"){
          cerr << "\033[1;31mNO CLOSED PAREN IN FN CALL OF 'close_fd'.\033[0m\n";
          assert(false);
        }
      }
      else if(cur.first=="read"){
        if(TK_STREAM[++i].first!="("){
          cerr << "\033[1;31mNO OPEN PAREN IN FN CALL OF 'read'.\033[0m\n";
          assert(false);
        }
        string fd = TK_STREAM[i+1].first;
        string type = literal_or_var(fd);
        if(type == "ID"){
          if(VAR_TYPE.find(fd)!=DATA_TYPES.end()) type = VAR_TYPE[fd];
          else if(DATA_TYPES.find(fd)!=DATA_TYPES.end()) type = DATA_TYPES[fd];
        }
        fd = type == "UINT" ? TK_STREAM[++i].first : "1";
        string buffer = TK_STREAM[++i].first;
        string count = TK_STREAM[++i].first;
        INTER.push_back("in");
        INTER.push_back("4");
        INTER.push_back("0");
        INTER.push_back("to");
        INTER.push_back(SYSCALL_ARGS[0]);
        INTER.push_back(";");
        if(literal_or_var(fd)=="UINT" || DATA_TYPES.find(fd)!=DATA_TYPES.end()) INTER.push_back("in");
        else INTER.push_back("insx");
        INTER.push_back("4");
        INTER.push_back(fd);
        INTER.push_back("to");
        INTER.push_back(SYSCALL_ARGS[1]);
        INTER.push_back(";");
        INTER.push_back("in");
        INTER.push_back("8");
        INTER.push_back(buffer);
        INTER.push_back("to");
        INTER.push_back(SYSCALL_ARGS[2]);
        INTER.push_back(";");
        if(literal_or_var(count)=="UINT" || DATA_TYPES.find(count)!=DATA_TYPES.end()) INTER.push_back("in");
        else INTER.push_back("insx");
        INTER.push_back("4");
        INTER.push_back(count);
        INTER.push_back("to");
        INTER.push_back(SYSCALL_ARGS[3]);
        INTER.push_back(";");
        INTER.push_back("syscall");
        INTER.push_back(";");
        if(TK_STREAM[++i].first!=")"){
          cerr << "\033[1;31mNO CLOSED PAREN IN FN CALL OF 'read'.\033[0m\n";
          assert(false);
        }
      }
      else if(cur.first=="fstats"){
        if(TK_STREAM[++i].first!="("){
          cerr << "\033[1;31mNO OPEN PAREN IN FN CALL OF 'fstats'.\033[0m\n";
          assert(false);
        }
        string fd = TK_STREAM[++i].first;
        string buffer = TK_STREAM[++i].first; // BUFFER SHOULD BE 144 BYTES
        INTER.push_back("in");
        INTER.push_back("4");
        INTER.push_back("5");
        INTER.push_back("to");
        INTER.push_back(SYSCALL_ARGS[0]);
        INTER.push_back(";");
        INTER.push_back("insx");
        INTER.push_back("4");
        INTER.push_back(fd);
        INTER.push_back("to");
        INTER.push_back(SYSCALL_ARGS[1]);
        INTER.push_back(";");
        INTER.push_back("in");
        INTER.push_back("8");
        INTER.push_back(buffer);
        INTER.push_back("to");
        INTER.push_back(SYSCALL_ARGS[2]);
        INTER.push_back(";");
        INTER.push_back("syscall");
        INTER.push_back(";");
        if(TK_STREAM[++i].first!=")"){
          cerr << "\033[1;31mNO CLOSED PAREN IN FN CALL OF 'fstats'.\033[0m\n";
          assert(false);
        }
      }
    }
  }
  INTER.push_back("SECTION");
  INTER.push_back(".DATA");
  INTER.push_back(";");
  for(pair<string, string> &c : DATA){
    string type = DATA_TYPES[c.first];
    if(type == "STRING"){
      INTER.push_back(c.first);
      INTER.push_back(" db ");
      INTER.push_back(c.second);
      INTER.push_back("\n");
    }
    else if(type == "UINT"){
      INTER.push_back(c.first);
      if(isNumber(c.second)) INTER.push_back(" dd ");
      INTER.push_back(c.second);
      INTER.push_back("\n");
    }
    else if(type =="CHAR"){
      INTER.push_back(c.first);
      INTER.push_back(" db ");
      INTER.push_back(c.second);
      INTER.push_back("\n");
    }
    else if(type =="BOOL"){
      INTER.push_back(c.first);
      INTER.push_back(" db ");
      if(c.second=="true") INTER.push_back("1");
      else INTER.push_back("0");
      INTER.push_back("\n");
    }
    else if(type =="ID"){
      cerr << "\033[1;31mCANNOT ASSIGN CONST '"<< c.first <<"' TO THE VAR '"<< c.second <<"'!\033[0m\n";
      assert(false);
    }
    else{
      cerr << "\033[1;31mCANNOT ASSIGN CONST '"<< c.first <<"' TO THE UNHANDLED TYPE OF '"<< type <<"'!\033[0m\n";
      assert(false);
    }
  }
  INTER.push_back("/ESC/");
  INTER.push_back("SECTION");
  INTER.push_back(".BSS");
  INTER.push_back(";");
  for(string &s : BSS){
    INTER.push_back(s);
  }
  INTER.push_back("/ESC/");
  return INTER;
}

string COMPILE_OP(int &i, string &tk, vector <string> &INTER, ofstream &out_stream, unordered_map<string, int> &ptr_map){
  if(!isNumber(INTER[++i])) cerr << "NOT NUMBER: "<< INTER[i] << "\n";
  int id = stoi(INTER[i]);
  if(tk=="+"){
    if(id==0){
      string a = INTER[++i];
      string b = INTER[++i];
      out_stream <<
      "    mov eax, "<< asm_get_value(a, ptr_map) <<"\n"
      "    add eax, "<< asm_get_value(b, ptr_map) <<"\n";
      i++;
      return "eax";
    }
    else if(id==1){
      string ptr = INTER[++i];
      string additive = INTER[++i];
      out_stream <<
      "    mov rax, " << asm_get_value(ptr, ptr_map) << "\n";
      if(literal_or_var(additive)=="ID"){
        out_stream <<
        "    movsx rdx, " << asm_get_value(additive, ptr_map) << "\n"
        "    add rax, rdx\n";
      }
      else{
        out_stream <<
        "    add rax, " << additive << "\n";
      }
      i++;
      return "rax";
    }
    else if(id==2){
      string additive = INTER[++i];
      string ptr = INTER[++i];
      out_stream <<
      "    mov rax, " << asm_get_value(ptr, ptr_map) << "\n";
      if(literal_or_var(additive)=="ID"){
        out_stream <<
        "    movsx rdx, " << asm_get_value(additive, ptr_map) << "\n"
        "    add rax, rdx\n";
      }
      else{
        out_stream <<
        "    add rax, " << additive << "\n";
      }
      i++;
      return "rax";
    }
  }
  else if(tk=="-"){
    if(id==0){
      string a = INTER[++i];
      string b = INTER[++i];
      out_stream <<
      "    mov eax, "<< asm_get_value(a, ptr_map) <<"\n"
      "    sub eax, "<< asm_get_value(b, ptr_map) <<"\n";
      i++;
      return "eax";
    }
    else if(id==1){
      string ptr = INTER[++i];
      string subtactor = INTER[++i];
      out_stream <<
      "    mov rax, " << asm_get_value(ptr, ptr_map) << "\n";
      if(literal_or_var(subtactor)=="ID"){
        out_stream <<
        "    movsx rdx, " << asm_get_value(subtactor, ptr_map) << "\n"
        "    add rax, rdx\n";
      }
      else{
        out_stream <<
        "    sub rax, " << subtactor << "\n";
      }
      i++;
      return "rax";
    }
  }
  else if(tk=="*"){
    if(id==0){
      string a = INTER[++i];
      string b = INTER[++i];
      out_stream <<
      "    mov eax, "<< asm_get_value(a, ptr_map) <<"\n"
      "    mul "<< asm_get_value(b, ptr_map) <<"\n";
      i++;
      return "eax";
    }
  }
  else if(tk=="/"){
    if(id==0){
      string a = INTER[++i];
      string b = INTER[++i];
      out_stream <<
      "    xor edx,edx\n"
      "    mov eax, "<< asm_get_value(a, ptr_map) <<"\n"
      "    div "<< asm_get_value(b, ptr_map) <<"\n";
      i++;
      return "eax";
    }
  }
  else if(tk=="%"){
    if(id==0){
      string a = INTER[++i];
      string b = INTER[++i];
      out_stream <<
      "    xor edx,edx\n"
      "    mov eax, "<< asm_get_value(a, ptr_map) <<"\n"
      "    div "<< asm_get_value(b, ptr_map) <<"\n";
      i++;
      return "edx";
    }
  }
  else if(tk=="++"){
    if(id==0||id==1){
      string a = INTER[++i];
      out_stream <<
      "    inc "<< asm_get_value(a, ptr_map) << "\n";
      i++;
    }
  }
  else if(tk=="--"){
    if(id==0||id==1){
      string a = INTER[++i];
      out_stream <<
      "    dec "<< asm_get_value(a, ptr_map) << "\n";
      i++;
    }
  }
  else if(tk==">"){
    if(id==0){
      string a = INTER[++i];
      string b = INTER[++i];
      out_stream <<
      "    mov eax, "<< asm_get_value(a, ptr_map) << "\n"
      "    cmp eax, "<< asm_get_value(b, ptr_map) << "\n"
      "    setg al\n";
      i++;
      return "al";
    }
  }
  else if(tk=="<"){
    if(id==0){
      string a = INTER[++i];
      string b = INTER[++i];
      out_stream <<
      "    mov eax, "<< asm_get_value(a, ptr_map) << "\n"
      "    cmp eax, "<< asm_get_value(b, ptr_map) << "\n"
      "    setl al\n";
      i++;
      return "al";
    }
  }
  else if(tk=="=="){
    if(id==0){
      string a = INTER[++i];
      string b = INTER[++i];
      out_stream <<
      "    mov eax, "<< asm_get_value(a, ptr_map) << "\n"
      "    cmp eax, "<< asm_get_value(b, ptr_map) << "\n"
      "    sete al\n";
      i++;
      return "al";
    }
  }
  else if(tk=="<="){
    if(id==0){
      string a = INTER[++i];
      string b = INTER[++i];
      out_stream <<
      "    mov eax, "<< asm_get_value(a, ptr_map) << "\n"
      "    cmp eax, "<< asm_get_value(b, ptr_map) << "\n"
      "    setle al\n";
      i++;
      return "al";
    }
  }
  else if(tk==">="){
    if(id==0){
      string a = INTER[++i];
      string b = INTER[++i];
      out_stream <<
      "    mov eax, "<< asm_get_value(a, ptr_map) << "\n"
      "    cmp eax, "<< asm_get_value(b, ptr_map) << "\n"
      "    setge al\n";
      i++;
      return "al";
    }
  }
  else if(tk=="!="){
    if(id==0){
      string a = INTER[++i];
      string b = INTER[++i];
      out_stream <<
      "    mov eax, "<< asm_get_value(a, ptr_map) << "\n"
      "    cmp eax, "<< asm_get_value(b, ptr_map) << "\n"
      "    setne al\n";
      i++;
      return "al";
    }
    else if(id==1||id==2){
      string a = INTER[++i];
      string b = INTER[++i];
      out_stream <<
      "    mov al, "<< asm_get_value(a, ptr_map) << "\n"
      "    cmp al, "<< asm_get_value(b, ptr_map) << "\n"
      "    setne al\n";
      i++;
      return "al";
    }
  }
  else if(tk=="!"){
    if(id==0){
      string a = INTER[++i];
      out_stream <<
      "    movsx eax, "<<asm_get_value(a, ptr_map) << "\n"
      "    xor eax, 1\n";
      i++;
      return "al";
    }
  }
  else if(tk=="&&"){
    if(id==0){
      string a = INTER[++i];
      string b = INTER[++i];
      out_stream <<
      "    mov al, "<< asm_get_value(a, ptr_map) << "\n"
      "    and al, "<< asm_get_value(b, ptr_map) << "\n"
      "    shr al, 1\n"
      "    setc al\n";
      i++;
      return "al";
    }
  }
  else if(tk=="||"){
    if(id==0){
      string a = INTER[++i];
      string b = INTER[++i];
      out_stream <<
      "    mov al, "<< asm_get_value(a, ptr_map) << "\n"
      "    or al, "<< asm_get_value(b, ptr_map) << "\n"
      "    shr al, 1\n"
      "    setc al\n";
      i++;
      return "al";
    }
  }
  else if(tk=="-|"){
    if(id==0){
      string a = INTER[++i];
      string b = INTER[++i];
      out_stream <<
      "    mov al, "<< asm_get_value(a, ptr_map) << "\n"
      "    xor al, "<< asm_get_value(b, ptr_map) << "\n"
      "    shr al, 1\n"
      "    setc al\n";
      i++;
      return "al";
    }
  }
  return "NA";
}

void COMPILE_CONDITIONAL_JUMP(string &label, vector<string> &condition, ofstream &out_stream, unordered_map<string, int> &ptr_map, bool inverse = false){
  if(condition[0]!="con"){
    cerr << "\033[1;31mINPUTTED A NON-CONDITION\033[0m\n";
    assert(false);
  }
  if(condition.size()==3){
    if(inverse){
      out_stream <<
      "    cmp " << asm_get_value(condition[1], ptr_map) << ", 1\n"
      "    jne ." << label << "\n";
    }
    else{
      out_stream <<
      "    cmp " << asm_get_value(condition[1], ptr_map) << ", 0\n"
      "    jne ." << label << "\n";
    }
  }
  else if(OPS.find(condition[1]) != OPS.end()){
    string jmp = COMP_JMP[condition[1]];
    if(inverse) jmp = INV_JMP[jmp];
    if(literal_or_var(condition[4])!="ID"){
      out_stream <<
      "    cmp " << asm_get_value(condition[3], ptr_map) << ", " << asm_get_value(condition[4], ptr_map) << "\n"
      "    " << jmp << " ." << label << "\n";
    }
    else{
      string type;
      string var = condition[4];
      if(VAR_TYPE.find(var)!=VAR_TYPE.end()) type = VAR_TYPE[var];
      if(DATA_TYPES.find(var)!=DATA_TYPES.end()) type = DATA_TYPES[var];
      int size = TYPE_BITS[type];
      string reg;
      if(size==8) reg = "rax";
      else if(size==4) reg = "eax";
      else if(size==1) reg = "al";
      else {
        cerr << "\033[1;31m'"<< var << "' HAS AN INVALID SIZE OF '" << size << "' AND A TYPE OF '"<< type << "' IN CONDITIONAL JUMP COMPILATION.\033[0m\n";
        assert(false);
      }
      out_stream <<
      "    mov " << reg << ", " <<asm_get_value(condition[3], ptr_map) << "\n"
      "    cmp " << reg << ", " << asm_get_value(var, ptr_map) << "\n"
      "    " << jmp << " ." << label << "\n";
    }
  }
}

void COMPILE(string &wf, vector <string> &INTER){
  //OPEN FILE
  ofstream out_stream;
  out_stream.open(wf);
  if(!out_stream.is_open()){
    cerr << "\033[1;31mERROR: CAN'T OPEN/MAKE ASM FILE!\033[0m\n";
    assert(false);
  }
  //INIT
    out_stream <<
"SECTION .text		; code section\n"
"WRITE_UINT:\n"
"    mov    rbp, rsp\n"
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
"    lea    edx, [rsp+16]\n"
"    sub    edx, esi\n"
"    syscall\n"
"    add    rsp, 24\n"
"    ret\n"
"WRITE_CHAR:\n"
"    push rax\n"
"    mov eax, 1    ;write syscall\n"
"    mov rdi, rsi  ;stdout\n"
"    mov rsi, rsp  ;get char\n"
"    mov edx, 1    ;char amount\n"
"    syscall\n"
"    pop rax\n"
"    ret\n"
"WRITE_NEW_LINE:\n"
"    push 10\n"
"    mov rdi, rax  ;outstream\n"
"    mov eax, 1    ;write syscall\n"
"    mov rsi, rsp  ;get char\n"
"    mov edx, 1    ;char amount\n"
"    syscall\n"
"    pop rax\n"
"    ret\n"
"WRITE_PTR:\n" //Actually WRITE_STRING, but the way we generate it we have the name WRITE_PTR
"    mov eax, 1    ;write syscall\n"
"    syscall\n"
"    ret\n"
"WRITE_BOOL:\n"
"    test al, al\n"
"    mov rdi, rsi  ;outstream\n"
"    jz .false\n"
"    lea rsi, [@TRUE]\n"
"    mov edx, @TRUELEN\n"
"    jmp .write\n"
".false:\n"
"    lea rsi, [@FALSE]\n"
"    mov edx, @FALSELEN\n"
".write:\n"
"    mov eax, 1    ;write syscall\n"
"    syscall\n"
"    ret\n"
"@READ_BITS:\n" // rdi is val, esi is bytes, rdx is buffer
"    push 0\n"
".loop_in:\n"
"    shl rdi, 1\n"
"    jnc .azero\n"
"    pop rcx\n"
"    mov byte [rdx+rcx], 1\n"
"    jmp .loop_out\n"
".azero:\n"
"    pop rcx\n"
"    mov byte [rdx+rcx], 0\n"
".loop_out:\n"
"    inc rcx\n"
"    cmp rcx, rsi\n"
"    jl .loop_in\n"
"    push rcx\n"
"    ret\n"
"global _start		; make label available to linker\n";
  
  for(string &s : GLOBAL_FNS){
    out_stream << "global " << s << "\n";
  }
  for(string &s : EXTERN_FNS){
    out_stream << "extern " << s << "\n";
  }
  //INTERPRET
  string CB; //CURRENT_BLOCK
  vector<string> BLOCK_STACK;
  vector<string> STACK;
  string FNV; //VAR FROM FUNCTION CALL
  for(int i = 0; i < INTER.size(); i++){
    string tk = INTER[i];
    if(tk=="Label:"){
      if(INTER[++i]=="loop"){
        out_stream <<
        "    jmp ." << INTER[i] << INTER[++i] << "_in\n"
        "." << INTER[i-1] << INTER[i] << "_out:\n";
        BLOCK_STACK.push_back(INTER[i]);
        i++;
        if(INTER[++i]=="con"){
          int length = 0;
          while(INTER[i+length]!=";"){
            length++;
          }
          STACK.push_back(";");
          for(int j = length-1; j >= 1; j--){
            STACK.push_back(INTER[i+j]);
          }
          STACK.push_back("con");
          i+=length;
        }
        else{
          // cout << INTER[i] << "\n";
          cerr << "\033[1;31mLOOP WITHOUT A CONDITION.\033[0m\n";
          assert(false);
        }
      }
      else if(INTER[i]=="if"){
        string label = "if" + INTER[++i];
        BLOCK_STACK.push_back(INTER[i]);
        i++;
        if(INTER[++i]=="con"){
          vector<string> con;
          con.push_back("con");
          while(INTER[++i]!=";"){
            con.push_back(INTER[i]);
          }
          con.push_back(";");
          COMPILE_CONDITIONAL_JUMP(label, con, out_stream, PTR_MAPS[CB], true);
          STACK.push_back(";");
          STACK.push_back("."+label+":\n");
        }
        else{
          // cout << INTER[i] << "\n";
          cerr << "\033[1;31mIF WITHOUT A CONDITION.\033[0m\n";
          assert(false);
        }
      }
      else if(INTER[i]=="else"){
        string esc_id = to_string(ELSES[stoi(INTER[++i])]);
        string else_label = ".else" + esc_id;
        string if_label = STACK[STACK.size()-1];
        STACK.pop_back();
        STACK.pop_back();
        if(STACK[STACK.size()-1]==else_label+":\n"){
          STACK.pop_back();
          STACK.pop_back();
          BLOCK_STACK.pop_back();
        }
        BLOCK_STACK.pop_back();
        BLOCK_STACK.push_back("else");
        out_stream << "    jmp " << else_label << "\n";
        out_stream << if_label;
        STACK.push_back(";");
        STACK.push_back(else_label+":\n");
        i++;
      }
      else if(INTER[i]=="_start"){
        out_stream << INTER[i] << ":\n";
        CB=INTER[i];
        int local_storage = CALL_AMOUNT_MAPS[CB]*16 + (PTR_AMOUNT_MAPS[CB]%2)*8;
        out_stream <<
  "    push rbp\n"
  "    mov rbp, rsp\n"
  "    sub rsp, "<< local_storage <<"\n";
        i++;
      }
      else{
        out_stream << INTER[i] << ":\n";
        CB=INTER[i];
        int local_storage = CALL_AMOUNT_MAPS[CB]*16 + (PTR_AMOUNT_MAPS[CB]%2)*8;
        out_stream <<
  "    push rbp\n"
  "    mov rbp, rsp\n"
  "    sub rsp, "<< local_storage <<"\n";
        i++;
        if(INTER[i+1]=="arg"){
          int arg_count = 0;
          while(INTER[i+1]=="arg"){
            i++;
            string name = INTER[++i];
            string size = INTER[++i];
            string reg;
            if(size == "4"){
              BYTE_MAP[CB]+=4;
              reg = FUNC_ARG_REGISTER_LIST32[arg_count];
            }
            else if(size == "8"){
              BYTE_MAP[CB]+=8;
              reg = FUNC_ARG_REGISTER_LIST64[arg_count];
            }
            else if(size == "1"){
              BYTE_MAP[CB]+=1;
              reg = FUNC_ARG_REGISTER_LIST8[arg_count];
            }
            int PTR = BYTE_MAP[CB];
            PTR_MAPS[CB][name] = PTR;
            VAR_SIZE_MAPS[CB][name]=size;
            out_stream <<
            "    mov " << asm_var(PTR, stoi(size)) << ", " << reg << "\n";
            
            i++;
            arg_count++;
          }
          FN_ARG_COUNT[CB]=arg_count;
        }
      }
    }
    else if(tk == "def"){
      while(true){
        string name = INTER[++i];
        string size = INTER[++i];
        string value = INTER[++i];
        if(size == "4"){
          BYTE_MAP[CB]+=4;
          int PTR = BYTE_MAP[CB];
          PTR_MAPS[CB][name] = PTR;
          VAR_SIZE_MAPS[CB][name]=size;
          if(value != ";"){
            if(literal_or_var(value)=="ID"){
              out_stream <<
              "    mov eax, " << asm_get_value(value, PTR_MAPS[CB]) << "\n"
              "    mov " << asm_var(PTR, 4) << ", eax\n";
            }
            else{
              out_stream <<
              "    mov " << asm_var(PTR, 4) << ", " << asm_get_value(value, PTR_MAPS[CB]) << "\n";
            }
          }
          else break;
        }
        else if(size == "8"){
          int PTR;
          if(VAR_TYPE[name]!="PTR"){
            BYTE_MAP[CB]+=8;
            PTR = BYTE_MAP[CB];
            PTR_MAPS[CB][name] = PTR;
            VAR_SIZE_MAPS[CB][name]=size;
          }
          else{
            PTR = PTR_MAPS[CB][name];
          }
          if(value != ";"){
            if(literal_or_var(value)=="ID"){
              out_stream <<
              "    mov rax, " << asm_get_value(value, PTR_MAPS[CB]) << "\n"
              "    mov " << asm_var(PTR, stoi(size)) << ", rax\n";
            }
            else{
              out_stream <<
              "    mov " << asm_var(PTR, stoi(size)) << ", " << asm_get_value(value, PTR_MAPS[CB]) << "\n";
            }
          }
          else break;
        }
        else if(size == "1"){
          BYTE_MAP[CB]+=1;
          int PTR = BYTE_MAP[CB];
          PTR_MAPS[CB][name] = PTR;
          VAR_SIZE_MAPS[CB][name]=size;
          if(value != ";"){
            if(literal_or_var(value)=="ID"){
              out_stream <<
              "    mov al, " << asm_get_value(value, PTR_MAPS[CB]) << "\n"
              "    mov " << asm_var(PTR, stoi(size)) << ", al\n";
            }
            else{
              out_stream <<
              "    mov " << asm_var(PTR, stoi(size)) << ", " << asm_get_value(value, PTR_MAPS[CB]) << "\n";
            }
          }
          else break;
        }
        else{
          cerr << "\033[1;31mUNRECOGNIZED SIZE!\033[0m\n";
          cerr << "SIZE: " << size << "\n"
          "VALUE: " << value << "\n";
          assert(false);
        }
        if(INTER[++i]==",") continue;
        if(INTER[i]==";") break;
      }
    }
    else if(tk == "end"){
      if(INTER[i+1]=="loop"){
        i++;
        vector<string> con;
        while(STACK[STACK.size()-1]!=";"){
          con.push_back(STACK[STACK.size()-1]);
          STACK.pop_back();
        }
        STACK.pop_back();
        con.push_back(";");
        out_stream <<
        ".loop" << BLOCK_STACK[BLOCK_STACK.size()-1] << "_in:\n";
        ss << "loop" << BLOCK_STACK[BLOCK_STACK.size()-1] << "_out";
        string label = ss.str();
        ss.str("");
        COMPILE_CONDITIONAL_JUMP(label, con, out_stream, PTR_MAPS[CB]);
        BLOCK_STACK.pop_back();
        i++;
      }
      else if(INTER[i+1] == "if"){
        i++;
        out_stream << STACK[STACK.size()-1];
        STACK.pop_back();
        STACK.pop_back();
        BLOCK_STACK.pop_back();
        i++;
      }
      else if(INTER[i+1] == "else"){
        i++;
        out_stream << STACK[STACK.size()-1];
        STACK.pop_back();
        STACK.pop_back();
        BLOCK_STACK.pop_back();
        i++;
      }
      else if(INTER[i+1] == "elif"){
        i++;
        out_stream << STACK[STACK.size()-1];
        STACK.pop_back();
        STACK.pop_back();
        BLOCK_STACK.pop_back();
        out_stream << STACK[STACK.size()-1];
        STACK.pop_back();
        STACK.pop_back();
        BLOCK_STACK.pop_back();
        i++;
      }
      else{
        if(CB=="_start"){
          out_stream <<
  "    pop rbp\n"
  "    mov rax, 60\n"
  "    mov rdi, 0\n"
  "    syscall\n";
        }
        else if(RET_FNS.find(CB)==RET_FNS.end()){
          out_stream <<
          "    pop rbp\n"
          "    ret\n";
        }
        i++;
      }
    }
    else if(tk == "in" || tk == "insx"){
      string next_command = tk;
      int arg_count = 0;
      while(next_command=="in" || next_command=="insx"){
        string size = INTER[++i];
        string value = INTER[++i];
        string reg;
        if(INTER[i+1]!="to"){
          if(size == "4")reg = FUNC_ARG_REGISTER_LIST32[arg_count];
          else if(size == "8" || size == "0")reg = FUNC_ARG_REGISTER_LIST64[arg_count];
          else if(size == "1")reg = FUNC_ARG_REGISTER_LIST8[arg_count];
        }
        else{
          i++;
          reg = INTER[++i];
        }
        if(size == "0"){
          out_stream <<
          "    mov " << reg << ", [" <<value<<"]\n";
        }
        else if(next_command == "in"){
          out_stream <<
          "    mov " << reg <<", "<<asm_get_value(value, PTR_MAPS[CB]) << "\n";
        }
        else if(next_command == "insx"){
          out_stream <<
          "    movsx " << reg <<", "<<asm_get_value(value, PTR_MAPS[CB]) << "\n";
        }
        i++;
        next_command = INTER[++i];
        arg_count++;
      }
      if(next_command == "call"){
        i--;
      }
      else if(next_command == "syscall"){
        out_stream << "    syscall\n";
        i++;
      }
      else{
        cerr << next_command << "\n";
        cerr << "\033[1;31mUNREACHABLE!\033[0m\n";
        assert(false);
      }
    }
    else if(tk == "call"){
      out_stream << "    call " << INTER[++i] << "\n";
      if(FNV!=""){
        int PTR = PTR_MAPS[CB][FNV];
        out_stream <<
        "    mov " << asm_var(PTR, stoi(VAR_SIZE_MAPS[CB][FNV])) << ", " << RET_REG[INTER[i]] << "\n";
        FNV="";
      }
      i++;
    }
    else if(tk == "ret"){
      out_stream <<
      "    pop rbp\n"
      "    ret\n";
      i++;
    }
    else if(tk == "out"){
      string value = INTER[++i];
      if(INTER[++i]!="to"){
        cerr << "\033[1;31mUNREACHABLE\033[0m\n";
        assert(false);
      }
      string reg = INTER[++i];
      out_stream <<
      "    xor rax, rax\n"
      "    mov " << reg << ", " << asm_get_value(value, PTR_MAPS[CB]) << "\n";
      i++;
    }
    else if(tk == "regmov"){
      string value = INTER[++i];
      if(INTER[++i]!="to"){
        cerr << "\033[1;31mUNREACHABLE\033[0m\n";
        assert(false);
      }
      string reg = INTER[++i];
      out_stream <<
      "    mov " << asm_get_value(value, PTR_MAPS[CB]) << ", " << reg << "\n";
      i++;
    }
    else if(tk == "asm"){
      if(INTER[++i]!="out"){
        cerr << "\033[1;31mOUT REQUIRED IN ASM MACRO\033[0m\n";
        assert(false);
      }
      string out_reg = INTER[++i];
      string ASM = INTER[++i];
      while(ASM != ";"){
        if(ASM=="@"){
          out_stream << asm_get_value(INTER[++i], PTR_MAPS[CB]);
        }
        else{
          out_stream << ASM;
        }
        ASM = INTER[++i];
      }
      if(FNV!=""){
        int PTR = PTR_MAPS[CB][FNV];
        out_stream <<
        "    mov " << asm_var(PTR, stoi(VAR_SIZE_MAPS[CB][FNV])) << ", "<< out_reg <<"\n";
        FNV="";
      }
    }
    else if(tk == "alloc"){
      int bytes = stoi(INTER[++i]);
      BYTE_MAP[CB] += bytes;
      int PTR = BYTE_MAP[CB];
      if(FNV!=""){
        out_stream <<
        "    lea rax, [rbp-" << PTR << "]\n"
        "    mov " << asm_var(PTR_MAPS[CB][FNV], 8) << ", rax\n";
        FNV="";
      }
      i++;
    }
    else if(tk == "dealloc"){
      BYTE_MAP[CB] -= stoi(INTER[++i]);
    }
    else if(tk == "addr_sub"){
      string addr_var = INTER[++i];
      string additive = INTER[++i];
      if(FNV!=""){
        int PTR = PTR_MAPS[CB][FNV];
        out_stream <<
        "    mov rax, " << asm_get_value(addr_var, PTR_MAPS[CB]) << "\n";
        if(literal_or_var(additive)=="ID"){
          out_stream <<
          "    movsx rdx, " << asm_get_value(additive, PTR_MAPS[CB]) << "\n"
          "    sub rax, rdx\n";
        }
        else{
          out_stream <<
          "    sub rax, " << additive << "\n";
        }
        out_stream <<
        "    mov " << asm_var(PTR, 8) << ", rax\n";
        FNV="";
      }
      i++;
    }
    else if(tk == "addr_add"){
      string addr_var = INTER[++i];
      string additive = INTER[++i];
      if(FNV!=""){
        int PTR = PTR_MAPS[CB][FNV];
        out_stream <<
        "    mov rax, " << asm_get_value(addr_var, PTR_MAPS[CB]) << "\n";
        if(literal_or_var(additive)=="ID"){
          out_stream <<
          "    movsx rdx, " << asm_get_value(additive, PTR_MAPS[CB]) << "\n"
          "    add rax, rdx\n";
        }
        else{
          out_stream <<
          "    add rax, " << additive << "\n";
        }
        out_stream <<
        "    mov " << asm_var(PTR, 8) << ", rax\n";
        FNV="";
      }
      i++;
    }
    else if(tk == "addr_get"){
      int bytes = stoi(INTER[++i]);
      string ptr = INTER[++i];
      if(FNV!=""){
        string reg;
        string word = WORD_TABLE[bytes][0];
        if(bytes == 8) reg = "rax";
        else if(bytes == 4) reg = "eax";
        else if(bytes == 1) reg = "al";
        int PTR = PTR_MAPS[CB][FNV];
        out_stream <<
        "    mov rax, " << asm_get_value(ptr, PTR_MAPS[CB]) << "\n"
        "    mov " << reg << ", "<< WORD_TABLE[bytes][0] <<" [rax]\n"
        "    mov " << asm_var(PTR, bytes) << ", "<< reg <<"\n";
        FNV="";
      }
      i++;
    }
    else if(tk == "addr_equ"){
      int bytes = stoi(INTER[++i]);
      string ptr = INTER[++i];
      string value = INTER[++i];
      string reg;
      string word = WORD_TABLE[bytes][0];
      if(bytes == 8) reg = "rax";
      else if(bytes == 4) reg = "eax";
      else if(bytes == 1) reg = "al";
      out_stream <<
      "    mov rbx, " << asm_get_value(ptr, PTR_MAPS[CB]) << "\n";
      if(literal_or_var(value)=="ID"){
        out_stream <<
        "    mov " << reg << ", " << asm_get_value(value, PTR_MAPS[CB]) << "\n"
        "    mov " << word <<" [rbx], "<< reg <<"\n";
      }
      else{
        out_stream <<
        "    mov " << word <<" [rbx], "<< asm_get_value(value, PTR_MAPS[CB]) <<"\n";
      }
      i++;
    }
    else if(tk == "sleep"){
      string seconds = INTER[++i];
      string nanoseconds = INTER[++i];
      out_stream <<
      "    mov dword [tv_sec], "<< asm_get_value(seconds, PTR_MAPS[CB]) <<"\n"
      "    mov dword [tv_usec], "<< asm_get_value(nanoseconds, PTR_MAPS[CB]) <<"\n"
      "    mov rax, 0x23\n" //NR for nanosleep
      "    mov edi, timeval\n"
      "    mov esi, 0\n"
      "    syscall\n";
      i++;
    }
    else if(tk == "cast"){
      string type_to = INTER[++i];
      string var = INTER[++i];
      string type_of = VAR_TYPE[var];
      if(FNV!=""){
        int PTR = PTR_MAPS[CB][FNV];
        if(type_of == "BOOL"){
          if(type_to == "UINT"){
            out_stream <<
            "    xor eax, eax\n"
            "    movsx eax, " << asm_get_value(var, PTR_MAPS[CB]) << "\n"
            "    mov " << asm_var(PTR, 4) << ", eax\n";
          }
          else if(type_to == "CHAR"){
            out_stream <<
            "    movsx eax, " << asm_get_value(var, PTR_MAPS[CB]) << "\n"
            "    add eax, 48 \n"
            "    mov " << asm_var(PTR, 1) << ", al\n";
          }
          else{
            cerr << "\033[1;31mCASTING A '" << type_of << "'" << " TO A '" << type_to << "' IS NOT SUPPORTED!\033[0m\n";
            assert(false);
          }
        }
        else if(type_of == "CHAR"){
          if(type_to == "BOOL"){
            out_stream <<
            "    movsx eax, " << asm_get_value(var, PTR_MAPS[CB]) << "\n"
            "    sub eax, 48 \n"
            "    mov " << asm_var(PTR, 1) << ", al\n";
          }
          else if(type_to == "UINT"){
            out_stream <<
            "    movsx eax, " << asm_get_value(var, PTR_MAPS[CB]) << "\n"
            "    sub eax, 48 \n"
            "    mov " << asm_var(PTR, 4) << ", eax\n";
          }
          else{
            cerr << "\033[1;31mCASTING A '" << type_of << "'" << " TO A '" << type_to << "' IS NOT SUPPORTED!\033[0m\n";
            assert(false);
          }
        }
        else if(type_of == "UINT"){
          if(type_to == "BOOL"){
            out_stream <<
            "    mov eax, " << asm_get_value(var, PTR_MAPS[CB]) << "\n"
            "    mov " << asm_var(PTR, 1) << ", al\n";
          }
          else if(type_to == "CHAR"){
            out_stream <<
            "    mov eax, " << asm_get_value(var, PTR_MAPS[CB]) << "\n"
            "    mov " << asm_var(PTR, 1) << ", al\n";
          }
          else{
            cerr << "\033[1;31mCASTING A '" << type_of << "'" << " TO A '" << type_to << "' IS NOT SUPPORTED!\033[0m\n";
            assert(false);
          }
        }
        else{
          cerr << "\033[1;31mCASTING A '" << type_of << "'" << " TO A '" << type_to << "' IS NOT SUPPORTED!\033[0m\n";
          assert(false);
        }
        FNV="";
      }
      i++;
    }
    else if(tk == "open_fd"){
      string filename = INTER[++i];
      string flags = INTER[++i];
      string mode = INTER[++i];
      if(FNV!=""){
        out_stream <<
        "    mov rax, 2\n" // SYS_OPEN
        "    mov rdi, " << filename << "\n"
        "    mov esi, " << asm_get_value(flags, PTR_MAPS[CB]) << "\n"
        "    mov edx, " << asm_get_value(mode, PTR_MAPS[CB]) << "\n"
        "    syscall\n"
        "    mov " << asm_get_value(FNV, PTR_MAPS[CB]) << ", eax\n";
        i++;
        FNV="";
      }
    }
    else if(tk == "SECTION"){
      if(INTER[++i] == ".DATA"){
        out_stream << "SECTION .data\n"
        "@TRUE: db 'true'\n"
        "@TRUELEN   equ  $-@TRUE\n"
        "@FALSE: db 'false'\n"
        "@FALSELEN   equ  $-@FALSE\n"
        "timeval:\n"
        "   tv_sec  dd 0\n"
        "   tv_usec dd 0\n";
        i++;
        while(INTER[++i]!="/ESC/"){
          out_stream << INTER[i];
        }
      }
      else if(INTER[i] == ".BSS"){
        out_stream << "SECTION .bss\n";
        i++;
        while(INTER[++i]!="/ESC/"){
          out_stream << INTER[i];
        }
      }
    }
    else if(PTR_MAPS[CB].find(tk) != PTR_MAPS[CB].end()){
      if(INTER[++i]=="="){
        string type = literal_or_var(INTER[++i]);
        if(type == "ID" && OPS.find(INTER[i]) != OPS.end()){
          string in_reg = COMPILE_OP(i, INTER[i], INTER, out_stream, PTR_MAPS[CB]);
          int PTR = PTR_MAPS[CB][tk];
          out_stream <<
          "    mov " << asm_var(PTR, stoi(VAR_SIZE_MAPS[CB][tk])) << ", " << in_reg << "\n";
        }
        else{
          if(type == "ID"){
            type = VAR_TYPE[tk];
            int size = TYPE_BITS[type];
            int PTR = PTR_MAPS[CB][tk];
            string reg;
            if(size == 8) reg = "rax";
            else if(size == 4) reg = "eax";
            else if(size == 1) reg = "al";
            out_stream <<
            "    mov " << reg << ", " << asm_var(PTR_MAPS[CB][INTER[i]], stoi(VAR_SIZE_MAPS[CB][INTER[i]])) << "\n"
            "    mov " << asm_var(PTR, stoi(VAR_SIZE_MAPS[CB][tk])) << ", " << reg << "\n";
            i++;
          }
          else{
            int PTR = PTR_MAPS[CB][tk];
            out_stream <<
            "    mov " << asm_var(PTR, stoi(VAR_SIZE_MAPS[CB][tk])) << ", " << asm_get_value(INTER[i], PTR_MAPS[CB]) << "\n";
            i++;
          }
        }
      }
      else if(INTER[i]=="<--"){
        FNV = tk;
        i++;
      }
    }
    else if(OPS.find(tk) != OPS.end()){
      COMPILE_OP(i, tk, INTER, out_stream, PTR_MAPS[CB]);
    }
    else{
      cerr << "\033[1;31mUNSUPPORTED COMMAND\033[0m\n" <<
      tk <<"\n"
      << INTER[i-1] << "\n"
      << INTER[i-2] << "\n"
      << INTER[i-3] << "\n";
      assert(false);
    }
  }
}

namespace flags{
  void DEBUGFLAG(string NA){
    DEBUG_MODE = true;
  }
  
  void RUNFLAG(string NA){
    RUN = true;
  }
  
  void ADPL_FILE(string FILENAME){
    FILENAME.erase(FILENAME.size()-1);
    if(FILENAME.find(".adpl") != string::npos){
      FILENAME.erase(FILENAME.size()-5);
      FILE_BASENAME = FILENAME;
    }else{
      FILE_BASENAME = FILENAME;
    }
  }
  
  void EXEC_FILE_SET(string EXEC_FILENAME){
    EXEC_FILE = EXEC_FILENAME;
  }
  
  void VERSIONFLAG(string NA){
    cout << "ADPL VERSION 0.0.9\n";
  }
}

int main(int argc, char** argv){
  assert(argc > 0);
  start_timer();
  
  string help_desc = "This is the ADPL help desc!!!\nYou can bring up this page by using the --help flag.";
  kf::FlagProxy prox(help_desc);
  
  string version_flag = "--version";
  string version_flag_desc = "Provides the version of ADPL being used.";
  prox.AddFlag(version_flag, flags::VERSIONFLAG, version_flag_desc, 0);
  
  string debug_flag = "-d";
  string debug_flag_desc = "Prints debug text while compiling.";
  prox.AddFlag(debug_flag, flags::DEBUGFLAG, debug_flag_desc, 0);
  
  string run_flag = "-r";
  string run_flag_desc = "Runs program after compilation.";
  prox.AddFlag(run_flag, flags::RUNFLAG, run_flag_desc, 0);
  
  string file_flag = "-f";
  string file_flag_desc = "Provides ADPL file to compiler (Required).";
  prox.AddFlag(file_flag, flags::ADPL_FILE, file_flag_desc, 1);
  
  string exec_flag = "-o";
  string exec_flag_desc = "Name the exec file.";
  prox.AddFlag(exec_flag, flags::EXEC_FILE_SET, exec_flag_desc, 1);
  
  prox.AddHelp("--build", "Rebuilds the compiler.");
  prox.AddHelp("For additional info check README.md and FUNCTIONS.txt in /home/$USR/ADPL/.");
  
  prox.Parse(argc, argv);
  
  char tmp[PATH_MAX];
  getcwd(tmp, sizeof(tmp));
  string directory(tmp);
  
  if(FILE_BASENAME!=""){
    
    //INIT OF ADPL AND ASM FILES
    ss << FILE_BASENAME << ".adpl";
    string adpl_file = directory+'/'+ss.str();
    ss.str("");
    if(DEBUG_MODE) cout << "ADPL FILE:\n" << adpl_file << "\n";
    
    ss << FILE_BASENAME << ".asm";
    string asm_file = directory+'/'+ss.str();
    ss.str("");
    if(DEBUG_MODE) cout << "\nASM FILE:\n" << asm_file << "\n";
    
    //COMPILATION OF ADPL
    INIT_TABLE();
    
    
    stop_timer("START");
    start_timer();
    
    //LEXING
    if(DEBUG_MODE) cout << "\nTOKEN STREAM:\n";
    LEXER(adpl_file.c_str());
    if(DEBUG_MODE){
      for(pair<string, string> &tk : TK_STREAM){
        cout<<"("<<tk.first<<","<<tk.second<<"), ";
        if(tk.first=="NL"){
          cout<<"\n";
        }
      }
      cout << "\n";
    }
    stop_timer("LEXER");
    
    //CHECK SYNTAX ERRORS ECT.
    start_timer();
    CHECK_GRAMMER();
    if(DEBUG_MODE){
      cout << "\nEDITTED TKS:\n";
      for(pair<string, string> &tk : TK_STREAM){
        cout<<"("<<tk.first<<","<<tk.second<<"), ";
        if(tk.first=="NL"){
          cout<<"\n";
        }
      }
      cout << "\n";
    }
    stop_timer("GRAMMER CHECK");
    
    //TYPE CHECK
    start_timer();
    INIT_FUNC_ARG_REGISTER_LIST();
    vector<string> INTER_CODE = INTERMEDIATE();
    if(DEBUG_MODE){
      for(string &tk : INTER_CODE){
        cout<<tk<<" ";
        if(tk==";"){
          cout<<"\n";
        }
      }
      cout << "\n";
    }
    PRESCAN(INTER_CODE);
    stop_timer("INTERMEDIATE CODE GEN");
    
    start_timer();
    COMPILE(asm_file, INTER_CODE);
    stop_timer("COMPILATION");
    
    start_timer();
  
    //COMPILATION OF ASM CODE
    ss << "nasm -felf64 -o "<< FILE_BASENAME << ".o " << asm_file;
    string cmd_line = ss.str();
    const char* char_line = cmd_line.c_str();
    ss.str("");
    system(char_line);
    
    vector<string> LINK_BASENAMES;
    
    for(int i = 0; i < LINKED_FILES.size(); i++){
      cout << LINKED_FILES[i] << "\n";
      //BROKE AS HELL, DW AB IT
      // if(LINKED_FILES[i].find(".adpl")){
      //   //ADD -nr FLAG (DOESN'T RUN PROG)
      //   //ADD -nc DOESN'T COMPILE NASM
      //   //REVAMP FLAG SYSTEM
      //   //CAN ONLY LINK ASM FOR NOW
      //   //DESIRED RESULT:
      //   //  ;;include 'stdlib.adpl'
      //   // cerr << "\033[1;31m[TODO!]CAN'T LINK ADPL FOR NOW!\033[0m\n";
      //   // assert(false);
      // }
      // else{
        string raw = LINKED_FILES[i];
        string linking_basename = "";
        for(int j = 0; j < raw.size()-4; j++){
          linking_basename += raw[j];
        }
        ss << "nasm -felf64 -o "<< linking_basename << ".o " << raw;
        string cmd_line = ss.str();
        const char* char_line = cmd_line.c_str();
        ss.str("");
        system(char_line);
        if(DEBUG_MODE)cerr << char_line << "\n";
        LINK_BASENAMES.push_back(linking_basename);
      // }
    }
      
    if(EXEC_FILE!=""){
      ss << "ld -z muldefs -o " << EXEC_FILE << " " << FILE_BASENAME << ".o ";
    }
    else{
      ss << "ld -z muldefs -o " << FILE_BASENAME << " " << FILE_BASENAME << ".o ";
    }
    for(string &s : LINK_BASENAMES){
      ss << s << ".o ";
    }
    cmd_line = ss.str();
    char_line = cmd_line.c_str();
    ss.str("");
    cerr << cmd_line << "\n";
    system(char_line);
    
    stop_timer("\nCOMPILATION OF ASM");
    if(RUN){
      start_timer();
      
      cout << "\nPROGRAM OUTPUT:\n";
      
      ss << "./" << FILE_BASENAME;
      cmd_line = ss.str();
      char_line = cmd_line.c_str();
      ss.str("");
      system(char_line);
      
      stop_timer("RUN");
      
      cout << "\n";
      
      start_timer();
    }
    
    //DELETES .O FILE
    if(DEBUG_MODE) cout << "DELETING "<< FILE_BASENAME << ".o...\n";
    ss << "shred -u " << FILE_BASENAME << ".o";
    cmd_line = ss.str();
    char_line = cmd_line.c_str();
    ss.str("");
    system(char_line);
    if(DEBUG_MODE) cout << "DONE!\n";
    
    for(string &s : LINK_BASENAMES){
      if(DEBUG_MODE) cout << "DELETING "<< s << ".o...\n";
      ss << "shred -u " << s << ".o";
      cmd_line = ss.str();
      char_line = cmd_line.c_str();
      ss.str("");
      system(char_line);
      if(DEBUG_MODE) cout << "DONE!\n";
    }
    
    //DELETES .ASM FILE
    if(DEBUG_MODE) cout << "DELETING "<< asm_file << "...\n";
    ss << "shred -u " << asm_file;
    cmd_line = ss.str();
    char_line = cmd_line.c_str();
    ss.str("");
    system(char_line);
    if(DEBUG_MODE) cout << "DONE!\n";
    
    stop_timer("DELETION");
  }
  else{
    cerr << "\033[1;31mPLEASE PROVIDE A FILE WITH THE -f FLAG\033[0m\n";
  }
  
  return 0;
}