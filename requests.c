/* requests.c */
#include "requests.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINELEN 100
#define BUFLEN 4096

static char *stringify_cookies(const char **cookies, int count) {
    if (count == 0) {
        return NULL;
    }
    size_t len = 0;
    for (int i = 0; i < count; i++) {
        len += strlen(cookies[i]) + 2;
    }
    char *result = calloc(len + 1, 1);
    for (int i = 0; i < count; i++) {
        strcat(result, cookies[i]);
        if (i < count - 1) {
            strcat(result, "; ");
        }
    }
    return result;
}

char *compute_post_request(const char *host, const char *url, const char *content_type, char **body_data, int body_data_fields_count,
                           const char **cookies, int cookies_count) {
    char *message = calloc(BUFLEN, sizeof(char));
    char *body = calloc(BUFLEN, sizeof(char));
    int i;
    for (i = 0; i < body_data_fields_count; ++i) {
        strcat(body, body_data[i]);
    }
    size_t content_length = strlen(body);

    sprintf(message,
        "POST %s HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %zu\r\n",
        url, host, content_type, content_length);

    char *cookie_header = stringify_cookies(cookies, cookies_count);
    if (cookie_header) {
        strcat(message, "Cookie: ");
        strcat(message, cookie_header);
        strcat(message, "\r\n");
        free(cookie_header);
    }
    strcat(message, "\r\n");
    strcat(message, body);

    free(body);
    return message;
}

char *compute_get_request(const char *host, const char *url, const char **cookies, int cookies_count) {
    char *message = calloc(BUFLEN, sizeof(char));
    sprintf(message,
        "GET %s HTTP/1.1\r\n"
        "Host: %s\r\n", url, host);

    char *cookie_header = stringify_cookies(cookies, cookies_count);
    if (cookie_header) {
        strcat(message, "Cookie: ");
        strcat(message, cookie_header);
        strcat(message, "\r\n");
        free(cookie_header);
    }
    strcat(message, "\r\n");
    return message;
}

char *compute_delete_request(const char *host, const char *url, const char **cookies, int cookies_count) {
    char *message = calloc(BUFLEN, sizeof(char));
    sprintf(message,
        "DELETE %s HTTP/1.1\r\n"
        "Host: %s\r\n", url, host);

    char *cookie_header = stringify_cookies(cookies, cookies_count);
    if (cookie_header) {
        strcat(message, "Cookie: ");
        strcat(message, cookie_header);
        strcat(message, "\r\n");
        free(cookie_header);
    }
    strcat(message, "\r\n");
    return message;
}

char *compute_put_request(const char *host, const char *url, const char *content_type, char **body_data, int body_data_fields_count,
                          const char **cookies, int cookies_count) {
    char *message = calloc(BUFLEN, sizeof(char));
    char *body = calloc(BUFLEN, sizeof(char));
    int i;
    for (i = 0; i < body_data_fields_count; i++) {
        strcat(body, body_data[i]);
    }
    size_t content_length = strlen(body);

    sprintf(message,
        "PUT %s HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %zu\r\n",
        url, host, content_type, content_length);

    char *cookie_header = stringify_cookies(cookies, cookies_count);
    if (cookie_header) {
        strcat(message, "Cookie: ");
        strcat(message, cookie_header);
        strcat(message, "\r\n");
        free(cookie_header);
    }
    strcat(message, "\r\n");
    strcat(message, body);

    free(body);
    return message;
}
