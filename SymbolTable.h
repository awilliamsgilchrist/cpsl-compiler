#include<map>
#include<stack>

#include "Expression.h"

class SymbolTable {
	public:
		std::stack<std::map<std::string, int*>> typeMap;
		std::stack<std::map<std::string, Expression>> exprMap;
		int offset;
		int typeCount;
		int exprCount;
		
		SymbolTable():
			this.offset = 0,
			this.typeCount = 0,
			this.exprCount = 0
		{
			typeMap.emplace();
			exprMap.emplace();
		}
		
		void stepInContext();
		void stepOutContext();
		void addType(std::string key, int* type_ptr);
		void addExpr(std::string key, Expression expr);
		void removeType(std::string key);
		void removeExpr(std::string key);
};