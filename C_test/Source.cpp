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
			printf("ID: %d Name: %s\n", entry.th32ProcessID, entry.szExeFile);
			if (strncmp(entry.szExeFile, exe_name, _MAX_PATH) == 0){
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
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, process_ID);

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

int main(void) {
	DWORD wnd_handle = GetProcessID("procexp64.exe");
	if (wnd_handle == NULL) {
		perror("Find Window");
		return -1;
	}

	std::vector<DWORD> vec;

	GetThreads(wnd_handle,vec);

	return 0;
}