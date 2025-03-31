#include "connecting.h"

int connection_init(CONNECTION *connection, SDL_Rect ip_rect, SDL_Rect port_rect) {
    connection->ip_addr.buffer = (char*) SDL_malloc(sizeof(char) * 20);
    connection->ip_addr.status = SELECTED;
    connection->ip_addr.rect = ip_rect;
    connection->port.buffer = (char*) SDL_malloc(sizeof(char) * 6);
    connection->port.status = NOT_SELECTED;
    connection->port.rect = port_rect;
}

void destroy_connection(CONNECTION *connection) {
    if (connection->ip_addr.buffer != NULL) {
        SDL_free(connection->ip_addr.buffer);
    }
    if (connection->port.buffer != NULL) {
        SDL_free(connection->port.buffer);
    }

}