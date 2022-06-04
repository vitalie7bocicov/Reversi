#include <SFML/Graphics.hpp>
using namespace sf;

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>

#include <math.h>
#include <arpa/inet.h>

#include "functions_client/close_conn.h"
int port;

void play(int sd)
{
    RenderWindow window(VideoMode(454, 454), "Reversi", Style::Close | Style::Titlebar);
    Event e;

    Texture t1;
    t1.loadFromFile("resources/rev.png");

    Image icon;
    icon.loadFromFile("resources/icon.png");
    window.setIcon(512, 512, icon.getPixelsPtr());

    Sprite boardImg(t1);
    Font font;
    if (!font.loadFromFile("resources/font.ttf"))
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
                        p1.loadFromFile("resources/white.png");
                        Sprite p(p1);
                        p.setScale(Vector2f(0.2, 0.2));
                        p.setPosition(j * getc() + getc(), i * getc() + getc());
                        window.draw(p);
                    }
                    else if (board[i][j] == 2)
                    {
                        Texture p2;
                        p2.loadFromFile("resources/black.png");
                        Sprite p(p2);
                        p.setScale(Vector2f(0.2, 0.2));
                        p.setPosition(j * getc() + getc(), i * getc() + getc());
                        window.draw(p);
                    }
                    else if (board[i][j] == 3)
                    {
                        Texture y;
                        y.loadFromFile("resources/yellow.png");
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

                                status = check_msg(sd);   // rcv score
                                status = check_msg(sd);   // and board
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
            status = check_msg(sd);

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