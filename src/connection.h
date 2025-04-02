#ifndef CONNECTING_H
#define CONNECTING_H

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

typedef struct _CONNECTION {
    TEXT_BOX ip_addr;
    TEXT_BOX port;
} CONNECTION;

CONNECTION* connection_init(SDL_FRect rect_ip, SDL_FRect rect_port);
void destroy_connection(CONNECTION *connection);

#endif