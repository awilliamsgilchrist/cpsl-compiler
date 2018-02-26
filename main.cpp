#include<vector>
#include<fstream>

#include"Expression.h"
#include"SymbolTable.h"
extern int yyparse();

//int is 0, bool is 1, char is 2, string is 3
int** type_list = new int*[4];
for(unsigned int i = 0; i < 4; i++)
{
	*type_list[i] = 5;
}

SymbolTable symbol_table;

std::vector<std::string> string_list;

std::ofstream out("out.asm");

//Helper functions

std::string getStringListIdentifier(std::string str)
{
	auto it = std::find(string_list.begin(), string_list.end(), str);
	int index = std::distance(string_list.begin(), it);
	return "STR" + std::to_string(index);
}

//Here follow the code generation functions

void outBlock()
{
	out << ".data\n";
	for(std::string str : string_list)
	{
		out << getStringListIdentifier(str) << ": asciiz " << str << std::endl;
	}
}

void outWriteStatement(std::string cont)
{
	out << "la $a0, " << getStringListIdentifier(cont) << std::endl;
	out << "li $v0, 4" << std::endl;
	out << "syscall" << std::endl;
}

int main()
{
	out << ".text\n";
	
	yyparse();
};
