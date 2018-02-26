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

void SymbolTable::addExpr(std::string key, Expression expr)
{
	exprMap.top().emplace(key, expr);
}

void SymbolTable::removeType(std::string key)
{
	typeMap.top().erase(key);
}

void SymbolTable::removeExpr(std::string key)
{
	exprMap.top().erase(key);
}