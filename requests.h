#ifndef REQUESTS_H
#define REQUESTS_H

char *compute_post_request(const char *host, const char *url, const char *content_type, char **body_data, int body_data_fields_count,
                           const char **cookies, int cookies_count);

char *compute_get_request(const char *host, const char *url, const char **cookies, int cookies_count);

char *compute_delete_request(const char *host, const char *url, const char **cookies, int cookies_count);

char *compute_put_request(const char *host, const char *url, const char *content_type, char **body_data, int body_data_fields_count,
                          const char **cookies, int cookies_count);

#endif
