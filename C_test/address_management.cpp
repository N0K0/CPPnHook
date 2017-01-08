#include <vector>
#include "CPPnHook.h"

class address_trans{
	address_size base; // What a "normal" base would be 
	address_size aslr;
	address_size translation_factor;

	address_size translate_addr(address_size address) {
		address_size ret = 0;
		return ret;
	}
};


address_size resolve_pointer_chain(std::vector<address_size> *chain, HANDLE proc) {

	address_size temp_addr = chain->at(0);
	printf("Base:\n[%x", temp_addr);
	for (int i = 1; i < chain->size()-1; i++) {
		address_size offset = chain->at(i);
		temp_addr = readMemory<address_size>(proc, (LPVOID)(temp_addr + offset), true);
		printf(" + %3x] -> %x\n", offset, temp_addr);
		printf("[%x", temp_addr);
	}
	printf("]\n");

	return temp_addr + chain->back();

}