#include<algorithm>
#include<vector>
#include<fstream>

#include"Express.h"
#include"SymbolTable.h"

extern int* type_int;
extern int* type_bool;
extern int* type_char;
extern int* type_string;

extern SymbolTable symbol_table;
extern std::vector<std::string> string_list;
extern std::ofstream out;
extern std::stack<std::string> register_pool;

extern std::string getStringListIdentifier(std::string str);

extern void outBlock();
extern void outWriteStatement(std::vector<Express*>*);
extern void outAssignment(std::string, Express*);
