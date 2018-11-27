#include <windows.h>

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain (HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpcmdLine, int nCmdShow)
{
    HWND hwnd;                  /* This is the handle for our window */
    MSG messages;               /* Here messages to the application are saved */
    WNDCLASSEX wc;              /* Data structure for the windowclass */

    /* The Window structure */
    wc.hInstance = hThisInstance;
    wc.lpszClassName = "MainWindow";
    wc.lpfnWndProc = WndProc;      /* This function is called by windows */
    wc.style = CS_DBLCLKS;
    wc.cbSize = sizeof (WNDCLASSEX);
    wc.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wc.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor (NULL, IDC_ARROW);
    wc.lpszMenuName = NULL;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    if (!RegisterClassEx (&wc))
    {
        MessageBoxA(NULL, "Unable to register window class!", "Fatal error!", MB_OK | MB_ICONERROR);
        return 0;
    }

    int ix, iy;
    ix = (GetSystemMetrics(SM_CXSCREEN) - 480)/2;
    iy = (GetSystemMetrics(SM_CYSCREEN) - 360)/2;
    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
           WS_EX_TOPMOST | WS_EX_TOOLWINDOW,                   /* Extended possibilites for variation */
           "MainWindow",         /* Classname */
           "Virus",     /* Title Text */
           WS_EX_PALETTEWINDOW, /* default window */
           ix,       /* Windows decides the position */
           iy,       /* where the window ends up on the screen */
           480,                 /* The programs width */
           360,                 /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    SetWindowLong(hwnd, GWL_STYLE, 0);
    ShowWindow (hwnd, nCmdShow);
    PlaySound("muzica.wav", NULL, SND_ASYNC);
    while (GetMessage (&messages, NULL, 0, 0))
    {
        TranslateMessage(&messages);
        DispatchMessage(&messages);
    }
    return messages.wParam;
}


/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    PAINTSTRUCT ps;
    static HBITMAP bmpsrc = NULL;
    static HDC hdcsource = NULL;

    switch (message)                  /* handle the messages */
    {
        case WM_CREATE:
            bmpsrc = (HBITMAP)LoadImage(NULL, "b.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
            if(!bmpsrc)
                MessageBoxA(NULL, "Unable to load background image!", "Fatal error", MB_OK | MB_ICONERROR);
            hdcsource = CreateCompatibleDC(GetDC(0));
            SelectObject(hdcsource, bmpsrc);
            break;
        case WM_PAINT:
            hdc = BeginPaint(hwnd, &ps);
            BitBlt(hdc, 0, 0, 1200, 1200, hdcsource, 0, 0, SRCCOPY);
            EndPaint(hwnd, &ps);
            break;

        case WM_DESTROY:
            PostQuitMessage (0);
            break;
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}
