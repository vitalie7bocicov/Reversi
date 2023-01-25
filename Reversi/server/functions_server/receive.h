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

int get_score(int **board, int player)
{
    int i, j, score = 0;
    for (i = 0; i < 8; i++)
        for (j = 0; j < 8; j++)
        {
            if (board[i][j] == player)
            {
                score++;
            }
        }
    return score;
}

int heuristic(int **board, int player)
{
    int opponent = get_opponent(player);
    int ourScore = get_score(board, player);
    int opponentScore = get_score(board, opponent);
    return (ourScore - opponentScore);
}

int get_possible_moves(int **board, int *possible_moves, int player, int k)
{
    int i = 0, j = 0;
    for (i = 0; i < 8; i++)
        for (j = 0; j < 8; j++)
        {
            if (board[i][j] == 3) // possible move
            {
                possible_moves[k++] = i * 10 + j;
            }
        }
    return k;
}

int **copy_board(int **to_copy)
{
    int i, j;
    int **board = (int **)malloc(sizeof(int *) * 8);
    for (i = 0; i < 8; i++)
        board[i] = (int *)malloc(sizeof(int) * 8);
    for (i = 0; i < 8; i++)
        for (j = 0; j < 8; j++)
            board[i][j] = to_copy[i][j];
    return board;
}

int make_move(int **board, int move, int player)
{
    int i = move / 10;
    int j = move % 10;
    int dumb_score[3];
    dumb_score[1] = 2;
    dumb_score[2] = 2;
    board[i][j] = player;
    remove_possible_moves(board);
    capture_pieces(board, dumb_score, i, j, player);
    mark_possible_moves(board, get_opponent(player));
}

int minimax_value(int **board, int player, int current_player, int depth)
{
    if ((depth == 7) || check_endgame(board))
    {
        return heuristic(board, player);
    }
    int *possible_moves = malloc(sizeof(int) * 60);
    int num_possible_moves = 0;
    int opponent = get_opponent(current_player);
    num_possible_moves = get_possible_moves(board, possible_moves, current_player, num_possible_moves);
    if (num_possible_moves == 0)
    {
        return minimax_value(board, player, opponent, depth + 1);
    }
    else
    {

        int best_move_val = -99999;
        if (player != current_player) // opponent
            best_move_val = 99999;
        // Try out every single move
        for (int i = 0; i < num_possible_moves; i++)
        {
            // Apply the move to a new board
            int **temp_board = copy_board(board);
            make_move(temp_board, possible_moves[i], current_player);
            // Recursive call
            int val = minimax_value(temp_board, player, opponent,
                                    depth + 1);
            // Remember best move
            if (player == current_player)
            {
                // Remember max if it's the originator's turn
                if (val > best_move_val)
                {
                    best_move_val = val;
                }
            }
            else
            {
                // Remember min if it's opponent turn
                if (val < best_move_val)
                {
                    best_move_val = val;
                }
            }
        }
        return best_move_val;
    }
    return -1; // Should never get here
}

int minimax_decision(int **board, int player)
{
    int x = 0, y = 0;
    int *possible_moves = malloc(sizeof(int) * 60);
    int num_possible_moves = 0;
    int opponent = get_opponent(player);
    num_possible_moves = get_possible_moves(board, possible_moves, player, num_possible_moves);
    if (num_possible_moves == 0) // if no moves return -1
    {
        return -1;
    }
    else
    {
        int best_move_val = -99999;
        int bestXY = possible_moves[0];
        for (int i = 0; i < num_possible_moves; i++)
        {
            int **temp_board = copy_board(board);
            make_move(temp_board, possible_moves[i], player);
            int val = minimax_value(temp_board, player, opponent, 1);
            if (val > best_move_val)
            {
                best_move_val = val;
                bestXY = possible_moves[i];
            }
        }
        return bestXY;
    }
}

int rcv_move_auto(int **board, int player)
{
    int i, j, end;
    end = check_endgame(board);
    if (!end)
    {
        // srand(time(NULL));
        // do
        // {
        //     i = rand() % 8;
        //     j = rand() % 8;
        // } while (board[i][j] != 3);
        // return i * 10 + j;
        int best_move = minimax_decision(board, player);
        return best_move;
    }
    return -1;
}
