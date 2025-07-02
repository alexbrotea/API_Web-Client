#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "helper.h"
#include "requests.h"
#include "parson.h"

#define SERVER_IP "63.32.125.183"
#define SERVER_PORT 8081

static void login_admin(char **session_cookie) {
    char line[LINELEN], username[101], password[101];
    printf("username="); 
    fflush(stdout);
    if (!fgets(line, sizeof(line), stdin)) {
        return;
    }

    line[strcspn(line, "\n")] = '\0';
    strncpy(username, line, sizeof(username) - 1); username[sizeof(username) - 1] = '\0';
    printf("password=");
    fflush(stdout);
    if (!fgets(line, sizeof(line), stdin)) {
        return;
    }

    line[strcspn(line, "\n")] = '\0'; 
    strncpy(password, line, sizeof(password) - 1); 
    password[sizeof(password) - 1] = '\0';
    char *json_body = calloc(BUFLEN, 1);
    sprintf(json_body, "{\"username\":\"%s\",\"password\":\"%s\"}", username, password);
    char *body_data[1] = { json_body };
    int sockfd = open_connection(SERVER_IP, SERVER_PORT, AF_INET, SOCK_STREAM, 0);
    char *req = compute_post_request(SERVER_IP, "/api/v1/tema/admin/login", "application/json", body_data, 1, NULL, 0);
    send_to_server(sockfd, req);
    char *resp = receive_from_server(sockfd);
    int status = 0;
    sscanf(resp, "HTTP/1.1 %d", &status);
    if (status >= 200 && status < 300) {
        printf("SUCCESS: Admin autentificat cu succes\n");
        char *h = strstr(resp, "Set-Cookie:");
        if (h) {
            h += strlen("Set-Cookie: ");
            char *e = strstr(h, ";");
            size_t len = e ? (size_t)(e - h) : strlen(h);
            *session_cookie = strndup(h, len);
        }
    } else {
        char *err = basic_extract_json_response(resp);
        if (err) {
            printf("ERROR: %s\n", err);
        } else {
            printf("ERROR: Autentificare esuata\n");
        }

        if (err) {
            free(err);
        }
    }
    free(json_body);
    free(req);
    free(resp);
    close_connection(sockfd);
}

static void add_user(const char *session_cookie) {
    char line[LINELEN], username[101], password[101];

    printf("username=");
    fflush(stdout);
    if (!fgets(line, sizeof(line), stdin)) {
        return;
    }
    line[strcspn(line, "\n")] = '\0';
    strncpy(username, line, sizeof(username) - 1);
    username[sizeof(username) - 1] = '\0';

    printf("password=");
    fflush(stdout);
    if (!fgets(line, sizeof(line), stdin)) {
        return;
    }
    line[strcspn(line, "\n")] = '\0';
    strncpy(password, line, sizeof(password) - 1);
    password[sizeof(password) - 1] = '\0';

    char *json_body = calloc(BUFLEN, 1);
    sprintf(json_body, "{\"username\":\"%s\",\"password\":\"%s\"}", username, password);

    char *body_data[1] = { json_body };
    const char *cookies[1] = { session_cookie };

    int sockfd = open_connection(SERVER_IP, SERVER_PORT, AF_INET, SOCK_STREAM, 0);
    char *req = compute_post_request(SERVER_IP, "/api/v1/tema/admin/users", "application/json", body_data, 1, cookies, 1);

    send_to_server(sockfd, req);
    char *resp = receive_from_server(sockfd);

    int status = 0;
    sscanf(resp, "HTTP/1.1 %d", &status);

    if (status >= 200 && status < 300) {
        printf("SUCCESS: Utilizator adaugat cu succes\n");
    } else {
        char *err = basic_extract_json_response(resp);
        if (err) {
            printf("ERROR: %s\n", err);
        } else {
            printf("ERROR: Adaugare esuata\n");
        }

        if (err) {
            free(err);
        }
    }

    free(json_body);
    free(req);
    free(resp);
    close_connection(sockfd);
}

static void get_users(const char *session_cookie) {
    const char *cookies[1] = { session_cookie };

    int sockfd = open_connection(SERVER_IP, SERVER_PORT, AF_INET, SOCK_STREAM, 0);
    char *req = compute_get_request(SERVER_IP, "/api/v1/tema/admin/users", cookies, 1);

    send_to_server(sockfd, req);
    char *resp = receive_from_server(sockfd);

    int status = 0;
    sscanf(resp, "HTTP/1.1 %d", &status);

    if (status >= 200 && status < 300) {
        printf("SUCCESS: Lista utilizatorilor\n");

        char *body = basic_extract_json_response(resp);
        JSON_Value *root = json_parse_string(body);
        JSON_Array *arr = json_object_get_array(json_value_get_object(root), "users");

        for (size_t i = 0; i < json_array_get_count(arr); i++) {
            JSON_Object *u = json_array_get_object(arr, i);
            printf("#%d %s:%s\n",
                   (int)json_object_get_number(u, "id"),
                   json_object_get_string(u, "username"),
                   json_object_get_string(u, "password"));
        }

        json_value_free(root);
        free(body);
    } else {
        printf("ERROR: Lipsa permisiuni admin\n");
    }

    free(req);
    free(resp);
    close_connection(sockfd);
}

static void delete_user(const char *session_cookie) {
    char line[LINELEN], username[101], path[BUFLEN];

    printf("username=");
    fflush(stdout);
    if (!fgets(line, sizeof(line), stdin)) {
        return;
    }

    line[strcspn(line, "\n")] = '\0';
    strncpy(username, line, sizeof(username) - 1);
    username[sizeof(username) - 1] = '\0';

    sprintf(path, "/api/v1/tema/admin/users/%s", username);

    const char *cookies[1] = { session_cookie };

    int sockfd = open_connection(SERVER_IP, SERVER_PORT, AF_INET, SOCK_STREAM, 0);
    char *req = compute_delete_request(SERVER_IP, path, cookies, 1);

    send_to_server(sockfd, req);
    char *resp = receive_from_server(sockfd);

    int status = 0;
    sscanf(resp, "HTTP/1.1 %d", &status);

    if (status >= 200 && status < 300) {
        printf("SUCCESS: Utilizator sters\n");
    } else {
        char *err = basic_extract_json_response(resp);
        if (err) {
            printf("ERROR: %s\n", err);
        } else {
            printf("ERROR: Username invalid\n");
        }   

        if (err) {
            free(err);
        }
    }

    free(req);
    free(resp);
    close_connection(sockfd);
}

static void logout_admin(char **session_cookie) {
    if (!*session_cookie) {
        printf("ERROR: Neautentificat\n");
        return;
    }

    const char *cookies[1] = { *session_cookie };

    int sockfd = open_connection(SERVER_IP, SERVER_PORT, AF_INET, SOCK_STREAM, 0);
    char *req = compute_get_request(SERVER_IP, "/api/v1/tema/admin/logout", cookies, 1);

    send_to_server(sockfd, req);
    char *resp = receive_from_server(sockfd);

    int status = 0;
    sscanf(resp, "HTTP/1.1 %d", &status);

    if (status >= 200 && status < 300) {
        printf("SUCCESS: Admin delogat\n");
        free(*session_cookie);
        *session_cookie = NULL;
    } else {
        printf("ERROR: Neautentificat\n");
    }

    free(req);
    free(resp);
    close_connection(sockfd);
}

static void login(char **user_cookie) {
    char line[LINELEN], admin[101], user[101], pass[101];
    printf("admin_username=");
    fflush(stdout);
    if (!fgets(line, sizeof(line), stdin)) {
        return;
    }

    line[strcspn(line, "\n")] = '\0'; 
    strncpy(admin, line, sizeof(admin) - 1); admin[sizeof(admin) - 1] = '\0';
    printf("username="); {
        fflush(stdout);
    }

    if (!fgets(line, sizeof(line), stdin)) {
        return;
    }
    line[strcspn(line, "\n")] = '\0';
    strncpy(user, line, sizeof(user) - 1); user[sizeof(user) - 1] = '\0';
    printf("password=");
    fflush(stdout);
    if (!fgets(line, sizeof(line), stdin)){
        return;
    }

    line[strcspn(line, "\n")] = '\0';
    strncpy(pass, line, sizeof(pass) - 1);
    pass[sizeof(pass) - 1] = '\0';

    char *json_body = calloc(BUFLEN, 1);
    sprintf(json_body, "{\"admin_username\":\"%s\",\"username\":\"%s\",\"password\":\"%s\"}", admin, user, pass);
    char *body_data[1] = { json_body };
    int sockfd = open_connection(SERVER_IP, SERVER_PORT, AF_INET, SOCK_STREAM, 0);
    char *req = compute_post_request(SERVER_IP, "/api/v1/tema/user/login", "application/json", body_data, 1, NULL, 0);
    send_to_server(sockfd, req);
    char *resp = receive_from_server(sockfd);
    int status = 0; sscanf(resp, "HTTP/1.1 %d", &status);
    if (status >= 200 && status < 300) {
        printf("SUCCESS: Autentificare reusita\n");
        char *h = strstr(resp, "Set-Cookie:");
        if (h) {
            h += strlen("Set-Cookie: ");
            char *e = strstr(h, ";");
            size_t len;
            if (e) {
                len = (size_t)(e - h);
            } else {
                len = strlen(h);
            }
            *user_cookie = strndup(h, len);
        }
    } else {
        char *err = basic_extract_json_response(resp);
        if (err) {
            printf("ERROR: %s\n", err);
        } else {
            printf("ERROR: Autentificare esuata\n");
        }

        if (err) {
            free(err);
        }
    }
    free(json_body);
    free(req);
    free(resp);
    close_connection(sockfd);
}

static void get_access(const char *user_cookie, char **jwt_token) {
    const char *cookies[1] = { user_cookie };

    int sockfd = open_connection(SERVER_IP, SERVER_PORT, AF_INET, SOCK_STREAM, 0);
    char *req = compute_get_request(SERVER_IP, "/api/v1/tema/library/access", cookies, 1);

    send_to_server(sockfd, req);
    char *resp = receive_from_server(sockfd);

    int status = 0;
    sscanf(resp, "HTTP/1.1 %d", &status);

    if (status >= 200 && status < 300) {
        printf("SUCCESS: Token JWT primit\n");

        char *body = basic_extract_json_response(resp);
        JSON_Value *root = json_parse_string(body);
        const char *token = json_object_get_string(json_value_get_object(root), "token");

        *jwt_token = strdup(token);

        json_value_free(root);
        free(body);
    } else {
        printf("ERROR: Neautentificat\n");
    }

    free(req);
    free(resp);
    close_connection(sockfd);
}

static void get_movies(const char *jwt_token) {
    if (!jwt_token) {
        printf("ERROR: Fara acces library\n");
        return;
    }

    int sockfd = open_connection(SERVER_IP, SERVER_PORT, AF_INET, SOCK_STREAM, 0);

    char *req = calloc(BUFLEN, 1);
    sprintf(req,
        "GET /api/v1/tema/library/movies HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Authorization: Bearer %s\r\n"
        "\r\n",
        SERVER_IP, jwt_token);

    send_to_server(sockfd, req);
    char *resp = receive_from_server(sockfd);

    int status = 0;
    sscanf(resp, "HTTP/1.1 %d", &status);

    if (status >= 200 && status < 300) {
        char *body = basic_extract_json_response(resp);
        JSON_Value *val = json_parse_string(body);
        JSON_Array *arr = NULL;

        if (json_value_get_type(val) == JSONArray) {
            arr = json_value_get_array(val);
        } else if (json_value_get_type(val) == JSONObject) {
            JSON_Object *obj = json_value_get_object(val);
            JSON_Value *aux = json_object_get_value(obj, "movies");
            if (aux && json_value_get_type(aux) == JSONArray)
                arr = json_value_get_array(aux);
        }

        if (!arr) {
            printf("ERROR: Raspuns JSON invalid\n");
        } else {
            size_t n = json_array_get_count(arr);
            printf("SUCCESS: Lista filmelor\n");
            for (size_t i = 0; i < n; i++) {
                JSON_Object *movie = json_array_get_object(arr, i);
                int id = (int)json_object_get_number(movie, "id");
                const char *title = json_object_get_string(movie, "title");
                printf("#%d %s\n", id, title);
            }
        }

        json_value_free(val);
        free(body);
    } else {
        printf("ERROR: Fara acces library\n");
    }

    free(req);
    free(resp);
    close_connection(sockfd);
}

static void get_movie(const char *jwt_token) {
    if (!jwt_token) {
        printf("ERROR: Fara acces library\n");
        return;
    }

    printf("id=");
    fflush(stdout);
    char line[LINELEN];
    if (!fgets(line, sizeof line, stdin)) {
        printf("ERROR: id lipsa\n");
        return;
    }

    int id;
    if (sscanf(line, "%d", &id) != 1 || id < 0) {
        printf("ERROR: id invalid\n");
        return;
    }

    int sockfd = open_connection(SERVER_IP, SERVER_PORT, AF_INET, SOCK_STREAM, 0);
    char *req = calloc(BUFLEN, 1);
    sprintf(req,
        "GET /api/v1/tema/library/movies/%d HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Authorization: Bearer %s\r\n"
        "\r\n",
        id, SERVER_IP, jwt_token);
    send_to_server(sockfd, req);

    char *resp = receive_from_server(sockfd);
    int status = 0;
    sscanf(resp, "HTTP/1.1 %d", &status);

    if (status >= 200 && status < 300) {
        char *body = basic_extract_json_response(resp);
        JSON_Value *root = json_parse_string(body);

        if (json_value_get_type(root) != JSONObject) {
            printf("ERROR: Raspuns JSON invalid\n");
        } else {
            JSON_Object *movie = json_value_get_object(root);
            printf("SUCCESS: Detalii film\n");
            printf("title: %s\n", json_object_get_string(movie, "title"));
            printf("year: %d\n", (int)json_object_get_number(movie, "year"));
            printf("description: %s\n", json_object_get_string(movie, "description"));

            double rating = 0.0;
            if (json_object_has_value_of_type(movie, "rating", JSONNumber)) {
                rating = json_object_get_number(movie, "rating");
            } else if (json_object_has_value_of_type(movie, "rating", JSONString)) {
                const char *r = json_object_get_string(movie, "rating");
                if (r) {
                    rating = atof(r);
                }
            }
            printf("rating: %.1f\n", rating);
            json_value_free(root);
        }
        free(body);
    } else if (status == 404) {
        printf("ERROR: Filmul cu id=%d nu exista\n", id);
    } else if (status == 403) {
        printf("ERROR: Fara acces library\n");
    } else {
        printf("ERROR: Cerere esuata (cod %d)\n", status);
    }

    free(req);
    free(resp);
    close_connection(sockfd);
}

static void add_movie(const char *jwt_token) {
    if (!jwt_token) {
        printf("ERROR: Fara acces library\n");
        return;
    }

    char line[LINELEN];
    char title[BUFLEN / 2], description[BUFLEN / 2];
    int year;
    double rating;

    printf("title=");
    fflush(stdout);
    if (!fgets(line, sizeof(line), stdin)) {
        return;
    }
    line[strcspn(line, "\n")] = '\0';
    strncpy(title, line, sizeof(title) - 1);
    title[sizeof(title) - 1] = '\0';

    printf("year=");
    fflush(stdout);
    if (!fgets(line, sizeof(line), stdin)) {
        return;
    }
    line[strcspn(line, "\n")] = '\0';
    year = atoi(line);

    printf("description=");
    fflush(stdout);
    if (!fgets(line, sizeof(line), stdin)) {
        return;
    }
    line[strcspn(line, "\n")] = '\0';
    strncpy(description, line, sizeof(description) - 1);
    description[sizeof(description) - 1] = '\0';

    printf("rating=");
    fflush(stdout);
    if (!fgets(line, sizeof(line), stdin)) {
        return;
    }
    line[strcspn(line, "\n")] = '\0';
    rating = atof(line);

    char *json_body = calloc(BUFLEN, 1);
    sprintf(json_body,
        "{\"title\":\"%s\",\"year\":%d,\"description\":\"%s\",\"rating\":%.1f}",
        title, year, description, rating);

    size_t len_body = strlen(json_body);
    char *message = calloc(BUFLEN + len_body, 1);
    sprintf(message,
        "POST /api/v1/tema/library/movies HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Authorization: Bearer %s\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %zu\r\n"
        "\r\n"
        "%s",
        SERVER_IP, jwt_token, len_body, json_body);

    int sockfd = open_connection(SERVER_IP, SERVER_PORT, AF_INET, SOCK_STREAM, 0);
    send_to_server(sockfd, message);

    char *response = receive_from_server(sockfd);
    int status = 0;
    sscanf(response, "HTTP/1.1 %d", &status);

    if (status >= 200 && status < 300) {
        printf("SUCCESS: Film adaugat\n");
    } else {
        char *err = basic_extract_json_response(response);
        printf("ERROR: %s\n", err ? err : "Adaugare film esuata");
        if (err) {
            free(err);
        }
    }

    free(json_body);
    free(message);
    free(response);
    close_connection(sockfd);
}

static void delete_movie(const char *jwt_token) {
    if (!jwt_token) {
        printf("ERROR: Fara acces library\n");
        return;
    }

    printf("id=");
    fflush(stdout);

    char line[LINELEN];
    if (!fgets(line, sizeof(line), stdin)) {
        printf("ERROR: id lipsa\n");
        return;
    }
    line[strcspn(line, "\r\n")] = '\0';

    int id = -1;
    if (sscanf(line, "%d", &id) != 1 || id < 0) {
        printf("ERROR: id invalid\n");
        return;
    }

    int sockfd = open_connection(SERVER_IP, SERVER_PORT, AF_INET, SOCK_STREAM, 0);
    char *req = calloc(BUFLEN, 1);
    sprintf(req,
        "DELETE /api/v1/tema/library/movies/%d HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Authorization: Bearer %s\r\n"
        "\r\n",
        id, SERVER_IP, jwt_token);

    send_to_server(sockfd, req);
    char *resp = receive_from_server(sockfd);

    int status = 0;
    sscanf(resp, "HTTP/1.1 %d", &status);

    if (status >= 200 && status < 300) {
        printf("SUCCESS: Film sters cu succes\n");
    } else if (status == 404) {
        printf("ERROR: Filmul cu id=%d nu exista\n", id);
    } else if (status == 403) {
        printf("ERROR: Fara acces library\n");
    } else {
        printf("ERROR: Cerere esuata (cod %d)\n", status);
    }

    free(req);
    free(resp);
    close_connection(sockfd);
}

static void update_movie(const char *jwt_token) {
    if (!jwt_token) {
        printf("ERROR: Fara acces library\n");
        return;
    }

    printf("id="); fflush(stdout);
    char line[LINELEN];
    if (!fgets(line, sizeof line, stdin)) {
        printf("ERROR: id lipsa\n");
        return;
    }
    line[strcspn(line, "\r\n")] = '\0';
    int id = -1;
    if (sscanf(line, "%d", &id) != 1 || id < 0) {
        printf("ERROR: id invalid\n");
        return;
    }

    char title[LINELEN] = {0};
    char description[LINELEN] = {0};
    int year_set = 0;
    int year_val = 0;
    int rating_set = 0;
    double rating_val = 0.0;

    printf("title=");
    fflush(stdout);
    if (!fgets(title, sizeof title, stdin)) {
        printf("ERROR: camp lipsa\n");
        return;
    }
    title[strcspn(title, "\r\n")] = '\0';

    printf("year="); fflush(stdout);
    if (!fgets(line, sizeof line, stdin)) {
        printf("ERROR: camp lipsa\n");
        return;
    }
    line[strcspn(line, "\r\n")] = '\0';
    if (sscanf(line, "%d", &year_val) == 1 && year_val > 1800) {
        year_set = 1;
    }

    printf("description="); fflush(stdout);
    if (!fgets(description, sizeof description, stdin)) {
        printf("ERROR: camp lipsa\n");
        return;
    }
    description[strcspn(description, "\r\n")] = '\0';

    printf("rating="); fflush(stdout);
    if (!fgets(line, sizeof line, stdin)) {
        printf("ERROR: camp lipsa\n");
        return;
    }
    line[strcspn(line, "\r\n")] = '\0';
    if ((sscanf(line, "%lf", &rating_val) == 1) &&
        (rating_val >= 0.0 && rating_val <= 10.0)) {
        rating_set = 1;
    }
    if (!rating_set) {
        printf("ERROR: rating invalid\n");
        return;
    }

    JSON_Value *root = json_value_init_object();
    JSON_Object *obj = json_value_get_object(root);

    if (strlen(title) > 0) {
        json_object_set_string(obj, "title", title);
    }
    if (strlen(description) > 0) {
        json_object_set_string(obj, "description", description);
    }
    if (year_set) {
        json_object_set_number(obj, "year", year_val);
    }

    json_object_set_number(obj, "rating", rating_val);
    if (json_object_get_count(obj) == 0) {
        printf("ERROR: Nu a fost introdus niciun camp\n");
        json_value_free(root);
        return;
    }

    char *payload = json_serialize_to_string(root);
    size_t plen = strlen(payload);
    char *request = calloc(plen + 512, 1);

    sprintf(request,
        "PUT /api/v1/tema/library/movies/%d HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Authorization: Bearer %s\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %zu\r\n"
        "\r\n"
        "%s",
        id, SERVER_IP, jwt_token, plen, payload);

    int sockfd = open_connection(SERVER_IP, SERVER_PORT, AF_INET, SOCK_STREAM, 0);
    send_to_server(sockfd, request);

    char *resp = receive_from_server(sockfd);
    int status = 0;
    sscanf(resp, "HTTP/1.1 %d", &status);
    if (status >= 200 && status < 300) {
        printf("SUCCESS: Film actualizat\n");
    } else if (status == 404) {
        printf("ERROR: Filmul cu id=%d nu exista\n", id);
    } else if (status == 403) {
        printf("ERROR: Fara acces library\n");
    } else {
        printf("ERROR: Cerere esuata (cod %d)\n", status);
    }

    json_free_serialized_string(payload);
    json_value_free(root);
    free(request);
    free(resp);
    close_connection(sockfd);
}

static void get_collections(const char *jwt_token) {
    if (!jwt_token) {
        printf("ERROR: Fara acces library\n");
        return;
    }

    int sockfd = open_connection(SERVER_IP, SERVER_PORT, AF_INET, SOCK_STREAM, 0);

    char *req = calloc(BUFLEN, 1);
    sprintf(req,
        "GET /api/v1/tema/library/collections HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Authorization: Bearer %s\r\n"
        "\r\n",
        SERVER_IP, jwt_token);

    send_to_server(sockfd, req);
    char *resp = receive_from_server(sockfd);

    int status = 0;
    sscanf(resp, "HTTP/1.1 %d", &status);

    if (status >= 200 && status < 300) {
        printf("SUCCESS: Lista colectiilor\n");

        char *body = basic_extract_json_response(resp);
        JSON_Value *root = json_parse_string(body);
        JSON_Array *arr = NULL;

        if (json_value_get_type(root) == JSONArray) {
            arr = json_value_get_array(root);
        } else {
            JSON_Object *obj = json_value_get_object(root);
            JSON_Value *tmp = json_object_get_value(obj, "collections");
            if (tmp && json_value_get_type(tmp) == JSONArray) {
                arr = json_value_get_array(tmp);
            }
        }

        if (arr) {
            size_t n = json_array_get_count(arr);
            for (size_t i = 0; i < n; i++) {
                JSON_Object *col = json_array_get_object(arr, i);
                int id = (int)json_object_get_number(col, "id");
                const char *name = json_object_get_string(col, "name");
                printf("#%d: %s\n", id, name);
            }
        } else {
            printf("ERROR: Raspuns JSON invalid\n");
        }

        json_value_free(root);
        free(body);

    } else if (status == 403) {
        printf("ERROR: Fara acces library\n");
    } else {
        printf("ERROR: Cerere esuata (cod %d)\n", status);
    }

    free(req);
    free(resp);
    close_connection(sockfd);
}

static void get_collection(const char *jwt_token) {
    if (!jwt_token) {
        printf("ERROR: Fara acces library\n");
        return;
    }

    printf("id=");
    fflush(stdout);
    char line[LINELEN];
    if (!fgets(line, sizeof(line), stdin)) {
        printf("ERROR: id lipsa\n");
        return;
    }
    line[strcspn(line, "\r\n")] = '\0';

    int id = -1;
    if (sscanf(line, "%d", &id) != 1 || id < 0) {
        printf("ERROR: id invalid\n");
        return;
    }

    int sockfd = open_connection(SERVER_IP, SERVER_PORT, AF_INET, SOCK_STREAM, 0);
    char *req = calloc(BUFLEN, 1);
    sprintf(req,
        "GET /api/v1/tema/library/collections/%d HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Authorization: Bearer %s\r\n"
        "\r\n",
        id, SERVER_IP, jwt_token);
    send_to_server(sockfd, req);

    char *resp = receive_from_server(sockfd);
    int status = 0;
    sscanf(resp, "HTTP/1.1 %d", &status);

    if (status >= 200 && status < 300) {
        char *body = basic_extract_json_response(resp);
        JSON_Value *val = json_parse_string(body);
        JSON_Object *root_obj = json_value_get_object(val);

        JSON_Object *col = root_obj;
        if (json_object_has_value(root_obj, "collection")) {
            col = json_object_get_object(root_obj, "collection");
        }

        const char *title = json_object_get_string(col, "title");
        const char *owner = json_object_get_string(col, "owner");
        JSON_Array *movies = json_object_get_array(col, "movies");
        size_t n;
        if (movies) {
            n = json_array_get_count(movies);
        } else {
            n = 0;
        }

        printf("SUCCESS: Detalii colectie\n");
        printf("title: %s\n", title);
        printf("owner: %s\n", owner);
        for (size_t i = 0; i < n; i++) {
            JSON_Object *m = json_array_get_object(movies, i);
            int mid = (int)json_object_get_number(m, "id");
            const char *mt = json_object_get_string(m, "title");
            printf("#%d: %s\n", mid, mt);
        }

        json_value_free(val);
        free(body);
    } else if (status == 403) {
        printf("ERROR: Fara acces library\n");
    } else if (status == 404) {
        printf("ERROR: Colectia cu id=%d nu exista\n", id);
    } else {
        printf("ERROR: Cerere esuata (cod %d)\n", status);
    }

    free(req);
    free(resp);
    close_connection(sockfd);
}

static void add_collection(const char *jwt_token) {
    if (!jwt_token) {
        printf("ERROR: Fara acces library\n");
        return;
    }

    char line[LINELEN];
    printf("title=");
    fflush(stdout);
    if (!fgets(line, sizeof(line), stdin)) {
        printf("ERROR: fara title\n");
        return;
    }
    line[strcspn(line, "\r\n")] = '\0';
    char title[LINELEN];
    strcpy(title, line);

    printf("num_movies=");
    fflush(stdout);
    if (!fgets(line, sizeof(line), stdin)) {
        printf("ERROR: num_movies lipsa\n");
        return;
    }
    int num = 0;
    if (sscanf(line, "%d", &num) != 1 || num < 0) {
        printf("ERROR: num_movies invalid\n");
        return;
    }

    int *movie_ids = malloc(sizeof(int) * num);
    for (int i = 0; i < num; i++) {
        printf("movie_id[%d]=", i);
        fflush(stdout);
        if (!fgets(line, sizeof(line), stdin)) {
            printf("ERROR: movie_id lipsa\n");
            free(movie_ids);
            return;
        }
        if (sscanf(line, "%d", &movie_ids[i]) != 1 || movie_ids[i] < 0) {
            printf("ERROR: movie_id invalid\n");
            free(movie_ids);
            return;
        }
    }

    JSON_Value *val = json_value_init_object();
    JSON_Object *obj = json_value_get_object(val);
    json_object_set_string(obj, "title", title);
    char *body_str = json_serialize_to_string(val);
    size_t body_len = strlen(body_str);

    int sockfd = open_connection(SERVER_IP, SERVER_PORT, AF_INET, SOCK_STREAM, 0);
    size_t req_len = snprintf(NULL, 0,
        "POST /api/v1/tema/library/collections HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Content-Type: application/json\r\n"
        "Authorization: Bearer %s\r\n"
        "Content-Length: %zu\r\n"
        "\r\n"
        "%s",
        SERVER_IP, jwt_token, body_len, body_str);
    char *req = malloc(req_len + 1);
    sprintf(req,
        "POST /api/v1/tema/library/collections HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Content-Type: application/json\r\n"
        "Authorization: Bearer %s\r\n"
        "Content-Length: %zu\r\n"
        "\r\n"
        "%s",
        SERVER_IP, jwt_token, body_len, body_str);
    send_to_server(sockfd, req);
    char *resp = receive_from_server(sockfd);

    int status = 0;
    sscanf(resp, "HTTP/1.1 %d", &status);
    if (status < 200 || status >= 300) {
        printf("ERROR: Creare colectie esuata (cod %d)\n", status);
        free(movie_ids);
        free(body_str);
        json_value_free(val);
        free(req);
        free(resp);
        close_connection(sockfd);
    }

    char *resp_body = basic_extract_json_response(resp);
    JSON_Value *rval = json_parse_string(resp_body);
    JSON_Object *robj = json_value_get_object(rval);
    int col_id = (int)json_object_get_number(robj, "id");
    json_value_free(rval);
    free(resp_body);

    for (int i = 0; i < num; i++) {
        JSON_Value  *mv_val = json_value_init_object();
        JSON_Object *mv_obj = json_value_get_object(mv_val);
        json_object_set_number(mv_obj, "id", movie_ids[i]);
        char *mv_str = json_serialize_to_string(mv_val);
        size_t mv_len = strlen(mv_str);

        size_t mreq_len = snprintf(NULL, 0,
            "POST /api/v1/tema/library/collections/%d/movies HTTP/1.1\r\n"
            "Host: %s\r\n"
            "Content-Type: application/json\r\n"
            "Authorization: Bearer %s\r\n"
            "Content-Length: %zu\r\n"
            "\r\n"
            "%s",
            col_id, SERVER_IP, jwt_token, mv_len, mv_str);
        char *mreq = malloc(mreq_len + 1);
        sprintf(mreq,
            "POST /api/v1/tema/library/collections/%d/movies HTTP/1.1\r\n"
            "Host: %s\r\n"
            "Content-Type: application/json\r\n"
            "Authorization: Bearer %s\r\n"
            "Content-Length: %zu\r\n"
            "\r\n"
            "%s",
            col_id, SERVER_IP, jwt_token, mv_len, mv_str);
        send_to_server(sockfd, mreq);
        char *mresp = receive_from_server(sockfd);

        int mstatus = 0; sscanf(mresp, "HTTP/1.1 %d", &mstatus);
        if (mstatus < 200 || mstatus >= 300) {
            printf("ERROR: Adaugare film %d esuata (cod %d)\n",
                   movie_ids[i], mstatus);
            free(mreq); free(mresp);
            json_free_serialized_string(mv_str);
            json_value_free(mv_val);
            free(movie_ids);
            free(body_str);
            json_value_free(val);
            free(req);
            free(resp);
            close_connection(sockfd);
        }

        free(mreq);
        free(mresp);
        json_free_serialized_string(mv_str);
        json_value_free(mv_val);
    }

    printf("SUCCESS: Colectie adaugata\n");
}

static void delete_collection(const char *jwt_token) {
    if (!jwt_token) {
        printf("ERROR: Fara acces library\n");
        return;
    }

    printf("id=");
    fflush(stdout);
    char line[LINELEN];
    if (!fgets(line, sizeof(line), stdin)) {
        printf("ERROR: id lipsa\n");
        return;
    }
    line[strcspn(line, "\r\n")] = '\0';

    int id = -1;
    if (sscanf(line, "%d", &id) != 1 || id < 0) {
        printf("ERROR: id invalid\n");
        return;
    }

    int sockfd = open_connection(SERVER_IP, SERVER_PORT, AF_INET, SOCK_STREAM, 0);
    char *req = calloc(BUFLEN, 1);
    sprintf(req,
        "DELETE /api/v1/tema/library/collections/%d HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Authorization: Bearer %s\r\n"
        "\r\n",
        id, SERVER_IP, jwt_token);
    send_to_server(sockfd, req);

    char *resp = receive_from_server(sockfd);
    int status = 0;
    sscanf(resp, "HTTP/1.1 %d", &status);

    if (status >= 200 && status < 300) {
        printf("SUCCESS: Colectie stearsa\n");
    } else if (status == 403) {
        printf("ERROR: Fara acces library\n");
    } else if (status == 404) {
        printf("ERROR: Colectia cu id=%d nu exista\n", id);
    } else {
        printf("ERROR: Cerere esuata (cod %d)\n", status);
    }

    free(req);
    free(resp);
    close_connection(sockfd);
}

static void add_movie_to_collection(const char *jwt_token) {
    if (!jwt_token) {
        printf("ERROR: Fara acces library\n");
        return;
    }

    char line[LINELEN];
    int col_id, movie_id;

    printf("collection_id="); fflush(stdout);
    if (!fgets(line, sizeof(line), stdin)) {
        printf("ERROR: id lipsa\n");
        return;
    }
    if (sscanf(line, "%d", &col_id) != 1 || col_id < 0) {
        printf("ERROR: id invalid\n");
        return;
    }

    printf("movie_id="); fflush(stdout);
    if (!fgets(line, sizeof(line), stdin)) {
        printf("ERROR: id lipsa\n");
        return;
    }
    if (sscanf(line, "%d", &movie_id) != 1 || movie_id < 0) {
        printf("ERROR: id invalid\n");
        return;
    }

    JSON_Value *val = json_value_init_object();
    JSON_Object *obj = json_value_get_object(val);
    json_object_set_number(obj, "id", movie_id);
    char *json_str = json_serialize_to_string(val);
    size_t json_len = strlen(json_str);

    const char *fmt =
        "POST /api/v1/tema/library/collections/%d/movies HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Content-Type: application/json\r\n"
        "Authorization: Bearer %s\r\n"
        "Content-Length: %zu\r\n"
        "\r\n"
        "%s";

    size_t req_len = snprintf(NULL, 0, fmt, col_id, SERVER_IP, jwt_token, json_len, json_str);
    char *req = malloc(req_len + 1);
    sprintf(req, fmt, col_id, SERVER_IP, jwt_token, json_len, json_str);

    int sockfd = open_connection(SERVER_IP, SERVER_PORT, AF_INET, SOCK_STREAM, 0);
    send_to_server(sockfd, req);
    char *resp = receive_from_server(sockfd);

    int status = 0;
    sscanf(resp, "HTTP/1.1 %d", &status);

    if (status >= 200 && status < 300) {
        printf("SUCCESS: Film adaugat in colectie\n");
    } else if (status == 403) {
        printf("ERROR: Fara acces library\n");
    } else if (status == 404) {
        printf("ERROR: Id invalid\n");
    } else {
        printf("ERROR: Cerere esuata (cod %d)\n", status);
    }

    free(req);
    free(resp);
    json_free_serialized_string(json_str);
    json_value_free(val);
    close_connection(sockfd);
}

static void delete_movie_from_collection(const char *jwt_token) {
    if (!jwt_token) {
        printf("ERROR: Fara acces library\n");
        return;
    }

    char line[LINELEN];
    int col_id, movie_id;

    printf("collection_id="); fflush(stdout);
    if (!fgets(line, sizeof(line), stdin)) {
        printf("ERROR: collection_id lipsa\n");
        return;
    }
    if (sscanf(line, "%d", &col_id) != 1 || col_id < 0) {
        printf("ERROR: collection_id invalid\n");
        return;
    }

    printf("movie_id="); fflush(stdout);
    if (!fgets(line, sizeof(line), stdin)) {
        printf("ERROR: movie_id lipsa\n");
        return;
    }
    if (sscanf(line, "%d", &movie_id) != 1 || movie_id < 0) {
        printf("ERROR: movie_id invalid\n");
        return;
    }

    int sockfd = open_connection(SERVER_IP, SERVER_PORT, AF_INET, SOCK_STREAM, 0);
    size_t req_cap = BUFLEN + 50;
    char *req = calloc(req_cap, 1);
    snprintf(req, req_cap,
        "DELETE /api/v1/tema/library/collections/%d/movies/%d HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Authorization: Bearer %s\r\n"
        "\r\n",
        col_id, movie_id, SERVER_IP, jwt_token);
    send_to_server(sockfd, req);

    char *resp = receive_from_server(sockfd);
    int status = 0;
    sscanf(resp, "HTTP/1.1 %d", &status);

    if (status >= 200 && status < 300) {
        printf("SUCCESS: Film sters din colectie\n");
    } else if (status == 403) {
        printf("ERROR: Fara acces library\n");
    } else if (status == 404) {
        printf("ERROR: Id invalid\n");
    } else {
        printf("ERROR: Cerere esuata (cod %d)\n", status);
    }

    free(req);
    free(resp);
    close_connection(sockfd);
}

static void logout(char **user_cookie) {
    if(!*user_cookie) {
        printf("ERROR: Neautentificat\n");
        return;
    }
    const char *cookies[1] = {*user_cookie};
    int sockfd = open_connection(SERVER_IP, SERVER_PORT, AF_INET, SOCK_STREAM, 0);
    char *req = compute_get_request(SERVER_IP, "/api/v1/tema/user/logout", cookies, 1);
    send_to_server(sockfd, req);
    char *resp = receive_from_server(sockfd);
    int status = 0; sscanf(resp, "HTTP/1.1 %d", &status);
    if (status >= 200 && status < 300) {
        printf("SUCCESS: Utilizator delogat\n");
        free(*user_cookie);
        *user_cookie = NULL;
    } else {
        printf("ERROR: Neautentificat\n");
    }
    free(req);
    free(resp);
    close_connection(sockfd);
}

int main(void) {
    char cmd[LINELEN];
    char *admin_cookie = 0;
    char *user_cookie = 0;
    char *jwt_token = 0;

    while (fgets(cmd, sizeof(cmd), stdin)) {
        cmd[strcspn(cmd, "\n")] = '\0';

        if (strcmp(cmd, "login_admin") == 0) {
            login_admin(&admin_cookie);
        } else if (strcmp(cmd, "add_user") == 0) {
            if (admin_cookie) add_user(admin_cookie);
            else printf("ERROR: Lipsa rol admin\n");
        } else if (strcmp(cmd, "get_users") == 0) {
            if (admin_cookie) get_users(admin_cookie);
            else printf("ERROR: Lipsa permisiuni admin\n");
        } else if (strcmp(cmd, "delete_user") == 0) {
            if (admin_cookie) delete_user(admin_cookie);
            else printf("ERROR: Lipsa rol admin\n");
        } else if (strcmp(cmd, "logout_admin") == 0) {
            logout_admin(&admin_cookie);
        } else if (strcmp(cmd, "login") == 0) {
            login(&user_cookie);
        } else if (strcmp(cmd, "get_access") == 0) {
            if (user_cookie) get_access(user_cookie, &jwt_token);
            else printf("ERROR: Neautentificat\n");
        } else if (strcmp(cmd, "get_movies") == 0) {
            get_movies(jwt_token);
        } else if (strcmp(cmd, "get_movie") == 0) {
            get_movie(jwt_token);
        } else if (strcmp(cmd, "add_movie") == 0) {
            add_movie(jwt_token);
        } else if (strcmp(cmd, "update_movie") == 0) {
            update_movie(jwt_token);
        } else if (strcmp(cmd, "delete_movie") == 0) {
            delete_movie(jwt_token);
        } else if (strcmp(cmd, "get_collections") == 0) {
            get_collections(jwt_token);
        } else if (strcmp(cmd, "get_collection") == 0) {
            get_collection(jwt_token);
        } else if (strcmp(cmd, "add_collection") == 0) {
            add_collection(jwt_token);
        } else if (strcmp(cmd, "delete_collection") == 0) {
            delete_collection(jwt_token);
        } else if (strcmp(cmd, "add_movie_to_collection") == 0) {
            add_movie_to_collection(jwt_token);
        } else if (strcmp(cmd, "delete_movie_from_collection") == 0) {
            delete_movie_from_collection(jwt_token);
        } else if (strcmp(cmd, "logout") == 0) {
            logout(&user_cookie);
        } else if (strcmp(cmd, "exit") == 0) {
            break;
        } else {
            printf("ERROR: Comanda necunoscuta\n");
        }
    }

    if (admin_cookie) {
        free(admin_cookie);
    }
    if (user_cookie) {
        free(user_cookie);
    }
    if (jwt_token) {
        free(jwt_token);
    }
    return 0;
}
