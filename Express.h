#ifndef _EXPRESS_H
#define _EXPRESS_H
#include<string>
#include<iostream>

#include"Type.h"

class Express {
	public:
		int* type_ptr;
		int raw_val;
		bool regist;

		Express(int* type_ptr, int raw_val, bool regist = false):
			type_ptr(type_ptr),
			raw_val(raw_val),
			regist(regist)
		{}
};

#endif
