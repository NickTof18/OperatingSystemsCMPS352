// Nicholas Tofani
// PROGRAMMING ASSIGNMENT 5 - Gomoku-N Game
// 10/23/2017
// Compile -> run "cc -lpthread asgn5-tofanin2.c"
// Execute: -> "./a.out"

/*
  A Gomoku game using threads in C. Gomoku is a two-player
   board game. The players alternate in placing a stone of
   their symbol (X and 0) in a space of the board which has
   8 columns and 8 rows, and whoever having five (5) consecutive
   stones horizontally, vertically, or diagonally wins the
   game. This game, Gomoku-N is an extended version of Gomoku
   by changing the number of winning consecutive stones to
   N and the size of the board to 2*N by 2*N.
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

struct gameOverview {
    //2D Array Representing the Gomoku GameBoard
    char **gameBoard;
    //Player 1 or Player 2
    int playerTurn;
    //Player 1 Symbol == X; Player 2 Symbol == O
    char playerSymbol;
    //N dictated by player before game started
    int N;
    //Number of Possible moves left on the board
    int movesLeft;
    //X cordinate of the current move
    int currentMoveX;
    //Y cordinate of the current move
    int currentMoveY;
    //0==Game Still being played;  1==Game Over
    int gameOver;//Treat as bool
};

void initGameOverview(struct gameOverview *newGame, int newN) {
    //Initializing 2D array for gameboard
    newGame->gameBoard = (char **) malloc(sizeof(char *) * (newN * 2));
    for(int i = 0; i < (newN * 2); i++) {
        newGame->gameBoard[i] = (char*) malloc(sizeof(char) * (newN * 2));
    }
    //Setting the board to all empty spots
    for (int j = 0; j < (newN * 2); j++) {
        for (int k = 0; k < (newN * 2); k++) {
            newGame->gameBoard[j][k] = '*';
        }
    }
    //Setting newGame's N here to keep above cleaner
    newGame->N = newN;
    //Calculating the max number of moves on board
    newGame->movesLeft = (2*newN) * (2*newN);
    //Player 1 will go first
    newGame->playerTurn = 1;
    newGame->playerSymbol = 'X';
    //Game is starting
    newGame->gameOver = 0;
}

void dumpBoard(struct gameOverview *currentGame);
void changeTurns(struct gameOverview *currentGame);
int checkIfValidMove(struct gameOverview *currentGame);
void *checkDownWin(void *ptr);
void *checkAcrossWin(void *ptr);
void *checkDiagonalLeftToRight(void *ptr);
void *checkDiagonalRightToLeft(void *ptr);
void gameIsOver(struct gameOverview *currentGame);

int main() {
    int N = 0;
    printf("Welcome to Gomoku-N Game \n");
    //Loop till a valid number is recieved
    while(N < 2) {
        printf("What should the N be for the game? N must be > 1. \n");
        scanf("%d", &N);
    }
    struct gameOverview currentGame;
    struct gameOverview *currentGamePtr = &currentGame;
    initGameOverview(currentGamePtr, N);
    //Init threads
    pthread_t threadDown, threadAcross, threadDiagonalLeftToRight,
               threadDiagonalRightToLeft;
    int retD, retA, retDLTR, retDRTL;

    do {
        //Dump board for players to see
        dumpBoard(currentGamePtr);
        printf("Player %d make your move. \n", currentGame.playerTurn);
        scanf("%d %d", &(currentGamePtr->currentMoveY),
                &(currentGamePtr->currentMoveX));
        //Makes sure its a valid move.. If not dont change turns, reprompt player to
        //  Enter valid move
        if(checkIfValidMove(currentGamePtr) == 1) {
            //check for wins with Threads
            retD = pthread_create(&threadDown, NULL, checkDownWin,
                                  (void*) currentGamePtr);
            retA = pthread_create(&threadAcross, NULL, checkAcrossWin,
                                  (void*) currentGamePtr);
            retDLTR = pthread_create(&threadDiagonalLeftToRight, NULL,
                                      checkDiagonalLeftToRight,(void*) currentGamePtr);
            retDRTL = pthread_create(&threadDiagonalRightToLeft, NULL,
                                      checkDiagonalRightToLeft,(void*) currentGamePtr);
            //Waits for threads to be done
            pthread_join(threadDown, NULL);
            pthread_join(threadAcross, NULL);
            pthread_join(threadDiagonalLeftToRight, NULL);
            pthread_join(threadDiagonalRightToLeft, NULL);
            
            changeTurns(currentGamePtr);
            currentGamePtr->movesLeft--;
        }
    } while (currentGame.gameOver != 1 && currentGame.movesLeft > 0);

    //Game is Finished, May be tie or someone won
    gameIsOver(currentGamePtr);
    return 0;
}

/*
   Checks if the move entered by the player is valid
    To be valid the move must:
      *Have cordinates on the board
      *Must not have been picked already during a previous turn
    Returns 1 if valid turn
    Returns 0 if not a valid turn
*/
int checkIfValidMove(struct gameOverview *currentGame) {
    //Makes sure the move cordinates are within the board
    // && move hasn't already been done this game
    if(currentGame->currentMoveX < (currentGame->N * 2) &&
        currentGame->currentMoveY < (currentGame->N * 2) &&
        currentGame->gameBoard[currentGame->currentMoveX][currentGame->currentMoveY]
         == '*')
    {
        //Place current player's symbol on the board
        currentGame->gameBoard[currentGame->currentMoveX][currentGame->currentMoveY] =
          currentGame->playerSymbol;
        return 1;
    }
    printf("Invalid Move! \n");
    return 0;
}

/*
  Checks if current player has won by checking the column of the current
   Move for N consecutive Player symbols
*/
void *checkAcrossWin(void *ptr) {
    //Used to compare to N for win
    int countInARow = 0;
    //Grabbing Value of gameboard passed from pThreadCreate
    struct gameOverview *currentGame = (struct gameOverview*)ptr;
    //Checking down the column for win. Exit early if win is found
    for (int i = 0; i < currentGame->N + countInARow && currentGame->gameOver == 0;
          i++)
    {
        if(currentGame->gameBoard[i][currentGame->currentMoveY] ==
             currentGame->playerSymbol)
        {
            countInARow++;
            if(countInARow == currentGame->N) {
                //Current Player has won!
                currentGame->gameOver = 1;
            }
        }
        else {
            //Reset because they MUST be consecutive
            countInARow = 0;
        }
    }
    return NULL;
}

/*
  Checks if current player has won by checking the Row of the current
   Move for N consecutive Player symbols
*/
void *checkDownWin(void *ptr) {
    //Used to compare to N for win
    int countInARow = 0;
    //Grabbing Value of gameboard passed from pThreadCreate
    struct gameOverview *currentGame = (struct gameOverview*)ptr;
    //Checking Across the Row for win. Exit early if win is found
    for (int i = 0; i <= currentGame->N + countInARow && currentGame->gameOver == 0;
          i++) {
        if((currentGame->gameBoard[currentGame->currentMoveX][i]) ==
             currentGame->playerSymbol)
        {
            countInARow++;
            if(countInARow == currentGame->N) {
                //Current Player has won!
                currentGame->gameOver = 1;
            }
        }
        else {
            //Reset because they MUST be consecutive
            countInARow = 0;
        }
    }
    return NULL;
}

/*
  Checks if current player has won by checking a diagonal line in
   both directions from the Top-Left corner to the bottom-right corner
   from current Move for N consecutive Player symbols
*/
void *checkDiagonalLeftToRight(void *ptr) {
    //Used to compare to N for win
    int countInARow = 0;
    //Grabbing Value of gameboard passed from pThreadCreate
    struct gameOverview *currentGame = (struct gameOverview*)ptr;
    int startingX = currentGame->currentMoveX;
    int startingY = currentGame->currentMoveY;
    //Moving checker to the top-Left most part of current Move
    //  So we can check the entire diagonal area
    while(startingX > 0 && startingY > 0) {
        startingY--;
        startingX--;
    }
    //Makes sure its still possible to win, Loop ends when no
    // Longer possible to win this turn or someone wins
    while(startingX <= (currentGame->N + countInARow)  &&
           startingY <= (currentGame->N + countInARow)  &&
           currentGame->gameOver == 0) {
        if((currentGame->gameBoard[startingX][startingY]) ==
             currentGame->playerSymbol) {
            countInARow++;
            if(countInARow == currentGame->N) {
                //Player won!
                currentGame->gameOver = 1;
            }
        }
        else {
            //Reset because they MUST be consecutive
            countInARow = 0;
        }
        startingY++;
        startingX++;
    }
    return NULL;
}

/*
  Checks if current player has won by checking a diagonal line in
   both directions from the Top-Right corner to the bottom-left corner
   from current Move for N consecutive Player symbols
 */
void *checkDiagonalRightToLeft(void *ptr) {
    //Used to compare to N for win
    int countInARow = 0;
    //Grabbing Value of gameboard passed from pThreadCreate
    struct gameOverview *currentGame = (struct gameOverview*)ptr;
    int startingX = currentGame->currentMoveX;
    int startingY = currentGame->currentMoveY;
    //Moving checker to the top-Right most part of current Move
    //  So we can check the entire diagonal area
    while(startingX < ((currentGame->N *2) - 1) && startingY > 0) {
        startingX++;
        startingY--;
    }
    //Makes sure its still possible to win, Loop ends when no
    // Longer possible to win this turn or someone wins
    while(startingX >= (currentGame->N -1) - countInARow &&
           startingY <=  currentGame->N + countInARow &&
           currentGame->gameOver == 0) {
        if(currentGame->gameBoard[startingX][startingY] ==
            currentGame->playerSymbol) {
            countInARow++;
            if(countInARow == currentGame->N) {
                //Player Wins!
                currentGame->gameOver = 1;
            }
        }
        else {
            //Reset because they MUST be consecutive
            countInARow = 0;
        }
        startingX--;
        startingY++;
    }
    return NULL;
}

/*
  Switches turns between Player 1 & Player 2
   Also swithes PlayerSymbol to proper Symbol:
     *Player 1 == X
     *Player 2 == O
*/
void changeTurns(struct gameOverview *currentGame) {
    if(currentGame->playerTurn == 1) {
        //Player 2's turn
        currentGame->playerTurn = 2;
        currentGame->playerSymbol = 'O';
    }
    else {
        //Player 1's turn
        currentGame->playerTurn = 1;
        currentGame->playerSymbol = 'X';
    }
}

/*
  The game is finished whether through Tie or someone won. The
   board will be printed, then depending on if tie or not Output
   alerts the players to who won or that the game is a tie.
*/
void gameIsOver(struct gameOverview *currentGame) {
    //Set back to previous player who actually won
    changeTurns(currentGame);
    dumpBoard(currentGame);
    //Checking for a tie
    if(currentGame->movesLeft == 0) {
        //Is a tie
        printf("The board is full... The Game is a tie! \n");
    }
    else {
        //Not a tie
        printf("Player %d wins! \n", currentGame->playerTurn);
    }
    //Free GameBoard Memory because game is over
    for (int i = 0; i < (currentGame->N * 2); i++) {
        free(currentGame->gameBoard[i]);
    }
    free(currentGame->gameBoard);
}

/*
  Simply prints the current Board's state
     * Stars('*') represent unused spaces
     * Player 1 == X
     * Player 2 == O
*/
void dumpBoard(struct gameOverview *currentGame) {
    for (int i = 0; i < currentGame->N * 2; i++) {
        for(int j = 0; j < currentGame->N * 2; j++) {
            printf("%c", currentGame->gameBoard[i][j]);
        }
        printf("\n");
    }
}

