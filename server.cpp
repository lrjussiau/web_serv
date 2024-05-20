#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 4242  // le port de notre serveur
#define IP "127.0.0.1"

int create_server_socket(void);
void accept_new_connection(int listener_socket, fd_set *all_sockets, int *fd_max);
void read_data_from_socket(int socket, fd_set *all_sockets, int fd_max, int server_socket);

int main(void)
{
    printf("---- SERVER ----\n\n");
	
    int server_socket;
    int status;

    // Pour surveiller les sockets clients :
    fd_set all_sockets; // Ensemble de toutes les sockets du serveur
    fd_set read_fds;    // Ensemble temporaire pour select()
    int fd_max;         // Descripteur de la plus grande socket
    struct timeval timer;

    // Création de la socket du serveur
    server_socket = create_server_socket();
    if (server_socket == -1) {
        return (1);
    }

    // Écoute du port via la socket
    printf("[Server] Listening on port %d\n", PORT);
    status = listen(server_socket, 10);
    if (status != 0) {
        fprintf(stderr, "[Server] Listen error: %s\n", strerror(errno));
        return (3);
    }

    // Préparation des ensembles de sockets pour select()
    FD_ZERO(&all_sockets);
    FD_ZERO(&read_fds);
    FD_SET(server_socket, &all_sockets); // Ajout de la socket principale à l'ensemble
    fd_max = server_socket; // Le descripteur le plus grand est forcément celui de notre seule socket
    printf("[Server] Set up select fd sets\n");

    while (1) { // Boucle principale
        // Copie l'ensemble des sockets puisque select() modifie l'ensemble surveillé
        read_fds = all_sockets;
        // Timeout de 2 secondes pour select()
        timer.tv_sec = 2;
        timer.tv_usec = 0;
		
        // Surveille les sockets prêtes à être lues
        status = select(fd_max + 1, &read_fds, NULL, NULL, &timer);
        if (status == -1) {
            fprintf(stderr, "[Server] Select error: %s\n", strerror(errno));
            exit(1);
        }
        else if (status == 0) {
            // Aucun descipteur de fichier de socket n'est prêt pour la lecture
            printf("[Server] Waiting...\n");
            continue;
        }

        // Boucle sur nos sockets
        for (int i = 0; i <= fd_max; i++) {
            if (FD_ISSET(i, &read_fds) != 1) {
                // Le fd i n'est pas une socket à surveiller
                // on s'arrête là et on continue la boucle
                continue ;
            }
            printf("[%d] Ready for I/O operation\n", i);
            // La socket est prête à être lue !
            if (i == server_socket) {
                // La socket est notre socket serveur qui écoute le port
                accept_new_connection(server_socket, &all_sockets, &fd_max);
            }
            else {
                // La socket est une socket client, on va la lire
                read_data_from_socket(i, &all_sockets, fd_max, server_socket);
            }
        }
    }
    return (0);
}

// Renvoie la socket du serveur liée à l'adresse et au port qu'on veut écouter
int create_server_socket(const char *ip) {
    struct sockaddr_in sa;
    int socket_fd;

    // Préparaton de l'adresse et du port pour la socket de notre serveur
    memset(&sa, 0, sizeof sa);
    sa.sin_family = AF_INET; // IPv4
    inet_pton(AF_INET, IP, &sa.sin_addr);
    sa.sin_port = htons(PORT);

    // Création de la socket
    socket_fd = socket(sa.sin_family, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        fprintf(stderr, "[Server] Socket error: %s\n", strerror(errno));
        return (-1);
    }
    printf("[Server] Created server socket fd: %d\n", socket_fd);

    // Liaison de la socket à l'adresse et au port
    if (bind(socket_fd, (struct sockaddr *)&sa, sizeof sa)) {
        fprintf(stderr, "[Server] Bind error: %s\n", strerror(errno));
        return (-1);
    }
    printf("[Server] Bound socket to %s port %d\n", ip, PORT);
    return (socket_fd);
}

// Accepte une nouvelle connexion et ajoute la nouvelle socket à l'ensemble des sockets
void accept_new_connection(int server_socket, fd_set *all_sockets, int *fd_max)
{
    int client_fd;
    char msg_to_send[BUFSIZ];
    int status;

    client_fd = accept(server_socket, NULL, NULL);
    if (client_fd == -1) {
        fprintf(stderr, "[Server] Accept error: %s\n", strerror(errno));
        return ;
    }
    FD_SET(client_fd, all_sockets); // Ajoute la socket client à l'ensemble
    if (client_fd > *fd_max) {
        *fd_max = client_fd; // Met à jour la plus grande socket
    }
    printf("[Server] Accepted new connection on client socket %d.\n", client_fd);
    memset(&msg_to_send, '\0', sizeof msg_to_send);
    sprintf(msg_to_send, "Welcome. You are client fd [%d]\n", client_fd);
    status = send(client_fd, msg_to_send, strlen(msg_to_send), 0);
    if (status == -1) {
        fprintf(stderr, "[Server] Send error to client %d: %s\n", client_fd, strerror(errno));
    }
}

// Lit le message d'une socket et relaie le message à toutes les autres
void read_data_from_socket(int socket, fd_set *all_sockets, int fd_max, int server_socket)
{
    char buffer[BUFSIZ];
    char msg_to_send[BUFSIZ];
    int bytes_read;
    int status;

    memset(&buffer, '\0', sizeof buffer);
    bytes_read = recv(socket, buffer, BUFSIZ, 0);
    if (bytes_read <= 0) {
        if (bytes_read == 0) {
            printf("[%d] Client socket closed connection.\n", socket);
        }
        else {
            fprintf(stderr, "[Server] Recv error: %s\n", strerror(errno));
        }
        close(socket); // Ferme la socket
        FD_CLR(socket, all_sockets); // Enlève la socket de l'ensemble
    }
    else {
        // Renvoie le message reçu à toutes les sockets connectées
        // à part celle du serveur et celle qui l'a envoyée
        printf("[%d] Got message: %s", socket, buffer);
        memset(&msg_to_send, '\0', sizeof msg_to_send);
        sprintf(msg_to_send, "[%d] says: %s", socket, buffer);
        for (int j = 0; j <= fd_max; j++) {
            if (FD_ISSET(j, all_sockets) && j != server_socket && j != socket) {
                status = send(j, msg_to_send, strlen(msg_to_send), 0);
                if (status == -1) {
                    fprintf(stderr, "[Server] Send error to client fd %d: %s\n", j, strerror(errno));
                }
            }
        }
    }
}



/*// server.c - un micro-serveur qui accepte une connexion client, attend un message, et y répond
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
#define IP "10.11.1.1" // l'adresse IP de notre serveur
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

    if (bind(socket_fd, (struct sockaddr *)&sa, sizeof sa)) {
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
}*/