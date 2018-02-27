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

void SymbolTable::addType(std::string key, int* type_ptr)
{
	typeMap.top().emplace(key, type_ptr);
}

void SymbolTable::addExpr(std::string key, Express expr)
{
	exprMap.top().emplace(key, expr);
}

void SymbolTable::findType(std::string key)
{
	std::stack<std::map<std::string, int*>> checkTable(typeMap);
	
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
			return &checkTable.top().at(key);
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
