#include "db.h"

void msg_endgame(char *ldboard, int desc1, int desc2, int *score)
{
    int win = 0;
    char *winner;
    if (score[1] > score[2])
    {
        msg_winner(desc1);
        msg_loser(desc2);
        win = 1;
    }

    else if (score[1] < score[2])
    {
        msg_winner(desc2);
        msg_loser(desc1);
        win = 2;
    }
    else
    {
        msg_tie(desc1);
        msg_tie(desc2);
    }
    if (win == 1)
    {
        winner = msg_rcv_username(desc1);
        insert_db(win, desc1, score[1], winner);
        select_db(ldboard);
        msg_leaderboard(desc1, ldboard);
    }
    else if (win == 2)
    {
        winner = msg_rcv_username(desc2);
        insert_db(win, desc2, score[2], winner);
        select_db(ldboard);
        msg_leaderboard(desc2, ldboard);
    }
}