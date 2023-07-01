#include "util.h"

int check_insert(int sd, char *buff)
{   
    if (strstr(buff, "Please insert your move") || strstr(buff, "Invalid move! Please insert valid move!"))
    {
        return 1;
    }
    return 0;
}

int check_username(int sd, char *buff)
{
    if (strstr(buff, "Type in your username"))
    {
        return 1;
    }
    return 0;
}

int check_player_disconnected(int sd, char *buff)
{
    if (strstr(buff, "continue your game"))
    {
        printf("%s", buff);
        fflush(stdout);

        return 1;
    }
    return 0;
}

int check_score(int sd, char *buff)
{
    if (strlen(buff) == 5 && buff[2] == '-')
    {
        c_score[0] = '\0';
        strcat(c_score, "WHITE ");
        strcat(c_score, buff);
        strcat(c_score, " BLACK");
        return 1;
    }
    return 0;
}

int check_conn(int sd, char *buff)
{
    if (strstr(buff, "Connected succesful!"))
    {
        player = buff[8] - '0';
        return 1;
    }
    return 0;
}

int check_gameover(int sd, char *buff)
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

int check_win(int sd, char *buff)
{
    if (strstr(buff, "You won!"))
    {
        return 1;
    }
    return 0;
}

int check_lost(int sd, char *buff)
{
    if (strstr(buff, "You lost!"))
    {
        return 1;
    }
    return 0;
}

int check_draw(int sd, char *buff)
{
    if (strstr(buff, "It's a draw!"))
    {
        return 1;
    }
    return 0;
}

int check_leaderboard(int sd, char *buff)
{
    if (strstr(buff, "LEADERBOARD"))
    {
        return 1;
    }
    return 0;
}

int check_invalid_username(int sd, char *buff)
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
    printf("msg received: %s\n", buff);
    printf("msg len: %d\n", m_len);

    if (check_conn(sd, buff))
        return 1;
    if (check_score(sd, buff))
        return 2;
    if (check_board(buff, m_len))
        return 3;
    if (check_insert(sd, buff))
        return 4;

    if (check_player_disconnected(sd, buff))
        return 6;

    if (check_gameover(sd, buff))
    {
        printf("%s\n", buff);
        fflush(stdout);
        if (check_win(sd, buff))
            return 7;
        if (check_lost(sd, buff))
            return 8;
        if (check_draw(sd, buff))
            return 9;
        if (check_username(sd, buff))
            return 10;
        if (check_leaderboard(sd, buff))
            return 11;
        if (check_invalid_username(sd, buff))
            return 12;
    }

    return 0;
}
