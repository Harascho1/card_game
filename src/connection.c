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

    connection->connection = not_connected;

    return connection;
}

int connect_to_server(CONNECTION *con) {
    if (con == NULL) {
        return 0;
    }

    const char *ip = con->ip_addr.buffer;
    const char *port = con->port.buffer;
    if (ip == NULL || port == NULL) {
        return 0;
    }
    if (strlen(ip) == 0 || strlen(port) == 0) {
        return 0;
    }

    int port_num = atoi(port);

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        SDL_Log("socket error: %s\n", SDL_GetError());
        return 0;
    }

    fcntl(fd, F_SETFL, O_NONBLOCK);

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_num);
    if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
        SDL_Log("inet_pton error: %s\n", SDL_GetError());
        close(fd);
        return 0;
    }

    if (connect(fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        if (errno != EINPROGRESS) {
            perror("connect");
            SDL_Log("connect error: %s\n", SDL_GetError());
            close(fd);
            return 0;
        }
    }

    fd_set set;
    struct timeval timeout;
    FD_ZERO(&set);
    FD_SET(fd, &set);
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    int res = select(fd + 1, NULL, &set, NULL, &timeout);
    if (res > 0) {
        int optval;
        socklen_t optlen = sizeof(optval);
        getsockopt(fd, SOL_SOCKET, SO_ERROR, &optval, &optlen);
        if (optval == 0) {
            printf("Connected successfully\n");
            
        } else {
            printf("Connection failed: %s\n", strerror(optval));
            close(fd);
            return 0;
        }
    } else {
        printf("Connection timed out\n");
        close(fd);
        return 0;
    }
    con->connection = connected;

    SDL_Log("Connected to server %s:%d\n", ip, port_num);
    int flags = fcntl(fd, F_GETFL, 0);

    fcntl(fd, F_SETFL, flags & ~O_NONBLOCK);

    int recv_bytes;
    char buffer[1024];
    recv_bytes = recv(fd, buffer, sizeof(buffer), 0);
    if (recv_bytes < 0) {
        perror("recv");
        close(fd);
        return 0;
    } else if (recv_bytes == 0) {
        printf("Server closed the connection\n");
        close(fd);
        return 0;
    }

    printf("Received %d bytes: %s\n", recv_bytes, buffer);

    close(fd);
    return 1;

}

void destroy_connection(CONNECTION *connection) {
    if (connection->ip_addr.buffer != NULL) {
        SDL_free(connection->ip_addr.buffer);
    }
    if (connection->port.buffer != NULL) {
        SDL_free(connection->port.buffer);
    }

}