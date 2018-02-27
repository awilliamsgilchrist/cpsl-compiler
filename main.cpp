#include"main.h"
extern int yyparse();

int* type_int = new int(5);
int* type_bool = new int(5);
int* type_char = new int(5);
int* type_string = new int(5);

SymbolTable symbol_table;
std::vector<std::string> string_list;
std::stack<std::string> register_pool;

std::ofstream out("out.asm");

//Helper functions

std::string getStringListIdentifier(std::string str)
{
	auto it = std::find(string_list.begin(), string_list.end(), str);
	int index = std::distance(string_list.begin(), it);
	return "STR" + std::to_string(index);
}

std::string getRegister()
{
	if(register_pool.size() > 0)
	{
		std::string reg = register_pool.top();
		register_pool.pop();
		return "$" + reg;
	}
	else
	{
		return "";
	}
}

void restoreRegister(std::string reg)
{
	std::string nReg = "it";
	nReg[0] = reg[1];
	nReg[1] = reg[2];
	register_pool.push(nReg);
}

//Here follow the code generation functions

void outBlock()
{
	out << ".data\n";
	for(std::string str : string_list)
	{
		out << getStringListIdentifier(str) << ": .asciiz " << str << std::endl;
	}
	
	out << "GA:";
}

void outWriteStatement(std::vector<Express*>* vect)
{
	std::cout << "This is getting called" << std::endl;

	for(unsigned int i = 0; i < vect->size(); i++)
	{
		if(vect->at(i)->type_ptr == type_string)
		{
			out << "li $v0, 4" << std::endl;
			out << "la $a0, STR" << vect->at(i)->raw_val << std::endl;
		}
		else if(vect->at(i)->type_ptr == type_char)
		{
			out << "li $v0, 11" << std::endl;
			out << "li $a0, " << vect->at(i)->raw_val << std::endl;
		}
		else
		{
			out << "li $v0, 1" << std::endl;
			out << "li $a0, " << vect->at(i)->raw_val << std::endl;
		}
		
		out << "syscall" << std::endl;
	}
}

void outAssignment(std::string str, Express* expr)
{
	Express* oldExpr = symbol_table.findExpr(str);
	std::string reg = getRegister();
	if(expr->type_ptr == oldExpr->type_ptr)
	{
		if(expr->type_ptr != type_string)
		{
			out << "li " << reg << ", " << expr->raw_val << std::endl;
			out << "sw " << reg << ", " << symbol_table.offset << "($gp)" << std::endl;
			
			oldExpr->raw_val = symbol_table.offset;
			oldExpr->regist = true;
			symbol_table.offset += 4;
		}
		else
		{
			oldExpr->raw_val = expr->raw_val;
		}
	}
	
	restoreRegister(reg);
}

int main()
{
	out << ".text\n";
	out << "main:" << std::endl;
	out << "la GA, $gp" << std::endl;
	
	//Initialize register_pool
	register_pool.push("a1");
	register_pool.push("a2");
	register_pool.push("a3");
	register_pool.push("v1");
	register_pool.push("s0");
	register_pool.push("s1");
	register_pool.push("s2");
	register_pool.push("s3");
	register_pool.push("s4");
	register_pool.push("s5");
	register_pool.push("s6");
	register_pool.push("s7");
	register_pool.push("t0");
	register_pool.push("t1");
	register_pool.push("t2");
	register_pool.push("t3");
	register_pool.push("t4");
	register_pool.push("t5");
	register_pool.push("t6");
	register_pool.push("t7");
	register_pool.push("t8");
	
	symbol_table.addType("integer", type_int);
	symbol_table.addType("INTEGER", type_int);
	symbol_table.addType("boolean", type_bool);
	symbol_table.addType("BOOLEAN", type_bool);
	symbol_table.addType("char", type_char);
	symbol_table.addType("CHAR", type_char);
	symbol_table.addType("string", type_string);
	symbol_table.addType("STRING", type_string);
	
	string_list.push_back("");
	
	symbol_table.stepInContext();
	
	yyparse();
};
