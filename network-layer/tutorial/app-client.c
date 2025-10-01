/**
 * @example app-client.c
 * @author Swayam Palrecha
 * @brief An example that uses Client API to send and receive data
 */

#include <csnl.h>
#include <stdio.h>
#include <string.h>
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

    // code to send request data

    // for (i = 0; i < 500000; i++) // loop to populat dummy data
    // {
    //     data[i] = (char)((i % 10) + 48);
    // }

    // tcp_connection_send(connection, data, 500000);

    strcpy(data, "Hello I am client of yours , I wish to ask a question related to a product\n");
    strcat(data, "1. I live in Bangalore. Bangalore is famous for tech jobs. It is very cool place\n");
    strcat(data, "2. The worst thing is traffic pf bangalore\n");
    strcat(data, "3. Where i am living is electronic city which is 55 km far from airport\n");
    strcat(data, "4. I live in Bangalore. Bangalore is famous for tech jobs. It is very cool place\n");
    strcat(data, "5. The worst thing is traffic pf bangalore\n");
    strcat(data, "6. Where i am living is electronic city which is 55 km far from airport\n");
    strcat(data, "7. I live in Bangalore. Bangalore is famous for tech jobs. It is very cool place\n");
    strcat(data, "8. The worst thing is traffic pf bangalore\n");
    strcat(data, "9. Where i am living is electronic city which is 55 km far from airport\n");
    strcat(data, "10. I live in Bangalore. Bangalore is famous for tech jobs. It is very cool place\n");
    strcat(data, "11. The worst thing is traffic pf bangalore\n");
    strcat(data, "12. Where i am living is electronic city which is 55 km far from airport\n");
    strcat(data, "13. I live in Bangalore. Bangalore is famous for tech jobs. It is very cool place\n");
    strcat(data, "14. The worst thing is traffic pf bangalore\n");
    strcat(data, "15. Where i am living is electronic city which is 55 km far from airport\n");
    strcat(data, "16. I live in Bangalore. Bangalore is famous for tech jobs. It is very cool place\n");
    strcat(data, "17. The worst thing is traffic pf bangalore\n");
    strcat(data, "18. Where i am living is electronic city which is 55 km far from airport\n");

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

    // code to receive response data starts here

    response = tcp_connection_receive(connection, &response_size);
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

    printf("Number of bytes received in response %u\n", response_size);
    for (i = 0; i < response_size; i++)
    {
        printf("%c", response[i]);
    }
    printf("\n");   // to ensure that all bytes get printed as \n ensures that the
                    // contents of internl buffer are flushed to output stream (stdout)
    free(response); // this has to be well documented
    // code to receive response data ends here

    disconnect_tcp_connection(connection);
    release_tcp_connection(connection);
    return 0;
}