#include <windows.h>
#include <stdio.h>

const char g_szClassName[] = "snakeWindow";
const int CLIENT_AREA_HEIGHT = 800;
const int CLIENT_AREA_WIDTH = 800;
const int WINDOW_HEIGHT = CLIENT_AREA_HEIGHT + 43;
const int WINDOW_WIDTH = CLIENT_AREA_WIDTH + 20;
const int SQUARE_LENGTH = 50;
const int PLAY_AREA_LENGTH = 500;

//Colours of stuff
const COLORREF BACKGROUND_COLOUR = RGB(0,0,0);
const COLORREF SNAKE_COLOUR = RGB(200,200,0);
const COLORREF FOOD_COLOUR = RGB(255,0,0);


typedef struct square {
    int x; //x coordinate upper left corner
    int y; //y coordinate upper left corner
} Square;

Square head;

void clearScreen(HWND hwnd){
    HBRUSH backgroundBrush = CreateSolidBrush(BACKGROUND_COLOUR);
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
        printf("x: %d, y: %d\n", head.x, head.y);
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

    HBRUSH backgroundBrush = CreateSolidBrush(BACKGROUND_COLOUR);

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
    HBRUSH snakeBrush = CreateSolidBrush(SNAKE_COLOUR);

    RECT r;
    GetClientRect(hwnd, &r);
    printf("left = %ld\n", r.left);
    printf("top = %ld\n", r.top);
    printf("right = %ld\n", r.right);
    printf("bottom = %ld\n", r.bottom);

    
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

