#ifndef __SSCL__
#define __SSCL__

#include <stdint.h>
struct _tcp_action_server;
struct _tcp_action_request;
struct _tcp_action_response;

typedef struct _tcp_action_server tcp_action_server;
typedef struct _tcp_action_request tcp_action_request;
typedef struct _tcp_action_response tcp_action_response;

tcp_action_server *allocate_tcp_action_server(unsigned short int port);
void on_tcp_action_server_started(tcp_action_server *server, void (*handler)(unsigned short int));
void on_tcp_action_server_stopped(tcp_action_server *server, void (*handler)(unsigned short int));
void tcp_action_server_add_action_mapping(tcp_action_server *server, const char *commond, void (*action)(tcp_action_request *, tcp_action_response *));

void start_tcp_action_server(tcp_action_server *server);

void stop_tcp_action_server(tcp_action_server *server);
void release_tcp_action_server(tcp_action_server *server);

int tcp_action_server_failed(tcp_action_server *server);
void tcp_action_server_error(tcp_action_server *server, char **ptr);

char *tcp_action_request_get_action_name(tcp_action_request *request);
char *tcp_action_request_get_local_ip(tcp_action_request *request);
char *tcp_action_request_get_remote_ip(tcp_action_request *request);
unsigned short int tcp_action_request_get_local_port(tcp_action_request *request);
unsigned short int tcp_action_request_get_remote_port(tcp_action_request *request);

int tcp_action_request_name_exists(tcp_action_request *request, const char *name);
char *tcp_action_request_get_string(tcp_action_request *request, const char *name);
char tcp_action_request_get_char(tcp_action_request *request, const char *name);
int8_t tcp_action_request_get_int8(tcp_action_request *request, const char *name);
int16_t tcp_action_request_get_int16(tcp_action_request *request, const char *name);
int32_t tcp_action_request_get_int32(tcp_action_request *request, const char *name);
int64_t tcp_action_request_get_int64(tcp_action_request *request, const char *name);
uint8_t tcp_action_request_get_uint8(tcp_action_request *request, const char *name);
uint16_t tcp_action_request_get_uint16(tcp_action_request *request, const char *name);
uint32_t tcp_action_request_get_uint32(tcp_action_request *request, const char *name);
uint64_t tcp_action_request_get_uint64(tcp_action_request *request, const char *name);
float tcp_action_request_get_float(tcp_action_request *request, const char *name);
double tcp_action_request_get_double(tcp_action_request *request, const char *name);
long double tcp_action_request_get_long_double(tcp_action_request *request, const char *name);

void tcp_action_response_set_string(tcp_action_response *response, const char *name, const char *value);
void tcp_action_response_set_char(tcp_action_response *response, const char *name, char value);
void tcp_action_response_set_int8(tcp_action_response *response, const char *name, int8_t value);
void tcp_action_response_set_int16(tcp_action_response *response, const char *name, int16_t value);
void tcp_action_response_set_int32(tcp_action_response *response, const char *name, int32_t value);
void tcp_action_response_set_int64(tcp_action_response *response, const char *name, int64_t value);
void tcp_action_response_set_uint8(tcp_action_response *response, const char *name, uint8_t value);
void tcp_action_response_set_uint16(tcp_action_response *response, const char *name, uint16_t value);
void tcp_action_response_set_uint32(tcp_action_response *response, const char *name, uint32_t value);
void tcp_action_response_set_uint64(tcp_action_response *response, const char *name, uint64_t value);
void tcp_action_response_set_float(tcp_action_response *response, const char *name, float value);
void tcp_action_response_set_double(tcp_action_response *response, const char *name, double value);
void tcp_action_response_set_long_double(tcp_action_response *response, const char *name, long double value);

void send_tcp_action_response(tcp_action_response *response);

void tcp_action_request_close(tcp_action_request *request);
void release_tcp_action_request(tcp_action_request *request);

int tcp_action_request_failed(tcp_action_request *request);
void tcp_action_request_error(tcp_action_request *request, char **ptr);

void release_tcp_action_response(tcp_action_response *response);

int tcp_action_response_failed(tcp_action_response *response);
void tcp_action_response_error(tcp_action_response *response, char **ptr);

tcp_action_server *get_tcp_action_server(tcp_action_request *request);

#endif