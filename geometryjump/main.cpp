#include <windows.h>
#include <gl/gl.h>
#include <iostream>
#include "obstacle.h"
#include <time.h>
#include <stdlib.h>
#include <string.h>

using namespace std;
HBITMAP bmpsrc = NULL;
HDC hdcbmp = NULL;
BITMAPINFO di;


LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);

void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

struct pixel
{
    unsigned char B;
    unsigned char G;
    unsigned char R;
    unsigned char A;
};
pixel buf[2510];
int TextureID;
HDC hGlobalDC;
char cbuf [14] = "Score: \x00\x00\x00\x00\x00\x00\x00";
//game stuff
double distanceToLast = 0;
double velocity;
double y = 0;
int t0;
int tick = 0;
double g;
int liber = 3;
bool air = false;
int score = 0;

int time1, time2;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wcex;
    HWND hwnd;
    HDC hDC;
    HGLRC hRC;
    MSG msg;
    BOOL bQuit = FALSE;

    /* register window class */
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "MainWnd";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;


    if (!RegisterClassEx(&wcex))
        return 0;

    /* create main window */
    hwnd = CreateWindowEx(0,
                          "MainWnd",
                          "Geometry Jump",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          800,
                          600,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nCmdShow);

    bmpsrc = (HBITMAP)LoadImage(NULL, "playerHD.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        if(!bmpsrc)
                MessageBoxA(NULL, "Unable to load background image!", "Fatal error", MB_OK | MB_ICONERROR);
    hdcbmp = CreateCompatibleDC(GetDC(0));
    SelectObject(hdcbmp, bmpsrc);

    ZeroMemory(&di.bmiHeader, sizeof(BITMAPINFOHEADER));
    di.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

    if (!GetDIBits(hdcbmp, bmpsrc, 0, 50, NULL, &di, DIB_RGB_COLORS))
        MessageBox(NULL, "FAIL AT GETDIBITS MARE (INFO)", "ERROR", MB_OK);

    if (!GetDIBits(hdcbmp, bmpsrc, 0, 50, buf, &di, DIB_RGB_COLORS))
        MessageBox(NULL, "FAIL AT GETDIBITS 2", "ERROR", MB_OK);

    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);
    hGlobalDC = hDC;
    /*glGenTextures(1, &TextureID);
    glBindTexture(GL_TEXTURE_2D, TextureID);

    glTexImage2D(GL_TEXTURE_2D, 0, 3, 50, 50, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, buf);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_CLAMP);
    glBindTexture(GL_TEXTURE_2D, 0);
    glOrtho(0, 800, 0, 600, -1, 1);*/

    obstacle * test[3] = {NULL, NULL, NULL};
    srand(time(0));

    /* program main loop */
    while (!bQuit)
    {
        /* check for messages */
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            /* handle or dispatch messages */
            if (msg.message == WM_QUIT)
            {
                bQuit = TRUE;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            if(liber && distanceToLast > 1)
            {
                liber--;
                for(int i = 0; i < 3; ++i)
                {
                if(test[i]!=NULL)
                    continue;
                if(rand() % 2 == 0)
                    test[i] = new obstacleUp(1.2, 0.5);
                else
                    test[i] = new obstacleDown(1.2, 0.5);

                double newHeight = ((double) rand() / (1.43 *(RAND_MAX)) - 0.3);
                //cout<<"Generating spike with height "<<newHeight<<"\n";
                test[i]->adjustHeight(newHeight);

                distanceToLast = 0;
                break;
                }
            }
            /* OpenGL animation code goes here */
            time1 = GetTickCount();
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            glPushMatrix();

            glBegin(GL_POLYGON);
            glColor3ub(140, 225, 200);
            glVertex2d(-1, -0.3);
            glVertex2d(-1, 1);
            glVertex2d(1, 1);
            glVertex2d(1, -0.3);
            glEnd();

            glPopMatrix();

            for(int i = 0; i < 3; ++i)
                if(test[i]!=NULL)
                    test[i]->render();

            glPushMatrix();

            glBegin(GL_POLYGON);
            glColor3ub(124, 147, 150);
            glVertex2d(-1, -1);
            glVertex2d(-1, -0.3);
            glVertex2d(1, -0.3);
            glVertex2d(1, -1);
            glEnd();

            glPopMatrix();

            glPushMatrix();

            float y0 = 1.0f * velocity * (tick - t0) - 0.5f* g * (tick -t0) * (tick - t0);
            y0/= 509.5;
            //cout<<"Velocity: "<<velocity<<", y0: "<<y0<<"\n";
            if(y0 < 0.06 && y0 > 0)
            {
                //cout<<"Setting air to false!\n";
                air = false;
            }
            if(y0 < 0)
            {
                y0 = 0;
                velocity = 0;
                g = 0;
            }
            y0 -= 0.3;

            /*glBegin(GL_POLYGON);

            glColor3ub(255, 0, 0);
            glVertex2f(-0.5, y0);
            glVertex2f(-0.5, y0 + 0.18);
            glVertex2f(-0.375, y0 + 0.18);
            glVertex2f(-0.375, y0);
            glEnd();*/

            glRasterPos2f(-0.5, y0);
            glDrawPixels(50, 50, GL_BGRA_EXT, GL_UNSIGNED_BYTE, buf);
            glPopMatrix();


            SwapBuffers(hDC);
            RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);

            if(check_collisions(y0, test))
            {
                itoa(score/3, cbuf + 7, 10);
                MessageBoxA(NULL, cbuf, "AI MURIT!", MB_OK);
                score = 0;
                for(int i = 0; i < 3; ++i)
                if(test[i]!=NULL)
                    {
                        delete test[i];
                        test[i] = NULL;
                        liber += 1;
                    }
                velocity = 0;
                y0 = 0;
                g = 0;
                t0 = tick;
            }

            for(int i = 0; i < 3; ++i)
                if(test[i]!=NULL)
                    test[i]->advance();
            distanceToLast += 0.03;
            for(int i = 0; i < 3; ++i)
                if(test[i]!=NULL)
                    if(test[i]->isPassed())
                    {
                        delete test[i];
                        test[i] = NULL;
                        liber += 1;
                    }


            ++tick;
            ++score;
            time2 = GetTickCount();
            //cout<<"Rendering took "<<time2 - time1<<"ms!\n";
        }
    }

    /* shutdown OpenGL */
    DisableOpenGL(hwnd, hDC, hRC);

    /* destroy the window explicitly */
    DestroyWindow(hwnd);

    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    cout<<"Recived "<<uMsg<<"\n";
    switch (uMsg)
    {
        case WM_CLOSE:
            PostQuitMessage(0);
        break;

        case WM_DESTROY:
            return 0;

        case WM_PAINT:
            SetBkColor(hGlobalDC, RGB(140,140,140));
            SetBkMode(hGlobalDC, OPAQUE);

            SetTextColor(hGlobalDC, RGB(0,0,0));
            itoa(score/3, cbuf + 7, 10);
            TextOut(hGlobalDC, 380, 15, cbuf, strlen(cbuf));

            //UpdateWindow(hwnd);
            //ReleaseDC(hwnd, hdc);
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        case WM_KEYDOWN:
        {
            switch (wParam)
            {
                case VK_ESCAPE:
                    PostQuitMessage(0);
                    break;
                case VK_UP:
                case VK_SPACE:
                    {
                        if(!air)
                        {
                            air = true;
                            cout<<"User hit space: jumping!\n";
                            t0 = tick;
                            g = 3;
                            velocity = 50;
                        }
                    }
                    break;
            }
        }
        break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC)
{
    PIXELFORMATDESCRIPTOR pfd;

    int iFormat;

    /* get the device context (DC) */
    *hDC = GetDC(hwnd);

    /* set the pixel format for the DC */
    ZeroMemory(&pfd, sizeof(pfd));

    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW |
                  PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;

    iFormat = ChoosePixelFormat(*hDC, &pfd);

    SetPixelFormat(*hDC, iFormat, &pfd);

    /* create and enable the render context (RC) */
    *hRC = wglCreateContext(*hDC);

    wglMakeCurrent(*hDC, *hRC);
}

void DisableOpenGL (HWND hwnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hwnd, hDC);
}

