#include <windows.h>
#include <windowsx.h>
#include <gl/gl.h>
#include <iostream>
using namespace std;

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

struct pixel
{
    unsigned char B;
    unsigned char G;
    unsigned char R;
    unsigned char A;    //alpha(transparenta), nu folosim
};

int oldx = 0, oldy = 0, newx, newy;
int dx, dy;

float thetax = 0;
float thetay = 0;
float scale = 1;

void truncate(float & theta)
{
    if(theta > 360.0)
        theta -= 360.0;
    if(theta < - 360.0)
        theta += 360.0;
}
pixel buf[640*395];
int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    WNDCLASSEX wcex;
    HWND hwnd;
    HDC hDC;
    HGLRC hRC;
    MSG msg;
    BOOL bQuit = FALSE;

    ///image stuff
    HBITMAP bmpsrc = NULL;
    HDC hdcbmp = NULL;
    BITMAPINFO di;


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
    wcex.lpszClassName = "Fituica";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;


    if (!RegisterClassEx(&wcex))
        return 0;

    /* create main window */
    hwnd = CreateWindowEx(0,
                          "Fituica",
                          "cmd.exe",    //orice nume care nu bate la ochi
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          640,
                          480,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    /* load fituica as bitmap */
    bmpsrc = (HBITMAP)LoadImage(NULL, "fituica.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    if(!bmpsrc)
        MessageBoxA(NULL, "Unable to load background image!", "Fatal error", MB_OK | MB_ICONERROR);
    hdcbmp = CreateCompatibleDC(GetDC(0));
    SelectObject(hdcbmp, bmpsrc);

    ZeroMemory(&di.bmiHeader, sizeof(BITMAPINFOHEADER));
    di.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

    if (!GetDIBits(hdcbmp, bmpsrc, 0, 395, NULL, &di, DIB_RGB_COLORS))  //395 px e numarul de linii (inaltimea) imaginii. Pe viitor trebuie automatizat
        MessageBox(NULL, "Failed getting bitmap info", "ERROR", MB_OK);

    if (!GetDIBits(hdcbmp, bmpsrc, 0, 395, buf, &di, DIB_RGB_COLORS))
        MessageBox(NULL, "Failed getting image data", "ERROR", MB_OK);

    int width = di.bmiHeader.biWidth;
    int height = di.bmiHeader.biHeight;

    /* set alpha to 255 - make image not transparent */
    for(int i = 0; i < width * height; ++i)
        buf[i].A = 255;

    ShowWindow(hwnd, nCmdShow);

    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);

    /* init texture from raw pixel data */
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, buf);


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
            /* OpenGL animation code goes here */

            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glEnable(GL_DEPTH_TEST);

            glDepthFunc(GL_LESS);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glMatrixMode(GL_MODELVIEW);

            glPushMatrix();

            glScalef(scale, scale, scale);
            glRotatef(thetax, 0, -1.0, 0);
            glRotatef(thetay, -1.0, 0, 0);

            glBegin(GL_POLYGON);
            //fata spate
            glColor3ub(255, 0 , 0);
            glVertex3f(-0.5f, -0.5f, -0.5f);
            glVertex3f(-0.5f, 0.5f, -0.5f);
            glVertex3f(0.5f, 0.5f, -0.5f);
            glVertex3f(0.5f, -0.5f, -0.5f);
            glEnd();

            //fetele sus si jos
            glBegin(GL_POLYGON);
            glColor3ub(0, 255 , 0);
            glVertex3f(-0.5f, -0.5f, 0.5f);
            glVertex3f(0.5f, -0.5f, 0.5f);
            glVertex3f(0.5f, -0.5f, -0.5f);
            glVertex3f(-0.5f, -0.5f, -0.5f);
            glEnd();

            glBegin(GL_POLYGON);
            glColor3ub(0, 255 , 0);
            glVertex3f(-0.5f, 0.5f, 0.5f);
            glVertex3f(0.5f, 0.5f, 0.5f);
            glVertex3f(0.5f, 0.5f, -0.5f);
            glVertex3f(-0.5f, 0.5f, -0.5f);
            glEnd();

            //fetele stanga si dreapta
            glBegin(GL_POLYGON);
            glColor3ub(0, 0 , 255);
            glVertex3f(-0.5f, -0.5f, 0.5f);
            glVertex3f(-0.5f, -0.5f, -0.5f);
            glVertex3f(-0.5f, 0.5f, -0.5f);
            glVertex3f(-0.5f, 0.5f, 0.5f);
            glEnd();

            glBegin(GL_POLYGON);
            glColor3ub(0, 0 , 255);
            glVertex3f(0.5f, -0.5f, 0.5f);
            glVertex3f(0.5f, -0.5f, -0.5f);
            glVertex3f(0.5f, 0.5f, -0.5f);
            glVertex3f(0.5f, 0.5f, 0.5f);
            glEnd();

            //fata fata

            glEnable(GL_TEXTURE_2D);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
            glBindTexture(GL_TEXTURE_2D, textureID);

            glBegin(GL_QUADS);
            //glColor3ub(255, 255 , 0);
            glTexCoord2f(1, 0); glVertex3f(-0.5f, -0.5f, 0.5f);
            glTexCoord2f(1, 1); glVertex3f(-0.5f, 0.5f, 0.5f);
            glTexCoord2f(0, 1); glVertex3f(0.5f, 0.5f, 0.5f);
            glTexCoord2f(0, 0); glVertex3f(0.5f, -0.5f, 0.5f);
            glEnd();

            glDisable(GL_TEXTURE_2D);
            glPopMatrix();

            SwapBuffers(hDC);

            Sleep (1);
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
    switch (uMsg)
    {
        case WM_CLOSE:
            PostQuitMessage(0);
        break;

        case WM_DESTROY:
            return 0;

        case WM_KEYDOWN:
        {
            switch (wParam)
            {
                case VK_ESCAPE:
                    PostQuitMessage(0);
                    break;
                case VK_ADD:
                    scale += 0.5;
                    break;
                case VK_SUBTRACT:
                    scale -= 0.5;
                    break;

                break;
            }
        }

        case WM_MOUSEMOVE:
        {
            if(wParam == MK_LBUTTON) //este apasat click
            {
                newx = GET_X_LPARAM(lParam);
                newy = GET_Y_LPARAM(lParam);

                dx = newx - oldx;
                dy = newy - oldy;

                oldx = newx;
                oldy = newy;

                thetax += 0.1 * (float)dx;
                thetay += 0.1 * (float)dy;

                truncate(thetax);
                truncate(thetay);

                //cout<<"dx: "<<dx<<", dy: "<<dy<<"\n";
                //cout<<"thetax: "<<thetax<<"thetay: "<<thetay<<"\n";
            }
            break;
        }
        case WM_LBUTTONDOWN:
            {
                oldx = GET_X_LPARAM(lParam);
                oldy = GET_Y_LPARAM(lParam);
                break;
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

