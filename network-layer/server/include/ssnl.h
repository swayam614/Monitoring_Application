/**
 * @file ssnl.h
 * The functions of this library encaspulates the server side system programming
 * details of sending and receiving data over TCP Sockets.
 *
 */
#ifndef __SSNL__
#define __SSNL__

#include <stdint.h>

/**
 * @brief Exit code to use to end the application if a TCP operation fails
 */
#define TCP_SERVER_ERROR_EXIT_CODE 54

/**
 * @brief Data type to represent TCP server
 */
struct _tcp_server;

/**
 * @brief Data type to represent connected TCP client
 */
struct _tcp_client;

/**
 * @brief Alias of data type that represents TCP server
 */
typedef struct _tcp_server tcp_server;

/**
 * @brief Alias of data type that represents connected TCP client
 */
typedef struct _tcp_client tcp_client;

/**
 * @brief Allocates resources for TCP server
 * @param[in] port port number to associated with the TCP server
 * @return returns address of a structure that represents TCP server
 * @post If the function fails , error message will be setup
 * @post To check if the function failed or not , make use of \ref tcp_server_failed function
 * @post The error message can be extracted using \ref tcp_server_error function
 */
tcp_server *allocate_tcp_server(unsigned short int port);

/**
 * @brief sets the handler function that will be called when the server starts
 * @param[in] server address of tcp_server type structure
 * @param[in] handler address of the handler function that should get called when server starts
 * @note It is not essential to setup this event
 */
void on_tcp_server_started(tcp_server *server, void (*handler)(unsigned short int port));

/**
 * @brief sets the handler function that will be called when the server stops
 * @param[in] server address of tcp_server type structure
 * @param[in] handler address of the handler function that should get called when server stops
 * @note It is not essential to setup this event
 */
void on_tcp_server_stoppped(tcp_server *server, void (*handler)(unsigned short int));

/**
 * @brief sets the handler function that will be called whenever a client connects
 * @param[in] server address of tcp_server type structure
 * @param[in] handler address of the handler function that should get called whenever client connects
 * @note It is essential to set this event. If not set , then the function \ref tcp_start_server will fail
 */
void on_tcp_client_connected(tcp_server *server, void (*handler)(unsigned short int, tcp_server *, tcp_client *));

/**
 * @brief Starts the TCP server
 * @param[in] server address of tcp_server type structure
 * @pre Setting up tcp_client_connected event through \ref on_tcp_client_connected function is essential
 * @post If the function fails , error message will be setup
 * @post To check if the function failed or not , make use of \ref tcp_server_failed function
 * @post The error message can be extracted using \ref tcp_server_error function
 */
void tcp_start_server(tcp_server *server);

/**
 * @brief Send data via a connected TCP connection
 * @param[in] client address of tcp_client type structure
 * @param[in] buffer buffer address of string buffer that contains the string to be send to connected client
 * @param[in] len length of string buffer , cannot exceed 4294967295
 * @post If the function fails , error message will be setup
 * @post To check if the function failed or not , make use of \ref tcp_server_failed function
 * @post The error message can be extracted using \ref tcp_server_error function
 */
void tcp_client_send(tcp_client *client, const char *buffer, uint32_t len);

/**
 * @brief Receive data via a connected TCP connection
 * @param[in] client address of tcp_client type structure
 * @param[out] len address of uint32_t type block where the length of data received is to be kept
 * @return returns base address of string that contains the data received via TCP connection , in case of failure , NULL is returned
 * @post If the function fails , error message will be setup
 * @post To check if the function failed or not , make use of \ref tcp_server_failed function
 * @post The error message can be extracted using \ref tcp_server_error function
 */
char *tcp_client_receive(tcp_client *client, uint32_t *len);

/**
 * @brief Get server IP of TCP connection through which the server has accepted the connection
 * @param[in] client address of tcp_client type structure
 * @return returns base address of string that contains the server IP of TCP connection
 * @post If the function fails , error message will be setup
 * @post To check if the function failed or not , make use of \ref tcp_server_failed function
 * @post The error message can be extracted using \ref tcp_server_error function
 * @note When the server IP is more required then the memory should be released  using free function. The argument to the free function should be the base address of the string that contains the server IP.
 */
char *tcp_client_get_local_ip(tcp_client *client);

/**
 * @brief Get server port of TCP connection through which the server has accepted the connection
 * @param[in] client address of tcp_client type structure
 * @return returns server port of TCP connection
 * @post If the function fails , error message will be setup
 * @post To check if the function failed or not , make use of \ref tcp_server_failed function
 * @post The error message can be extracted using \ref tcp_server_error function
 */
unsigned short int tcp_client_get_local_port(tcp_client *client);

/**
 * @brief Get client IP of TCP connection
 * @param[in] client address of tcp_client type structure
 * @return returns base address of string that contains the client IP of TCP connection
 * @post If the function fails , error message will be setup
 * @post To check if the function failed or not , make use of \ref tcp_server_failed function
 * @post The error message can be extracted using \ref tcp_server_error function
 * @note When the client IP is more required then the memory should be released  using free function. The argument to the free function should be the base address of the string that contains the client IP.
 */
char *tcp_client_get_remote_ip(tcp_client *client);

/**
 * @brief Get client port of TCP connection
 * @param[in] client address of tcp_client type structure
 * @return returns client port of TCP connection
 * @post If the function fails , error message will be setup
 * @post To check if the function failed or not , make use of \ref tcp_server_failed function
 * @post The error message can be extracted using \ref tcp_server_error function
 */
unsigned short int tcp_client_get_remote_port(tcp_client *client);

/**
 * @brief Disconnect TCP connection with TCP client
 * @param[in] client address of tcp_client type structure
 */
void disconnect_tcp_client(tcp_client *client);

/**
 * @brief Release resources associated with a TCP client
 * @param[in] client address of tcp_client type structure
 */
void release_tcp_client(tcp_client *client);

/**
 * @brief Checks if an operation failed
 * @param[in] client  address of tcp_client type structure
 * @return returns int that represents true(non-zero) or false(zero)
 * @pre tcp_client_failed should be called after either of the function are called
 * \ref tcp_client_send ,
 * \ref tcp_client_receive ,
 * \ref tcp_client_get_local_ip ,
 * \ref tcp_client_get_local_port ,
 * \ref tcp_client_get_remote_ip
 * or
 * \ref tcp_client_get_remote_port
 * @post The error message can be extracted using \ref tcp_client_error function
 */
int tcp_client_failed(tcp_client *client);

/**
 * @brief Extracts error message
 * @param[in] client address of tcp_client type structure
 * @param[out] ptr address of char * type pointer where the base address of the error string is to be kept
 * @pre before calling \ref tcp_client_error , you should call \ref tcp_client_failed to determine if an operation failed. If an operation has failed then error would exit
 * @post if error message exits , base address of the string with error message will be assigned to the pointer to which ptr is pointing to
 * @post if error message does not exist or enough memeory to store error message is not available , NULL will be assigned to the pointer to which ptr is pointing to
 * @note when the error message is no more required , the memory should be released using the free function. The argument to the free function should be base address of the string that contains the error message.
 */
void tcp_client_error(tcp_client *client, char **ptr);

/**
 * @brief Stop TCP server
 * @param[in] server address of tcp_server type structure
 */
void tcp_stop_server(tcp_server *server);

/**
 * @brief Releases resources associated with TCP server
 * @param[in] server address of tcp_server type structure
 */
void release_tcp_server(tcp_server *server);

/**
 * @brief Checks if an operation failed
 * @param[in] server  address of tcp_server type structure
 * @return returns int that represents true(non-zero) or false(zero)
 * @pre tcp_server_failed should be called after either of the function are called
 * \ref allocate_tcp_server,
 * or
 * \ref tcp_start_server ,
 * @post The error message can be extracted using \ref tcp_server_error function
 */
int tcp_server_failed(tcp_server *server);

/**
 * @brief Extracts error message
 * @param[in] server address of tcp_server type structure
 * @param[out] ptr address of char * type pointer where the base address of the error string is to be kept
 * @pre before calling \ref tcp_server_error , you should call \ref tcp_server_failed to determine if an operation failed. If an operation has failed then error would exit
 * @post if error message exits , base address of the string with error message will be assigned to the pointer to which ptr is pointing to
 * @post if error message does not exist or enough memeory to store error message is not available , NULL will be assigned to the pointer to which ptr is pointing to
 * @note when the error message is no more required , the memory should be released using the free function. The argument to the free function should be base address of the string that contains the error message.
 */
void tcp_server_error(tcp_server *server, char **ptr);

#endif