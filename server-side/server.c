#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

// following code will be part of server.h - the header file

#define TCP_SERVER_ERROR_EXIT_CODE 54
// very very important part , the header file will contain struct declaration not defination
// following declaration are called forward declaration , definations will be in libaray

struct _tcp_server;
struct _tcp_client;

typedef struct _tcp_server tcp_server;
typedef struct _tcp_client tcp_client;

// core functions
tcp_server *allocate_tcp_server(int);
void release_tcp_server(tcp_server *);
void tcp_start_server(tcp_server *);
void tcp_stop_server(tcp_server *);
void *request_processor(void *);

// error related functions
int tcp_server_failed(tcp_server *);
void tcp_server_error(tcp_server *, char **);

// functions for setting up event handler
void on_tcp_server_started(tcp_server *, void (*)(int));
void on_tcp_server_stoppped(tcp_server *, void (*handler)(int));
void on_tcp_client_connected(tcp_server *, void (*)(int, tcp_client *));

// following code will be the part of server.c that will get compliled to a library

typedef struct _tcp_server
{

} tcp_server;
typedef struct _tcp_client
{

} tcp_client;

tcp_server *allocate_tcp_server(int port)
{
}

void release_tcp_server(tcp_server *server)
{
}

void tcp_start_server(tcp_server *server)
{
}

void tcp_stop_server(tcp_server *server)
{
}

void *request_processor(void *gen_ptr)
{
}

int tcp_server_failed(tcp_server *server)
{
}

void tcp_server_error(tcp_server *server, char **error_str)
{
}

void on_tcp_server_started(tcp_server *server, void (*handler)(int))
{
}
void on_tcp_server_stoppped(tcp_server *server, void (*handler)(int))
{
}
void on_tcp_client_connected(tcp_server *server, void (*handler)(int, tcp_client *))
{
}

// the following code will be written by the serverside network layer library user
// lets assume the name of the prpgrammer to be Bobby
// we will remove the followig code , while creating library ,
// as library should not contain main function , that is supposed to be written by library user
// we are writing it right now , just to test our code

void tcp_server_started_handler(int server_port)
{
    // code that will be executed when server starts (one time act only)
}

void tcp_server_stoppped_handler(int server_port)
{
    // code that will be executed when server stops (one time act only)
}

void tcp_server_client_connected_handler(int server_port, tcp_client *connected_client)
{
    // code that will be executed whenever a connection is accepted
}

int main()
{
    char *error_str;
    tcp_server *server;
    server = allocate_tcp_server(5046); // Bobby may write code to pick port number from some file
    if (tcp_server_failed(server))
    {
        release_tcp_server(server);
        tcp_server_error(server, &error_str);
        if (error_str != NULL)
        {
            printf("%s\n", error_str);
            free(error_str);
        }
        exit(TCP_SERVER_ERROR_EXIT_CODE); // application terminates with an error code (non zero)
    }

    // server allocated to accept request on 5046, not yet accepting connection

    // setting up some handlers
    on_tcp_server_started(server, tcp_server_started_handler);
    on_tcp_server_stoppped(server, tcp_server_stoppped_handler);
    on_tcp_client_connected(server, tcp_server_client_connected_handler);

    // starting the server
    tcp_start_server(server); // now the server is in waiting mode to accept the connection
    if (tcp_server_failed(server))
    {
        release_tcp_server(server);
        tcp_server_error(server, &error_str);
        if (error_str != NULL)
        {
            printf("%s\n", error_str);
            free(error_str);
        }
        pthread_exit(NULL); // lets not terminate the running threads
    }
    else
    {
        release_tcp_server(server);
    }
    pthread_exit(NULL); // so that running thread wont terminate
}