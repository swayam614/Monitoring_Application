/**
 * @file csnl.h
 * The functions of this library encaspulates the client side system programming
 * details of sending and reciving data over TCP sockets.
 *
 */

#ifndef __CSNL__
#define __CSNL__

#include <stdint.h>

/**
 * @brief Exit code to use to end the application if a TCP operation fails
 */
#define TCP_CONNECTION_CODE_ERROR_EXIT_CODE 55

/**
 * @brief Data type to represent TCP connection
 */
struct _tcp_connection;

/**
 * @brief Alias of data type that represents TCP connection
 */
typedef struct _tcp_connection tcp_connection;

/**
 * @brief Establishes a TCP connection with TCP server
 *
 * @param[in] server_ip_or_name Base address of a null terminated string that contains TCP server IP/name
 * @param[in] port Port number of the TCP server
 * @return returns address of a structure that represents TCP connection
 * @post if the function fails , error message will be setup
 * @post To check if the function failed or not , make use of \ref tcp_connection_failed function
 * @post The error message can be extracted using \ref tcp_connection_error function
 */
tcp_connection *tcp_connect(const char *server_ip_or_name, unsigned short int port);

/**
 * @brief Send data via a connected TCP connection
 * @param[in] connection address of tcp_connection type structure
 * @param[in] buffer buffer base address of string buffer that contains the string to be sent to server
 * @param[in] len length of string buffer , cannot exceed 4294967295
 * @post if the function fails , error message will be setup
 * @post To check if the function failed or not , make use of \ref tcp_connection_failed function
 * @post The error message can be extracted using \ref tcp_connection_error function
 */
void tcp_connection_send(tcp_connection *connection, const char *buffer, uint32_t len);

/**
 * @brief Receive data via a TCP connection
 * @param[in] connection address of tcp_connection type structure
 * @param[out] len address of uint32_t type block where the length of data received is to be kept
 * @return returns base address of a string that contains the data received via TCP connection , in case of failure NULL will be returned
 * @post if the function fails , error message will be setup
 * @post To check if the function failed or not , make use of \ref tcp_connection_failed function
 * @post The error message can be extracted using \ref tcp_connection_error function
 * @note When the received string is no more required , the memory should be released using the free function . The argument to the free function should be the base address of the string that contains the received data.
 */
char *tcp_connection_receive(tcp_connection *connection, uint32_t *len);

/**
 * @brief Get client IP of TCP connection
 * @param[in] connection address of tcp_connection type structure
 * @return returns base address of a string that contains the client IP of TCP connection
 * @post if the function fails , error message will be setup
 * @post To check if the function failed or not , make use of \ref tcp_connection_failed function
 * @post The error message can be extracted using \ref tcp_connection_error function
 * @note When the client IP is no more required , the memory should be released using the free function . The argument to the free function should be the base address of the string that contains the client IP.
 */
char *tcp_connection_get_local_ip(tcp_connection *connection);

/**
 * @brief Get client port of TCP connection
 * @param[in] connection address of tcp_connection type structure
 * @return returns client port of TCP connection
 * @post if the function fails , error message will be setup
 * @post To check if the function failed or not , make use of \ref tcp_connection_failed function
 * @post The error message can be extracted using \ref tcp_connection_error function
 */
unsigned short int tcp_connection_get_local_port(tcp_connection *connection);

/**
 * @brief Get server IP of TCP connection
 * @param[in] connection address of tcp_connection type structure
 * @return returns base address of a string that contains the server IP of TCP connection
 * @post if the function fails , error message will be setup
 * @post To check if the function failed or not , make use of \ref tcp_connection_failed function
 * @post The error message can be extracted using \ref tcp_connection_error function
 * @note When the server IP is no more required , the memory should be released using the free function . The argument to the free function should be the base address of the string that contains the server IP.
 */
char *tcp_connection_get_remote_ip(tcp_connection *connection);

/**
 * @brief Get server port of TCP connection
 * @param[in] connection address of tcp_connection type structure
 * @return returns server port of TCP connection
 * @post if the function fails , error message will be setup
 * @post To check if the function failed or not , make use of \ref  tcp_connection_failed function
 * @post The error message can be extracted using \ref tcp_connection_error function
 */
unsigned short int tcp_connection_get_remote_port(tcp_connection *connection);

/**
 * @brief Disconnect TCP connection with TCP server
 * @param[in] connection address of tcp_connection type structure
 */
void disconnect_tcp_connection(tcp_connection *connection);

/**
 * @brief Release resources associated with TCP connection
 * @param[in] connection address of tcp_connection type structure
 */
void release_tcp_connection(tcp_connection *connection);

/**
 * @brief Checks if an operation failed
 * @param[in] connection address of tcp_connection type structure
 * @return returns int that represents true(non-zero) or false(zero)
 * @pre tcp_connection_failed should be called after either of the function are called
 * \ref tcp_connect ,
 * \ref tcp_connection_send ,
 * \ref tcp_connection_receive ,
 * \ref tcp_connection_get_local_ip ,
 * \ref tcp_connection_get_local_port ,
 * \ref tcp_connection_get_remote_ip
 * or
 * \ref tcp_connection_get_remote_port
 * @post The error message can be extracted using \ref tcp_connection_error function
 */
int tcp_connection_failed(tcp_connection *connection);

/**
 * @brief Extracts error message
 * @param[in] connection address of tcp_connection type structure
 * @param[out] ptr address of char * type pointer where the base address of the error string is to be kept
 * @pre before calling \ref tcp_connection_error , you should call \ref tcp_connection_failed to determine if an operation failed. If an operation has failed then error would exit
 * @post if error message exits , base address of the string with error message will be assigned to the pointer to which ptr is pointing to
 * @post if error message does not exist or enough memeory to store error message is not available , NULL will be assigned to the pointer to which ptr is pointing to
 * @note when the error message is no more required , the memory should be released using the free function. The argument to the free function should be base address of the string that contains the error message.
 */
void tcp_connection_error(tcp_connection *connection, char **ptr);

#endif