#include<map>
#include<string>

class Type{
	public:
	
	Type* simple_type;
	std::string usd_type;
	std::map<std::string, int> indexing_map;
	int size;
	int* data;
	
	Type():
		simple_type(nullptr),
		usd_type(""),
		data(nullptr),
		size(4)
	{}
	
	//For arrays
	Type(Type* simple_type, int min, int max):
		simple_type(simple_type),
		usd_type("array")
	{
		for(unsigned int i = 0; i < max - min; i++)
		{
			indexing_map.emplace(std::to_string(min + i), i);
		}
		
		size = (max - min) * 4;
		data = new int[max - min];
		
	}
};