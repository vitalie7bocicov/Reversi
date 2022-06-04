#include "check_msg.h"
#include <time.h>

int rcv_username(int sd)
{
    char input_username[30], username[32];
    int leng;
    leng = read(0, input_username, 30);
    if (leng <= 0)
    {
        return 0;
    }
    input_username[leng] = '\0';
    leng--;
    username[0] = leng / 10 + '0';
    username[1] = leng % 10 + '0';
    username[2] = '\0';
    strcat(username, input_username);
    leng += 2;
    if (write(sd, username, leng) <= 0)
    {
        perror("error at write username\n");
    }
    return 1;
}

int rcv_move_auto()
{
    int i, j, end;
    end = check_endgame();
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
