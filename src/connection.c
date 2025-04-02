#include "connection.h"

CONNECTION* connection_init(SDL_FRect ip_rect, SDL_FRect port_rect) {
    CONNECTION *connection = SDL_malloc(sizeof(CONNECTION));
    connection->ip_addr.buffer = (char*) SDL_malloc(sizeof(char) * 20);
    memset(connection->ip_addr.buffer, 0, sizeof(char) * 20);
    connection->ip_addr.status = SELECTED;
    connection->ip_addr.rect = ip_rect;
    connection->ip_addr.max_length = 20;
    connection->port.buffer = (char*) SDL_malloc(sizeof(char) * 6);
    memset(connection->port.buffer, 0, sizeof(char) * 6);
    connection->port.status = NOT_SELECTED;
    connection->port.rect = port_rect;
    connection->port.max_length = 6;
    return connection;
}

void destroy_connection(CONNECTION *connection) {
    if (connection->ip_addr.buffer != NULL) {
        SDL_free(connection->ip_addr.buffer);
    }
    if (connection->port.buffer != NULL) {
        SDL_free(connection->port.buffer);
    }

}