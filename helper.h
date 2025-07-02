#ifndef HELPER_H
#define HELPER_H

#define LINELEN 100
#define BUFLEN 4096

#include <stddef.h>

char *unchunk(const char *chunked);
int open_connection(const char *host_ip, int portno, int ip_type, int socket_type, int flag);
void close_connection(int sockfd);
void send_to_server(int sockfd, const char *msg);
char *receive_from_server(int sockfd);
void error(const char *msg);
char *basic_extract_json_response(const char *response);

#endif
