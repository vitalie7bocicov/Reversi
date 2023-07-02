#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>

void msg_invalid_answer(int desc);
void msg_player_disconnected(int desc);
void remove_possible_moves(int **board);
int get_opponent(int player);
void mark_possible_moves(int **board, int player);
int check_endgame(int **board);
void capture_pieces(int **board, int *score, int x, int y, int player);
int minimax_decision(int **board, int player);

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
    return 1;
}

int rcv_move(int desc1, int desc2)
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

int rcv_move_auto(int **board, int player)
{
    int end;
    end = check_endgame(board);
    if (!end)
    {
        int best_move = minimax_decision(board, player);
        return best_move;
    }
    return -1;
}
