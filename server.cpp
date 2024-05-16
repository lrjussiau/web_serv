// server.c - un micro-serveur qui accepte une connexion client, attend un message, et y répond
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <stdlib.h>
#include <fcntl.h>

#define PORT 8080  // le port de notre serveur
#define IP "10.11.1.5" // l'adresse IP de notre serveur
#define BACKLOG 10 // nombre max de demandes de connexion

void get_current_date(char *buffer, size_t buffer_size) {
    time_t now = time(NULL);
    struct tm tm = *gmtime(&now);
    strftime(buffer, buffer_size, "%a, %d %b %Y %H:%M:%S %Z", &tm);
}


int main(void)
{
    printf("---- SERVER ----\n\n");
    struct sockaddr_in sa;
    int socket_fd;
    int client_fd;
    int status;
    struct sockaddr_storage client_addr; 
    socklen_t addr_size;
    char buffer[BUFSIZ];
    int bytes_read;

    // on prépare l'adresse et le port pour la socket de notre serveur
    memset(&sa, 0, sizeof sa);
    sa.sin_family = AF_INET; // IPv4
    inet_pton(AF_INET, IP, &sa.sin_addr);
    sa.sin_port = htons(PORT);

    // on crée la socket, on a lit et on écoute dessus
    socket_fd = socket(sa.sin_family, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        fprintf(stderr, "socket fd error: %s\n", strerror(errno));
        return (1);
    }
    printf("Created server socket fd: %d\n", socket_fd);

    status = bind(socket_fd, (struct sockaddr *)&sa, sizeof sa);
    if (status != 0) {
        fprintf(stderr, "bind error: %s\n", strerror(errno));
        return (2);
    }
    printf("Bound socket to localhost port %d\n", PORT);

    printf("Listening on port %d\n", PORT);
    status = listen(socket_fd, BACKLOG);
    if (status != 0) {
        fprintf(stderr, "listen error: %s\n", strerror(errno));
        return (3);
    }

    // on accepte une connexion entrante
    addr_size = sizeof client_addr;
    client_fd = accept(socket_fd, (struct sockaddr *)&client_addr, &addr_size);
    if (client_fd == -1) {
        fprintf(stderr, "client fd error: %s\n", strerror(errno));
        return (4);
    }
    printf("Accepted new connection on client socket fd: %d\n", client_fd);

    // on recoit un message via la socket client
    bytes_read = recv(client_fd, buffer, BUFSIZ, 0);
    if (bytes_read == 0) {
        printf("Client socket %d: closed connection.\n", client_fd);
    }
    else if (bytes_read == -1) {
        fprintf(stderr, "recv error: %s\n", strerror(errno));
    }
    else {
        // Read the contents of index.html
        FILE *file = fopen("index.html", "r");
        if (file == NULL) {
            fprintf(stderr, "Failed to open index.html: %s\n", strerror(errno));
            close(client_fd);
            close(socket_fd);
            return (5);
        }
        
        fseek(file, 0, SEEK_END);
        long file_size = ftell(file);
        fseek(file, 0, SEEK_SET);

        char *file_content = (char *)malloc(file_size + 1);
        if (file_content == NULL) {
            fprintf(stderr, "Memory allocation error\n");
            fclose(file);
            close(client_fd);
            close(socket_fd);
            return (6);
        }

        fread(file_content, 1, file_size, file);
        fclose(file);
        file_content[file_size] = '\0';

        // Construct the HTTP response
        char date[128];
        get_current_date(date, sizeof(date));

        char response_header[BUFSIZ];
        strcpy(response_header, "HTTP/1.1 200 OK\r\n");
        strcat(response_header, "Server: nginx/1.14.2\r\n");
        strcat(response_header, "Date: ");
        strcat(response_header, date);
        strcat(response_header, "\r\n");
        strcat(response_header, "Content-Type: text/html; charset=UTF-8\r\n");
        strcat(response_header, "Content-Length: ");
        
        char content_length[20];
        sprintf(content_length, "%ld", file_size);
        strcat(response_header, content_length);
        strcat(response_header, "\r\n");
        strcat(response_header, "Connection: keep-alive\r\n");
        strcat(response_header, "Link: </index.html>; rel=\"https://api.w.org/\"\r\n");
        strcat(response_header, "\r\n");

        // Send headers
        int bytes_sent = send(client_fd, response_header, strlen(response_header), 0);
        if (bytes_sent == -1) {
            fprintf(stderr, "send error: %s\n", strerror(errno));
        }

        // Send file content
        bytes_sent = send(client_fd, file_content, file_size, 0);
        if (bytes_sent == -1) {
            fprintf(stderr, "send error: %s\n", strerror(errno));
        } else {
            printf("Sent full message to client socket %d\n", client_fd);
        }

        free(file_content);
    }

    // Close client and server sockets
    printf("Closing client socket\n");
    close(client_fd);
    printf("Closing server socket\n");
    close(socket_fd);

    return (0);
}