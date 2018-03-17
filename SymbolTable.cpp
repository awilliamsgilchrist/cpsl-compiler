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


//If there are weird, ienxplicable things happening, check here first
void SymbolTable::addExpr(std::string key, Express expr)
{
	exprMap.top().emplace(key, expr);
	
	if(expr.type_ptr->usd_type == "array")
	{
		for(unsigned int i = 0; i < expr.type_ptr->size; i++)
		{
			Express subExpr(expr.type_ptr->simple_type, this->offset, true);
			subExpr.arr_expr = &expr;
			
			if(subExpr.type_ptr->usd_type == "array")
			{
				this->addExpr(key + "[" + std::to_string(i) + "]", subExpr);
			}
			else
			{
				this->offset += 4;
				exprMap.top().emplace(key + "[" + std::to_string(i) + "]", subExpr);
			}
		}
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
	std::stack<std::map<std::string, Express>> checkTable(exprMap);
	
	while(checkTable.size() > 0)
	{
		try
		{
			return new Express(checkTable.top().at(key).type_ptr, checkTable.top().at(key).raw_val, checkTable.top().at(key).regist);
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
