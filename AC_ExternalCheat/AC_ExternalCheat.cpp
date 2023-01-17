#include <iostream>
#include "proc.h"

int main()
{
    int desiredAmmoAmount = 0;

    // Get process id
    DWORD procId = GetProcId(L"ac_client.exe");
    std::cout << "PID found: " << procId << '\n';
    
    // get module base address
    uintptr_t modBase = GetModuleBaseAddress(procId, L"ac_client.exe");
    std::cout << "ac_client.exe MZ-Start: " << std::hex << modBase << '\n';

    // gets a handle to the game process with all possible rights a process can have(read, write etc).
    HANDLE hProc = 0;
    hProc = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);

    // resolve base address of the pointer chain
    uintptr_t dynPtrBaseAddr = modBase + 0x10F4F4;
    std::cout << "dynPtrBaseAddr: 0x" << std::hex << dynPtrBaseAddr << '\n';

    // resolve ammo chain
    std::vector<unsigned int> ammoOffsets = { 0x374, 0x14, 0x0 };

    // ammo vars initialization
    int ammoValue = 0;
    int newAmmo = 1337;

    // get ammo address
    uintptr_t ammoAddr = FindDMAAddy(hProc, dynPtrBaseAddr, ammoOffsets);
    std::cout << "ammoAddr: 0x" << std::hex << ammoAddr << '\n';

    // read ammo value
    ReadProcessMemory(hProc, (void*)ammoAddr, &ammoValue, sizeof(ammoValue), nullptr);
    std::cout << "Ammo value: " << std::dec << ammoValue << '\n';

    std::cout << "Type the amount of ammo you want: ";
    std::cin >> desiredAmmoAmount;

    // write to ammo
    WriteProcessMemory(hProc, (void*)ammoAddr, &desiredAmmoAmount, sizeof(newAmmo), nullptr);

    // read new ammo value
    ReadProcessMemory(hProc, (void*)ammoAddr, &ammoValue, sizeof(ammoValue), nullptr);
    std::cout << "New ammo value: " << std::dec << ammoValue << '\n';

    getchar();
    return 0;
}