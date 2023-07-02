#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>


int board[8][8], player = 0;
char c_score[20];

int check_msg(int sd);

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
    int i, l = 0, c = 0;
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
    int m_len = 0, pow = 1;
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

void close_conn(int sd, int status)
{
    int flags = fcntl(sd, F_GETFL, 0);
    flags &= ~O_NONBLOCK;
    fcntl(sd, F_SETFL, flags);

    int receive = 0, send = 0;
    int k = 0;
    if (status != 6)
    {
        while (k != 2)
        {
            receive = check_msg(sd);
            if (receive)
                k++;
        }
    }

    send = write(sd, "dis", 3);
    if (send <= 0)
    {
        perror("error: [close_conn]-write");
    }
    printf("CLOSING CONNECTION\n");
    fflush(stdout);
    close(sd);
}