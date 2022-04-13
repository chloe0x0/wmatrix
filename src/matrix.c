#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>

#define ACTIVE_P        0.0005  // Probability that an inactive cell will become active
#define FADE_P          0.002   // Probability that an active cell will become inactive
#define STATE_PHASE     0.9     // Probability that a cell will randomly change its state
#define MAX_LIFESPAN    5       // Max number of iterations a cell can stay active
#define INIT_ACTIVE_P   0.01    // Probability that a cell will be active upon initialization
#define SLEEP_MS        75      // Time in MS to sleep after printing matrix
#define WIDTH           100     // Width of the matrix in Cells      
#define HEIGHT          50      // Height of the matrix in Cells

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

typedef struct{
    char state;
    byte lifespan;
    bool active;
}Node;

Node* InitMatrix(unsigned int cells){
    Node* matrix = malloc( sizeof(Node) * cells );

    for (int i = 0; i < cells; ++i){
        matrix[i].state = RandChar();
        matrix[i].active = ((float)rand() / RAND_MAX) < INIT_ACTIVE_P;
        matrix[i].lifespan = matrix[i].active;
    }

    return matrix;
}

Node* Simulate(Node* Matrix, unsigned int cells){
    Node* NewMatrix = malloc( sizeof(Node) * cells );
    memcpy(NewMatrix, Matrix, sizeof(Node) * cells);

    float p;

    for (int i = 0; i < cells; ++i){        
        p = (float)rand() / RAND_MAX;
        
        if (p < STATE_PHASE){ NewMatrix[i].state = RandChar(); }
        if (Matrix[i].active){
            NewMatrix[i].active = (++NewMatrix[i].lifespan != MAX_LIFESPAN);
            NewMatrix[i].lifespan *= NewMatrix[i].active;

            if (i + WIDTH < cells){ NewMatrix[i + WIDTH].active = true; }
        }else{
            if (p < ACTIVE_P){
                NewMatrix[i].active = true;
            }
        }
    }

    free(Matrix);
    return NewMatrix;
}

void DisplayMatrix(Node* Matrix, unsigned int cells){
    char* matrix = malloc( sizeof(char)*cells + HEIGHT + 1);

    for (int i = 0; i < cells; ++i){
        if ((i + 1) % WIDTH == 0){ matrix[i] = '\n'; continue; }

        Node n = Matrix[i];
        matrix[i] = n.active ? n.state : ' ';
    }
    matrix[cells + HEIGHT + 1] = '\0';

    fwrite(matrix, sizeof(char), cells + 1, stdout);
    free(matrix);
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
    Node* matrix = InitMatrix(cells);

    SetConsoleTextAttribute(handle, GREEN);
    while (true){
        matrix = Simulate(matrix, cells);
        DisplayMatrix(matrix, cells);
        SetConsoleCursorPosition(handle, InitPos);
        Sleep(SLEEP_MS);
    }

    free(matrix);
}