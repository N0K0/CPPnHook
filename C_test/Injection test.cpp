#include <Windows.h>

void * basepointer;

/*
int main() {
	char *win_name = "Untitled - Notepad";
	auto notepad_win = FindWindow(NULL, win_name);
	if (notepad_win == NULL) {
		perror("Find window");
	}

	HMODULE k32 = GetModuleHandle("kernel32.dll");
	LPVOID k32_getModuleHandle = GetProcAddress(k32, "GetModuleHandleW");
	if (k32_getModuleHandle == NULL) {
		k32_getModuleHandle = GetProcAddress(k32, "GetModuleHandleA");
	}
}
*/