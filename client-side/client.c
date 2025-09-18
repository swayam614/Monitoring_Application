#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>

// following will go in header file client.h
#define TCP_CONNECTION_CODE_ERROR_EXIT_CODE 55
struct _tcp_connection; // will be defined in client.c
typedef struct _tcp_connection tcp_connection;

tcp_connection *tcp_connect(const char *, unsigned short int);
int tcp_connection_failed(tcp_connection *);
void tcp_connection_error(tcp_connection *, char **error_str);
void tcp_disconnect(tcp_connection *);
void release_tcp_connection(tcp_connection *);

// following will go in client.c

tcp_connection *tcp_connect(const char *server_ip_or_name, unsigned short int port)
{
    return NULL;
}
int tcp_connection_failed(tcp_connection *connection)
{
    return 0;
}
void tcp_connection_error(tcp_connection *connection, char **error_str)
{
}
void tcp_disconnect(tcp_connection *connection)
{
}
void release_tcp_connection(tcp_connection *connection)
{
}

// following is the code to test the client implementation of network layer

int main()
{
    char *error_str;
    tcp_connection *connection = tcp_connect("localhost", 5046);
    if (tcp_connection_failed(connection))
    {
        tcp_connection_error(connection, &error_str);
        if (error_str != NULL)
        {
            printf("%s\n", error_str);
            free(error_str);
        }
        release_tcp_connection(connection);
        exit(TCP_CONNECTION_CODE_ERROR_EXIT_CODE);
    }

    // code to send / revice data
    tcp_disconnect(connection);
    release_tcp_connection(connection);
    return 0;
}