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
void disconnect_tcp_connection(tcp_connection *);
void release_tcp_connection(tcp_connection *);

// following will go in client.c

typedef struct _tcp_connection
{
    int socket_descriptor;
    int socket_descriptor_closed;
    unsigned short int port;
    int error_number;
    char error_type;
} tcp_connection;

tcp_connection *tcp_connect(const char *server_ip_or_name, unsigned short int port)
{
    char port_str[6];
    struct addrinfo hints;
    struct addrinfo *ptr2top_node;
    struct addrinfo *temp;
    int failed_state;

    tcp_connection *connection;
    connection = (tcp_connection *)malloc(sizeof(tcp_connection));
    if (connection == NULL)
        return NULL; //  null returned

    connection->socket_descriptor = 0;
    connection->socket_descriptor_closed = 1; // yes by default it is closed
    connection->port = 0;
    connection->error_number = 0;
    connection->error_type = ' '; // P,C,G

    if (port <= 0)
    {
        connection->error_number = 801; // invalid port number
        connection->error_type = 'C';
        return connection;
    }

    connection->port = port;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    sprintf(port_str, "%u", port);

    failed_state = getaddrinfo(server_ip_or_name, port_str, &hints, &ptr2top_node);
    if (failed_state == -1)
    {
        connection->error_number = errno;
        connection->error_type = 'G';
        return connection;
    }

    for (temp = ptr2top_node; temp != NULL; temp = temp->ai_next)
    {
        connection->socket_descriptor = socket(temp->ai_family, temp->ai_socktype, temp->ai_protocol);
        if (connection->socket_descriptor == -1)
            continue;
        connection->socket_descriptor_closed = 0; // false , now it is open as socket got created

        failed_state = connect(connection->socket_descriptor, temp->ai_addr, temp->ai_addrlen);

        if (failed_state == -1)
        {
            close(connection->socket_descriptor);
            connection->socket_descriptor = 0;
            connection->socket_descriptor_closed = 1; // yes it is closed;
            continue;
        }
        break;
    }

    free(ptr2top_node);
    if (temp == NULL)
    {
        connection->error_number = 802;
        connection->error_type = 'C';
        return connection;
    }

    return connection;
}

int tcp_connection_failed(tcp_connection *connection)
{
    if (connection == NULL || connection->error_number != 0)
        return 1; // yes something failed
    return 0;
}

void tcp_connection_error(tcp_connection *connection, char **error_str)
{
    char *error801 = "Invalid Port Number";
    char *error802 = "Unable to connect";

    if (connection == NULL || error_str == NULL)
        return;

    if (connection->error_type == 'G')
    {
        *error_str = (char *)malloc(sizeof(char) * (strlen(gai_strerror(connection->error_number)) + 1));
        if (*error_str != NULL)
            strcpy(*error_str, gai_strerror(connection->error_number));
    }
    else if (connection->error_type == 'P')
    {
        *error_str = (char *)malloc(sizeof(char) * (strlen(strerror(connection->error_number)) + 1));
        if (*error_str != NULL)
            strcpy(*error_str, strerror(connection->error_number));
    }
    else if (connection->error_type == 'C')
    {
        if (connection->error_number == 801)
        {
            *error_str = (char *)malloc(sizeof(char) * (strlen(error801) + 1));
            if (*error_str != NULL)
                strcpy(*error_str, error801);
        }
        else if (connection->error_number == 802)
        {
            *error_str = (char *)malloc(sizeof(char) * (strlen(error802) + 1));
            if (*error_str != NULL)
                strcpy(*error_str, error802);
        }
        else
        {
            *error_str = NULL;
        }
    }
    else
    {
        *error_str = NULL;
    }
}
void disconnect_tcp_connection(tcp_connection *connection)
{
    if (connection == NULL)
        return;
    if (connection->socket_descriptor_closed == 0) // it means not closed 0 is false
    {
        close(connection->socket_descriptor);
        connection->socket_descriptor_closed = 1; // yes not it is closed
        connection->socket_descriptor = 0;
    }
}
void release_tcp_connection(tcp_connection *connection)
{
    if (connection == NULL)
        return;
    if (connection->socket_descriptor_closed == 0)
        close(connection->socket_descriptor);
    free(connection);
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
        disconnect_tcp_connection(connection);
        release_tcp_connection(connection);
        exit(TCP_CONNECTION_CODE_ERROR_EXIT_CODE);
    }

    // code to send / revice data
    disconnect_tcp_connection(connection);
    release_tcp_connection(connection);
    return 0;
}