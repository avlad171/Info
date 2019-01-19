#include <iostream>
#include <iomanip>
#include <windows.h>

using namespace std;

HANDLE hCon;

bool initialized = false;

void initialize()
{
    hCon = GetStdHandle(STD_OUTPUT_HANDLE);
}
struct textcolor { int color; };

inline textcolor cl( int color)
{
    return { color };
}

inline ostream& operator << (ostream &os, const textcolor tc)
{
    if(!hCon)
    {
        if(!initialized)
        {
            initialized = 1;
            initialize();
        }
        else
            os<<"Failed setting color with error "<<GetLastError()<<"\n";
    }
    SetConsoleTextAttribute(hCon, tc.color);
    return os;
}
int main()
{

    cout <<cl(0x08)<<'['<<cl(0x0a)<<"INFO"<<cl(0x08)<<']'<<cl(0x0f)<<"Imi place limbajul c++";
	/*
    while(1)
        {
        for(int i = 0; i < 80; ++i)
            cout<<cl(0x0c)<<(unsigned char)219;
        //cout<<'\n';
        for(int i = 0; i < 80; ++i)
            cout<<cl(0x0a)<<(unsigned char)219;
        //cout<<'\n';
        for(int i = 0; i < 80;++i)
            cout<<cl(0x0b)<<(unsigned char)219;
        //cout<<'\n';
        }*/
    return 0;
}
