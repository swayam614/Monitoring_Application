/**
 * @file cscl.h
 * The functions of this library encaspulates the client side system programming
 * details of sending and reciving data over TCP sockets.
 *
 */

#ifndef __CSCL__
#define __CSCL__

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
 * @brief Data type to represent request data to be sent via TCP connection
 */
struct _tcp_connection_request;

/**
 * @brief Data type to represent response data received  via TCP connection
 */
struct _tcp_connection_response;

/**
 * @brief Alias of data type that represents TCP connection
 */
typedef struct _tcp_connection tcp_connection;

/**
 * @brief Alias of data type that represents request data to be sent via TCP connection
 */
typedef struct _tcp_connection_request tcp_connection_request;

/**
 * @brief Alias of data type that represents response data recevied via  TCP connection
 */
typedef struct _tcp_connection_response tcp_connection_response;

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
 * @param[in] request address of tcp_connection_request type structure
 * @post if the function fails , error message will be setup
 * @post To check if the function failed or not , make use of \ref tcp_connection_failed function
 * @post The error message can be extracted using \ref tcp_connection_error function
 */
void tcp_connection_send_request(tcp_connection *connection, tcp_connection_request *request);

/**
 * @brief Receive data via a TCP connection
 * @param[in] connection address of tcp_connection type structure
 * @return returns address of tcp_connection_response type structure , which contains the response received via TCP connection
 * @post if the function fails , error message will be setup
 * @post To check if the function failed or not , make use of \ref tcp_connection_failed function
 * @post The error message can be extracted using \ref tcp_connection_error function
 * @note When the received response is no more required , the memory should be released using the free function . The argument to the free function should be the address of the tcp_connection_response type structure.
 */
tcp_connection_response *tcp_connection_receive_response(tcp_connection *connection);

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

// will document later on
tcp_connection_request *create_tcp_connection_request(tcp_connection *connection);

void tcp_connection_request_set_action_name(tcp_connection_request *request, const char *action);
char *tcp_connection_request_get_action_name(tcp_connection_request *request);

void tcp_connection_request_set_string(tcp_connection_request *request, const char *name, const char *value);
void tcp_connection_request_set_char(tcp_connection_request *request, const char *name, char value);
void tcp_connection_request_set_int8(tcp_connection_request *request, const char *name, int8_t value);
void tcp_connection_request_set_int16(tcp_connection_request *request, const char *name, int16_t value);
void tcp_connection_request_set_int32(tcp_connection_request *request, const char *name, int32_t value);
void tcp_connection_request_set_int64(tcp_connection_request *request, const char *name, int64_t value);
void tcp_connection_request_set_uint8(tcp_connection_request *request, const char *name, uint8_t value);
void tcp_connection_request_set_uint16(tcp_connection_request *request, const char *name, uint16_t value);
void tcp_connection_request_set_uint32(tcp_connection_request *request, const char *name, uint32_t value);
void tcp_connection_request_set_uint64(tcp_connection_request *request, const char *name, uint64_t value);
void tcp_connection_request_set_float(tcp_connection_request *request, const char *name, float value);
void tcp_connection_request_set_double(tcp_connection_request *request, const char *name, double value);
void tcp_connection_request_set_long_double(tcp_connection_request *request, const char *name, long double value);

int tcp_connection_response_name_exists(tcp_connection_response *response, const char *name);
char *tcp_connection_response_get_string(tcp_connection_response *response, const char *name);
char tcp_connection_response_get_char(tcp_connection_response *response, const char *name);
int8_t tcp_connection_response_get_int8(tcp_connection_response *response, const char *name);
int16_t tcp_connection_response_get_int16(tcp_connection_response *response, const char *name);
int32_t tcp_connection_response_get_int32(tcp_connection_response *response, const char *name);
int64_t tcp_connection_response_get_int64(tcp_connection_response *response, const char *name);
uint8_t tcp_connection_response_get_uint8(tcp_connection_response *response, const char *name);
uint16_t tcp_connection_response_get_uint16(tcp_connection_response *response, const char *name);
uint32_t tcp_connection_response_get_uint32(tcp_connection_response *response, const char *name);
uint64_t tcp_connection_response_get_uint64(tcp_connection_response *response, const char *name);
float tcp_connection_response_get_float(tcp_connection_response *response, const char *name);
double tcp_connection_response_get_double(tcp_connection_response *response, const char *name);
long double tcp_connection_response_get_long_double(tcp_connection_response *response, const char *name);

void release_tcp_connection_request(tcp_connection_request *request);

int tcp_connection_request_failed(tcp_connection_request *request);
void tcp_connection_request_error(tcp_connection_request *request, char **ptr);

void release_tcp_connection_response(tcp_connection_response *response);

int tcp_connection_response_failed(tcp_connection_response *response);
void tcp_connection_response_error(tcp_connection_response *response, char **ptr);

#endif