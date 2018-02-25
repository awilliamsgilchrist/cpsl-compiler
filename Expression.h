#include<string>

class Expression {
	public:
		int* type_ptr;
		int raw_val;
		std::string regist;

		Expression(int* type_ptr, int raw_val, std::string regist = ""):
			this.type_ptr = type_ptr,
			this.raw_val = raw_val,
			this.regist = regist
			{}

};
