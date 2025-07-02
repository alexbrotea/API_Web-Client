#ifndef CLIENT_H
#define CLIENT_H

void login_admin(char **session_cookie);
void add_user(const char *session_cookie);
void get_users(const char *session_cookie);
void delete_user(const char *session_cookie);
void logout_admin(char **session_cookie);
void login(char **user_cookie);
void get_access(const char *user_cookie, char **jwt_token);
void get_movies(const char *jwt_token);
void get_movie(const char *jwt_token);
void add_movie(const char *jwt_token);
void delete_movie(const char *jwt_token);
void update_movie(const char *jwt_token);
void get_collections(const char *jwt_token);
void get_collection(const char *jwt_token);
void add_collection(const char *jwt_token);
void delete_collection(const char *jwt_token);
void add_movie_to_collection(const char *jwt_token);
void delete_movie_from_collection(const char *jwt_token);
void logout(char **user_cookie);

#endif
