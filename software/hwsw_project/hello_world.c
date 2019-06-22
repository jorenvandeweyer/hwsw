#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "alt_types.h"
#include "system.h"
#include "altera_avalon_pio_regs.h"

#define WIDTH 40
#define HEIGHT 32

#define N 19//14//30 // num rows
#define M 23//18//38 // num cols
#define ZX 7 // origin x
#define ZY 7 // origin y

#define OP_BOARD 0x00010000
#define OP_SPRITE 0x00020000
#define OP_COLOR 0x00030000

#define SPRITE_SIZE 256

#define LEFT_KEY 7
#define RIGHT_KEY 11

alt_u32 prev = 0;
alt_u32 value = 1;

void wr(alt_u32 op, int data){
  IOWR_ALTERA_AVALON_PIO_DATA(OPERATION_BASE, op);
  IOWR_ALTERA_AVALON_PIO_DATA(DATA_BASE, data);
}

/**
 * write to color
 */
void wc(int offset, int data){
  wr(OP_COLOR | offset, data);
}

/**
 * write to sprites
 */
void ws(int offset, int data){
  wr(OP_SPRITE | offset, data);
}

/**
 * write to Board
 */
void wb(int offset, int data){
  wr(OP_BOARD | offset, data);
}

void wbcoord(int row, int col, int data){
  wb(row * 80 + col, data);
}

/**
 * reead the Keys
 */
void rd(){
    alt_u32 temp = IORD_ALTERA_AVALON_PIO_DATA(BUTTONS_BASE);

    if (temp != prev){
        value = temp;
        prev = temp;
    }else value = 1;
}

enum Direction{North, East, South, West} direction;

int Field[N][M];
int x, y;
int gy, head, tail;
int game;

int food, a, b;
int score;
int speed;

/**
 * clear the whole screen
 */
void clearScreen(){
    for (int i = 0; i < 1280; i++){
        wb(i, 0);
    }
}

/**
 * clear play field
 */
void clearBoard(){
  for (int i = ZX + 1; i < ZX + M + 1; i++){
    for (int j = ZY + 1; j < ZY + N + 1; j++){
      wbcoord(j, i, 0);
    }
  }
}

void createTitle(){

}

void initBoard(){
    createTitle();

    int sprite = 2;
    for (int col = ZX; col < ZX + M + 2; col++){
      wbcoord(ZY, col, sprite);
      wbcoord(ZY + N + 1, col, sprite);
    }

    for (int row = ZY; row < ZY + N + 2; row++){
        wbcoord(row, ZX, sprite);
        wbcoord(row, ZX + M + 1, sprite);
    }

    int tx = ZX;
    int ty = 1;

    // S
    for (int i = tx; i < tx + 4; i++){
      wbcoord(ty, i, 3);
      wbcoord(ty + 2, i, 3);
      wbcoord(ty + 4, i, 3);
    }
    wbcoord(ty + 1, tx, 3);
    wbcoord(ty + 3, tx + 3, 3);

    tx += 5;

    // N
    for (int j = ty; j < ty + 5; j++){
      wbcoord(j, tx, 3);
      wbcoord(j, tx + 4, 3);
    }
    for (int k = 1; k < 4; k++)wbcoord(ty + k, tx + k, 3);

    tx += 6;

    // A
    for (int i = tx; i < tx + 4; i++){
      wbcoord(ty, i, 3);
      wbcoord(ty + 2, i, 3);
    }
    for (int j = ty + 1; j < ty + 5; j++){
      wbcoord(j, tx, 3);
      wbcoord(j, tx + 3, 3);
    }

    tx += 5;

    // K
    for (int j = ty; j < ty + 5; j++) wbcoord(j, tx, 3);
    for (int k = 1; k < 4; k++){
      wbcoord(ty + 3 - k, tx + k, 3);
      wbcoord(ty + 1 + k, tx + k, 3);
    }

    tx += 5;

    // E
    for (int j = ty; j < ty + 5; j++) wbcoord(j, tx, 3);
    for (int i = tx + 1; i < tx + 5; i++){
      wbcoord(ty, i, 3);
      wbcoord(ty + 4, i, 3);
    }
    for (int i = tx + 1; i < tx + 4; i++) wbcoord(ty + 2, i, 3);    

    for (int i = 0; i < ZX; i++){
      for (int j = 0; j < HEIGHT; j++) wbcoord(j, i, 0);
    }

    for (int i = ZX + M + 2; i < WIDTH; i++){
      for (int j = 0; j < HEIGHT; j++) wbcoord(j, i, 0);
    }    

    wbcoord(HEIGHT - 6, 7, 2);
    wbcoord(HEIGHT - 5, 7, 2);
}

void init(){
    for (int i = 0x00000000; i < 0x00000040; i++){
        wc(i, 0x000000);
    }
    
    for (int i = 0x00000000; i < 0x00002000; i++){
	    ws(i, 0);
    }

    clearScreen();

    wc(1, 0xffffff);
    wc(2, 0xff0000);
    wc(3, 0x00ff00);

    for (int i = 0; i <= 4; i++){
        int start = SPRITE_SIZE * i;
        for (int j = start; j < start + SPRITE_SIZE; j++){
            if (i == 0) ws(j, 0);
            if (i == 1) ws(j, 1);
            if (i == 2) ws(j, 1);
            if (i == 3) ws(j, 2);
            if (i == 4) ws(j, 3);
        }
    }

    initBoard();
}

void snakeInit(){
    for (int i = 0; i < N; i++){
        for (int j = 0; j < M; j++){
            Field[i][j] = 0;
        }
    }

    x = N / 2;
    y = M / 2;

    gy = y;
    head = 5;
    tail = 1;
    game = 0; // 0 = running, 1 = end game
    food = 0;
    
    direction = East;

    score = 0;
    speed = 150;

    for (int i = 0; i < head; i++){
        gy++;
        Field[x][gy - head] = i + 1;
    }
}

/* 
    CONDITIONS:

    -> Field[i][j] == 0 => empty square
    -> Field[i][j] > 0 && Field[i][j] != head => body
    -> Field[i][j] == head => head
    -> Field[i][j] == -1 => food
*/
void show(){
    clearBoard();
    //initBoard();

    for (int i = 0; i < N; i++){
        for (int j = 0; j < M; j++){
            int py = i + 1 + ZY;
            int px = j + 1 + ZX;
            if (Field[i][j] == 0) wbcoord(py, px, 0); // empty square
            if (Field[i][j] > 0 || Field[i][j] == head) wbcoord(py, px, 2); // snake
            if (Field[i][j] == -1) wbcoord(py, px, 3); // food
        }
    }
}

void random(){
    srand(time(0));
    a = 1 + rand() % (N - 2); // random num 1->18
    b = 1 + rand() % (M - 2);

    if (food == 0 && Field[a][b] == 0){ // als er geen eten is
        Field[a][b] = -1;
        food = 1;

        if (speed > 70 && score != 0) speed -= 5;
    }
}

void gameOver(){
    clearBoard();

    int tx = ZX + 2;
    int ty = ZY + 3;

    // G
    for (int i = tx; i < tx + 4; i++) {
      wbcoord(ty, i, 3);
      wbcoord(ty + 4, i, 3);
    }

    for (int j = ty + 1; j < ty + 4; j++) wbcoord(j, tx, 3);
    for (int j = ty + 2; j < ty + 4; j++) wbcoord(j, tx + 3, 3);
    wbcoord(ty + 2, tx + 2, 3);

    tx += 5;
    
    // A
    for (int i = tx; i < tx + 4; i++){
      wbcoord(ty, i, 3);
      wbcoord(ty + 2, i, 3);
    }
    for (int j = ty + 1; j < ty + 5; j++){
      wbcoord(j, tx, 3);
      wbcoord(j, tx + 3, 3);
    }

    tx += 5;

    // M
    for (int i = tx; i < tx + 5; i++) wbcoord(ty, i, 3);
    for (int j = ty + 1; j < ty + 5; j++){
      wbcoord(j, tx, 3);
      wbcoord(j, tx + 4, 3);
    }
    for (int j = ty + 1; j < ty + 4; j++) wbcoord(j, tx + 2, 3);

    tx += 6;

    // E
    for (int j = ty; j < ty + 5; j++) wbcoord(j, tx, 3);
    for (int i = tx + 1; i < tx + 5; i++){
      wbcoord(ty, i, 3);
      wbcoord(ty + 4, i, 3);
    }
    for (int i = tx + 1; i < tx + 4; i++) wbcoord(ty + 2, i, 3);

    tx = ZX + 2;
    ty += 9;

    // O
    for (int i = tx; i < tx + 4; i++){
      wbcoord(ty, i, 3);
      wbcoord(ty + 4, i, 3);
    }
    for (int j = ty + 1; j < ty + 4; j++){
      wbcoord(j, tx, 3);
      wbcoord(j, tx + 3, 3);
    }

    tx += 5;

    // V
    for (int j = ty; j < ty + 3; j++){
      wbcoord(j, tx, 3);
      wbcoord(j, tx + 4, 3);
    }
    wbcoord(ty + 3, tx + 1, 3);
    wbcoord(ty + 3, tx + 3, 3);
    wbcoord(ty + 4, tx + 2, 3);

    tx += 6;

    // E
    for (int j = ty; j < ty + 5; j++) wbcoord(j, tx, 3);
    for (int i = tx + 1; i < tx + 5; i++){
      wbcoord(ty, i, 3);
      wbcoord(ty + 4, i, 3);
    }
    for (int i = tx + 1; i < tx + 4; i++) wbcoord(ty + 2, i, 3);

    tx += 6;

    // R
    for (int j = ty; j < ty + 5; j++) wbcoord(j, tx, 3);
    for (int i = tx + 1; i < tx + 4; i++){
      wbcoord(ty, i, 3);
      wbcoord(ty + 2, i, 3);
    }
    wbcoord(ty + 1, tx + 3, 3);
    wbcoord(ty + 3, tx + 2, 3);
    wbcoord(ty + 4, tx + 3, 3);

    for (int i = 0; i < ZX; i++){
      for (int j = 0; j < HEIGHT; j++) wbcoord(j, i, 0);
    }

    for (int i = ZX + M + 2; i < WIDTH; i++){
      for (int j = 0; j < HEIGHT; j++) wbcoord(j, i, 0);
    }

    wbcoord(HEIGHT - 6, 7, 2);
    wbcoord(HEIGHT - 5, 7, 2);

    while (1){
        rd();
        if (value == LEFT_KEY){
            clearBoard();
            snakeInit();
            break;
        }else if (value == RIGHT_KEY){
            game = 1;
            break;
        }
    }

    /*
        Clear screen    
    */
}

void checkGameOver(){
    if (Field[x][y] != 0 && Field[x][y] != -1) gameOver();
}

void checkFood(){
    if (Field[x][y] == -1){
        food = 0;
        score += 5;
        tail -= 2;
    }
}

void updateHead(){
    head++;
    Field[x][y] = head;
}

void movement(){
    rd();

    if (value == LEFT_KEY){
        switch (direction){
            case East: direction = North; break;
            case North: direction = West; break;
            case West: direction = South; break;
            default: direction = East; break;
        }
    }else if (value == RIGHT_KEY){
        switch (direction){
            case East: direction = South; break;
            case South: direction = West; break;
            case West: direction = North; break;
            default: direction = East; break;
        }
    }

    switch (direction){
        case East:
            y++;
            if (y == M) y = 0; break;
        case West: 
            y--;
            if (y == -1) y = M - 1; break;
        case South:
            x++;
            if (x == N) x = 0; break;
        case North: 
            x--;
            if (x == -1) x = N - 1; break;
    }

    checkGameOver();
    checkFood();
    updateHead();
}

void tailRemove(){
    for (int i = 0; i < N; i++){
        for (int j = 0; j < M; j++){
            if (Field[i][j] == tail){
                Field[i][j] = 0;
            }
        }
    }

    tail++;
}

void delay(int millis){
  clock_t start_time = clock();
  while (clock() < start_time + millis);
}

int main(){
    init();
    snakeInit();
    while (game == 0){
        show();
        random();
        movement();
        tailRemove();
        delay(speed);
    }

    return 0;
}



