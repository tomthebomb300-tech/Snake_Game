#include <windows.h>
#include <stdio.h>

const char g_szClassName[] = "snakeWindow";
HWND hwnd;//handle to window
HDC hdc;//handle of client area

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

HBRUSH backgroundBrush;
HBRUSH snakeBrush;
HBRUSH foodBrush;


typedef struct square {
    int x; //x coordinate upper left corner
    int y; //y coordinate upper left corner
} Square;

typedef struct snakeSquare {
    Square square; //details of square
    Square lastSquare;//Prior position of square
    struct snakeSquare* next; //next snake square if any.
} SnakeSquare;

typedef enum{
    NONE,
    BOUNDARY,
    FOOD,
    TAIL
} CollisionType;

typedef enum{
    RIGHT,
    LEFT,
    UP,
    DOWN
} KEYDOWN;

int game_score;
SnakeSquare head;
Square food;

int key_down[sizeof(KEYDOWN)];



void addTail(){
    SnakeSquare *node = &head;
    while(node->next != NULL){node = node->next;}

    Square s = {node->lastSquare.x, node->lastSquare.y};
    Square ls = {0,0};
    SnakeSquare *tail = malloc(sizeof(SnakeSquare));
    tail->square = s;
    tail->lastSquare = ls;
    tail->next = NULL;
    node->next = tail;
}

int tailCollision(){
    SnakeSquare *node = &head;
    while(node->next != NULL){
        node = node->next;
        //if coords are same its a hit
        if(node->square.x == head.square.x && node->square.y == head.square.y)return 1;
    }
    return 0;
}

int boundaryCollision(){
    int snakeX = head.square.x;
    int snakeY = head.square.y;
    if(snakeX < 0 || snakeX >= CLIENT_AREA_WIDTH || snakeY < 0 || snakeY >= CLIENT_AREA_HEIGHT)
        return 1;
    return 0;
}

int insideCol(int x){
    int snakeX = head.square.x;
    if(x >= snakeX && x <= snakeX+SNAKE_SQUARE_LENGTH)
        return 1;
    return 0;
}

int insideRow(int y){
    int snakeY = head.square.y;
    if(y >= snakeY && y <= snakeY+SNAKE_SQUARE_LENGTH)
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
    else if(tailCollision())
        return TAIL;
    return NONE;
}

void clearScreen(){
    RECT fullBackground;
    GetClientRect(hwnd, &fullBackground);
    FillRect(hdc, &fullBackground, backgroundBrush);
}

void render(){
    clearScreen();
    SelectObject(hdc, foodBrush);
    Rectangle(hdc, food.x, food.y, food.x+FOOD_SQUARE_LENGTH, food.y+FOOD_SQUARE_LENGTH);
    SelectObject(hdc, snakeBrush);
    Rectangle(hdc, head.square.x,head.square.y,head.square.x+SNAKE_SQUARE_LENGTH,head.square.y+SNAKE_SQUARE_LENGTH);

    SnakeSquare *node = &head;
    while(node->next != NULL){
        node = node->next;
        Rectangle(hdc, node->square.x,node->square.y,node->square.x+SNAKE_SQUARE_LENGTH,node->square.y+SNAKE_SQUARE_LENGTH);
    }   
}

void moveto(int x, int y){
    head.lastSquare = head.square;
    head.square.x = x;
    head.square.y = y;

    SnakeSquare *node = &head;
    while(node->next != NULL){
        node->next->lastSquare.x = node->next->square.x;
        node->next->lastSquare.y = node->next->square.y;
        node->next->square.x = node->lastSquare.x;
        node->next->square.y = node->lastSquare.y;
        node = node->next;
    }
    render();
}

int update(){

    //Move Snake, set key_down=0 to not have quick movment
    if(key_down[DOWN]){
        key_down[DOWN]=0;
        moveto(head.square.x, head.square.y + SNAKE_SQUARE_LENGTH); 
    }
    if(key_down[UP]){
        key_down[UP]=0;
        moveto(head.square.x, head.square.y - SNAKE_SQUARE_LENGTH);
    }
    if(key_down[RIGHT]){
        key_down[RIGHT]=0;
        moveto(head.square.x + SNAKE_SQUARE_LENGTH, head.square.y); 
    }
    if(key_down[LEFT]){
        key_down[LEFT]=0;
        moveto(head.square.x - SNAKE_SQUARE_LENGTH, head.square.y); 
    }


    switch(checkCollision()){
        case BOUNDARY:
            return 0;
        case FOOD:
            //Food eaten -> increase score, clear screen to remove food, get coords new food, add another square to snake.
            game_score++;
            food.x = rand()%(CLIENT_AREA_WIDTH-FOOD_SQUARE_LENGTH);
            food.y = rand()%(CLIENT_AREA_HEIGHT-FOOD_SQUARE_LENGTH);
            addTail();
            render();
            printf("Game_Score: %d\n", game_score);
            break;
        case TAIL:
            return 0;
    }


    return 1;
}

//Window Procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
    switch(msg){
        case WM_KEYDOWN:
            if(wParam == VK_DOWN){key_down[DOWN]=1;}
            else if(wParam == VK_UP){key_down[UP]=1;}
            else if(wParam == VK_RIGHT){key_down[RIGHT]=1;}
            else if(wParam == VK_LEFT){key_down[LEFT]=1;}
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
    MSG Msg;

    backgroundBrush = CreateSolidBrush(BACKGROUND_COLOUR);

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

    hdc = GetDC(hwnd);
    snakeBrush = CreateSolidBrush(SNAKE_COLOUR);
    foodBrush = CreateSolidBrush(FOOD_COLOUR);

    
    //Snake starting point + food starting point
    head.square.x = 0;
    head.square.y = 0;
    head.next = NULL;
    food.x = rand()%(CLIENT_AREA_WIDTH-FOOD_SQUARE_LENGTH);
    food.y = rand()%(CLIENT_AREA_HEIGHT-FOOD_SQUARE_LENGTH);

    game_score = 0;

    //Game loop
    int game_over = 1;
    int quit = 1;
    render();
    while(game_over && quit){
        //The Message Loop
        while(PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE)){
            if(Msg.message == WM_QUIT)
                quit = 0;
            TranslateMessage(&Msg);
            DispatchMessage(&Msg);
        }
        game_over = update();
    }

    DeleteObject(foodBrush);
    DeleteObject(snakeBrush);
    DeleteObject(backgroundBrush);

    return Msg.wParam;
}

