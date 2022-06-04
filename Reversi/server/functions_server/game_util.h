#include "receive.h"

int get_opponent(int player)
{
    if (player == 1)
        return 2;
    return 1;
}

void update_board(int **board, int xy, int player)
{
    int i = xy / 10;
    int j = xy % 10;

    if (player == 1)
    {
        board[i][j] = 1;
    }
    else if (player == 2)
    {
        board[i][j] = 2;
    }
}

int valid_move(int **board, int x, int y)
{
    if (x < 0 || y < 0 || x > 7 || y > 7)
        return 0;
    if (board[x][y] != 0)
        return 0;
    return 1;
}

int out_of_borders(int x, int y)
{
    if (x < 0 || y < 0 || x > 7 || y > 7)
        return 1;
    return 0;
}

int is_playable(int **board, int x, int y)
{
    return board[x][y] == 3;
}

int distance(int x1, int y1, int x2, int y2)
{
    int di = abs(x1 - x2), dj = abs(y1 - y2);
    if (di > 0)
        return di;
    return dj;
}

void mark_possible_moves(int **board, int player)
{
    int i, j, opponent;
    if (player == 1)
        opponent = 2;
    else
        opponent = 1;
    for (i = 0; i < 8; i++)
        for (j = 0; j < 8; j++)
        {
            if (board[i][j] == player)
            {
                int x = i, y = j - 1; // LEFT
                while (!out_of_borders(x, y) && board[x][y] == opponent)
                {
                    y--;
                }

                if (valid_move(board, x, y) && distance(i, j, x, y) > 1)
                    board[x][y] = 3; // playable

                x = i - 1, y = j - 1; // LEFT UP
                while (!out_of_borders(x, y) && board[x][y] == opponent)
                {
                    x--, y--;
                }

                if (valid_move(board, x, y) && distance(i, j, x, y) > 1)
                    board[x][y] = 3; // playable

                x = i - 1, y = j; // UP
                while (!out_of_borders(x, y) && board[x][y] == opponent)
                {
                    x--;
                }

                if (valid_move(board, x, y) && distance(i, j, x, y) > 1)
                    board[x][y] = 3; // playable

                x = i - 1, y = j + 1; // UP RIGHT
                while (!out_of_borders(x, y) && board[x][y] == opponent)
                {
                    x--;
                    y++;
                }

                if (valid_move(board, x, y) && distance(i, j, x, y) > 1)
                    board[x][y] = 3; // playable

                x = i, y = j + 1; // RIGHT
                while (!out_of_borders(x, y) && board[x][y] == opponent)
                {
                    y++;
                }

                if (valid_move(board, x, y) && distance(i, j, x, y) > 1)
                    board[x][y] = 3; // playable

                x = i + 1, y = j + 1; // RIGHT DOWN
                while (!out_of_borders(x, y) && board[x][y] == opponent)
                {
                    x++, y++;
                }

                if (valid_move(board, x, y) && distance(i, j, x, y) > 1)
                    board[x][y] = 3; // playable

                x = i + 1, y = j; // DOWN
                while (!out_of_borders(x, y) && board[x][y] == opponent)
                {
                    x++;
                }

                if (valid_move(board, x, y) && distance(i, j, x, y) > 1)
                    board[x][y] = 3; // playable

                x = i + 1, y = j - 1; // DOWN LEFT
                while (!out_of_borders(x, y) && board[x][y] == opponent)
                {
                    x++;
                    y--;
                }

                if (valid_move(board, x, y) && distance(i, j, x, y) > 1)
                    board[x][y] = 3; // playable
            }
        }
}

void remove_possible_moves(int **board)
{
    int i, j;
    for (i = 0; i < 8; i++)
        for (j = 0; j < 8; j++)
            if (board[i][j] == 3)
                board[i][j] = 0;
}

void capture_pieces(int **board, int *score, int x, int y, int player)
{
    score[player]++;
    int i = x, j = y - 1, opponent; // LEFT
    opponent = get_opponent(player);

    while (!out_of_borders(i, j) && board[i][j] == opponent)
    {
        j--;
    }
    if (!out_of_borders(i, j))
    {
        if (board[i][j] == player && distance(x, y, i, j) > 1)
        {
            j++;
            while (board[i][j] != player)
            {
                board[i][j++] = player;
                score[player]++;
                score[opponent]--;
            }
        }
    }

    i = x - 1, j = y - 1; // LEFT UP
    while (!out_of_borders(i, j) && board[i][j] == opponent)
    {
        i--, j--;
    }
    if (!out_of_borders(i, j))
    {
        if (board[i][j] == player && distance(x, y, i, j) > 1)
        {
            i++, j++;
            while (board[i][j] != player)
            {
                board[i++][j++] = player;
                score[player]++;
                score[opponent]--;
            }
        }
    }

    i = x - 1, j = y; // UP
    while (!out_of_borders(i, j) && board[i][j] == opponent)
    {
        i--;
    }
    if (!out_of_borders(i, j))
    {
        if (board[i][j] == player && distance(x, y, i, j) > 1)
        {
            i++;
            while (board[i][j] != player)
            {
                board[i++][j] = player;
                score[player]++;
                score[opponent]--;
            }
        }
    }

    i = x - 1, j = y + 1; // UP RIGHT
    while (!out_of_borders(i, j) && board[i][j] == opponent)
    {
        i--;
        j++;
    }
    if (!out_of_borders(i, j))
    {
        if (board[i][j] == player && distance(x, y, i, j) > 1)
        {
            i++;
            j--;
            while (board[i][j] != player)
            {
                board[i++][j--] = player;
                score[player]++;
                score[opponent]--;
            }
        }
    }

    i = x, j = y + 1; // RIGHT
    while (!out_of_borders(i, j) && board[i][j] == opponent)
    {
        j++;
    }
    if (!out_of_borders(i, j))
    {
        if (board[i][j] == player && distance(x, y, i, j) > 1)
        {
            j--;
            while (board[i][j] != player)
            {
                board[i][j--] = player;
                score[player]++;
                score[opponent]--;
            }
        }
    }

    i = x + 1, j = y + 1; // RIGHT DOWN
    while (!out_of_borders(i, j) && board[i][j] == opponent)
    {
        i++, j++;
    }
    if (!out_of_borders(i, j))
    {
        if (board[i][j] == player && distance(x, y, i, j) > 1)
        {
            i--, j--;
            while (board[i][j] != player)
            {
                board[i--][j--] = player;
                score[player]++;
                score[opponent]--;
            }
        }
    }

    i = x + 1, j = y; // DOWN
    while (!out_of_borders(i, j) && board[i][j] == opponent)
    {
        i++;
    }
    if (!out_of_borders(i, j))
    {
        if (board[i][j] == player && distance(x, y, i, j) > 1)
        {
            i--;
            while (board[i][j] != player)
            {
                board[i--][j] = player;
                score[player]++;
                score[opponent]--;
            }
        }
    }

    i = x + 1, j = y - 1; // DOWN LEFT
    while (!out_of_borders(i, j) && board[i][j] == opponent)
    {
        i++;
        j--;
    }
    if (!out_of_borders(i, j))
    {
        if (board[i][j] == player && distance(x, y, i, j) > 1)
        {
            i--;
            j++;
            while (board[i][j] != player)
            {
                board[i--][j++] = player;
                score[player]++;
                score[opponent]--;
            }
        }
    }
}
