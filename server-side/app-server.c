#include <ssnl.h>

void tcp_server_started_handler(unsigned short int server_port)
{
    // code that will be executed when server starts (one time act only)
    printf("Server is ready to accept the request on port : %u\n", server_port);
}

void tcp_server_stoppped_handler(unsigned short int server_port)
{
    // code that will be executed when server stops (one time act only)
    printf("Server listening on port %u has stopped\n", server_port);
}

void tcp_server_client_connected_handler(unsigned short int server_port, tcp_server *server, tcp_client *connected_client)
{
    // code that will be executed whenever a connection is accepted
    char *error_str;
    char *request;
    uint32_t request_size;
    uint32_t i;
    char data[250000];
    char *local_ip, *remote_ip;
    unsigned short int local_port, remote_port;

    printf("Server listening on port %u has accepted connection request\n", server_port);

    remote_ip = tcp_client_get_remote_ip(connected_client);
    if (tcp_client_failed(connected_client))
    {
        tcp_client_error(connected_client, &error_str);
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

    remote_port = tcp_client_get_remote_port(connected_client);
    if (tcp_client_failed(connected_client))
    {
        tcp_client_error(connected_client, &error_str);
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

    local_ip = tcp_client_get_local_ip(connected_client);
    if (tcp_client_failed(connected_client))
    {
        tcp_client_error(connected_client, &error_str);
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

    local_port = tcp_client_get_local_port(connected_client);
    if (tcp_client_failed(connected_client))
    {
        tcp_client_error(connected_client, &error_str);
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

    // code to receive request data starts here

    request = tcp_client_receive(connected_client, &request_size);
    if (tcp_client_failed(connected_client))
    {
        tcp_client_error(connected_client, &error_str);
        if (error_str != NULL)
        {
            printf("%s\n", error_str);
            free(error_str);
        }
        disconnect_tcp_client(connected_client);
        release_tcp_client(connected_client);
        return;
    }

    printf("Number of bytes received in request %u\n", request_size);
    for (i = 0; i < request_size; i++)
    {
        printf("%c", request[i]);
    }
    printf("\n"); // to ensure that all bytes get printed as \n ensures that the
                  // contents of internl buffer are flushed to output stream (stdout)
    // we will assume that the data in request has been processed over here

    if (strncmp(request, "shutdown-server", request_size) == 0)
    {
        tcp_stop_server(server);
        disconnect_tcp_client(connected_client);
        release_tcp_client(connected_client);
        free(request);
        return;
    }

    free(request); // this has to be well documented
    // code to receive request data ends here

    // code to send response data

    // for (i = 0; i < 250000; i++) // loop to populate dummy data
    // {
    //     data[i] = (char)((i % 10) + 48);
    // }

    // tcp_client_send(connected_client, data, 250000);
    strcpy(data, "Thank You for contacting Swayam Palrecha Server. I feel great\n");
    strcat(data, "1. We have great products to offer. We have electronic products , stationery\n");
    strcat(data, "2. We also deal in computers and printers. You can call us any time for product\n");
    strcat(data, "3. enquiry. Our Sales person will be happy to help you whenever required\n");
    strcat(data, "4. We have great products to offer. We have electronic products , stationery\n");
    strcat(data, "5. We also deal in computers and printers. You can call us any time for product\n");
    strcat(data, "6. enquiry. Our Sales person will be happy to help you whenever required\n");
    strcat(data, "7. We have great products to offer. We have electronic products , stationery\n");
    strcat(data, "8. We also deal in computers and printers. You can call us any time for product\n");
    strcat(data, "9. enquiry. Our Sales person will be happy to help you whenever required\n");
    strcat(data, "10. We have great products to offer. We have electronic products , stationery\n");
    strcat(data, "11. We also deal in computers and printers. You can call us any time for product\n");
    strcat(data, "12. enquiry. Our Sales person will be happy to help you whenever required\n");
    strcat(data, "13. We have great products to offer. We have electronic products , stationery\n");
    strcat(data, "14. We also deal in computers and printers. You can call us any time for product\n");
    strcat(data, "15. enquiry. Our Sales person will be happy to help you whenever required\n");
    strcat(data, "16. We have great products to offer. We have electronic products , stationery\n");
    strcat(data, "17. We also deal in computers and printers. You can call us any time for product\n");
    strcat(data, "18. enquiry. Our Sales person will be happy to help you whenever required\n");
    strcat(data, "19. We have great products to offer. We have electronic products , stationery\n");
    strcat(data, "20. We also deal in computers and printers. You can call us any time for product\n");
    strcat(data, "21. enquiry. Our Sales person will be happy to help you whenever required\n");
    strcat(data, "22. We have great products to offer. We have electronic products , stationery\n");
    strcat(data, "23. We also deal in computers and printers. You can call us any time for product\n");
    strcat(data, "24. enquiry. Our Sales person will be happy to help you whenever required\n");
    strcat(data, "25. We have great products to offer. We have electronic products , stationery\n");
    strcat(data, "26. We also deal in computers and printers. You can call us any time for product\n");
    strcat(data, "27. enquiry. Our Sales person will be happy to help you whenever required\n");

    tcp_client_send(connected_client, data, strlen(data));
    if (tcp_client_failed(connected_client))
    {
        tcp_client_error(connected_client, &error_str);
        if (error_str != NULL)
        {
            printf("%s\n", error_str);
            free(error_str);
        }
        disconnect_tcp_client(connected_client);
        release_tcp_client(connected_client);
        return;
    }

    // code to send response data ends here
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