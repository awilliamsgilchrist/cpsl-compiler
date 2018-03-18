#include"main.h"
extern int yyparse();

Type* type_int = new Type();
Type* type_bool = new Type();
Type* type_char = new Type();
Type* type_string = new Type();

int auto_counter = 0;
int ref_counter = 0;

std::string label_auto()
{
	auto_counter++;
	return "a" + std::to_string(auto_counter);
}

std::string name_ref()
{
	ref_counter++;
	return "ref_expr" + std::to_string(ref_counter);
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
		std::string res = std::to_string(symbol_table.offset) + GLOBAL_PTR;
		symbol_table.offset += 4;
		return res;
	}
}

void restoreRegister(std::string reg)
{
	if(reg.size() > 3)
	{
		return;
	}
	std::string nReg = "it";
	nReg[0] = reg[1];
	nReg[1] = reg[2];
	register_pool.push(nReg);
}

Express* arLvalHelper(Express* refExpr, std::string arr)
{
	Express* nExpr = new Express(refExpr->type_ptr, -10, true);
	nExpr->arr_expr = symbol_table.findExpr(arr);
	nExpr->ref_expr = refExpr;
	std::cout << nExpr << std::endl;
	std::cout << nExpr->ref_expr << std::endl;
	return nExpr;
}

//Here follow the code generation functions

//This gets a register containing the address where the content of an unknowable reference is stored
//NOTE: Does not free its register. Must be manually freed after calling
std::string outRefReg(Express* expr, bool isFirstCall, std::string reg = "")
{
	if(reg == "")
	{
		reg = getRegister();
	}
	
	if(!expr->ref_expr)
	{
		out << "lw " << reg << ", " << expr->raw_val << GLOBAL_PTR << std::endl;
	}
	else
	{
		reg = outRefReg(expr->ref_expr, false, reg);
		out << "addi " << reg <<", " << reg << ", -" << expr->arr_expr->type_ptr->min << std::endl;
		out << "sll " << reg << ", " << reg << ", 2" << std::endl;
		std::cout << expr->ref_expr << std::endl;
		std::cout << expr->ref_expr->arr_expr << std::endl;
		out << "addi " << reg << ", " << reg << ", " << expr->ref_expr->arr_expr->raw_val << std::endl;
		out << "add " << reg << ", $gp, " << reg << std::endl;
		
		if(!isFirstCall)
		{
			out << "lw " << reg << ", " << reg << std::endl;
		}
	}
	
	
	return reg;
}

void outBlock()
{
	out << "END: " << std::endl;
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
		if(vect->at(i)->ref_expr)
		{
			std::string reg = outRefReg(vect->at(i), true);
			
			if(vect->at(i)->type_ptr == type_char)
			{
				out << "li $v0, 11" << std::endl;
			}
			else
			{
				out << "li $v0, 1" << std::endl;
			}
			
			out << "lw $a0, (" << reg << ")" << std::endl;
			
			restoreRegister(reg);
		}
		else
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
					out << "lw $a0, " << vect->at(i)->raw_val << GLOBAL_PTR << std::endl;
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
					out << "lw $a0, " << vect->at(i)->raw_val << GLOBAL_PTR << std::endl;
				}
				else
				{
					out << "li $a0, " << vect->at(i)->raw_val << std::endl;
				}
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
		
		if(expr->ref_expr)
		{
			std::string reg = outRefReg(expr, true);
			
			out << "sw $v0, 0(" << reg << ")" << std::endl;
			
			restoreRegister(reg);
		}
		else if(expr->regist)
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
		symbol_table.addExpr(vect->at(i), expr);
	}
}

void outAssignment(std::string str, Express* expr)
{
	std::cout << "Key: " << str << std::endl;
	
	Express* oldExpr = symbol_table.findExpr(str);
	
	std::cout << "OldExpr: " << oldExpr << std::endl;
	std::cout << "RefExpr: " << oldExpr->ref_expr << std::endl; 
	std::cout << "Expr: " << expr << std::endl;
	
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
			symbol_table.addExpr(str, oldExpr);
		}
		else
		{
			oldExpr->raw_val = expr->raw_val;
		}
	}
	else if(expr->type_ptr == oldExpr->type_ptr)
	{
		if(oldExpr->regist)
		{
			out << "lw " << reg << ", " << expr->raw_val << GLOBAL_PTR << std::endl;
			
			if(oldExpr->ref_expr)
			{
				std::string refReg = outRefReg(oldExpr, true);
				
				out << "sw " << reg << ", 0(" << refReg << ")" << std::endl;
				
				restoreRegister(refReg);
			}
			else
			{
				out << "sw " << reg << ", " << oldExpr->raw_val << GLOBAL_PTR << std::endl;
			}
		}
		else
		{
			out << "li " << reg << ", " << expr->raw_val << std::endl;
			out << "sw " << reg << ", " << symbol_table.offset << GLOBAL_PTR << std::endl;
			
			oldExpr->raw_val = symbol_table.offset;
			oldExpr->regist = true;
			symbol_table.offset += 4;
		}
		
		//symbol_table.removeExpr(str);
		//symbol_table.addExpr(str, oldExpr);
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
	
	if(expr1->ref_expr)
	{
		std::string refReg = outRefReg(expr1, true);
		
		out << "lw " << reg1 << ", 0(" << refReg << ")" << std::endl;
		
		restoreRegister(refReg);
	}
	else if(expr1->regist)
	{
		out << "lw " << reg1 << ", " << expr1->raw_val << GLOBAL_PTR << std::endl;
	}
	else
	{
		out << "li " << reg1 << ", " << expr1->raw_val << std::endl;
	}
	if(expr2->ref_expr)
	{
		std::string refReg = outRefReg(expr2, true);
		
		out << "lw " << reg2<< ", 0(" << refReg << ")" << std::endl;
		
		restoreRegister(refReg);
	}
	else if(expr2->regist)
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
		out << "blez " << reg1 << ", " << label << std::endl;
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
		out << "bltz " << reg1 << ", " << label << std::endl;
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
	
	if(expr1->ref_expr)
	{
		std::string refReg = outRefReg(expr1, true);
		
		out << "lw " << reg1 << ", 0(" << refReg << ")" << std::endl;
		
		restoreRegister(refReg);
	}
	else if(expr1->regist)
	{
		out << "lw " << reg1 << ", " << expr1->raw_val << GLOBAL_PTR << std::endl;
	}
	else
	{
		out << "li " << reg1 << ", " << expr1->raw_val << std::endl;
	}
	if(expr2->ref_expr)
	{
		std::string refReg = outRefReg(expr2, true);
		
		out << "lw " << reg2<< ", 0(" << refReg << ")" << std::endl;
		
		restoreRegister(refReg);
	}
	else if(expr2->regist)
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

Express* uniCompare(Express* expr, std::string kind)
{
	Express* nExpr;
	
	if(kind == "chr")
	{
		if(expr->type_ptr == type_int)
		{
			nExpr = new Express(type_char, expr->raw_val, expr->regist);
		}
		else
		{
			std::cerr << "Error: chr undefined for non-integer types" << std::endl;
		}
	}
	else if(kind == "ord")
	{
		if(expr->type_ptr == type_char)
		{
			nExpr = new Express(type_int, expr->raw_val, expr->regist);
		}
		else
		{
			std::cerr << "Error: ord undefined for non-character types" << std::endl;
		}
	}
	else if(kind == "neg" && expr->type_ptr == type_int)
	{
		nExpr = intCompare(expr, new Express(type_int, -1), "mult");
	}
	else if(kind == "not")
	{
		nExpr = boolCompare(expr, new Express(type_int, 0), "neq");
	}
	else if(kind == "pred")
	{
		std::string reg = getRegister();
		if(expr->regist)
		{
			if(expr->type_ptr == type_int || expr->type_ptr == type_char)
			{
				out << "lw " << reg << ", " << expr->raw_val << GLOBAL_PTR << std::endl;
				out << "addi " << reg << ", " << reg << ", -1" << std::endl;
				out << "sw " << reg << ", " << expr->raw_val << GLOBAL_PTR << std::endl;
				nExpr = expr;
			}
			else
			{
				nExpr = boolCompare(expr, new Express(type_bool, 0), "eq");
			}
		}
		else
		{
			if(expr->type_ptr == type_bool && expr->raw_val == 0)
			{
				expr->raw_val += 1;
			}
			else
			{
				expr->raw_val -= 1;
			}
			
			nExpr = expr;
		}
		
		restoreRegister(reg);
	}
	else if(kind == "succ")
	{
		std::string reg = getRegister();
		if(expr->regist)
		{
			if(expr->type_ptr == type_int || expr->type_ptr == type_char)
			{
				out << "lw " << reg << ", " << expr->raw_val << GLOBAL_PTR << std::endl;
				out << "addi " << reg << ", " << reg << ", 1" << std::endl;
				out << "sw " << reg << ", " << expr->raw_val << GLOBAL_PTR << std::endl;
				nExpr = expr;
			}
			else
			{
				nExpr = boolCompare(expr, new Express(type_bool, 0), "eq");
			}
		}
		else
		{
			if(expr->type_ptr == type_bool && expr->raw_val == 0)
			{
				expr->raw_val -= 1;
			}
			else
			{
				expr->raw_val += 1;
			}
			
			nExpr = expr;
		}
		
		restoreRegister(reg);
	}
	
	return nExpr;
}

void outStopStatement()
{
	out << "j END" << std::endl;
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
	
	symbol_table.addExpr("true", (new Express(type_bool, 1)));
	symbol_table.addExpr("TRUE", (new Express(type_bool, 1)));
	symbol_table.addExpr("false", (new Express(type_bool, 0)));
	symbol_table.addExpr("FALSE", (new Express(type_bool, 0)));
	
	symbol_table.setStrType(type_string);
	
	string_list.push_back("");
	symbol_table.stepInContext();
	
	yyparse();
};
