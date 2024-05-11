#define _CRT_SECURE_NO_WARNINGS 1
#include <Windows.h>
#include <stdio.h>
#include <TlHelp32.h>
#include <fstream>

// Btw this function is a bit unsafe lol
DWORD GetRobloxPid() {
	DWORD pid = 0;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 procEntry;
	procEntry.dwSize = sizeof(PROCESSENTRY32);


	while (Process32Next(hSnapshot, &procEntry)) {
		if (!_stricmp("RobloxPlayerBeta.exe", procEntry.szExeFile)) {
			printf("Roblox found!\n");

			pid = procEntry.th32ProcessID;
			break;
		}
	}

	CloseHandle(hSnapshot);

	return pid;
}

DWORD64 GetModuleBase(const char* modName, DWORD rbxPID) {
	printf("Scanning for: %d\n", rbxPID);
	DWORD64 imageBase = 0;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, rbxPID);
	MODULEENTRY32 modEntry;
	modEntry.dwSize = sizeof(MODULEENTRY32);

	while (Module32Next(hSnapshot, &modEntry)) {
		if (!_strcmpi(modName, modEntry.szModule)) {
			printf("Retrieving %s.. (size: %llx)\n", modEntry.szModule, modEntry.modBaseSize);

			imageBase = (DWORD64)modEntry.modBaseAddr;
			break;
		}
	}

	CloseHandle(hSnapshot);

	return imageBase;
}

int dump(const char* what) {
	DWORD64 modBase = GetModuleBase(what, GetRobloxPid());
	HANDLE robloxProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, GetRobloxPid());
	IMAGE_DOS_HEADER dosHeader;

	size_t nBytes = 0;
	if (!ReadProcessMemory(robloxProcess, (void*)modBase, &dosHeader, sizeof(IMAGE_DOS_HEADER), &nBytes)) {
		printf("Something went wrong while reading base.\n");

		printf("Exiting..");

		CloseHandle(robloxProcess);
		return 1;
	}

	nBytes = 0;

	printf("(%s) DOS header: 0x%llx\n", what, modBase);
	printf("(%s) DOS Signature: 0x%x\n", what, dosHeader.e_magic);

	// As a Rust dev this burns my eyes
	IMAGE_NT_HEADERS64 ntHeaders;
	if (!ReadProcessMemory(robloxProcess, (void*)(modBase + dosHeader.e_lfanew), &ntHeaders, sizeof(IMAGE_NT_HEADERS64), &nBytes)) {
		printf("Something went wrong while taking image snapshot.\n");

		printf("Exiting..");

		CloseHandle(robloxProcess);
		return 1;
	}

	IMAGE_OPTIONAL_HEADER optHeader = ntHeaders.OptionalHeader;
	printf("(%s) Subsystem: %d\n(%s) Size: 0x%llx\n", what, optHeader.Subsystem, what, optHeader.SizeOfImage);

	DWORD imageSize = optHeader.SizeOfImage;
	BYTE* data = new BYTE[static_cast<UINT_PTR>(imageSize)];

	std::ofstream output;
	std::string name = std::string("_RW_") + std::string(what);
	output.open(name.c_str(), std::ios_base::binary);

	DWORD dBytesRead = 0;
	for (DWORD64 address = modBase; address < (modBase + imageSize); address++) {
		nBytes = 0;
		BYTE b = 0x0;

		ReadProcessMemory(robloxProcess, (void*)address, &b, 0x1, &nBytes);

		data[dBytesRead] = b;
		dBytesRead += 1;
	}

	output.write((char*)data, imageSize);
	printf("%s has been successfully dumped.\n -> %s\n", what, name.c_str());

	output.close();
	delete[] data;
	CloseHandle(robloxProcess);
}