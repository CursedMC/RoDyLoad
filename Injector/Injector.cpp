#include <iostream>
#include <string>
#include <Windows.h>
#include <ctime>
#include <fstream>

void error(const char* message) {
	std::cout << message << "\n";
	system("pause");
}

int main(int argc, char* argv[]) {
	std::cout << "Injecting DLL...\n";

	// absolute dll path
	std::string relativeDllPath = std::string(".\\") + argv[1];
	const char* dllPath = _fullpath(NULL, relativeDllPath.c_str(), 256);
	if (dllPath == NULL) {
		error("Couldn't find DLL");
		return -1;
	}
	std::cout << "DLL path: " << dllPath << "\n";

	// roblox window
	HWND window = FindWindowA(NULL, "Roblox");
	if (window == NULL) {
		error("Failed to find Roblox");
		return -1;
	}
	std::cout << "Found Roblox\n";

	// process id
	DWORD procId = 0;
	GetWindowThreadProcessId(window, &procId);
	if (procId == 0) {
		error("Failed to find process ID for Roblox");
		return -1;
	}

	// process handle
	HANDLE proc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procId);
	if (proc == NULL) {
		error("Failed to retrieve process handle");
		return -1;
	}
	std::cout << "Retrieved process handle\n";

	// LoadLibraryA
	LPVOID entryAddress = (LPVOID)GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");
	if (entryAddress == NULL) {
		error("Failed to get LoadLibraryA");
		return -1;
	}

	// allocate memory in process and inject dll path
	LPVOID pathAddress = (LPVOID)VirtualAllocEx(proc, NULL, strlen(dllPath), MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (pathAddress == NULL) {
		error("Failed to allocate memory in target process");
		return -1;
	}
	BOOL writeStatus = WriteProcessMemory(proc, pathAddress, dllPath, strlen(dllPath), NULL);
	if (!writeStatus) {
		error("Failed to write to memory in target process");
		return -1;
	}

	// read the dll path we injected earlier
	char* injectedString = new char[strlen(dllPath)];
	memset(injectedString, 0, strlen(dllPath));
	BOOL readStatus = ReadProcessMemory(proc, pathAddress, injectedString, strlen(dllPath), NULL);
	if (!readStatus) {
		error("Failed to read injected path from memory");
		return -1;
	}
	std::cout << std::string(injectedString) << "\n";

	// create remote thread attached to roblox and load the dll
	HANDLE remoteThread = CreateRemoteThread(proc, NULL, 0, (LPTHREAD_START_ROUTINE)entryAddress, pathAddress, NULL, NULL);
	if (remoteThread == NULL) {
		error("Failed to create remote thread in target process");
		return -1;
	}

	DWORD threadId = GetThreadId(remoteThread);
	DWORD processId = GetProcessIdOfThread(remoteThread);
	printf("Injected\nThread ID: %u\nProcess ID: %u\n", threadId, processId);

	// close thread
	CloseHandle(remoteThread);

	// get the time aqnbd write to file
	std::time_t tTime = std::time(NULL);
	std::ofstream based;
	based.open("based.txt");
	based << tTime;
	based.close();

	system("pause");

	return 0;
}
