#include<string>

class Express {
	public:
		int* type_ptr;
		int raw_val;
		std::string regist;

		Express(int* type_ptr, int raw_val, std::string regist = ""):
			type_ptr(type_ptr),
			raw_val(raw_val),
			regist(regist)
			{}

};
