#include "SocketsOps.h"
#include <sys/socket.h>

int createSocket()
{
    return socket(AF_INET, SOCK_STREAM, 0);
}