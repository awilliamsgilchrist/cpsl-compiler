#include<string>

class Expression {
	public:
		int* type_ptr;
		int raw_val;
		std::string regist;

		Expression(int* type_ptr, int raw_val, std::string regist = ""):
			type_ptr(type_ptr),
			raw_val(raw_val),
			regist(regist)
			{}

};
