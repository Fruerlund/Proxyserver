#include "../headers/network.h"

//========================================= SOCKET/SERVER =========================================//

/**
 * @brief Create a socket object
 * 
 * @return int 
 */
int create_socket(void) {

    int fd = -1;

    // Set socket timeout

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == 0) {
        return -1;
    }
    else {
        return fd;
    }
}



/**
 * @brief 
 * @param listener 
 * @param clientfd 
 */
void handleConnection(struct listener_h *listener, int clientfd) {


    if(listener->CLIENTCONNECTIONS.number_connections == MAX_CONN) {
        printf("Server capacity reached. Connection rejected!\n");
        return;
    }

    handleRequest(listener, clientfd);

}



/**
 * @brief 
 * 
 * @param listener 
 * @param port 
 * @return int 
 */
int startListener(struct listener_h *listener, int port) {


    int server_fd = create_socket();
    if(server_fd == -1) {
        printf("ERROR: create_socket() returned -1\n");
        return -1;
    }

    struct sockaddr_in address;
    int opt = 1;

    if( (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &port, sizeof(opt))) ) {
        printf("ERROR: setsockopt()\n");
        return -1;
    }

    address.sin_port = htons(port);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;

    int addr_len = sizeof(address);

    if( bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        printf("ERROR: bind()\n");
        return -1;
    }

    if( listen(server_fd, MAX_CONN) < 0) {
        printf("ERROR: listen()\n");
        return -1;
    }

    
    // Start thread for multiplexing
    pthread_t p;
    pthread_create(&p, NULL, pooling, listener);
    

    int client_fd = -1;
    while(1) {

        if( (client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addr_len)) < 0) {
            printf("ERROR: accept()\n");
            return -1;
        }

        handleConnection(listener, client_fd);

    }
    return 0;
}