#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>
#include <conio.h>

#define ACTIVE_P        0.000005// Probability that an inactive cell will become active
#define FADE_P          0.002   // Probability that an active cell will become inactive
#define STATE_PHASE     0.9     // Probability that a cell will randomly change its state
#define MAX_LIFESPAN    3       // Max number of iterations a cell can stay active
#define INIT_ACTIVE_P   0.01    // Probability that a cell will be active upon initialization
#define SLEEP_MS        2       // Time in MS to sleep after printing matrix
#define WIDTH           50      // Width of the matrix in Cells      
#define HEIGHT          50      // Height of the matrix in Cells

#define WHITE 7
#define GREEN 2

static HANDLE handle;
static WORD   Attrs;

void ResetConsole(){
    SetConsoleTextAttribute(handle, Attrs);
    system("cls");
}

char RandChar(){
    return 'A' + (rand() % 100);
}

WORD RandColor(){
    return rand() % 256;
}

typedef struct{
    char state;

    byte lifespan, to_activate;
    bool active;
}Node;


Node* InitMatrix(unsigned int cells){
    Node* matrix = malloc( sizeof(Node) * cells );

    for (int i = 0; i < cells; ++i){
        matrix[i].state = RandChar();
        matrix[i].active = ((float)rand() / RAND_MAX) < INIT_ACTIVE_P;
        matrix[i].to_activate = 0;
        matrix[i].lifespan = matrix[i].active;
    }

    return matrix;
}

void Simulate(Node* Matrix, unsigned int cells, unsigned int width){
    for (int i = 0; i < cells; ++i){
        float p = (float)rand() / RAND_MAX;
        if (p < STATE_PHASE){ Matrix[i].state = RandChar(); }

        if (Matrix[i].to_activate + 1 == 3){ Matrix[i].active = true; Matrix[i].to_activate = 0; }

        if (Matrix[i].active){
            if ((p < FADE_P) || (++Matrix[i].lifespan >= MAX_LIFESPAN)){
                Matrix[i].active = false;
                Matrix[i].lifespan = 0;
                Matrix[i].to_activate = 0;
            }

            Matrix[i + width].to_activate++;
        }else{
            if (p < ACTIVE_P){
                Matrix[i].to_activate++;
            }
        }
    }
}

void DisplayMatrix(Node* Matrix, unsigned int cells, unsigned int width, WORD* ColorArray){
    for (int i = 0; i < cells; ++i){
        Node n = Matrix[i];
        SetConsoleTextAttribute(handle, ColorArray[n.lifespan]);
        putchar(n.state);
        if ((i + 1) % width == 0){
            putchar('\n');
        }
    }
}

int main(void){
    atexit(ResetConsole);
    srand(time(NULL));

    handle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(handle, &csbi);
    Attrs = csbi.wAttributes;

    unsigned int Width, Height;
    Width = 100;
    Height = 50;

    unsigned int cells = Width * Height;

    COORD InitPos = csbi.dwCursorPosition;

    for (int i = 0; i < cells; ++i){
        printf(" ");
        if ((i + 1) % Width == 0){
            printf("\n");
        }
    }

    WORD* Colors = calloc(MAX_LIFESPAN, sizeof(WORD));
    Colors[0] = 0x0;
    Colors[1] = WHITE;
    for (int i = 2; i < MAX_LIFESPAN; ++i){ Colors[i] = GREEN; }

    Node* matrix = InitMatrix(Width * Height);

    for (int i = 0; i < 10000; ++i){
        SetConsoleCursorPosition(handle, InitPos);
        Simulate(matrix, Width * Height, Width);
        DisplayMatrix(matrix, Width * Height, Width, Colors);
    }
}