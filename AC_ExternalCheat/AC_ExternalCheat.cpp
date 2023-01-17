#include <iostream>
#include "proc.h"

int main()
{
    // Initialize vars.
    int newAmmo = 1337;
    int ammoValue = 0;
    int desiredAmmoAmount = 0;

    // Get PID.
    DWORD procId = GetProcId(L"ac_client.exe");
    std::cout << "PID found: " << procId << '\n';
    
    // Get Module Base Address.
    uintptr_t modBase = GetModuleBaseAddress(procId, L"ac_client.exe");
    std::cout << "ac_client.exe MZ-Start: " << std::hex << modBase << '\n';

    // Gets a handle to the game process with all possible rights we can have(read, write etc).
    HANDLE hProc = 0;
    hProc = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);

    // Resolve Base Address of the Pointer Chain to get our LocalPlayer.
    uintptr_t localPlayer = modBase + 0x10F4F4;
    std::cout << "dynPtrBaseAddr: 0x" << std::hex << localPlayer << '\n';

    // Store Ammo Offsets in a variable.
    std::vector<unsigned int> ammoOffsets = { 0x374, 0x14, 0x0 };

    // Get Ammo Address
    uintptr_t ammoAddr = FindDMAAddy(hProc, localPlayer, ammoOffsets);
    std::cout << "ammoAddr: 0x" << std::hex << ammoAddr << '\n';

    // Read Ammo Value
    ReadProcessMemory(hProc, (void*)ammoAddr, &ammoValue, sizeof(ammoValue), nullptr);
    std::cout << "Ammo value: " << std::dec << ammoValue << '\n';

    std::cout << "Type the amount of ammo you want: ";
    std::cin >> desiredAmmoAmount;

    // Write to Ammo
    WriteProcessMemory(hProc, (void*)ammoAddr, &desiredAmmoAmount, sizeof(newAmmo), nullptr);

    // Read new Ammo value
    ReadProcessMemory(hProc, (void*)ammoAddr, &ammoValue, sizeof(ammoValue), nullptr);
    std::cout << "New ammo value: " << std::dec << ammoValue << '\n';

    getchar();
    return 0;
}