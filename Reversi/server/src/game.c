#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>

void mark_possible_moves(int **board, int player);
int get_opponent(int player);
int move_pc(int **board, int *score, int player);
void msg_player_1(int desc, int status);
void msg_score(int desc, int *score);
void msg_invalid_username(int desc);
void msg_winner(int desc);
void msg_tie(int desc);
void msg_loser(int desc);
void msg_board(int **board, int desc);
void msg_leaderboard(int desc, char *ldboard);
char *msg_rcv_username(int desc);
void msg_endgame(char *ldboard, int desc1, int desc2, int *score);
int move_player(int **board, int *score, int desc, int player);
void insert_db(int desc, int score, char *winner);
void select_db(char *ldboard);
int move_player_1(int **board, int *score, int desc1, int desc2);
int move_player_2(int **board, int *score, int desc1, int desc2);
void msg_gameOver(int desc);

int **init_board()
{
    int i, j;
    int **board = (int **)malloc(sizeof(int *) * 8);
    for (i = 0; i < 8; i++)
        board[i] = (int *)malloc(sizeof(int) * 8);

    for (i = 0; i < 8; i++)
        for (j = 0; j < 8; j++)
            board[i][j] = 0;
    board[3][3] = 1;
    board[4][4] = 1;
    board[3][4] = 2;
    board[4][3] = 2;
    mark_possible_moves(board, 1);
    return board;
}

void free_board(int **board)
{
    for (int i = 0; i < 8; i++)
        free(board[i]);
    free(board);
}

char *init_ldboard()
{

    char *ldboard;
    ldboard = malloc(sizeof(char) * 1000);
    ldboard[0] = '\0';
    strcat(ldboard, "--------------\n");
    strcat(ldboard, "|LEADERBOARD:|\n");
    strcat(ldboard, "--------------\n");
    strcat(ldboard, "-------------------\n");
    strcat(ldboard, "|SCORE|USERNAME   |\n");
    strcat(ldboard, "-------------------\n");

    return ldboard;
}

void pc_vs_player(int **board, char *ldboard, int *score, int desc, int player)
{
    int opp = get_opponent(player);

    int gameOver = 0;
    while (!gameOver && gameOver != -1)
    {
        gameOver = move_pc(board, score, opp);
        if (gameOver)
            break;
        gameOver = move_player(board, score, desc, player);
    }
    if (gameOver == 1)
    {
        msg_score(desc, score);
        msg_board(board, desc);
        if (score[opp] > score[player])
        {
            msg_loser(desc);
        }
        else if (score[opp] == score[player])
        {
            msg_tie(desc);
        }
        else if (score[player] > score[opp])
        {
            char *winner;
            msg_winner(desc);
            winner = msg_rcv_username(desc);
            insert_db(desc, score[player], winner);
            select_db(ldboard);
            msg_leaderboard(desc, ldboard);
        }
    }
}

void endgame(int **board, char *ldboard, int *score, int desc1, int desc2)
{
    msg_score(desc1, score);
    msg_board(board, desc1);
    msg_score(desc2, score);
    msg_board(board, desc2);
    msg_endgame(ldboard, desc1, desc2, score);
}

void play(int **board, char *ldboard, int *score, int desc1, int desc2)
{
    int status1, status2;
    int gameOver = 0;
    while (!gameOver)
    {

        status1 = move_player_1(board, score, desc1, desc2);
        if (status1 == -3 || status1 == -2 || status1 == -1 || status1 == 1)
        {
            gameOver = 1;
            break;
        }
        status2 = move_player_2(board, score, desc1, desc2);
        if (status2 == -3 || status2 == -2 || status2 == -1 || status2 == 1)
        {
            gameOver = 1;
            break;
        }
    }
    if (status1 != -1 && status2 != -1)
    {
        if (status1 != -3 && status2 != -3 && status1 != -2 && status2 != -2) // both players connected
            endgame(board, ldboard, score, desc1, desc2);
        else
        {
            if (status1 == -3) // player 2 doesn't want to continue
                msg_gameOver(desc2);
            else if (status2 == -3) // player 1 doesn't want to continue
                msg_gameOver(desc1);
            else if (status1 == -2) // player 2 wants to continue
            {
                pc_vs_player(board, ldboard, score, desc2, 2);
            }
            else if (status2 == -2) // player 1 wants to continue
                pc_vs_player(board, ldboard, score, desc1, 1);
        }
    }
    free_board(board);
    free(ldboard);
    close(desc1);
    close(desc2);
    printf("MATCH OVER!\n");
    fflush(stdout);
}

void start_game(int desc1, int desc2)
{
    msg_player_1(desc1, 1);
    int **board = init_board();
    int score[3];
    score[1] = 2;
    score[2] = 2;
    char *ldboard = init_ldboard();
    play(board, ldboard, score, desc1, desc2);
}
