#include <windows.h>
#include <stdio.h>

const char g_szClassName[] = "snakeWindow";
const int WINDOW_HEIGHT = 800;
const int WINDOW_WIDTH = 800;
const int SQUARE_LENGTH = 50;

//Colours of stuff
const COLORREF backgroundColour = RGB(0,0,255);
const COLORREF snakeColour = RGB(200,200,0);
const COLORREF foodColour = RGB(255,0,0);


typedef struct square {
    int x; //x coordinate upper left corner
    int y; //y coordinate upper left corner
} Square;

Square head;

void clearScreen(HWND hwnd){
    HBRUSH backgroundBrush = CreateSolidBrush(backgroundColour);
    RECT fullBackground;
    GetClientRect(hwnd, &fullBackground);
    FillRect(GetDC(hwnd), &fullBackground, backgroundBrush);
    DeleteObject(backgroundBrush);
}

//Window Procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
    switch(msg){
        case WM_KEYDOWN:
            if(wParam == VK_DOWN){
                head.y = head.y + SQUARE_LENGTH;
            }
            else if(wParam == VK_UP){
                head.y = head.y - SQUARE_LENGTH;
            }
            else if(wParam == VK_RIGHT){
                head.x = head.x + SQUARE_LENGTH;
            }
            else if(wParam == VK_LEFT){
                head.x = head.x - SQUARE_LENGTH;
            }
        clearScreen(hwnd);
        break;
        case WM_CLOSE:
            DestroyWindow(hwnd);
        break;
        case WM_DESTROY:
            PostQuitMessage(0);
        break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
    WNDCLASSEX wc;
    HWND hwnd;
    MSG Msg;

    HBRUSH backgroundBrush = CreateSolidBrush(backgroundColour);

    //Registering the Window Class
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = backgroundBrush;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    if(!RegisterClassEx(&wc)){
        MessageBox(NULL, "Window Registration Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    //Creating the Window
    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        g_szClassName,
        "Snake",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH, WINDOW_HEIGHT,
        NULL, NULL, hInstance, NULL);

    if(hwnd == NULL){
        MessageBox(NULL, "Window Creation Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    HDC hdc = GetDC(hwnd);
    HBRUSH snakeBrush = CreateSolidBrush(snakeColour);

    
    head.x = 0;
    head.y = 0;
    
    //Game loop
    int run = 1;
    while(run){
        //The Message Loop
        while(PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE)){
            if(Msg.message == WM_QUIT)
                run = 0;
            TranslateMessage(&Msg);
            DispatchMessage(&Msg);
        }

        


        SelectObject(hdc, snakeBrush);
        Rectangle(hdc, head.x,head.y,head.x+SQUARE_LENGTH,head.y+SQUARE_LENGTH);
    }

    DeleteObject(snakeBrush);
    DeleteObject(backgroundBrush);

    return Msg.wParam;
}

