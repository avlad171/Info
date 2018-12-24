#include <iostream>
#include <windows.h>
#include <string.h>

using namespace std;

INPUT ipD, ipU;
INPUT shiftD, shiftU;

char spamString[] = "Nu-mi da seen!\n";

void typeInput(char c)
{
    if(isdigit(c))
    {
        ipD.ki.wVk = (int) c;
        ipD.ki.wScan = MapVirtualKeyA(ipD.ki.wVk, MAPVK_VK_TO_VSC);

        ipU = ipD;
        ipU.ki.dwFlags = KEYEVENTF_KEYUP;

        SendInput(1, &ipD, sizeof(INPUT));
        Sleep(5);
        SendInput(1, &ipU, sizeof(INPUT));
    }
    else if (isalpha(c))
    {
        if(isupper(c))
             SendInput(1, &shiftD, sizeof(INPUT));
        ipD.ki.wVk = toupper(c);
        ipD.ki.wScan = MapVirtualKeyA(ipD.ki.wVk, MAPVK_VK_TO_VSC);

        ipU = ipD;
        ipU.ki.dwFlags = KEYEVENTF_KEYUP;

        SendInput(1, &ipD, sizeof(INPUT));
        Sleep(5);
        SendInput(1, &ipU, sizeof(INPUT));

        if(isupper(c))
                SendInput(1, &shiftU, sizeof(INPUT));
    }
    else
    {
        switch (c)
        {
        case '\n':
            {
                Sleep(10);
                ipD.ki.wVk = VK_RETURN;
                ipD.ki.wScan = MapVirtualKeyA(ipD.ki.wVk, MAPVK_VK_TO_VSC);

                ipU = ipD;
                ipU.ki.dwFlags = KEYEVENTF_KEYUP;

                SendInput(1, &ipD, sizeof(INPUT));
                Sleep(5);
                SendInput(1, &ipU, sizeof(INPUT));
                break;
            }
        case '-':
            {
                ipD.ki.wVk = VK_SUBTRACT;
                ipD.ki.wScan = MapVirtualKeyA(ipD.ki.wVk, MAPVK_VK_TO_VSC);

                ipU = ipD;
                ipU.ki.dwFlags = KEYEVENTF_KEYUP;
                SendInput(1, &ipD, sizeof(INPUT));
                Sleep(5);
                SendInput(1, &ipU, sizeof(INPUT));
                break;
            }
        case '!':
            {
                SendInput(1, &shiftD, sizeof(INPUT));
                ipD.ki.wVk = '1';
                ipD.ki.wScan = MapVirtualKeyA(ipD.ki.wVk, MAPVK_VK_TO_VSC);

                ipU = ipD;
                ipU.ki.dwFlags = KEYEVENTF_KEYUP;

                SendInput(1, &ipD, sizeof(INPUT));
                Sleep(5);
                SendInput(1, &ipU, sizeof(INPUT));

                SendInput(1, &shiftU, sizeof(INPUT));
                break;
            }
        default:    ///este o lipsa severa de caractere speciale. Daca vreti sa va jucati cu asta luati keyloggeru, apasati pe tasta pe care e caracteru, vedeti daca e necesar SHIFT si implementati ca mai sus
            {
                ipD.ki.wVk = VK_SPACE;
                ipD.ki.wScan = MapVirtualKeyA(ipD.ki.wVk, MAPVK_VK_TO_VSC);

                ipU = ipD;
                ipU.ki.dwFlags = KEYEVENTF_KEYUP;

                SendInput(1, &ipD, sizeof(INPUT));
                Sleep(5);
                SendInput(1, &ipU, sizeof(INPUT));
                break;
            }
        }
    }

}

int main()
{
    cout << "Welcome to chat-spammer 69000!\n";

    ipD.type = INPUT_KEYBOARD;
    ipU.type = INPUT_KEYBOARD;
    ipD.ki.wScan = 0;
    ipD.ki.time = 0;
    ipD.ki.dwExtraInfo = 0;
    ipD.ki.dwFlags = 0;

    shiftD.type = INPUT_KEYBOARD;
    shiftU.type = INPUT_KEYBOARD;

    shiftD = ipD;
    shiftD.ki.wVk = VK_LSHIFT;
    shiftD.ki.wScan = MapVirtualKeyA(shiftD.ki.wVk, MAPVK_VK_TO_VSC);

    shiftU = shiftD;
    shiftU.ki.dwFlags = KEYEVENTF_KEYUP;


    Sleep(3000);
    for(int t = 100; t; t--)
    {
        for(int i = 0; i < strlen(spamString); ++i)
        {
            char ct = spamString[i];
            typeInput(ct);


        }
        Sleep(50);

    }


    return 0;

}
