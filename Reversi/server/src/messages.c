#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void insert_db(int desc, int score, char *winner);
void select_db(char *ldboard);

void msg_player_1(int desc, int status)
{
    switch (status)
    {
    case 0:
        if (write(desc, "67 [player 1]Connected succesful! Waiting for second player to join!\n", 70) <= 0)
        {
            perror("error:[msg_player_1.0]\n");
        }
        break;
    case 1:
        if (write(desc, "46 [player 1]Player 2 connected! Starting game!\n", 49) <= 0)
        {
            perror("error:[msg_player_1.1]\n");
        }
        break;
    case 2:
        if (write(desc, "46 [player 1]Please insert your move (ex: 1 1)!\n", 49) <= 0)
        {
            perror("error:[msg_player_1.2]\n");
        }
        break;
    case 3:
        if (write(desc, "41 [player 1]Waiting for player 2 to move!\n", 44) <= 0)
        {
            perror("error:[msg_player_1.3]\n");
        }
        break;
    case 4:
        if (write(desc, "51 [player 1]Invalid move! Please insert valid move!\n", 54) <= 0)
        {
            perror("error:[msg_player_1.4]\n");
        }
        break;
    default:
        break;
    }
}

void msg_player_2(int desc, int status)
{
    switch (status)
    {
    case 0:
        if (write(desc, "51 [player 2]Connected succesful! Starting the game!\n", 54) <= 0)
        {
            perror("error:[msg_player_2.0]\n");
        }
        break;
    case 1:
        if (write(desc, "26 [player 2]Starting game!\n", 29) <= 0)
        {
            perror("error:[msg_player_2.1]\n");
        }
        break;
    case 2:
        if (write(desc, "47 [player 2]Please insert your move (ex: 1 1)! \n", 50) <= 0)
        {
            perror("error:[msg_player_2.2]\n");
        }
        break;
    case 3:
        if (write(desc, "41 [player 2]Waiting for player 1 to move!\n", 44) <= 0)
        {
            perror("error:[msg_player_2.3]\n");
        }
        break;
    case 4:
        if (write(desc, "51 [player 2]Invalid move! Please insert valid move!\n", 54) <= 0)
        {
            perror("error:[msg_player_2.4]\n");
        }
        break;
    default:
        break;
    }
}

void msg_board(int **board, int desc)
{
    char *char_board = (char *)malloc(sizeof(char) * 67);
    if (char_board == NULL)
    {
        perror("error: Failed to allocate memory\n");
        return;
    }
    int k = 3;
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            char_board[k++] = board[i][j] + '0';
    char_board[k] = '\0';
    char_board[0] = '6';
    char_board[1] = '4'; // char_board length
    char_board[2] = ' ';
    if (write(desc, char_board, 67) <= 0)
    {
        perror("error:[msg_board]\n");
    }
    free(char_board);
}

void msg_score(int desc, int *score)
{
    char char_score[10];
    char_score[0] = '5';
    char_score[1] = ' ';
    char_score[2] = score[1] / 10 + '0';
    char_score[3] = score[1] % 10 + '0';
    char_score[4] = '-';

    char_score[5] = score[2] / 10 + '0';
    char_score[6] = score[2] % 10 + '0';
    char_score[7] = '\0';

    if (write(desc, char_score, 8) <= 0)
    {
        perror("error:[msg_score]\n");
    }
}

void msg_winner(int desc)
{
    if (write(desc, "10 You won!\n", 13) <= 0)
    {
        perror("error:[msg_winner]\n");
    }
}

void msg_loser(int desc)
{
    if (write(desc, "11 You lost!\n", 14) <= 0)
    {
        perror("error:[msg_loser]\n");
    }
}

void msg_tie(int desc)
{
    if (write(desc, "14 It's a draw!\n", 17) <= 0)
    {
        perror("error:[msg_tie]\n");
    }
}

void msg_gameOver(int desc)
{
    if (write(desc, "12 Game Over!\n", 15) < 0)
    {
        perror("error:[msg_gameOver]\n");
    }
}

char *msg_rcv_username(int desc)
{
    if (write(desc, "44 Type in your username for the leaderboard:", 46) <= 0)
    {
        perror("error:[msg_msg_rcv_username]-write\n");
    }
    char *winner;
    winner = malloc(sizeof(char) * 30);
    int k_len;
    char msg_len[3];
    read(desc, msg_len, 2);
    msg_len[2] = '\0';

    k_len = atoi(msg_len);
    if (read(desc, winner, k_len) <= 0)
    {
        perror("error:[msg_msg_rcv_username]-read\n");
    }
    winner[k_len] = '\0';

    return winner;
}

void msg_invalid_username(int desc)
{
    if (write(desc, "20 \nUsername is taken!", 23) <= 0)
    {
        perror("[username request]Eroare la write() catre client.\n");
    }
}

void msg_invalid_answer(int desc)
{
    if (write(desc, "97 Invalid answer! Opponent disconnected! Do you want to continue your game with the computer?[y/n]", 100) <= 0)
    {
        perror("error:[msg_invalid_answer]\n");
    }
}

void msg_player_disconnected(int desc)
{
    if (write(desc, "81 Opponent disconnected! Do you want to continue your game with the computer?[y/n]", 84) <= 0)
    {
        perror("error:[msg_player_disconnected]\n");
    }
}

void msg_leaderboard(int desc, char *ldboard)
{
    char msg_ldboard[1002];
    int k = 0, copy = strlen(ldboard), len_ldboard = strlen(ldboard);
    while (copy)
    {
        k++, copy /= 10;
    }
    msg_ldboard[k] = '\0';
    for (int i = k - 1; i >= 0; i--)
    {
        msg_ldboard[i] = len_ldboard % 10 + '0';
        len_ldboard /= 10;
    }
    strcat(msg_ldboard, " ");
    strcat(msg_ldboard, ldboard);

    if (write(desc, msg_ldboard, strlen(msg_ldboard)) <= 0)
    {
        perror("error:[msg_leaderboard]\n");
    }
}

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
        insert_db(desc1, score[1], winner);
        select_db(ldboard);
        msg_leaderboard(desc1, ldboard);
    }
    else if (win == 2)
    {
        winner = msg_rcv_username(desc2);
        insert_db(desc2, score[2], winner);
        select_db(ldboard);
        msg_leaderboard(desc2, ldboard);
    }
}