#include "proc.h"
#include <iostream>

// procName starting value: address to first letter in the array.
DWORD GetProcId(const wchar_t* procName)
{
	// Initialize procId variable & get handle to list containing all running processes in the system.
	DWORD procId = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	// Check if hSnap is valid.
	if (hSnap != INVALID_HANDLE_VALUE)
	{
		// Return struct containing the values of a snapped process.
		PROCESSENTRY32 procEntry;

		// Gets the size of the struct, if we don't do it we can't use Process32First().
		procEntry.dwSize = sizeof(procEntry);

		// Gets a "picture" to the first process in the snapshot.
		if (Process32First(hSnap, &procEntry))
		{
			// Loops through every subsequent process in the snapshot.
			do
			{
				// Checks if the process name is equal to our target game name.
				if (!_wcsicmp(procEntry.szExeFile, procName))
				{
					// Assign PID of the correct process to procId & breaks the loop.
					procId = procEntry.th32ProcessID;
					break;
				}
			} while (Process32Next(hSnap, &procEntry));
		}
	}

	// Close handle to the snapshot containing all the processes & return target process PID.
	CloseHandle(hSnap);
	return procId;
}

// Gets the base address of the target module.
uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName)
{
	// Initialize modBaseAddr var & creates a snapshot containing all the 32/64-bit modules being used inside of the target process.
	// TH32CS_SNAPMODULE = all 64-bit modules.
	// TH32CS_SNAPMODULE32 = all 32-bit modules.
	uintptr_t modBaseAddr = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
	
	// Checks if the snapshot was created successfully.
	if (hSnap != INVALID_HANDLE_VALUE)
	{
		// Return struct containing the values of a snapped module.
		MODULEENTRY32 modEntry;

		// Get the size of the struct, we have to do this before calling Module32First or Module32Next else it won't allow us to.
		modEntry.dwSize = sizeof(modEntry);

		// Gets information about the first module loaded in the snapshot and stores its information inside of modEntry struct.
		if (Module32First(hSnap, &modEntry))
		{
			// Loops through every subsequent module in the snapshot and stores its information in modEntry struct.
			do
			{
				// Checks if the module name is equal to our target module name.
				if (!_wcsicmp(modEntry.szModule, modName))
				{
					// Assign the base address of the correct module to modBaseAddr & breaks the loop.
					modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
					break;
				}
			} while (Module32Next(hSnap, &modEntry));
		}
	}

	// Close handle to the snapshot containing all loaded modules inside of the target process and-
	// return the module base address.
	CloseHandle(hSnap);
	return modBaseAddr;
}

// Loops from the base mod address through every offset and add it subsequently until we get to the final address.
uintptr_t FindDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets)
{
	// addr is used as our buffer.
	uintptr_t addr = ptr;

	// Loop through the offsets and add them to addr
	for (unsigned int i = 0; i < offsets.size(); ++i)
	{
		// Read new address into address.
		ReadProcessMemory(hProc, (void*)addr, &addr, sizeof(addr), 0);

		// Add current offset to addr.
		addr += offsets[i];
	}

	// Return final address with all the offsets added.
	return addr;
}