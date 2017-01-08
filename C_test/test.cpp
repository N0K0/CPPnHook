#include "CPPnHook.h"

int main(void) {
	DWORD proc_handle = GetProcessID("Tutorial-i386.exe");
	if (proc_handle == NULL) {
		perror("Find Window");
		Sleep(5000);
		return -1;
	}

	std::vector<DWORD> vec;
	GetThreads(proc_handle, vec);

	HANDLE open_proc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, proc_handle);
	if (open_proc == NULL) {
		perror("RIP");
		Sleep(5000);
	}
	printf("Open Proc: %d\n", open_proc);


	std::vector<address_size> chain = { 0x01819B28,0xC,0x14,0x0,0x18 };

	address_size addr = resolve_pointer_chain(&chain, open_proc);
	printf("Addr: %x\n", addr);

	address_size val = readMemory<address_size>(open_proc, (LPVOID)addr, true);
	printf("Val: %d\n", val);

	Sleep(200000);


	return 0;
}