#include "check.h"
#include <time.h>

int rcv_answer(int desc)
{
    char answer[2];

    if (read(desc, answer, 2) <= 0)
    {
        perror("error:[rcv_answer]\n");
        return -1;
    }
    answer[1] = '\0';

    if (answer[0] == 'd')
        return -1;

    while (answer[0] != 'y' && answer[0] != 'n')
    {
        msg_invalid_answer(desc);
        if (read(desc, answer, 2) <= 0)
        {
            perror("error:[rcv_answer]\n");
        }
        answer[1] = '\0';
    }
    if (answer[0] == 'y')
    {
        return -2;
    }
    else if (answer[0] == 'n')
    {
        return -3;
    }
}

int rcv_move(int **board, int desc1, int desc2)
{
    char pos[4];
    int xy, conn;
    conn = read(desc1, pos, 3);
    if (conn < 0)
    {
        printf("CONNECTION LOST!\n");
        fflush(stdout);
        return -1;
    }
    if (conn == 0)
    {
        printf("CONNECTION LOST!\n");
        fflush(stdout);
        msg_player_disconnected(desc2);
        if (desc1 != desc2)
            return rcv_answer(desc2);
        return -1;
    }

    pos[3] = '\0';
    if (strstr(pos, "d"))
    {
        printf("CONNECTION LOST!\n");
        fflush(stdout);
        msg_player_disconnected(desc2);
        if (desc1 != desc2)
            return rcv_answer(desc2);
        return -1;
    }

    strcpy(pos + 1, pos + 2);
    xy = atoi(pos);
    return xy;
}

int rcv_move_auto(int **board)
{
    int i, j, end;
    end = check_endgame(board);
    if (!end)
    {
        srand(time(NULL));
        do
        {
            i = rand() % 8;
            j = rand() % 8;
        } while (board[i][j] != 3);

        return i * 10 + j;
    }
    return -1;
}
