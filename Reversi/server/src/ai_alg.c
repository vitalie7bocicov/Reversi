#include <malloc.h>

int get_possible_moves(int **board, int *possible_moves, int k);
int get_opponent(int player);
int check_endgame(int **board);
void make_move(int **board, int move, int player);
void free_board(int **board);

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

int minimax_value(int **board, int player, int current_player, int depth)
{
    if ((depth == 5) || check_endgame(board))
    {
        return heuristic(board, player);
    }
    int *possible_moves = malloc(sizeof(int) * 60);
    int num_possible_moves = 0;
    int opponent = get_opponent(current_player);
    num_possible_moves = get_possible_moves(board, possible_moves, num_possible_moves);
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
            free_board(temp_board);
        }
        free(possible_moves);

        return best_move_val;
    }
    return -1; // Should never get here
}

int minimax_decision(int **board, int player)
{
    int *possible_moves = malloc(sizeof(int) * 60);
    int num_possible_moves = 0;
    int opponent = get_opponent(player);
    num_possible_moves = get_possible_moves(board, possible_moves, num_possible_moves);
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
            free_board(temp_board);
        }
        return bestXY;
    }
}
