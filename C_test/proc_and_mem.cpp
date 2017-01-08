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

