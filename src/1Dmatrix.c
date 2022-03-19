/////////////////////////////////////////////////////////////
///                                                       ///
///           1 Dimensional Matrix Rain                   ///
///                                                       ///
///                                                       ///
///                                                       ///
///                                                       ///
/////////////////////////////////////////////////////////////


#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>

#define CELLS           150 // Width of the Automata
#define CELL_COLOR      2   // Color of the cell as a WORD
#define ACTIVE_P        0.05// Probability that an inactive cell will become active
#define FADE_P          0.4 // Probability that an active cell will become inactive
#define STATE_PHASE     0.1 // Probability that a cell will randomly change its state
#define MAX_LIFESPAN    5   // Max number of iterations a cell can stay active
#define INIT_ACTIVE_P   0.01// Probability that a cell will be active upon initialization
#define SLEEP_MS        15  // Time in MS to sleep after printing matrix

static HANDLE handle;
static WORD   Attrs;
// Reset the console attributes upon exit
void ResetConsole(){
    SetConsoleTextAttribute(handle, Attrs);
}

char RandChar(){
    return 'A' + (rand() % 55);
}

typedef struct{
    char state;
    unsigned int lifespan;

    bool active;
}Cell;

Cell* InitMatrix(){
    Cell* matrix = malloc( sizeof(Cell) * CELLS );

    for (int i = 0; i < CELLS; ++i){
        matrix[i].state = RandChar();
        matrix[i].active = ((float)rand() / RAND_MAX) < INIT_ACTIVE_P;
        matrix[i].lifespan = matrix[i].active;
    }

    return matrix;
}

void Simulate(Cell* Matrix){
    for (int i = 0; i < CELLS; ++i){
        Cell cell = Matrix[i];
        float p = ((float)rand() / RAND_MAX);

        if (p < STATE_PHASE){
            Matrix[i].state = RandChar();
        }

        if (Matrix[i].active){
            if ((p < FADE_P) || (Matrix[i].lifespan++ >= MAX_LIFESPAN)){
                Matrix[i].active = false;
                Matrix[i].lifespan = 0;
            }
        }else{
            Matrix[i].active = p < ACTIVE_P;
        }
    }
}

void DisplayMatrix(Cell* Matrix){
    for (int i = 0; i < CELLS; ++i){
        Cell cell = Matrix[i];

        if(cell.active){
            putchar(cell.state);
        }else{
            putchar(' ');
        }
    }

    putchar('\n');
}


int main(void){
    srand(time(NULL));
    atexit(ResetConsole);

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    handle = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(handle, &csbi);

    Attrs = csbi.wAttributes;

    SetConsoleTextAttribute(handle, CELL_COLOR);


    Cell* Matrix = InitMatrix();
 
    while(1){
        DisplayMatrix(Matrix);
        Simulate(Matrix);
        Sleep(SLEEP_MS);
        fflush(stdout);
    }
}