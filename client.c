// client.c - un micro-client qui envoie un message à un serveur et attend sa réponse
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 4242 // le port du serveur auquel on va se connecter
#define IP "10.11.1.5" 

int main(int ac, char **av)
{
    printf("---- CLIENT ----\n\n");
    struct sockaddr_in sa;
    int socket_fd;
    int status;
    char buffer[BUFSIZ];
    int bytes_read;
    char *msg;
    int msg_len;
    int bytes_sent;

    if (ac != 2) {
        printf("Usage: ./client \"Message to send\"");
        return (1);
    }

    // on prépare l'adresse et le port auquel on veut se connecter
    memset(&sa, 0, sizeof sa);
    sa.sin_family = AF_INET; // IPv4
    inet_pton(AF_INET, IP, &sa.sin_addr);
    sa.sin_port = htons(PORT);

    // on crée la socket et on la connecte au serveur distant
    socket_fd = socket(sa.sin_family, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        fprintf(stderr, "socket fd error: %s\n", strerror(errno));
        return (2);
    }
    printf("Created socket fd: %d\n", socket_fd);

    status = connect(socket_fd, (struct sockaddr *)&sa, sizeof sa);
    if (status != 0) {
        fprintf(stderr, "connect error: %s\n", strerror(errno));
        return (3);
    }
    printf("Connected socket to localhost port %d\n", PORT);

    // on envoie un message au serveur
    msg = av[1];
    msg_len = strlen(msg);
    bytes_sent = send(socket_fd, msg, msg_len, 0);
    if (bytes_sent == -1) {
        fprintf(stderr, "send error: %s\n", strerror(errno));
    }
    else if (bytes_sent == msg_len) {
        printf("Sent full message: \"%s\"\n", msg);
    }
    else {
        printf("Sent partial message: %d bytes sent.\n", bytes_sent);
    }
        
    // on attend de recevoir un message via la socket
    bytes_read = 1;
    while (bytes_read >= 0) {
        bytes_read = recv(socket_fd, buffer, BUFSIZ, 0);
        if (bytes_read == 0) {
            printf("Server closed connection.\n");
            break ;
        }
        else if (bytes_read == -1) {
            fprintf(stderr, "recv error: %s\n", strerror(errno));
            break ;
        }
        else {
            // Si on a bien reçu un message, on va l'imprimer
            buffer[bytes_read] = '\0';
            printf("Message received: \"%s\"\n", buffer);
            break ;
        }
    }

    printf("Closing socket\n");
    close(socket_fd);
    return (0);
}