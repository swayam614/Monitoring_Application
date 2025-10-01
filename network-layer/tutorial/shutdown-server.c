/**
 * @example shutdown-server.c
 * @author Swayam Palrecha
 * @brief An example that uses Client API to send a shutdown request
 */

#include <csnl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    char *error_str;
    char data[500000];
    int i;
    char *response;
    uint32_t response_size;
    char *local_ip, *remote_ip;
    unsigned short int local_port, remote_port;

    tcp_connection *connection = tcp_connect("localhost", 5046);
    if (tcp_connection_failed(connection))
    {
        tcp_connection_error(connection, &error_str);
        if (error_str != NULL)
        {
            printf("%s\n", error_str);
            free(error_str);
        }
        disconnect_tcp_connection(connection); // if you want this line can be removed
        release_tcp_connection(connection);
        exit(TCP_CONNECTION_CODE_ERROR_EXIT_CODE);
    }

    remote_ip = tcp_connection_get_remote_ip(connection);
    if (tcp_connection_failed(connection))
    {
        tcp_connection_error(connection, &error_str);
        if (error_str != NULL)
        {
            printf("%s\n", error_str);
            free(error_str);
        }
    }
    else
    {
        printf("Remote IP address : %s\n", remote_ip);
        free(remote_ip);
    }

    remote_port = tcp_connection_get_remote_port(connection);
    if (tcp_connection_failed(connection))
    {
        tcp_connection_error(connection, &error_str);
        if (error_str != NULL)
        {
            printf("%s\n", error_str);
            free(error_str);
        }
    }
    else
    {
        printf("Remote PORT number : %u\n", remote_port);
    }

    local_ip = tcp_connection_get_local_ip(connection);
    if (tcp_connection_failed(connection))
    {
        tcp_connection_error(connection, &error_str);
        if (error_str != NULL)
        {
            printf("%s\n", error_str);
            free(error_str);
        }
    }
    else
    {
        printf("Local IP address : %s\n", local_ip);
        free(local_ip);
    }

    local_port = tcp_connection_get_local_port(connection);
    if (tcp_connection_failed(connection))
    {
        tcp_connection_error(connection, &error_str);
        if (error_str != NULL)
        {
            printf("%s\n", error_str);
            free(error_str);
        }
    }
    else
    {
        printf("Local PORT number : %u\n", local_port);
    }

    strcpy(data, "shutdown-server");

    tcp_connection_send(connection, data, strlen(data));
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

    // code to send reuqest data ends here
    disconnect_tcp_connection(connection);
    release_tcp_connection(connection);
    return 0;
}