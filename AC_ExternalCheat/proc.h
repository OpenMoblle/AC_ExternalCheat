#pragma once

#include <vector>
#include <Windows.h>
#include <TlHelp32.h>

// Get PID.
DWORD GetProcId(const wchar_t* procName);

// Get Base Address of a Module.
uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName);

// Resolve Pointer Chains.
uintptr_t FindDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets);