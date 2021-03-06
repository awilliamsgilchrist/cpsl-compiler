#ifndef _EXPRESS_H
#define _EXPRESS_H
#include<string>
#include<iostream>

#include"Type.h"

class Express {
	public:
		Type* type_ptr;
		int raw_val;
		bool regist;
		Express* ref_expr;
		Express* arr_expr;

		Express(Type* type_ptr, int raw_val, bool regist = false):
			type_ptr(type_ptr),
			raw_val(raw_val),
			regist(regist),
			ref_expr(nullptr),
			arr_expr(nullptr)
		{}
};

#endif
