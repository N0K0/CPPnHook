#pragma once

#include <stdio.h>
#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <vector>
#include <easyhook.h>

#ifdef _M_X64
typedef  uint64_t address_size;
#else
typedef  uint32_t address_size;
#endif

// Proc and mem funcs
DWORD GetProcessID(char * exe_name);
DWORD GetThreads(DWORD process_ID, std::vector<DWORD> vec);


template<typename T>
DWORD protectMemory(HANDLE proc, LPVOID adr, DWORD prot) {
	DWORD oldProt;
	VirtualProtectEx(proc, adr, sizeof(T), prot, &oldProt);
	return oldProt;
}

template<typename T>
T readMemory(HANDLE proc, LPVOID adr, boolean prot) {
	T val;
	DWORD oldprot;
	if (prot == TRUE) oldprot = protectMemory<T>(proc, adr, PAGE_READWRITE);
	if (ReadProcessMemory(proc, adr, &val, sizeof(T), NULL) == 0) {
		perror("ReadProcMem");

	}
	if (prot == TRUE) protectMemory<T>(proc, adr, oldprot);

	return val;
}


template<typename T>
void writeMemory(HANDLE proc, LPVOID adr, T val, boolean prot) {
	DWORD oldprot;
	if (prot == TRUE) oldprot = protectMemory<T>(proc, adr, PAGE_READWRITE);
	WriteProcessMemory(proc, adr, &val, sizeof(T), NULL);
	if (prot == TRUE) protectMemory<T>(proc, adr, oldprot);

}


// Address management stuff
address_size resolve_pointer_chain(std::vector<address_size> *chain, HANDLE proc);