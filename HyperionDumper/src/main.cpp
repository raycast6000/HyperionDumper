// HyperionDumper dumps the Roblox executable and its main DLL (RobloxPlayerBeta.dll)
// Anyways, credit me when using this code.

#include <conio.h>
#include <fstream>
#include <stdio.h>
#include <Windows.h>
#include "dumper.h"
#include <thread>

int main() {
	SetConsoleTitleA("Hyperion Cummer (Discord: @_raycast)");

	printf("Dumping takes between 5-10 minutes (depending on your CPU). Are you ready?\n");
	system("pause");
	std::thread exeDump(&dump, "RobloxPlayerBeta.exe");
	std::thread dllDump(&dump, "RobloxPlayerBeta.dll");

	exeDump.join();
	dllDump.join();
	printf("Done. Happy hacking!\n");
	system("pause");
	return 0;
}