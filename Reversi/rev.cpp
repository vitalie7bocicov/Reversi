#include <SFML/Graphics.hpp>
using namespace sf;

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <math.h>
#include <arpa/inet.h>
#include <time.h>
#include <fcntl.h>

int port, player = 0;
int board[8][8];
char c_score[20];

int getc()
{
    return 46.479;
}

int gety()
{
    return 50;
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

int check_endgame()
{
    int i, j;
    for (i = 0; i < 8; i++)
        for (j = 0; j < 8; j++)
            if (board[i][j] == 3)
                return 0;
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

int readS(int sd)
{
    int m_len, ok;
    m_len = msg_length(sd);
    char buff[m_len];
    ok = read(sd, buff, m_len);
    if (ok <= 0)
        return ok;

    buff[ok] = '\0';

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
            receive = readS(sd);
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

void play(int sd)
{
    RenderWindow window(VideoMode(454, 454), "Reversi", Style::Close | Style::Titlebar);
    Event e;

    Texture t1;
    t1.loadFromFile("rev.png");

    Image icon;
    icon.loadFromFile("icon.png");
    window.setIcon(512, 512, icon.getPixelsPtr());

    Sprite boardImg(t1);
    Font font;
    if (!font.loadFromFile("font.ttf"))
    {
        printf("error:loading font\n");
    }

    Text score;
    score.setFont(font);
    score.setCharacterSize(20);
    score.setFillColor(Color::Black);
    score.setStyle(sf::Text::Bold);
    score.setPosition(125, 10);

    window.draw(boardImg);
    window.display();

    Text you;
    you.setFont(font);
    you.setCharacterSize(20);
    you.setFillColor(Color::Black);
    you.setStyle(sf::Text::Bold);
    you.setPosition(130, 420);

    Text discon;
    discon.setFont(font);
    discon.setString("Opponent disconnected!\nClick if you want to continue\nyour game with the computer.");
    discon.setPosition(10, 10);
    discon.setCharacterSize(25);
    discon.setFillColor(Color::White);
    discon.setStyle(sf::Text::Bold);

    printf("Welcome to REVERSI! \n");
    fflush(stdout);
    int x, y;
    int status = 0;

    while (window.isOpen())
    {
        if (status == 3 || status == 4 || status == 7 || status == 8 || status == 9 || status == 100) // board move win lost draw
        {
            // draw board+score+you
            window.clear();
            window.draw(boardImg);
            for (int i = 0; i < 8; i++)
                for (int j = 0; j < 8; j++)
                {
                    if (board[i][j] == 1)
                    {
                        Texture p1;
                        p1.loadFromFile("white.png");
                        Sprite p(p1);
                        p.setScale(Vector2f(0.2, 0.2));
                        p.setPosition(j * getc() + getc(), i * getc() + getc());
                        window.draw(p);
                    }
                    else if (board[i][j] == 2)
                    {
                        Texture p2;
                        p2.loadFromFile("black.png");
                        Sprite p(p2);
                        p.setScale(Vector2f(0.2, 0.2));
                        p.setPosition(j * getc() + getc(), i * getc() + getc());
                        window.draw(p);
                    }
                    else if (board[i][j] == 3)
                    {
                        Texture y;
                        y.loadFromFile("yellow.png");
                        Sprite p(y);
                        p.setScale(Vector2f(0.2, 0.2));
                        p.setPosition(j * getc() + gety(), i * getc() + gety());
                        window.draw(p);
                    }
                }

            window.draw(score);
            window.draw(you);
            window.display();

            if (status == 4)
                status = 99;        // move inserted
            else if (status != 100) // not game over
                status = 0;
        }

        Vector2i pos = Mouse::getPosition(window);
        while (window.pollEvent(e))
        {
            if (e.type == Event::Closed)
            {
                window.close();
                if (status != 99 && status != 100)
                    close_conn(sd, status);
            }
            if (e.type == Event::MouseButtonPressed)
            {
                if (e.mouseButton.button == Mouse::Left)
                {
                    if (status == 99) // move was inserted
                    {
                        if (pos.x > 45 && pos.x < 405 && pos.y > 45 && pos.y < 405)
                        {
                            x = (pos.x - 40) / 46.5;
                            y = (pos.y - 40) / 46.5;
                            if (board[y][x] == 3)
                            {
                                printf("MOVE: %d %d\n", y, x);
                                char move[3];
                                move[0] = y + '0';
                                move[1] = ' ';
                                move[2] = x + '0';
                                move[3] = '\0';
                                if (write(sd, move, 3) <= 0)
                                {
                                    perror("error:write move\n");
                                }

                                status = readS(sd);       // rcv score
                                status = readS(sd);       // and board
                                score.setString(c_score); // update score
                            }
                            else
                            {
                                printf("[player %d] Invalid move!\n", player);
                            }
                        }
                    }                     // move status 99
                    else if (status == 6) // opp disconnected
                    {
                        char answer[1];
                        answer[0] = 'y';
                        answer[1] = '\0';
                        if (write(sd, answer, 2) <= 0)
                        {
                            perror("error:write answer\n");
                        }
                        printf("playing vs pc\n");
                        fflush(stdout);
                        status = 0;

                    } // opp dis

                } // left click
            }     // click
        }         // poll event
        // if (status == 99) // muto
        // {
        //     x = rcv_move_auto() / 10;
        //     y = rcv_move_auto() % 10;
        //     printf("MOVE: %d %d\n", y, x);
        //     char move[3];
        //     move[0] = x + '0';
        //     move[1] = ' ';
        //     move[2] = y + '0';
        //     move[3] = '\0';
        //     if (write(sd, move, 3) <= 0)
        //     {
        //         perror("[player move]Eroare la write() catre server.\n");
        //     }

        //     status = readS(sd);
        //     status = readS(sd);
        //     score.setString(c_score);
        // }
        if (status != 99 && status != 3 && status != 6 && status != 10 && status != 100) // not move,board,opp diss,insert username
            status = readS(sd);

        switch (status)
        {
        case 1: // connected
        {
            if (player == 1)
                you.setString("YOU ARE WHITE");
            else
                you.setString("YOU ARE BLACK");
            score.setString("WAITING FOR YOUR OPPONENT TO JOIN...");
            score.setPosition(10, 10);
            window.draw(score);
            window.draw(you);
            window.display();
            break;
        }
        case 2: // score
        {
            score.setString(c_score);
            score.setPosition(125, 10);
            break;
        }

        case 6: // oppponent disconnected
        {

            window.clear();
            window.draw(discon);
            window.display();
            break;
        }

        case 7: // won
        {
            you.setString("TYPE YOUR USERNAME IN THE TERMINAL!");
            you.setPosition(10, 420);
            break;
        }
        case 8: // lost
        {
            you.setString("YOU LOST!");
            you.setPosition(173, 420);
            status = 100;
            close(sd);
            break;
        }
        case 9: // draw
        {
            you.setString("IT'S A DRAW!");
            you.setPosition(170, 420);
            status = 100;
            close(sd);
            break;
        }
        case 10: // input username
        {
            int rcv = rcv_username(sd);
            if (rcv == 1)
            {
                you.setString("YOU WON!");
                you.setPosition(173, 420);
                status = 0;
            }
            break;
        }
        case 11:
        {
            status = 100;
            close(sd);
            break;
        }

        default:
            break;
        }
    }
}

int main(int argc, char *argv[])
{

    int sd;

    struct sockaddr_in server;

    if (argc != 3)
    {
        printf("SINTAX: %s <IP_ADDRESS> <PORS>\n", argv[0]);
        return -1;
    }

    port = atoi(argv[2]);

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("error:SOCKET\n");
        return errno;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_port = htons(port);

    if (connect(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
    {
        perror("error: connect\n");
        return errno;
    }

    int flags = fcntl(sd, F_GETFL, 0);
    fcntl(sd, F_SETFL, flags | O_NONBLOCK);

    play(sd);
    printf("GAME OVER!\n");
    close(sd);
}