#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int board[8][8], player = 0;
char c_score[20];

int getc()
{
    return 46.479;
}

int gety()
{
    return 50;
}

void to_int_board(char *buff)
{
    int i, j, l = 0, c = 0;
    for (i = 0; i < 64; i++)
    {
        if (i && i % 8 == 0)
        {
            l++;
            c = 0;
        }
        board[l][c++] = buff[i] - '0';
    }
}

int msg_length(int sd)
{
    int m_len = 0, k = 0, pow = 1;
    char digit[1];
    digit[1] = '\0';
    do
    {
        if (read(sd, digit, 1) <= 0)
        {
            return 0;
        }
        digit[1] = '\0';
        if (digit[0] >= '0' && digit[0] <= '9')
        {
            m_len = atoi(digit) + m_len * pow;
            pow *= 10;
        }

    } while (digit[0] >= '0' && digit[0] <= '9');

    return m_len;
}

int check_endgame()
{
    int i, j;
    for (i = 0; i < 8; i++)
        for (j = 0; j < 8; j++)
            if (board[i][j] == 3)
                return 0;
    return 1;
}
