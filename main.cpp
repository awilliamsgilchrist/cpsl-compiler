#include"main.h"
extern int yyparse();

int* type_int = new int(5);
int* type_bool = new int(5);
int* type_char = new int(5);
int* type_string = new int(5);

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

void outWriteStatement(std::vector<Express*>* vect)
{
	for(unsigned int i = 0; i < vect->size(); i++)
	{
		if(*vect[i]->type_ptr == type_string)
		{
			out << "li $v0, 4" << std::endl;
			out << "la $a0, STR" << *vect[i]->raw_value << std::endl;
		}
		else if(*vect[i]->type_ptr == type_char)
		{
			out << "li $v0, 11" << std::endl;
			out << "li $a0, " << *vect[i]->raw_value << std::endl;
		}
		else
		{
			out << "li $v0, 1" << std::endl;
			out << "li $a0, " << *vect[i]->raw_value << std::endl;
		}
		
		out << "syscall" << std::endl;
	}
}

int main()
{
	out << ".text\n";
	
	yyparse();
};
