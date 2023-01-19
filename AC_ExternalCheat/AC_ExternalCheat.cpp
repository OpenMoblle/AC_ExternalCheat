#include <iostream>
#include "proc.h"
#include "mem.h"

int main()
{
    // Variable initialization.
    HANDLE hProcess = 0;
    uintptr_t moduleBase = 0, localPlayerPtr = 0, healthAddr = 0;
    bool bHealth = false, bAmmo = false, bRecoil = false;
    int newValue = 1337;

    // Get game PID.
    DWORD procId = GetProcId(L"ac_client.exe");

    if (procId)
    {
        // Open handle to process with all access rights, not inherited.
        hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, procId);

        // Get module base address of module "ac_client.exe".
        moduleBase = GetModuleBaseAddress(procId, L"ac_client.exe");
        std::cout << "ac_client.exe Module Base Address: 0x" << std::hex << moduleBase << '\n';

        // Get address to LocalPlayer.
        localPlayerPtr = moduleBase + 0x10F4F4;
        std::cout << "LocalPlayer: 0x" << std::hex << localPlayerPtr << '\n';

        // Dynamically resolve health address with offset 0xF8.
        healthAddr = FindDMAAddy(hProcess, localPlayerPtr, { 0xF8 });
    }
    else // If it falls here that means game PID was not found.
    {
        std::cout << "Process not found, press ENTER to exit...\n";
        getchar();
        return 1;
    }

    // GetExitCodeProcess gets the current termination status of the cheat and stores it into dwExit, we then check if dwExit is STILL_ACTIVE which is what GetExitCodeProcess returns if the process is still opened. If the program is still opened we can continue.
    DWORD dwExit = 0;
    while (GetExitCodeProcess(hProcess, &dwExit) && dwExit == STILL_ACTIVE)
    {
        // Health toggle.
        if (GetAsyncKeyState(VK_NUMPAD1) & 1)
        {
            bHealth = !bHealth;
        }

        // Ammo toggle.
        if (GetAsyncKeyState(VK_NUMPAD2) & 1)
        {
            bAmmo = !bAmmo;

            // If bAmmo is toggled, patch bytes in decrease ammo function to instead increase our ammo by 1 on every shot.
            if (bAmmo)
            {
                mem::PatchEx((BYTE*)(moduleBase + 0x637E9), (BYTE*)"\xFF\x06", 2, hProcess);
            }
            else // If bAmmo is disabled, patch bytes to decrease ammo back into decrease ammo function.
            {
                mem::PatchEx((BYTE*)(moduleBase + 0x637E9), (BYTE*)"\xFF\x0E", 2, hProcess);
            }
        }

        // No Recoil toggle.
        if (GetAsyncKeyState(VK_NUMPAD3) & 1)
        {
            bRecoil = !bRecoil;

            if (bRecoil)
            {
                // Nop 10 instructions in the recoil function to remove gun recoil.
                mem::NopEx((BYTE*)(moduleBase + 0x63786), 10, hProcess);
            }
            else
            {
                // Write recoil instructions back into the recoil function so we can have recoil again.
                mem::PatchEx((BYTE*)(moduleBase + 0x63786), (BYTE*)"\x50\x8D\x4C\x24\x1C\x51\x8B\xCE\xFF\xD2", 10, hProcess);
            }
        }

        // Close program.
        if (GetAsyncKeyState(VK_INSERT) & 1)
        {
            return 0;
        }

        // Always write value of newValue to our health.
        if (bHealth)
        {
            // Continuously write value in newValue to our health on every loop.
            mem::PatchEx((BYTE*)healthAddr, (BYTE*)&newValue, sizeof(newValue), hProcess);
        }

        Sleep(5);
    }

    // It'll get here if we close the process.
    std::cout << "Process not found, press ENTER to exit...\n";
    getchar();
    return 0;
}