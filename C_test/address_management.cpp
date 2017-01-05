#include <vector>
#include "CPPnHook.h"

class address_trans{
	address_size base; // What a "normal" base would be 
	address_size aslr;
	address_size translation_factor;

	address_size translate_addr(address_size address) {
		address_size ret;
		return ret;
	}
}


class address_chain{

	address_size base;	
	std::vector<address_size> chain;

	void add_offset(address_size offset) {
		chain.push_back(offset);
	}

	void remove_offset() {
		auto pop = chain.back();
		chain.pop_back();
	}

	address_size calculate(){
		address_size ret = 
	}
};
