#ifndef _SYMBOL_TABLE_H
#define _SYMBOL_TABLE_H
#include<map>
#include<stack>
#include<stdexcept>

#include "Express.h"

class SymbolTable {
	public:
		std::stack<std::map<std::string, Type*>> typeMap;
		std::stack<std::map<std::string, Express*>> exprMap;
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
		void addType(std::string key, Type* type_ptr);
		void addExpr(std::string key, Express* expr);
		Type* findType(std::string key);
		Express* findExpr(std::string key);
		void removeType(std::string key);
		void removeExpr(std::string key);
		
		void setStrType(Type* string_type) { m_string_type = string_type; }
		
	private:
		Type* m_string_type;
};

#endif
