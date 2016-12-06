#include <stdio.h>
#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <vector>

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
T readMemory(HANDLE proc, LPVOID adr) {
	T val;
	if (ReadProcessMemory(proc, adr, &val, sizeof(T), NULL) == 0) {
		error();
	}
	return val;
}
template<typename T>
void writeMemory(HANDLE proc, LPVOID adr, T val) {
	WriteProcessMemory(proc, adr, &val, sizeof(T), NULL);
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
		

	_int64 base = 0x6B2137C859;
	for (__int64 i = base; i < (base + 1000); i = i + 1 ) {
		auto pre = readMemory<BYTE>(open_proc, (LPVOID)i);
		writeMemory<BYTE>(open_proc, (LPVOID)i, 0x90);
		auto post = readMemory<BYTE>(open_proc, (LPVOID)i);

		printf("(%x): %x\t(%x)\n", i, pre, post);
	}


	Sleep(2000);


	return 0;
}