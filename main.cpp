#include"main.h"
extern int yyparse();

int* type_int = new int(5);
int* type_bool = new int(5);
int* type_char = new int(5);
int* type_string = new int(5);

int auto_counter = 0;

std::string label_auto()
{
	auto_counter++;
	return "a" + std::to_string(auto_counter);
}

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
	
	out << "GA: .align 2";
}

void outWriteStatement(std::vector<Express*>* vect)
{
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
			if(vect->at(i)->regist)
			{
				out << "lw $a0, " << vect->at(i)->raw_val << "($gp)" << std::endl;
			}
			else
			{
				out << "li $a0, " << vect->at(i)->raw_val << std::endl;
			}
		}
		else
		{
			out << "li $v0, 1" << std::endl;
			if(vect->at(i)->regist)
			{
				out << "lw $a0, " << vect->at(i)->raw_val << "($gp)" << std::endl;
			}
			else
			{
				out << "li $a0, " << vect->at(i)->raw_val << std::endl;
			}
		}
		
		out << "syscall" << std::endl;
	}
}

void outReadStatement(std::vector<std::string>* vect)
{
	for(unsigned int i = 0; i < vect->size(); i++)
	{
		Express* expr = symbol_table.findExpr(vect->at(i));
		
		if(expr->type_ptr == type_int)
		{
			out << "li $v0, 5" << std::endl;
		}
		else if(expr->type_ptr == type_char)
		{
			out << "li $v0, 12" << std::endl;
		}
		else
		{
			std::cerr << "Error: read must have parameters of type integer or char" << std::endl;
		}
		
		out << "syscall" << std::endl;
		
		if(expr->regist)
		{
			out << "sw $v0, " << expr->raw_val << GLOBAL_PTR << std::endl;
		}
		else
		{
			out << "sw $v0, " << symbol_table.offset << GLOBAL_PTR << std::endl;
			expr->raw_val = symbol_table.offset;
			expr->regist = true;
			symbol_table.offset += 4;
		}
		
		symbol_table.removeExpr(vect->at(i));
		symbol_table.addExpr(vect->at(i), *expr);
	}
}

void outAssignment(std::string str, Express* expr)
{
	Express* oldExpr = symbol_table.findExpr(str);
	std::string reg = getRegister();
	if(expr->type_ptr == oldExpr->type_ptr && !expr->regist)
	{
		if(expr->type_ptr != type_string)
		{
			out << "li " << reg << ", " << expr->raw_val << std::endl;
			out << "sw " << reg << ", " << symbol_table.offset << GLOBAL_PTR << std::endl;
			
			oldExpr->raw_val = symbol_table.offset;
			oldExpr->regist = true;
			symbol_table.offset += 4;
			symbol_table.removeExpr(str);
			symbol_table.addExpr(str, *oldExpr);
		}
		else
		{
			oldExpr->raw_val = expr->raw_val;
		}
	}
	else if(expr->type_ptr == oldExpr->type_ptr)
	{
		out << "lw " << reg << ", " << expr->raw_val << GLOBAL_PTR << std::endl;
		if(oldExpr->regist)
		{
			out << "sw " << reg << oldExpr->raw_val << GLOBAL_PTR << std::endl;
		}
		else
		{
			out << "sw " << reg << ", " << symbol_table.offset << GLOBAL_PTR << std::endl;
			
			oldExpr->raw_val = symbol_table.offset;
			oldExpr->regist = true;
			symbol_table.offset += 4;
		}
		
		symbol_table.removeExpr(str);
		symbol_table.addExpr(str, *oldExpr);
	}
	else 
	{
		std::cerr << "Error: lvalue " << str << " and rvalue incompatible types" << std::endl;
	}
	restoreRegister(reg);
}

Express* boolCompare(Express* expr1, Express* expr2, std::string kind)
{
	std::string reg1 = getRegister();
	std::string reg2 = getRegister();
	std::string label = label_auto();
	std::string endLabel = label_auto();
	Express* nExpress;
	
	//This section does constant folding
	if(!expr1->regist && !expr2->regist)
	{
		int val1 = expr1->raw_val;
		int val2 = expr2->raw_val;
		
		if(kind == "and")
		{
			nExpress = new Express(type_bool, (int)(val1 && val2));
		}
		else if(kind == "or")
		{
			nExpress = new Express(type_bool, (int)(val1 || val2));
		}
		else if(kind == "less")
		{
			nExpress = new Express(type_bool, (int)(val1 < val2));
		}
		else if(kind == "great")
		{
			nExpress = new Express(type_bool, (int)(val1 > val2));
		}
		else if(kind == "lessEq")
		{
			nExpress = new Express(type_bool, (int)(val1 <= val2));
		}
		else if(kind == "great")
		{
			nExpress = new Express(type_bool, (int)(val1 >= val2));
		}
		else if(kind == "eq")
		{
			nExpress = new Express(type_bool, (int)(val1 == val2));
		}
		else if(kind == "neq")
		{
			nExpress = new Express(type_bool, (int)(val1 != val2));
		}
		
		restoreRegister(reg1);
		restoreRegister(reg2);
		return nExpress;
	}
	
	if(expr1->regist)
	{
		out << "lw " << reg1 << ", " << expr1->raw_val << GLOBAL_PTR << std::endl;
	}
	else
	{
		out << "li " << reg1 << ", " << expr1->raw_val << std::endl;
	}
	if(expr2->regist)
	{
		out << "lw " << reg2 << ", " << expr2->raw_val << GLOBAL_PTR << std::endl;
	}
	else
	{
		out << "li " << reg2 << ", " << expr2->raw_val << std::endl;
	}
	
	
	if(kind == "and")
	{
		out << "beq $zero, " << reg1 << ", " << label << std::endl;
		out << "beq $zero, " << reg2 << ", " << label << std::endl;
		out << "li " << reg1 << ", 1" << std::endl;
		out << "j " << endLabel << std::endl;
		out << label << ": li " << reg1 << ", 0" << std::endl;
	}
	else if(kind == "or")
	{
		out << "bne $zero, " << reg1 << ", " << label << std::endl;
		out << "bne $zero, " << reg2 << ", " << label << std::endl;
		out << "li " << reg1 << ", 0" << std::endl;
		out << "j " << endLabel << std::endl;
		out << label << ": li " << reg1 << ", 1" << std::endl;
	}
	else if(kind == "less")
	{
		out << "sub " << reg1 << ", " << reg1 << ", " << reg2 << std::endl;
		out << "bltz " << reg1 << ", " << label << std::endl;
		out << "li " << reg1 << ", 0" << std::endl;
		out << "j " << endLabel << std::endl;
		out << label << ": li " << reg1 << ", 1" << std::endl;
	}
	else if(kind == "great")
	{
		out << "sub " << reg1 << ", " << reg1 << ", " << reg2 << std::endl;
		out << "bltz " << reg1 << ", " << label << std::endl;
		out << "li " << reg1 << ", 1" << std::endl;
		out << "j " << endLabel << std::endl;
		out << label << ": li " << reg1 << ", 0" << std::endl;
	}
	else if(kind == "lessEq")
	{
		out << "sub " << reg1 << ", " << reg1 << ", " << reg2 << std::endl;
		out << "blez " << reg1 << ", " << label << std::endl;
		out << "li " << reg1 << ", 0" << std::endl;
		out << "j " << endLabel << std::endl;
		out << label << ": li " << reg1 << ", 1" << std::endl;
	}
	else if(kind == "greatEq")
	{
		out << "sub " << reg1 << ", " << reg1 << ", " << reg2 << std::endl;
		out << "blez " << reg1 << ", " << label << std::endl;
		out << "li " << reg1 << ", 1" << std::endl;
		out << "j " << endLabel << std::endl;
		out << label << ": li " << reg1 << ", 0" << std::endl;
	}
	else if(kind == "eq")
	{
		out << "beq " << reg1 << ", " << reg2 << ", " << label << std::endl;
		out << "li " << reg1 << ", 0" << std::endl;
		out << "j " << endLabel << std::endl;
		out << label << ": li " << reg1 << ", 1" << std::endl;
	}
	else if(kind == "neq")
	{
		out << "bne " << reg1 << ", " << reg2 << ", " << label << std::endl;
		out << "li " << reg1 << ", 0" << std::endl;
		out << "j " << endLabel << std::endl;
		out << label << ": li " << reg1 << ", 1" << std::endl;
	}
	
	out << endLabel << ": " << std::endl;
	out << "sw " << reg1 << ", " << symbol_table.offset << GLOBAL_PTR << std::endl;
	
	nExpress = new Express(type_bool, symbol_table.offset, true);
	symbol_table.offset += 4;
	
	restoreRegister(reg1);
	restoreRegister(reg2);
	return nExpress;
}

Express* intCompare(Express* expr1, Express* expr2, std::string kind)
{
	Express* nExpress;
	
	std::cout << expr1->raw_val << std::endl << expr1->regist << std::endl;
	std::cout << expr2->raw_val << std::endl << expr2->regist << std::endl;
	
	//This section does constant folding
	if(!expr1->regist && !expr2->regist)
	{
		int val1 = expr1->raw_val;
		int val2 = expr2->raw_val;
		
		if(kind == "plus")
		{
			nExpress = new Express(type_int, val1 + val2);
		}
		else if(kind == "sub")
		{
			nExpress = new Express(type_int, val1 - val2);
		}
		else if(kind == "mult")
		{
			nExpress = new Express(type_int, val1 * val2);
		}
		else if(kind == "div")
		{
			nExpress = new Express(type_int, val1 / val2);
		}
		else if(kind == "mod")
		{
			nExpress = new Express(type_int, val1 % val2);
		}
		
		return nExpress;
	}
	
	std::string reg1 = getRegister();
	std::string reg2 = getRegister();
	std::string label = label_auto();
	
	if(expr1->regist)
	{
		out << "lw " << reg1 << ", " << expr1->raw_val << GLOBAL_PTR << std::endl;
	}
	else
	{
		out << "li " << reg1 << ", " << expr1->raw_val << std::endl;
	}
	if(expr2->regist)
	{
		out << "lw " << reg2 << ", " << expr2->raw_val << GLOBAL_PTR << std::endl;
	}
	else
	{
		out << "li " << reg2 << ", " << expr2->raw_val << std::endl;
	}
	
	
	if(kind == "plus")
	{
		out << "add " << reg1 << ", " << reg1 << ", " << reg2 << std::endl;
	}
	else if(kind == "sub")
	{
		out << "sub " << reg1 << ", " << reg1 << ", " << reg2 << std::endl;
	}
	else if(kind == "mult")
	{
		out << "mult " << reg1 << ", " << reg2 << std::endl;
		out << "mflo " << reg1 <<  std::endl;
	}
	else if(kind == "div")
	{
		out << "div " << reg1 << ", " << reg2 << std::endl;
		out << "mflo " << reg1 <<  std::endl; 
	}
	else if(kind == "mod")
	{
		out << "div " << reg1 << ", " << reg2 << std::endl;
		out << "mfhi " << reg1 << std::endl;
	}
	
	out << "sw " << reg1 << ", " << symbol_table.offset << GLOBAL_PTR << std::endl;
	
	nExpress = new Express(type_int, symbol_table.offset, true);
	symbol_table.offset += 4;
	
	restoreRegister(reg1);
	restoreRegister(reg2);
	return nExpress;
}

int main()
{
	out << ".text\n";
	out << "main:" << std::endl;
	out << "la $gp, GA" << std::endl;
	
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
	
	std::cout << "About to enter yyparse" << std::endl;
	
	yyparse();
};
