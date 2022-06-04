#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>

#define PORT 2000

typedef struct thData
{
    int desc1;
    int desc2;
} th_desc;

extern int errno;
int N = 10;

static void *treat(void *);
void msg_player_1(int desc, int status);
void msg_player_2(int desc, int status);
void msg_board(int **board, int desc);
void msg_score(int desc, int *score);
void msg_winner(int desc);
void msg_loser(int desc);
void msg_tie(int desc);
void msg_invalid_username(int desc);
void msg_endgame(char *ldboard, int desc1, int desc2, int *score);
char *msg_rcv_username(int desc);
void msg_leaderboard(int desc, char *ldboard);
void msg_player_disconnected(int desc);
void msg_invalid_answer(int desc);

void start_game(int desc1, int desc2);
int **init_board();
void print_board(int **board);

int rcv_move_auto(int **board);
int rcv_move(int **board, int desc1, int desc2);
int rcv_answer(int desc);

void play(int **board, char *ldboard, int *score, int desc1, int desc2);
void update_board(int **board, int xy, int player);
int check_endgame(int **board);
int valid_move(int **board, int x, int y);
int out_of_borders(int x, int y);
int is_playable(int **board, int x, int y);
void mark_possible_moves(int **board, int player);
int distance(int x1, int y1, int x2, int y2);
void remove_possible_moves(int **board);
void capture_pieces(int **board, int *score, int x, int y, int player);
void endgame(int **board, char *ldboard, int *score, int desc1, int desc2);

int move_player(int **board, int *score, int desc, int player);
int get_opponent(int player);
void pc_vs_player(int **board, char *ldboard, int *score, int desc, int player);

void init_db();
void insert_db(int player, int desc, int score, char *winner);
void select_db(char *ldboard);

int move_player_1(int **board, int *score, int desc1, int desc2);
int move_player_2(int **board, int *score, int desc1, int desc2);

int get_opponent(int player)
{
    if (player == 1)
        return 2;
    return 1;
}

char *init_ldboard()
{

    char *ldboard;
    ldboard = malloc(sizeof(char) * 1000);
    ldboard[0] = '\0';
    strcat(ldboard, "--------------\n");
    strcat(ldboard, "|LEADERBOARD:|\n");
    strcat(ldboard, "--------------\n");
    strcat(ldboard, "-------------------\n");
    strcat(ldboard, "|SCORE|USERNAME   |\n");
    strcat(ldboard, "-------------------\n");

    return ldboard;
}

void init_db()
{
    sqlite3 *db;
    char *msg_err = 0;

    int ok = sqlite3_open("leaderboard.db", &db);

    if (ok != SQLITE_OK)
    {
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
    }

    char *sql = "DROP TABLE IF EXISTS Leaderboard;"
                "CREATE TABLE Leaderboard(Score INT, Name TEXT UNIQUE);";
    ok = sqlite3_exec(db, sql, 0, 0, &msg_err);
    if (ok != SQLITE_OK)
    {
        printf("SQL CREATE error: %s\n", msg_err);
        sqlite3_free(msg_err);
    }

    sqlite3_close(db);
}

void insert_db(int player, int desc, int score, char *winner)
{
    sqlite3 *db;
    char *msg_err = 0;

    int ok = sqlite3_open("leaderboard.db", &db);

    if (ok != SQLITE_OK)
    {
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
    }
    do
    {
        char *query = sqlite3_mprintf("INSERT into Leaderboard VALUES (%d,'%q');", score, winner);
        ok = sqlite3_exec(db, query, 0, 0, &msg_err);
        sqlite3_free(query);
        if (ok != SQLITE_OK)
        {
            printf("SQL INSERT error: %s\n", msg_err);
            sqlite3_free(msg_err);
            msg_invalid_username(desc);
            winner = msg_rcv_username(desc);
            query = sqlite3_mprintf("INSERT into Leaderboard VALUES (%d,'%q');", score, winner);
            ok = sqlite3_exec(db, query, 0, 0, &msg_err);
            sqlite3_free(query);
        }
    } while (ok != SQLITE_OK);

    free(winner);
    sqlite3_close(db);
}

void select_db(char *ldboard)
{

    sqlite3 *db;
    char *msg_err = 0;

    int ok = sqlite3_open("leaderboard.db", &db);

    if (ok != SQLITE_OK)
    {
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_stmt *stmt;

    char *sql = "SELECT * FROM Leaderboard ORDER BY 1 DESC";

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK)
    {
        printf("error: %s", sqlite3_errmsg(db));
        return;
    }
    int rows = 1;
    while (rows++ <= N && (rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {

        char *line = malloc(sizeof(char) * 100);
        line[0] = '\0';
        strcat(line, "|");

        const char *score = sqlite3_column_text(stmt, 0);
        const char *name = sqlite3_column_text(stmt, 1);

        strcat(line, score);
        strcat(line, "   |");
        strcat(line, name);
        strcat(line, "\n");

        strcat(ldboard, line);
        free(line);
    }

    sqlite3_finalize(stmt);

    sqlite3_close(db);
}

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
        if (write(desc, "87 [player 1]Please insert your move (ex: 1 1)! (your number is 1, possible move is 3)\n", 90) <= 0)
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
        if (write(desc, "89 [player 1]Invalid move! Please insert valid move!(your number is 1, possible move is 3)\n", 92) <= 0)
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
        if (write(desc, "87 [player 2]Please insert your move (ex: 1 1)! (your number is 2, possible move is 3)\n", 90) <= 0)
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
        if (write(desc, "89 [player 2]Invalid move! Please insert valid move!(your number is 2, possible move is 3)\n", 92) <= 0)
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
    char char_board[66], k = 3;
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
}

void msg_score(int desc, int *score)
{
    char char_score[10];
    char_score[0] = '6';
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
    if (write(desc, "44 Type in your username for the leaderboard:", 47) <= 0)
    {
        perror("error:[msg_msg_rcv_username]-write\n");
    }
    char *winner;
    winner = malloc(sizeof(char) * 30);
    int nr = 0, k_len;
    char msg_len[3];
    read(desc, msg_len, 2);
    msg_len[2] = '\0';

    int len;
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

int move_player_1(int **board, int *score, int desc1, int desc2)
{
    int mv1;
    mv1 = 99;
    msg_score(desc1, score);
    msg_board(board, desc1);
    msg_player_1(desc1, 2); // insert your move
    msg_player_2(desc2, 3); // wait for player 1
    mv1 = rcv_move(board, desc1, desc2);
    if (mv1 == -2 || mv1 == -3 || mv1 == -1)
        return mv1;
    while (!is_playable(board, mv1 / 10, mv1 % 10))
    {
        msg_player_1(desc1, 4); // player 1 : invalid move
        mv1 = rcv_move(board, desc1, desc2);
        if (mv1 == -2 || mv1 == -3)
            return mv1;
    }

    update_board(board, mv1, 1);

    remove_possible_moves(board);

    capture_pieces(board, score, mv1 / 10, mv1 % 10, 1);

    msg_score(desc1, score);
    msg_board(board, desc1);

    mark_possible_moves(board, 2);

    if (check_endgame(board) == 1)
        return 1;
    return 0;
}

int move_player_2(int **board, int *score, int desc1, int desc2)
{
    int mv2;
    msg_player_1(desc1, 3); // wait for player 2
    msg_score(desc2, score);
    msg_board(board, desc2);
    msg_player_2(desc2, 2); // insert move
    mv2 = rcv_move(board, desc2, desc1);
    if (mv2 == -2 || mv2 == -3 || mv2 == -1)
        return mv2;
    while (!is_playable(board, mv2 / 10, mv2 % 10))
    {
        msg_player_2(desc2, 4); // invalid move
        mv2 = rcv_move(board, desc2, desc1);
        if (mv2 == -2 || mv2 == -3)
            return mv2;
    }

    update_board(board, mv2, 2);

    remove_possible_moves(board);

    capture_pieces(board, score, mv2 / 10, mv2 % 10, 2);

    msg_score(desc2, score);
    msg_board(board, desc2);

    mark_possible_moves(board, 1);

    if (check_endgame(board) == 1)
        return 1;
    return 0;
}

int move_pc(int **board, int *score, int desc, int player)
{
    int opp = get_opponent(player);

    int mv;
    mv = rcv_move_auto(board);
    sleep(1);
    update_board(board, mv, player);
    remove_possible_moves(board);
    capture_pieces(board, score, mv / 10, mv % 10, player);
    mark_possible_moves(board, opp);
    if (check_endgame(board) == 1)
        return 1;
    return 0;
}

int move_player(int **board, int *score, int desc, int player)
{
    int opp = get_opponent(player);

    int mv;
    mv = 99;
    msg_score(desc, score);
    msg_board(board, desc);
    if (player == 1)
        msg_player_1(desc, 2); // insert move
    else
        msg_player_2(desc, 2); // insert move
    mv = rcv_move(board, desc, desc);
    if (mv == -1)
        return -1;
    while (!is_playable(board, mv / 10, mv % 10))
    {
        if (player == 1)
            msg_player_1(desc, 4); //  invalid move
        else
            msg_player_2(desc, 4); // invalid move
        mv = rcv_move(board, desc, desc);
        if (mv == -1)
            return -1;
    }

    update_board(board, mv, player);
    remove_possible_moves(board);
    capture_pieces(board, score, mv / 10, mv % 10, player);

    msg_score(desc, score);
    msg_board(board, desc);

    mark_possible_moves(board, opp);

    if (check_endgame(board) == 1)
        return 1;
    return 0;
}

void pc_vs_player(int **board, char *ldboard, int *score, int desc, int player)
{
    int opp = get_opponent(player);

    int gameOver = 0;
    while (!gameOver && gameOver != -1)
    {
        gameOver = move_pc(board, score, desc, opp);
        if (gameOver)
            break;
        gameOver = move_player(board, score, desc, player);
    }
    if (gameOver == 1)
    {
        msg_score(desc, score);
        msg_board(board, desc);
        if (score[opp] > score[player])
        {
            msg_loser(desc);
        }
        else if (score[opp] == score[player])
        {
            msg_tie(desc);
        }
        else if (score[player] > score[opp])
        {
            char *winner;
            msg_winner(desc);
            winner = msg_rcv_username(desc);
            insert_db(player, desc, score[player], winner);
            select_db(ldboard);
            msg_leaderboard(desc, ldboard);
        }
    }
}

int check_endgame(int **board)
{
    int i, j;
    for (i = 0; i < 8; i++)
        for (j = 0; j < 8; j++)
            if (board[i][j] == 3)
                return 0;
    return 1;
}

int rcv_move_auto(int **board)
{
    int i, j, end;
    end = check_endgame(board);
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

int distance(int x1, int y1, int x2, int y2)
{
    int di = abs(x1 - x2), dj = abs(y1 - y2);
    if (di > 0)
        return di;
    return dj;
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

void endgame(int **board, char *ldboard, int *score, int desc1, int desc2)
{
    msg_score(desc1, score);
    msg_board(board, desc1);
    msg_score(desc2, score);
    msg_board(board, desc2);
    msg_endgame(ldboard, desc1, desc2, score);
    close(desc1);
    close(desc2);
}

int **init_board()
{
    int i, j;
    int **board = (int **)malloc(sizeof(int *) * 8);
    for (i = 0; i < 8; i++)
        board[i] = (int *)malloc(sizeof(int) * 8);

    for (i = 0; i < 8; i++)
        for (j = 0; j < 8; j++)
            board[i][j] = 0;
    board[3][3] = 1;
    board[4][4] = 1;
    board[3][4] = 2;
    board[4][3] = 2;
    mark_possible_moves(board, 1);
    return board;
}

static void *treat(void *arg)
{
    struct thData tdL;
    tdL = *((struct thData *)arg);

    pthread_detach(pthread_self());
    start_game(tdL.desc1, tdL.desc2);
    return (NULL);
};

void start_game(int desc1, int desc2)
{
    msg_player_1(desc1, 1);
    int **board = init_board();
    int score[3];
    score[1] = 2;
    score[2] = 2;
    char *ldboard = init_ldboard();
    play(board, ldboard, score, desc1, desc2);
}

void play(int **board, char *ldboard, int *score, int desc1, int desc2)
{
    int status1, status2;
    int gameOver = 0;
    while (!gameOver)
    {

        status1 = move_player_1(board, score, desc1, desc2);
        if (status1 == -3 || status1 == -2 || status1 == -1 || status1 == 1)
        {
            gameOver = 1;
            break;
        }
        status2 = move_player_2(board, score, desc1, desc2);
        if (status2 == -3 || status2 == -2 || status2 == -1 || status2 == 1)
        {
            gameOver = 1;
            break;
        }
    }
    if (status1 != -1 && status2 != -1)
    {
        if (status1 != -3 && status2 != -3 && status1 != -2 && status2 != -2) // both players connected
            endgame(board, ldboard, score, desc1, desc2);
        else
        {
            if (status1 == -3) // player 2 doesn't want to continue
                msg_gameOver(desc2);
            else if (status2 == -3) // player 1 doesn't want to continue
                msg_gameOver(desc1);
            else if (status1 == -2) // player 2 wants to continue
            {
                pc_vs_player(board, ldboard, score, desc2, 2);
            }
            else if (status2 == -2) // player 1 wants to continue
                pc_vs_player(board, ldboard, score, desc1, 1);
        }
    }
    for (int i = 0; i < 8; i++)
        free(board[i]);

    free(board);
    printf("MATCH OVER!\n");
    fflush(stdout);
}

int main()
{
    init_db();
    struct sockaddr_in server;
    struct sockaddr_in from;
    int sd;

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) // tcp socket
    {
        perror("error: socket.\n");
        return errno;
    }

    int on = 1;
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    bzero(&server, sizeof(server));
    bzero(&from, sizeof(from));

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(PORT);

    if (bind(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
    {
        perror("[server]Eroare la bind().\n");
        return errno;
    }

    if (listen(sd, 2) == -1)
    {
        perror("error: listen\n");
        return errno;
    }

    int p1_desc, players = 0;
    printf("[server]WAITING FOR PLAYERS AT PORT %d...\n", PORT);
    fflush(stdout);
    while (1)
    {
        int player;
        int length = sizeof(from);

        if ((player = accept(sd, (struct sockaddr *)&from, &length)) < 0)
        {
            perror("error: accept");
            continue;
        }
        // player connected

        if (players == 0) // player 1 connected
        {
            printf("PLAYER 1 CONNECTED!\n");
            fflush(stdout);
            players++;
            p1_desc = player;        // saving player 1 descriptor
            msg_player_1(player, 0); // connected, waiting for player 2
        }
        else // player 2 connected
        {
            printf("PLAYER 2 CONNECTED!\n");
            fflush(stdout);
            players--;
            msg_player_2(player, 0); // connected, starting game

            th_desc *td;
            td = (struct thData *)malloc(sizeof(struct thData));
            td->desc1 = p1_desc;
            td->desc2 = player;

            printf("CREATING THREAD...\n");
            fflush(stdout);
            pthread_t thread;
            pthread_create(&thread, NULL, &treat, td);
        }
    } // while 1
} // main
