#ifndef CONNECTING_H
#define CONNECTING_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <fcntl.h>
#include <errno.h>

typedef enum _TEXT_BOX_STATUS {
    NOT_SELECTED,
    SELECTED
} TEXT_BOX_STATUS;

typedef struct _TEXT_BOX {
    char *buffer;
    int max_length;
    TEXT_BOX_STATUS status;
    SDL_FRect rect;
} TEXT_BOX;

typedef enum _CONNECTED {
    not_connected,
    connected
} CONNECTED;

typedef struct _CONNECTION {
    TEXT_BOX ip_addr;
    TEXT_BOX port;
    CONNECTED connection;
} CONNECTION;

CONNECTION* connection_init(SDL_FRect rect_ip, SDL_FRect rect_port);
int connect_to_server(CONNECTION *con);
void destroy_connection(CONNECTION *connection);

#endif