#include "receive.h"
#include <fcntl.h>
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