#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>

// TODO
// optimize somehow

#define ACTIVE_P        0.0005  // Probability that an inactive cell will become active
#define FADE_P          0.002   // Probability that an active cell will become inactive
#define STATE_PHASE     0.9     // Probability that a cell will randomly change its state
#define MAX_LIFESPAN    6       // Max number of iterations a cell can stay active
#define INIT_ACTIVE_P   0.01    // Probability that a cell will be active upon initialization
#define SLEEP_MS        2       // Time in MS to sleep after printing matrix
#define WIDTH           100     // Width of the matrix in Cells      
#define HEIGHT          25      // Height of the matrix in Cells

#define WHITE 7
#define GREEN 2

static HANDLE handle;
static WORD   Attrs;

void ResetConsole(){
    SetConsoleTextAttribute(handle, Attrs);
    system("cls");
}

char RandChar(){
    return 'A' + (rand() % 25);
}

WORD RandColor(){
    return rand() % 256;
}

typedef struct{
    char state;
    COORD MatrixPos;
    byte lifespan;
    bool active;
}Node;

Node* InitMatrix(unsigned int cells, COORD InitPos){
    Node* matrix = malloc( sizeof(Node) * cells );

    for (int i = 0; i < cells; ++i){
        COORD MatrixPos;
        MatrixPos.X = (i % WIDTH) + InitPos.X;
        MatrixPos.Y = floor((double)i / WIDTH) + InitPos.Y;

        matrix[i].state = RandChar();
        matrix[i].active = ((float)rand() / RAND_MAX) < INIT_ACTIVE_P;
        matrix[i].lifespan = matrix[i].active;
        matrix[i].MatrixPos = MatrixPos;
    }

    return matrix;
}

Node* Simulate(Node* Matrix, unsigned int cells, unsigned int width){
    Node* NewMatrix = malloc( sizeof(Node) * cells );
    memcpy(NewMatrix, Matrix, sizeof(Node) * cells);

    float p;
    int ix;
    bool xx;

    for (int i = 0; i < cells; ++i){        
        p = (float)rand() / RAND_MAX;

        if (Matrix[i].active){
            xx = (bool)!(++NewMatrix[i].lifespan == MAX_LIFESPAN);
            NewMatrix[i].active = xx;
            if (!NewMatrix[i].active){ NewMatrix[i].lifespan = 0; }

            NewMatrix[i + width].active = true;
        }else{
            if (p < ACTIVE_P){
                NewMatrix[i].active = true;
            }
            if (p < STATE_PHASE){ NewMatrix[i].state = RandChar(); }
        }
    }

    free(Matrix);
    return NewMatrix;
}

void DisplayMatrix(Node* Matrix, unsigned int cells, unsigned int width, WORD* ColorArray){
    for (int i = 0; i < cells; ++i){
        Node n = Matrix[i];
        
        SetConsoleCursorPosition(handle, n.MatrixPos);
        SetConsoleTextAttribute(handle, ColorArray[n.lifespan]);
        fwrite(&n.state, sizeof(char), 1, stdout);
    }
}

int main(void){
    atexit(ResetConsole);
    srand(time(NULL));

    handle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(handle, &csbi);
    Attrs = csbi.wAttributes;

    unsigned int cells = WIDTH * HEIGHT;

    COORD InitPos = csbi.dwCursorPosition;

    WORD* Colors = calloc(MAX_LIFESPAN, sizeof(WORD));
    Colors[0] = 0x0;
    Colors[1] = WHITE;
    for (int i = 2; i < MAX_LIFESPAN; ++i){ Colors[i] = GREEN; }

    Node* matrix = InitMatrix(cells, InitPos);

    while (true){
        matrix = Simulate(matrix, cells, WIDTH);
        DisplayMatrix(matrix, cells, WIDTH, Colors);
    }
}