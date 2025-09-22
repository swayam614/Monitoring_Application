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
#define TCP_RW_BUFFER_SIZE 16384
#define TCP_RW_HEADER_SIZE 4
#define TCP_SEND_LIMIT 4294967295 - TCP_RW_HEADER_SIZE

struct _tcp_connection; // will be defined in client.c
typedef struct _tcp_connection tcp_connection;

tcp_connection *tcp_connect(const char *, unsigned short int);
int tcp_connection_failed(tcp_connection *);
void tcp_connection_error(tcp_connection *, char **error_str);
void disconnect_tcp_connection(tcp_connection *);
void release_tcp_connection(tcp_connection *);

void tcp_connection_send(tcp_connection *, const char *, uint32_t);
char *tcp_connection_receive(tcp_connection *, uint32_t *);
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
    char *error800 = "Connection not established";
    char *error801 = "Invalid Port Number";
    char *error802 = "Unable to connect";
    char *error803 = "Connection is closed or not yet established";
    char *error804 = "No data to send";
    char *error805 = "Cannot send more than 4294967291 bytes in one go";
    char *error806 = "Low memory";
    char *error807 = "Argument is NULL";
    char *error808 = "Connection closed by Peer";

    if (error_str == NULL)
        return;

    if (connection == NULL)
    {
        *error_str = (char *)malloc(sizeof(char) * (strlen(error800) + 1));
        if (*error_str != NULL)
            strcpy(*error_str, error800);
    }

    else if (connection->error_type == 'G')
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
        else if (connection->error_number == 803)
        {
            *error_str = (char *)malloc(sizeof(char) * (strlen(error803) + 1));
            if (*error_str != NULL)
                strcpy(*error_str, error803);
        }
        else if (connection->error_number == 804)
        {
            *error_str = (char *)malloc(sizeof(char) * (strlen(error804) + 1));
            if (*error_str != NULL)
                strcpy(*error_str, error804);
        }
        else if (connection->error_number == 805)
        {
            *error_str = (char *)malloc(sizeof(char) * (strlen(error805) + 1));
            if (*error_str != NULL)
                strcpy(*error_str, error805);
        }
        else if (connection->error_number == 806)
        {
            *error_str = (char *)malloc(sizeof(char) * (strlen(error806) + 1));
            if (*error_str != NULL)
                strcpy(*error_str, error806);
        }
        else if (connection->error_number == 807)
        {
            *error_str = (char *)malloc(sizeof(char) * (strlen(error807) + 1));
            if (*error_str != NULL)
                strcpy(*error_str, error807);
        }
        else if (connection->error_number == 808)
        {
            *error_str = (char *)malloc(sizeof(char) * (strlen(error808) + 1));
            if (*error_str != NULL)
                strcpy(*error_str, error808);
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

void tcp_connection_send(tcp_connection *connection, const char *buffer, uint32_t buffer_size)
{
    uint32_t our_buffer_size;
    uint32_t our_buffer_index;
    uint32_t buffer_index;
    char *our_buffer;
    uint32_t pick_in_this_cycle;
    uint32_t number_of_bytes_to_send;
    ssize_t bytes_sent;
    uint32_t network_byte_order;

    if (connection == NULL)
    {
        return;
    }
    if (connection->socket_descriptor_closed == 1)
    {
        connection->error_number = 803; // connection is closed or not yet established
        connection->error_type = 'C';
        return;
    }
    if (buffer == NULL || buffer_size == 0)
    {
        connection->error_number = 804; // No data to send
        connection->error_type = 'C';
        return;
    }
    if (buffer_size > TCP_SEND_LIMIT)
    {
        connection->error_number = 805; // Cannot send more than 4294967291 bytes in one go
        connection->error_type = 'C';
        return;
    }

    if (buffer_size + TCP_RW_HEADER_SIZE <= TCP_RW_BUFFER_SIZE)
    {
        our_buffer_size = buffer_size + TCP_RW_HEADER_SIZE;
    }
    else
    {
        our_buffer_size = TCP_RW_BUFFER_SIZE;
    }

    our_buffer = (char *)malloc(our_buffer_size);
    if (our_buffer == NULL)
    {
        connection->error_number = 806; // low memory
        connection->error_type = 'C';
        return;
    }

    network_byte_order = htonl(buffer_size);
    memcpy(our_buffer, &network_byte_order, sizeof(uint32_t));

    number_of_bytes_to_send = buffer_size + TCP_RW_HEADER_SIZE;
    our_buffer_index = TCP_RW_HEADER_SIZE; // 0,1,2 and 3 are used for header
    buffer_index = 0;

    while (number_of_bytes_to_send > 0)
    {
        if (number_of_bytes_to_send > (our_buffer_size - TCP_RW_HEADER_SIZE))
        {
            pick_in_this_cycle = our_buffer_size - our_buffer_index;
        }
        else
        {
            pick_in_this_cycle = number_of_bytes_to_send - our_buffer_index;
        }
        memcpy(our_buffer + our_buffer_index, buffer + buffer_index, pick_in_this_cycle);
        bytes_sent = send(connection->socket_descriptor, our_buffer, our_buffer_index + pick_in_this_cycle, 0);
        if (bytes_sent == -1)
        {
            // some issue, it can be because of some raised signal or some other serious issue
            // we will abort the send process , we dont retry
            connection->error_number = errno;
            connection->error_type = 'G';
            free(our_buffer);
            return;
        }
        buffer_index = buffer_index + bytes_sent - our_buffer_index;
        our_buffer_index = 0;
        number_of_bytes_to_send = number_of_bytes_to_send - bytes_sent;
    } // loop to send data chunks , ends here

    free(our_buffer);
}
char *tcp_connection_receive(tcp_connection *connection, uint32_t *received_data_size)
{
    ssize_t bytes_received;
    uint32_t our_buffer_index;
    char *our_buffer[TCP_RW_BUFFER_SIZE];
    uint32_t network_byte_order;
    uint32_t host_byte_order;
    uint32_t number_of_bytes_to_receive;
    uint32_t receive_buffer_size;
    char *receive_buffer;
    uint32_t receive_buffer_index;

    if (connection == NULL)
    {
        return NULL;
    }
    if (received_data_size == NULL)
    {
        connection->error_number = 807; // Argument is NULL
        connection->error_type = 'C';
        return NULL;
    }
    *received_data_size = 0;
    our_buffer_index = 0;
    while (1)
    {
        bytes_received = recv(connection->socket_descriptor, our_buffer + our_buffer_index, TCP_RW_BUFFER_SIZE, 0);

        if (bytes_received == 0)
        {
            connection->error_number = 808; // Connection closed by peer
            connection->error_type = 'C';
            return NULL;
        }
        else if (bytes_received == -1)
        {
            connection->error_number = errno;
            connection->error_type = 'P';
            return NULL;
        }

        our_buffer_index = our_buffer_index + bytes_received;
        if (our_buffer_index >= 4)
            break; // we have what we want , the header bytes
    } // infinite loop to read atleast header bytes ends here

    memcpy(&network_byte_order, our_buffer, sizeof(uint32_t));
    host_byte_order = ntohl(network_byte_order);
    number_of_bytes_to_receive = host_byte_order; // does not include the size of header bytes

    receive_buffer_size = number_of_bytes_to_receive;
    receive_buffer = (char *)malloc(receive_buffer_size);
    if (receive_buffer == NULL)
    {
        connection->error_number = 806; // Low memory
        connection->error_type = 'C';
        return NULL;
    }

    receive_buffer_index = 0;
    // pick the data from our_buffer and copy it to receive buffer if the first read count was more than 4 bytes

    if (our_buffer_index > 4)
    {
        memcpy(receive_buffer, our_buffer + TCP_RW_HEADER_SIZE, our_buffer_index - TCP_RW_HEADER_SIZE);
        receive_buffer_index = our_buffer_index - TCP_RW_HEADER_SIZE;
    }
    number_of_bytes_to_receive = number_of_bytes_to_receive - (our_buffer_index - TCP_RW_HEADER_SIZE);
    while (number_of_bytes_to_receive > 0)
    {
        bytes_received = recv(connection->socket_descriptor, receive_buffer + receive_buffer_index, TCP_RW_BUFFER_SIZE, 0);
        if (bytes_received == 0)
        {
            connection->error_number = 808; // connection closed by peer
            connection->error_type = 'C';
            free(receive_buffer);
            return NULL;
        }
        else if (bytes_received == -1)
        {
            connection->error_number = errno;
            connection->error_type = 'P';
            free(receive_buffer);
            return NULL;
        }
        receive_buffer_index = receive_buffer_index + bytes_received;
        number_of_bytes_to_receive = number_of_bytes_to_receive - bytes_received;
    } // loop to receive all bytes end here
    *received_data_size = receive_buffer_size;
    return receive_buffer;
}

// following is the code to test the client implementation of network layer

int main()
{
    char *error_str;
    char data[500000];
    int i;
    char *response;
    uint32_t response_size;

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

    // code to send request data

    for (i = 0; i < 500000; i++)
    {
        data[i] = (char)((i % 10) + 48);
    }

    tcp_connection_send(connection, data, 500000);
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