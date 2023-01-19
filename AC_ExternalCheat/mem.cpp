#include "mem.h"

// Patch bytes of a assembly instruction.
// dst is the address of where our target function is, src is what we're going to write on that function, size is the amount of bytes we're going to write.
void mem::PatchEx(BYTE* dst, BYTE* src, unsigned int size, HANDLE hProcess)
{
	// Used to store old memory page permissions.
	DWORD oldprotect = 0;

	// Change the permissions in dst memory page to PAGE_EXECUTE_READWRITE(id 0x40) and store old permissions in oldprotect.
	VirtualProtectEx(hProcess, dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);

	// Overwrite bytes in dst with new bytes/instructions given in src.
	WriteProcessMemory(hProcess, dst, src, size, nullptr);

	// Reset permissions in dst back to old ones using oldprotect.
	VirtualProtectEx(hProcess, dst, size, oldprotect, &oldprotect);
}

// Nop bytes of a instruction so it does nothing.
// dst is the address to the target function we want to NOP instructions from. size is the amount of NOP instructions we want to place there.
void mem::NopEx(BYTE* dst, unsigned int size, HANDLE hProcess)
{
	// Creates a array with the size of how much NOP's we want and set each element in that array to 0x90(NOP opcode).
	BYTE* nopArray = new BYTE[size];
	memset(nopArray, 0x90, size);

	// Patch the instruction in dst to become NOP instructions, making them do nothing.
	PatchEx(dst, nopArray, size, hProcess);

	// Deallocate nopArray's memory.
	delete[] nopArray;
}