#include <windows.h>
#include <stdio.h>

const char g_szClassName[] = "snakeWindow";

const int CLIENT_AREA_HEIGHT = 800; //Height playable area
const int CLIENT_AREA_WIDTH = 800;  //Width playable area
const int WINDOW_HEIGHT = CLIENT_AREA_HEIGHT + 43; //Window height for playable area
const int WINDOW_WIDTH = CLIENT_AREA_WIDTH + 20;   //Window width for playable area

const int SNAKE_SQUARE_LENGTH = 50; //Length of snake square
const int FOOD_SQUARE_LENGTH = SNAKE_SQUARE_LENGTH/2; //Length of food square


//Colours of stuff
const COLORREF BACKGROUND_COLOUR = RGB(0,0,0);
const COLORREF SNAKE_COLOUR = RGB(200,200,0);
const COLORREF FOOD_COLOUR = RGB(255,0,0);


typedef struct square {
    int x; //x coordinate upper left corner
    int y; //y coordinate upper left corner
} Square;

int game_score;
Square head;
Square food;

typedef enum{
    NONE,
    BOUNDARY,
    FOOD
} CollisionType;



int boundaryCollision(){
    if(head.x < 0 || head.x >= CLIENT_AREA_WIDTH || head.y < 0 || head.y >= CLIENT_AREA_HEIGHT)
        return 1;
    return 0;
}

int insideCol(int x){
    if(x >= head.x && x <= head.x+SNAKE_SQUARE_LENGTH)
        return 1;
    return 0;
}

int insideRow(int y){
    if(y >= head.y && y <= head.y+SNAKE_SQUARE_LENGTH)
        return 1;
    return 0;
}

int foodCollision(){
    //check if any of the four food corners inside head
    if(insideCol(food.x) && insideRow(food.y))
        return 1;
    if(insideCol(food.x+FOOD_SQUARE_LENGTH) && insideRow(food.y))
        return 1;
    if(insideCol(food.x) && insideRow(food.y+FOOD_SQUARE_LENGTH))
        return 1;
    if(insideCol(food.x+FOOD_SQUARE_LENGTH) && insideRow(food.y+FOOD_SQUARE_LENGTH))
        return 1;
    return 0;
}

CollisionType checkCollision(){
    //Check boundry hit
    if(boundaryCollision())
        return BOUNDARY;
    else if(foodCollision())
        return FOOD;
    return NONE;
}


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
                head.y = head.y + SNAKE_SQUARE_LENGTH;
            }
            else if(wParam == VK_UP){
                head.y = head.y - SNAKE_SQUARE_LENGTH;
            }
            else if(wParam == VK_RIGHT){
                head.x = head.x + SNAKE_SQUARE_LENGTH;
            }
            else if(wParam == VK_LEFT){
                head.x = head.x - SNAKE_SQUARE_LENGTH;
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
    HBRUSH foodBrush = CreateSolidBrush(FOOD_COLOUR);

    
    //Snake starting point + food starting point
    head.x = 0;
    head.y = 0;
    food.x = rand()%(CLIENT_AREA_WIDTH-FOOD_SQUARE_LENGTH);
    food.y = rand()%(CLIENT_AREA_HEIGHT-FOOD_SQUARE_LENGTH);

    game_score = 0;
    
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
        Rectangle(hdc, head.x,head.y,head.x+SNAKE_SQUARE_LENGTH,head.y+SNAKE_SQUARE_LENGTH);

        SelectObject(hdc, foodBrush);
        Rectangle(hdc, food.x, food.y, food.x+FOOD_SQUARE_LENGTH, food.y+FOOD_SQUARE_LENGTH);
        
        switch(checkCollision()){
            case BOUNDARY:
                run = 0;
                break;
            case FOOD:
                clearScreen(hwnd);
                game_score++;
                printf("Game_Score: %d\n", game_score);
                food.x = rand()%(CLIENT_AREA_WIDTH-FOOD_SQUARE_LENGTH);
                food.y = rand()%(CLIENT_AREA_HEIGHT-FOOD_SQUARE_LENGTH);

        }
    }

    DeleteObject(snakeBrush);
    DeleteObject(backgroundBrush);

    return Msg.wParam;
}

