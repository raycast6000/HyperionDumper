// HyperionDumper dumps the Roblox executable and main DLL (RobloxPlayerBeta.dll
// Don't act like your anti-cheat is special if you're just embedding VMProtect into it. Roblox got scammed lmao.
// Anyways, credit me when using this code.

#include <conio.h>
#include <fstream>
#include <stdio.h>
#include <Windows.h>
#include "dumper.h"
#include <thread>

int main() {
	SetConsoleTitleA("Hyperion (Discord: @_raycast)");

	printf("Dumping takes between 5 and 10 minutes (depending on your CPU). Are you ready?\n");
	system("pause");
	std::thread exeDump(&dump, "RobloxPlayerBeta.exe");
	std::thread dllDump(&dump, "RobloxPlayerBeta.dll");

	exeDump.join();
	dllDump.join();
	printf("Done. Happy hacking!\n");
	system("pause");

	printf("Press any key to exit.\n");
	return 0;
}