#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <pthread.h>
#include "functions_server/game.h"
#define PORT 2000

typedef struct thData
{
    int desc1;
    int desc2;
} th_desc;

extern int errno;

static void *treat(void *);

static void *treat(void *arg)
{
    struct thData tdL;
    tdL = *((struct thData *)arg);

    pthread_detach(pthread_self());
    start_game(tdL.desc1, tdL.desc2);
    return (NULL);
};

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
