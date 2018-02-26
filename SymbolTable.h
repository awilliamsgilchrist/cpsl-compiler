#ifndef _SYMBOL_TABLE_H
#define _SYMBOL_TABLE_H
#include<map>
#include<stack>

#include "Express.h"

class SymbolTable {
	public:
		std::stack<std::map<std::string, int*>> typeMap;
		std::stack<std::map<std::string, Express>> exprMap;
		int offset;
		int typeCount;
		int exprCount;
		
		SymbolTable():
			offset(0),
			typeCount(0),
			exprCount(0)
		{
			typeMap.emplace();
			exprMap.emplace();
		}
		
		void stepInContext();
		void stepOutContext();
		void addType(std::string key, int* type_ptr);
		void addExpr(std::string key, Express expr);
		void removeType(std::string key);
		void removeExpr(std::string key);
};

#endif
