#include <iostream>
#include <windows.h>

using namespace std;

unsigned char newcode[] =
"\x81\xc2\x37\x13\x00\x00"
"\x89\x8E\x80\x00\x00\x00"
"\xE9\x60\x3D\xBD\xFF";
unsigned char inject[] =
"\xE9\x91\xC2\x42\x00"
"\x90\x90";
int main()
{
    HWND gamewnd;
    gamewnd = FindWindowEx(NULL, NULL, NULL, "Chicken Invaders COTDS");
    if(!gamewnd)
    {
        cout<<"Unable to find game window!\n";
        return 0;
    }
    DWORD pid;
    GetWindowThreadProcessId(gamewnd, &pid);
    cout<<"Found process pid: "<<pid<<"\n";
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);
    if(!hProcess)
    {
        cout<<"Unable to open process!\n";
        return 0;
    }

    DWORD addy = VirtualAllocEx(hProcess, NULL, 256, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    cout<<"Newmem address: "<<(void *)addy<<"\n";
    ULONG_PTR jeg;

    uint32_t jmpsize = ((DWORD)addy - 0x473D6A - 5);
    int32_t rev_jmpsize = -jmpsize;
    rev_jmpsize -= 16;

    cout<<"Jumping from: "<<hex<<0x473D6A<<" "<<hex<<jmpsize<<" bytes, leading to "<<hex<<jmpsize + 0x473D6A<<"\n";
    cout<<"Calculated reverse jump size: "<<rev_jmpsize<<"\n";
    memcpy(newcode + 13, &rev_jmpsize, 4);
    WriteProcessMemory(hProcess, addy, newcode, 17, &jeg);
    memcpy(inject + 1, &jmpsize, 4);
    VirtualProtectEx(hProcess, 0x473D6A, 16, PAGE_EXECUTE_READWRITE, &jeg);
    WriteProcessMemory(hProcess, 0x473D6A, inject, 7, &jeg);
    cout<<"DONE!\n";
    system("pause");

    return 0;
}
