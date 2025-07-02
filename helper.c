/* helper.c */
#include "helper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

#define BUFLEN 4096

/* intoarce un buffer nou cu body-ul reconstruit; caller trebuie sa-l free-uiasca */
char *unchunk(const char *chunked)
{
    size_t len = strlen(chunked);
    char  *out = calloc(len + 1, 1);      /* suficient – dupa decodare va fi mai mic */
    const char *p = chunked;
    char  *w = out;

    while (1) {
        unsigned sz = 0;
        /* citim lungimea in hex */
        if (sscanf(p, "%x", &sz) != 1) break;
        /* sarim peste linia cu hex + CRLF */
        char *nl = strstr(p, "\r\n");
        if (!nl) break;
        p = nl + 2;
        if (sz == 0) break;               /* chunk final */

        memcpy(w, p, sz);
        w += sz;
        p += sz + 2;                      /* sarim si CRLF-ul de la final de chunk */
    }
    *w = '\0';
    return out;
}


int open_connection(const char *host_ip, int portno, int ip_type, int socket_type, int flag) {
    struct sockaddr_in serv_addr;
    struct hostent *server;
    int sockfd = socket(ip_type, socket_type, flag);
    if (sockfd < 0) {
        error("ERROR opening socket");
    }
    server = gethostbyname(host_ip);
    if (!server) {
        error("ERROR, no such host");
    }
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = ip_type;
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        error("ERROR connecting");
    }
    return sockfd;
}

void close_connection(int sockfd) {
    close(sockfd);
}

void send_to_server(int sockfd, const char *msg) {
    int total = strlen(msg);
    int sent = 0;
    while (sent < total) {
        int n = write(sockfd, msg + sent, total - sent);
        if (n < 0) {
            error("ERROR writing to socket");
        }
        sent += n;
    }
}

char *receive_from_server(int sockfd) {
    char *response = calloc(BUFLEN, sizeof(char));
    char buffer[BUFLEN];
    int ret;
    while ((ret = read(sockfd, buffer, BUFLEN - 1)) > 0) {
        buffer[ret] = '\0';
        response = realloc(response, strlen(response) + ret + 1);
        strcat(response, buffer);
        if (ret < BUFLEN - 1) {
            break;
        }
    }
    if (ret < 0) {
        error("ERROR reading from socket");
    }
    return response;
}

void error(const char *msg) {
    perror(msg);
    exit(1);
}

char *basic_extract_json_response(const char *response) {
    const char *ptr = strstr(response, "\r\n\r\n");
    if (!ptr) {
        return NULL;
    }
    ptr += 4;
    return strdup(ptr);
}
