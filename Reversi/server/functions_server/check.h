#include "msg_endgame.h"

int check_endgame(int **board)
{
    int i, j;
    for (i = 0; i < 8; i++)
        for (j = 0; j < 8; j++)
            if (board[i][j] == 3)
                return 0;
    return 1;
}
