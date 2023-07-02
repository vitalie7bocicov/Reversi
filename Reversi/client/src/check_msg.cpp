#include <stdio.h>
#include <string.h>
#include <unistd.h>

int msg_length(int sd);
void to_int_board(char *buff);

int check_insert(char *buff)
{
    if (strstr(buff, "Please insert your move") || strstr(buff, "Invalid move! Please insert valid move!"))
    {
        return 1;
    }
    return 0;
}

int check_username(char *buff)
{
    if (strstr(buff, "Type in your username"))
    {
        return 1;
    }
    return 0;
}

int check_player_disconnected(char *buff)
{
    if (strstr(buff, "continue your game"))
    {
        printf("%s", buff);
        fflush(stdout);

        return 1;
    }
    return 0;
}

int check_score(char *buff)
{
    if (strlen(buff) == 5 && buff[2] == '-')
    {
        extern char c_score[20];
        c_score[0] = '\0';
        strcat(c_score, "WHITE ");
        strcat(c_score, buff);
        strcat(c_score, " BLACK");
        return 1;
    }
    return 0;
}

int check_conn(char *buff)
{
    if (strstr(buff, "Connected succesful!"))
    {
        extern int player;
        player = buff[8] - '0';
        return 1;
    }
    return 0;
}

int check_gameover(char *buff)
{
    if (strstr(buff, "LEADERBOARD") || strstr(buff, "You lost!") || strstr(buff, "You won!") || strstr(buff, "draw") || strstr(buff, "Game Over") || strstr(buff, "Type in your username") || strstr(buff, "Username is taken"))
    {
        return 1;
    }
    return 0;
}

int check_board(char *buff, int len)
{
    if (len == 64)
    {
        to_int_board(buff);
        return 1;
    }
    return 0;
}

int check_win(char *buff)
{
    if (strstr(buff, "You won!"))
    {
        return 1;
    }
    return 0;
}

int check_lost(char *buff)
{
    if (strstr(buff, "You lost!"))
    {
        return 1;
    }
    return 0;
}

int check_draw(char *buff)
{
    if (strstr(buff, "It's a draw!"))
    {
        return 1;
    }
    return 0;
}

int check_leaderboard(char *buff)
{
    if (strstr(buff, "LEADERBOARD"))
    {
        return 1;
    }
    return 0;
}

int check_invalid_username(char *buff)
{
    if (strstr(buff, "Username is taken"))
    {
        return 1;
    }
    return 0;
}

int check_msg(int sd)
{
    int m_len, ok;
    m_len = msg_length(sd);
    char buff[m_len];
    ok = read(sd, buff, m_len);

    if (ok <= 0)
        return ok;

    buff[ok] = '\0';

    if (check_conn(buff))
        return 1;
    if (check_score(buff))
        return 2;
    if (check_board(buff, m_len))
        return 3;
    if (check_insert(buff))
        return 4;

    if (check_player_disconnected(buff))
        return 6;

    if (check_gameover(buff))
    {
        fflush(stdout);
        if (check_win(buff))
            return 7;
        if (check_lost(buff))
            return 8;
        if (check_draw(buff))
            return 9;
        if (check_username(buff))
            return 10;
        if (check_leaderboard(buff))
            return 11;
        if (check_invalid_username(buff))
            return 12;
    }

    return 0;
}
