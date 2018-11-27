#include <iostream>
#include <windows.h>
#include <time.h>
#include <math.h>
#include <vector>

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

#define PI 3.14159265


using namespace std;

vector <pair <float, float> > hops;

float getSlope(float x1, float y1, float x2, float y2)
{
    ///tangenta e de 2 ori zero si de 2 ori nedefinita (infinit)
    cout<<"DEBUG: "<<x1<<" "<<y1<<" "<<x2<<" "<<y2<<"\n";
    if(y1 == y2)    //tangenta e 0
    {
        if(x2 > x1)
            return 0;
        else
            return PI;
    }
    else if(x2 == x1)
    {
        if(y2 > y1)
            return PI/2;
        else
            return 3*PI/2;
    }
    else
        return atan2((y2 - y1), (x2 - x1));
}
float distance2D(int x1, int y1, int x2, int y2)
{
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

int getZone(float x1, float y1, float x2, float y2)
{
    if(x2 > x1 && y2 > y1)
        return 1;
    else if(x2 < x1 && y2 > y1)
        return 2;
    else if(x2 < x1 && y2 < y1)
        return 3;
    else if(x2 > x1 && y2 < y1)
        return 4;
    return 0;
}

void commitMovement(float x, float y)
{
    INPUT destination;

    destination.type = INPUT_MOUSE;
    destination.mi.mouseData = 0;
    destination.mi.time = 0;
    destination.mi.dwExtraInfo = 0;
    destination.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
    destination.mi.dx = (x * (0xffff / (float)SCREEN_WIDTH));
    destination.mi.dy = (y * (0xffff / (float)SCREEN_HEIGHT));
    SendInput(1, &destination, sizeof(INPUT));
}

void moveTo(int x, int y)
{
    HWND desktopwnd = GetDesktopWindow();
	HDC desktopdc = GetWindowDC(desktopwnd);


    POINT cursorPos;
    GetCursorPos(&cursorPos);
    cout<<"Cursor position: "<<cursorPos.x<<" "<<cursorPos.y<<"\n";
    cout<<"Going to "<<x<<" "<<y<<"\n";

    float dist = distance2D(cursorPos.x, cursorPos.y, x, y);

    int nrhops = dist / 10;
    cout<<"I have to do "<<nrhops<<" hops to reach dest!\n";

    float alpha = getSlope(cursorPos.x, cursorPos.y, x, y);
    cout<<"Unghiul cu orizontala este: "<<alpha<<" radiani\n\n";

    if(abs(dist * cos(alpha) + cursorPos.x - x) > 1 && abs(dist * sin(alpha) + cursorPos.y - y) > 1)
       {
           cout<<"FATAL ERROR! SHOULD NOT HAPPEN!\n";
           cout<<"Program calculated: "<<dist * cos(alpha) + cursorPos.x<<", "<<dist * sin(alpha) + cursorPos.y<<"\n";
           return;
       }

    for(int i = 1; i <= nrhops; ++i)
    {
        ///generam punctele pe dreapta
        float versor = dist/nrhops;
        hops.push_back({(float)i * versor * cos(alpha) + cursorPos.x, (float)i * versor * sin(alpha) + cursorPos.y});
        cout<<"Generat: ("<<(float)i * versor * cos(alpha) + cursorPos.x<<", "<<(float)i * versor * sin(alpha) + cursorPos.y<<")\n";
    }


    int ix = GetSystemMetrics(SM_CXICON) / 2;   ///dimensiunile icoanei
    int iy = GetSystemMetrics(SM_CYICON) / 2;

    float noise = 1;

    for(auto it = hops.begin(); it < hops.end(); ++it)
    {
        noise = (rand() % 1000 - 500) / 200.;
        //cout<<noise<<"\n";
        DrawIcon(desktopdc, it->first + noise - ix, it->second + noise - iy, LoadIcon(NULL, IDI_ERROR));

        commitMovement(it->first + noise, it->second + noise);
        Sleep(4);
    }
    hops.clear();
}
int main()
{
    srand(time(NULL));

    //while(1)
    for(int i = 0; i < 10; ++i)
    {
        Sleep(1500);
        int x = rand() % SCREEN_WIDTH;
        int y = rand() % SCREEN_HEIGHT;
        /*cout<<"Going to "<<x<<" "<<y<<"\n";


        POINT cursorPos;
        GetCursorPos(&cursorPos);
        cout<<"Cursor position: "<<cursorPos.x<<" "<<cursorPos.y<<"\n";

        float dist = distance2D(cursorPos.x, cursorPos.y, x, y);
        cout<<"Distance to next click: "<<dist<<"\n";
        */
        moveTo(x, y);
        //GetCursorPos(&cursorPos);
        //cout<<"Cursor position: "<<cursorPos.x<<" "<<cursorPos.y<<"\n\n";


    }
}
