#ifndef __CSNL__
#define __CSNL__

#include <stdint.h>

#define TCP_CONNECTION_CODE_ERROR_EXIT_CODE 55

struct _tcp_connection;
typedef struct _tcp_connection tcp_connection;

tcp_connection *tcp_connect(const char *, unsigned short int);
int tcp_connection_failed(tcp_connection *);
void tcp_connection_error(tcp_connection *, char **);
void disconnect_tcp_connection(tcp_connection *);
void release_tcp_connection(tcp_connection *);
void tcp_connection_send(tcp_connection *, const char *, uint32_t);
char *tcp_connection_receive(tcp_connection *, uint32_t *);
char *tcp_connection_get_local_ip(tcp_connection *);
unsigned short int tcp_connection_get_local_port(tcp_connection *);
char *tcp_connection_get_remote_ip(tcp_connection *);
unsigned short int tcp_connection_get_remote_port(tcp_connection *);

#endif