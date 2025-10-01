/**
 * @file ssnl.h
 * The functions of this library encaspulates the server side system programming
 * details of sending and receiving data over TCP Sockets.
 *
 */
#ifndef __SSNL__
#define __SSNL__

#include <stdint.h>

#define TCP_SERVER_ERROR_EXIT_CODE 54

struct _tcp_server;
struct _tcp_client;

typedef struct _tcp_server tcp_server;
typedef struct _tcp_client tcp_client;

tcp_server *allocate_tcp_server(unsigned short int);
void release_tcp_server(tcp_server *);
void tcp_start_server(tcp_server *);
void tcp_stop_server(tcp_server *);
void disconnect_tcp_client(tcp_client *);
void release_tcp_client(tcp_client *);
void tcp_client_send(tcp_client *, const char *, uint32_t);
char *tcp_client_receive(tcp_client *, uint32_t *);
char *tcp_client_get_local_ip(tcp_client *);
unsigned short int tcp_client_get_local_port(tcp_client *);
char *tcp_client_get_remote_ip(tcp_client *);
unsigned short int tcp_client_get_remote_port(tcp_client *);
int tcp_server_failed(tcp_server *);
void tcp_server_error(tcp_server *, char **);
int tcp_client_failed(tcp_client *);
void tcp_client_error(tcp_client *, char **);
void on_tcp_server_started(tcp_server *, void (*)(unsigned short int));
void on_tcp_server_stoppped(tcp_server *, void (*handler)(unsigned short int));
void on_tcp_client_connected(tcp_server *, void (*)(unsigned short int, tcp_server *, tcp_client *));

#endif