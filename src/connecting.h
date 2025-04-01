#ifndef CONNECTING_H
#define CONNECTING_H

typedef enum _TEXT_BOX_STATUS {
    NOT_SELECTED,
    SELECTED
} TEXT_BOX_STATUS;

typedef struct _TEXT_BOX {
    char *buffer;
    TEXT_BOX_STATUS status;
} TEXT_BOX;

typedef struct _CONNECTION {
    TEXT_BOX ip_addr;
    TEXT_BOX port;
} CONNECTION;

int connection_init(SDL_Rect rect_ip, SDL_Rect rect_port);
void destroy_connection(TEXT_BOX *text_box);

#endif