// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <iostream>
#include <fstream>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH: {
		MessageBoxA(NULL, "Hello from DLL!", "Hello!", MB_OK | MB_ICONINFORMATION);

		std::cout << "cringe";

		std::ofstream out;
		out.open("C:\\Users\\tehc\\Documents\\cringe.txt");
		out << "ok";
		out.close();
		break;
	}
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		MessageBoxA(NULL, "Goodbye from DLL!", "Goodbye!", MB_OK | MB_ICONINFORMATION);
		break;
	}
	return TRUE;
}
