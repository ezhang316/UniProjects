// Assignment 1 21T2 COMP1511: Minesweeper
// minesweeper.c
//
// This program was written by Evan Zhang (z5383657)
// on 06/07/2021 - 12/07/2021
//
// Version 1.0.0 (2021-06-19): Assignment released.

#include <stdio.h>
#include <stdlib.h>

// Possible square states.
#define VISIBLE_SAFE    0
#define HIDDEN_SAFE     1
#define HIDDEN_MINE     2

// The size of the starting grid.
#define SIZE              8

// The possible command codes.
#define DETECT_ROW      1
#define DETECT_SQUARE   2
#define REVEAL_CROSS    3
#define GAME_MODE       4
#define FLAG_MINE       5
#define DEFUSE          6

// Add any extra #defines here.
#define GAME_WON 69
#define GAME_LOST 70
#define GAME_IN_PROGRESS 71

#define GP_SIZE 11
#define DEBUG_MODE 404
#define GP_MODE 405

void initialise_field(int minefield[SIZE][SIZE]);
void print_debug_minefield(int minefield[SIZE][SIZE]);

// Place your function prototyes here.

void detect_row(int row, int column, int length, int minefield[SIZE][SIZE]);
void detect_square(int row, int column, int length, int minefield[SIZE][SIZE]);
void modified_detect_square(int row, int column, int minefield[SIZE][SIZE]);
void print_formatted_minefield(int game_status, int minefield[SIZE][SIZE]);
void field_converter(int minefield[SIZE][SIZE], int game_status, int i, int j);
int format_field_num_mines_sqr(int row, int column, int minefield[SIZE][SIZE]);
void print_correct_minefield(int game_status, int mode, int minefield[SIZE][SIZE]); 


int main(void) {
    int minefield[SIZE][SIZE];

    initialise_field(minefield);
    printf("Welcome to minesweeper!\n");
    printf("How many mines? ");

    // TODO: Scan in the number of pairs of mines.
    int num_mine_pairs;
    scanf("%d", &num_mine_pairs);

    printf("Enter pairs:\n");

    // TODO: Scan in the pairs of mines and place them on the grid.
    int lc1 = 0;
    while (lc1 < num_mine_pairs && lc1 < SIZE * SIZE) {
        int mine_row = 0;
        int mine_column = 0;
        scanf("%d%d", &mine_row, &mine_column);
        if (mine_row < SIZE && mine_column < SIZE) {
            if (mine_row >= 0 && mine_column >= 0) {
                minefield[mine_row][mine_column] = HIDDEN_MINE;
            }
        }
        lc1++;
    }

    printf("Game Started\n");
    print_debug_minefield(minefield);


    // TODO: Scan in commands to play the game until the game ends.
    // A game ends when the player wins, loses, or enters EOF (Ctrl+D).
    // You should display the minefield after each command has been processed.
    
    
    int game_status = GAME_IN_PROGRESS;
    int used_hints = 0;
    int mode = DEBUG_MODE;
    
    // This while loop prompts commands until the game is won/lost.
    while (game_status == GAME_IN_PROGRESS) {
        int command = 0;
        int row = 0;
        int column = 0;
        int length = 0;
        scanf("%d", &command);
                
        // This if statement is responsible for the detect row command.
        if (command == DETECT_ROW) {

            scanf("%d%d%d", &row, &column, &length);

            //Coordinate eligibility checker.
            if (column + length - 1 >= SIZE || row >= SIZE) {
                printf("Coordinates not on map\n");
            }
            else if (row < 0 || column < 0) {
                printf("Coordinates not on map\n");
            }
            
            //Detect row command.
            else if (used_hints < 3) {
                detect_row(row, column, length, minefield);
                used_hints++;
                
            }
            //If user has used a hint >3 times.
            else {
                printf("Help already used\n");
            }
            
            print_correct_minefield(game_status, mode, minefield);
        }
       
        // This if statement is responsible for the detect square command.
        else if (command == DETECT_SQUARE) {
        
            scanf("%d%d%d", &row, &column, &length);
            
            //Coordinate eligibility checker.
            if (column >= SIZE || row >= SIZE) {
                printf("Coordinates not on map\n");
            }
            else if (row < 0 || column < 0) {
                printf("Coordinates not on map\n");
            }
            else if (used_hints < 3) {
                detect_square(row, column, length, minefield);
                used_hints++;
            }
            else {
                printf("Help already used\n");
            }
            print_correct_minefield(game_status, mode, minefield);
        }
        
        // This if statement is responsible for the reveal cross command.
        else if  (command == REVEAL_CROSS) {
        
            scanf("%d%d", &row, &column);
            
            //Coordinate eligibility checker.
            if (column >= SIZE || row >= SIZE) {
                printf("Coordinates not on map\n");
                print_correct_minefield(game_status, mode, minefield);
            } 
            else if (row < 0 || column < 0) {
                printf("Coordinates not on map\n");
                print_correct_minefield(game_status, mode, minefield);
            }
            else {
                //If picked coordinate is a mine.
                if (minefield[row][column] == HIDDEN_MINE) {
                    game_status = GAME_LOST;
                }
                //Checking for mines in a cross shape.
                else {
                    minefield[row][column] = 0;
                    modified_detect_square(row - 1, column, minefield);
                    modified_detect_square(row + 1, column, minefield);
                    modified_detect_square(row, column - 1, minefield);
                    modified_detect_square(row, column + 1, minefield);
                    print_correct_minefield(game_status, mode, minefield);
                }
            }
        }
        
        // Section responsible for changing the type of map printed.
        else if (command == GAME_MODE) {
            if (mode == GP_MODE) {
                mode = DEBUG_MODE;
                printf("Debug mode activated\n");
                print_correct_minefield(game_status, mode, minefield);
            }
            else {
                mode = GP_MODE;
                printf("Gameplay mode activated\n");
                print_correct_minefield(game_status, mode, minefield);
            }
            
        }
        
        // This section checks if all the squares have been revealed 
        // and so whether the game/while loop needs to be ended.
        int unrevealed_squares = 0;
        int lc5 = 0;
        while (lc5 < SIZE) {
            int lc6 = 0;
            while (lc6 < SIZE) {
                if (minefield[lc5][lc6] == 1) {
                    unrevealed_squares++;
                    lc6 = SIZE;
                    lc5 = SIZE;
                }
                lc6++;
            }
            lc5++;
        }
        if (unrevealed_squares == 0) {
            game_status = GAME_WON;
        }
    }
    
    // This section announces to the player when the game is won/lost.
    if (game_status == GAME_WON) {
        printf("Game Won!\n");
    }
    else if (game_status == GAME_LOST) {
        printf("Game over\n");
        print_correct_minefield(game_status, mode, minefield);
    }
    return 0;
}

//Function for detecting mines in a row with a given length.
void detect_row(int row, int column, int length, int minefield[SIZE][SIZE]){
    int num_mines_row = 0;
    int lc2 = 0;
    while (lc2 < length) {
        if (minefield[row][column + lc2] == HIDDEN_MINE) {
            num_mines_row++;
        }
        lc2++;
    }
    printf("There are %d mine(s) in row %d, ", num_mines_row, row); 
    printf("from column %d to %d\n", column, column + length - 1);
}


//Function that detects mines in a given square.
void detect_square(int row, int column, int length, int minefield[SIZE][SIZE]) {
    int num_mine_square = 0;
    int row_start = row - length / 2;
    int column_start = column - length / 2;
    int lc3 = 0;
    while (lc3 < length) {
        int lc4 = 0;
        while (lc4 < length) {
            //Coordinate eligibility checker.       
            if (row_start + lc3 >= SIZE || column_start + lc4 >= SIZE) {
            }
            else if (row_start + lc3 < 0 || column_start + lc4 < 0) {
            }
            //Records number of mines in the square.
            else if (minefield[row_start + lc3][column_start + lc4] == HIDDEN_MINE) {
                num_mine_square++;
            }
            lc4++;
        }
        lc3++;
    }
    printf("There are %d mine(s) in the square centered", num_mine_square);
    printf(" at row %d, column %d of size %d\n", row, column, length);
}

//Detect square function modified for use in the reveal cross function.
void modified_detect_square(int row, int column, int minefield[SIZE][SIZE]) {
    int num_mine_square = 0;
    int row_start = row - 1;
    int column_start = column - 1;
    int lc3 = 0;
    while (lc3 < 3) {
        int lc4 = 0;
        while (lc4 < 3) {
            //Coordinate eligibility checker.
            if (row_start + lc3 >= SIZE || column_start + lc4 >= SIZE) {
            }
            else if (row_start + lc3 < 0 || column_start + lc4 < 0) {
            }
            //Records number of mines in a square.
            else if (minefield[row_start + lc3][column_start + lc4] == HIDDEN_MINE) {
                num_mine_square++;
            }
            lc4++;
        }
        lc3++;
    }
    //Coordinate eligibility checker.
    if (column >= SIZE || row >= SIZE) {
    }
    else if (row < 0 || column < 0) {
    }
    //Changes detected square to 0.
    else if (num_mine_square == 0) {
        minefield[row][column] = 0;
    }
}

// Prints formatted minefield
void print_formatted_minefield(int game_status, int minefield[SIZE][SIZE]) {
    if (game_status == GAME_LOST) {
        printf("xx\n/\\\n");
    }
    else {
        printf("..\n\\/\n");
    }
    printf("    ");
    int i = 0;
    while (i < GP_SIZE) {
        int j = 0;
        while (j < GP_SIZE) {
            // This section is the border around the formatted minefield
            if (i == 0 && j < 10 && j > 1) {
                printf("0%d ", j - 2);
                if (j == 9) {
                    printf("\n   -");
                }
            }
            else if (i == 1 && j < 10 && j > 1) {
                printf("---");
                if (j == 9) {
                    printf("\n");
                }
            }
            else if (j == 0 && i < 10 && i > 1) {
                printf("0%d ", i - 2);
            }
            else if (j == 1 && i < 10 && i > 1) {
                printf("|");
            }
            else if (j == 10 && i < 10 && i > 1) {
                printf("|\n");
            }
            else if (i == 10 && j < 10 && j > 1) {
                printf("---");
                if (j == 9) {
                    printf("\n");
                }
            }
            else if (i == 10 && j == 0) {
                printf("   -");
            }
            // This section runs the debug to gameplay field converter
            // function within the formatted border.
            else if (i > 1 && i < 10) {
                if (j > 1 && j < 10) {
                    field_converter(minefield, game_status, i, j);
                }
            }
            j++;
        }
        i++;
    }
}  

void field_converter(int minefield[SIZE][SIZE], int game_status, int i, int j) {
    if (j == 9) {
        if (minefield[i - 2][j -2] == HIDDEN_MINE) {
            if (game_status == GAME_LOST) {
                printf("()");
            }
            else {
                printf("##");
            }
        }
        else if (minefield[i - 2][j -2] == HIDDEN_SAFE) {
            printf("##");
        }   
        else if (minefield[i - 2][j -2] == 0) {
            if (format_field_num_mines_sqr(i - 2, j - 2, minefield) == 0) {
                printf("  ");
            }
            else {
                printf("0%d", format_field_num_mines_sqr(i - 2, j - 2, minefield));
            }
        }
    }
    else {
        if (minefield[i - 2][j -2] == HIDDEN_MINE) {
            if (game_status == GAME_LOST) {
                printf("() ");
            }
            else {
                printf("## ");
            }
        }
        else if (minefield[i - 2][j -2] == HIDDEN_SAFE) {
            printf("## ");
        }   
        else if (minefield[i - 2][j -2] == 0) {
            if (format_field_num_mines_sqr(i - 2, j - 2, minefield) == 0) {
                printf("   ");
            }
            else {
                printf("0%d ", format_field_num_mines_sqr(i - 2, j - 2, minefield));
            }
        }
    }
}

//Detect square function modified for the formatted minefield
int format_field_num_mines_sqr(int row, int column, int minefield[SIZE][SIZE]) {
    int num_mine_square = 0;
    int row_start = row - 1;
    int column_start = column - 1;
    int lc3 = 0;
    while (lc3 < 3) {
        int lc4 = 0;
        while (lc4 < 3) {
            if (row_start + lc3 >= SIZE || column_start + lc4 >= SIZE) {
            }
            else if (row_start + lc3 < 0 || column_start + lc4 < 0) {
            }
            else if (minefield[row_start + lc3][column_start + lc4] == HIDDEN_MINE) {
                num_mine_square++;
            }
            lc4++;
        }
        lc3++;
    }
    return num_mine_square;
}


// Prints the correct minefield based on what mode the game is set
void print_correct_minefield(int game_status, int mode, int minefield[SIZE][SIZE]) {
    if (mode == GP_MODE) {
        print_formatted_minefield(game_status, minefield);
    }
    else {
        print_debug_minefield(minefield);
    }
}

// Set the entire minefield to HIDDEN_SAFE.
void initialise_field(int minefield[SIZE][SIZE]) {
    int i = 0;
    while (i < SIZE) {
        int j = 0;
        while (j < SIZE) {
            minefield[i][j] = HIDDEN_SAFE;
            j++;
        }
        i++;
    }
}


// Print out the actual values of the minefield.
void print_debug_minefield(int minefield[SIZE][SIZE]) {
    int i = 0;
    while (i < SIZE) {
        int j = 0;
        while (j < SIZE) {
            printf("%d ", minefield[i][j]);
            j++;
        }
        printf("\n");
        i++;
    }
}
