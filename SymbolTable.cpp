#include "SymbolTable.h"

void SymbolTable::stepInContext()
{
	typeMap.emplace();
	exprMap.emplace();
}

void SymbolTable::stepOutContext()
{
	typeMap.pop();
	exprMap.pop();
}

void SymbolTable::addType(std::string key, Type* type_ptr)
{
	typeMap.top().emplace(key, type_ptr);
}


//If there are weird, inexplicable things happening, check here first
void SymbolTable::addExpr(std::string key, Express* expr)
{
	std::cout << "Key: " << key << std::endl;
	std::cout << "USD Type: " << expr->type_ptr->usd_type << std::endl;
	std::cout << "Ref Expr: " << expr->ref_expr << std::endl;
	
	exprMap.top().emplace(key, expr);
	
	if(expr->type_ptr->usd_type == "array")
	{
		for(unsigned int i = 0; i < expr->type_ptr->size; i++)
		{
			Express* subExpr = new Express(expr->type_ptr->simple_type, this->offset, true);
			std::cout << "Raw value for array members " << subExpr->raw_val << std::endl;
			subExpr->arr_expr = expr;
			
			if(subExpr->type_ptr->usd_type == "array")
			{
				this->addExpr(key + "[" + std::to_string(i + expr->type_ptr->min) + "]", subExpr);
			}
			else if(subExpr->type_ptr == m_string_type)
			{
				subExpr->raw_val = 0;
				subExpr->regist = false;
				exprMap.top().emplace(key + "[" + std::to_string(i + expr->type_ptr->min) + "]", subExpr);
			}
			else
			{
				this->offset += 4;
				exprMap.top().emplace(key + "[" + std::to_string(i + expr->type_ptr->min) + "]", subExpr);
			}
		}
	}
	else if(expr->raw_val = -20 && expr->regist)
	{
		expr->raw_val = this->offset;
		this->offset += 4;
	}
}

Type* SymbolTable::findType(std::string key)
{
	std::stack<std::map<std::string, Type*>> checkTable(typeMap);
	
	while(checkTable.size() > 0)
	{
		try
		{
			return checkTable.top().at(key);
		}
		catch(const std::out_of_range& oor)
		{
			checkTable.pop();
		}
	}
	
	std::cerr << "Error: Type " << key << " undefined in current scope" << std::endl;
}

Express* SymbolTable::findExpr(std::string key)
{
	std::stack<std::map<std::string, Express*>> checkTable(exprMap);
	
	
	while(checkTable.size() > 0)
	{
		try
		{
			return checkTable.top().at(key);
		}
		catch(const std::out_of_range& oor)
		{
			checkTable.pop();
		}
	}
	
	std::cerr << "Error: Expression " << key << " undefined in current scope" << std::endl;
}

void SymbolTable::removeType(std::string key)
{
	typeMap.top().erase(key);
}

void SymbolTable::removeExpr(std::string key)
{
	exprMap.top().erase(key);
}
