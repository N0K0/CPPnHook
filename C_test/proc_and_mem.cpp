#include <stdio.h>
#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <vector>
#include <easyhook.h>
#include "CPPnHook.h"

DWORD GetProcessID(char * exe_name){

	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (Process32First(snapshot, &entry) == TRUE){
		while (Process32Next(snapshot, &entry) == TRUE){
			if (strncmp(entry.szExeFile, exe_name, _MAX_PATH) == 0){
				printf("ID: %d Name: %s\n", entry.th32ProcessID, entry.szExeFile);
				CloseHandle(snapshot);
				return entry.th32ProcessID;
			}
		}
	}
	CloseHandle(snapshot);
	return NULL;
}

DWORD GetThreads(DWORD process_ID, std::vector<DWORD> vec){

	THREADENTRY32 entry;
	entry.dwSize = sizeof(THREADENTRY32);
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);

	if (Thread32First(snapshot, &entry) == TRUE) {
		while (Thread32Next(snapshot, &entry) == TRUE) {
			if (process_ID == entry.th32OwnerProcessID) {
				printf("ID: %d\n", entry.th32ThreadID);
				vec.push_back(entry.th32ThreadID);
			}
		}
	}
	CloseHandle(snapshot);
	return NULL;
}

template<typename T>
DWORD protectMemory(HANDLE proc, void *adr, DWORD prot) {
	DWORD oldProt;
	VirtualProtectEx(proc, adr, sizeof(T), prot, &oldProt);
	return oldProt;
}

template<typename T>
T readMemory(HANDLE proc, void *adr, boolean prot = TRUE) {
	T val;
	DWORD oldprot;
	if(prot == TRUE) oldprot = protectMemory<T>(proc, adr, PAGE_READWRITE);
	if (ReadProcessMemory(proc, adr, &val, sizeof(T), NULL) == 0) {
		error();
	}
	if (prot == TRUE) protectMemory<T>(proc, adr, oldprot);

	return val;
}

template<typename T>
void writeMemory(HANDLE proc, void *adr, T val, boolean prot = TRUE) {
	DWORD oldprot;
	if(prot == TRUE) oldprot = protectMemory<T>(proc, adr, PAGE_READWRITE);
	WriteProcessMemory(proc, adr, &val, sizeof(T), NULL);
	if(prot == TRUE) protectMemory<T>(proc, adr, oldprot);

}

LPDWORD get_ASLR(HANDLE proc) {
	LPDWORD new_base;

	//Start by getting the k32 module addr (this is constant and not affected by ASLR, so equal between procs)
	HMODULE kernel32 = GetModuleHandle("kernel32.dll");

	LPVOID get_proc_addr = GetProcAddress(kernel32, "GetModuleHandleA");
	if(!get_proc_addr)
		get_proc_addr = GetProcAddress(kernel32, "GetModuleHandleW"); // We have to deal with Unicode bullshit :l

	HANDLE thread = CreateRemoteThread(proc, NULL, NULL, (LPTHREAD_START_ROUTINE)get_proc_addr, NULL, NULL, NULL); //Get_proc_addr without args = current module :)
	WaitForSingleObject(thread, INFINITE);
	GetExitCodeThread(thread, new_base);
	CloseHandle(thread);
	return new_base;
}


void error() {
	char err[256];
	memset(err, 0, 256);
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), err, 255, NULL);
}



int main(void) {
	DWORD proc_handle = GetProcessID("notepad.exe");
	if (proc_handle == NULL) {
		perror("Find Window");
		return -1;
	}

	std::vector<DWORD> vec;
	GetThreads(proc_handle,vec);
	
	HANDLE open_proc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, proc_handle);
	if (open_proc == NULL) error();

	printf("Open Proc: %d\n", open_proc);
		

	_int64 base = 0x007FFA35C21010;
	for (__int64 i = base; i < (base + 100000); i = i + 1 ) {
		auto pre = readMemory<BYTE>(open_proc, (LPVOID)i);
		writeMemory<BYTE>(open_proc, (LPVOID)i, 0x90);
		auto post = readMemory<BYTE>(open_proc, (LPVOID)i);

		printf("(%x): %x\t(%x)\n", i, pre, post);
	}


	Sleep(2000);


	return 0;
}