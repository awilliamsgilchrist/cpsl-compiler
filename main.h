#include<algorithm>
#include<vector>
#include<fstream>

#include"Express.h"
#include"SymbolTable.h"

extern Type* type_int;
extern Type* type_bool;
extern Type* type_char;
extern Type* type_string;

const std::string GLOBAL_PTR = "($gp)";
extern std::string label_auto();

extern SymbolTable symbol_table;
extern std::vector<std::string> string_list;
extern std::ofstream out;
extern std::stack<std::string> register_pool;

extern std::string getStringListIdentifier(std::string str);

extern void outBlock();
extern void outWriteStatement(std::vector<Express*>*);
extern void outReadStatement(std::vector<std::string>*);
extern void outAssignment(std::string, Express*);
extern Express* boolCompare(Express*, Express*, std::string);
extern Express* intCompare(Express*, Express*, std::string);
extern Express* uniCompare(Express*, std::string);
extern void outStopStatement();
