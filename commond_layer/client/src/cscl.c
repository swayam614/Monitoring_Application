#include <stdint.h>
#include <cscl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <clencoder.h>
#include <csnl.h>

#include <_stdio.h>

typedef struct _tcp_connection_request
{
    byte_stream *stream;
    int error_number;
    char *error_string;
    char *action_name;
} tcp_connection_request;

typedef struct _tcp_connection_response
{
    int error_number;
    char *error_string;
    byte_stream *stream;
    byte_stream_elements *elements;
} tcp_connection_response;

void tcp_connection_send_request(tcp_connection *connection, tcp_connection_request *request)
{
    char *str;
    uint32_t str_len;

    if (connection == NULL || request == NULL)
        return;

    if (request->action_name == NULL)
    {
        if (request->error_number == 501 && request->error_string != NULL)
        {
            free(request->error_string);
            request->error_string = NULL;
        }
        request->error_number = 505; // Action name not set
        return;
    }

    str = get_byte_stream_bytes(request->stream);
    if (str == NULL)
    {
        if (request->error_number == 501 && request->error_string != NULL)
        {
            free(request->error_string);
            request->error_string = NULL;
        }
        request->error_number = 504; // Request Empty
        return;
    }

    str_len = get_byte_stream_length(request->stream);
    if (str_len == 0)
    {
        if (request->error_number == 501 && request->error_string != NULL)
        {
            free(request->error_string);
            request->error_string = NULL;
        }
        request->error_number = 504; // Request Empty
        return;
    }

    tcp_connection_send(connection, str, str_len);
    if (tcp_connection_failed(connection))
    {
        if (request->error_number == 501 && request->error_string != NULL)
        {
            free(request->error_string);
            request->error_string = NULL;
        }
        request->error_number = 501;
        tcp_connection_error(connection, &(request->error_string));
    }
}

tcp_connection_response *tcp_connection_receive_response(tcp_connection *connection)
{
    tcp_connection_response *response;
    char *str;
    uint32_t str_len;

    if (connection == NULL)
        return NULL;

    response = (tcp_connection_response *)malloc(sizeof(tcp_connection_response));
    if (response == NULL)
        return NULL;

    response->elements = NULL;
    response->error_number = 0;
    response->error_string = NULL;
    response->stream = NULL;

    str = tcp_connection_receive(connection, &str_len);
    if (tcp_connection_failed(connection))
    {
        response->error_number = 501;
        tcp_connection_error(connection, &(response->error_string));
        return response;
    }

    response->stream = create_byte_stream_from_bytes(str, str_len);
    if (response->stream == NULL)
    {
        if (response->error_number == 501 && response->error_string != NULL)
        {
            free(response->error_string);
            response->error_string = NULL;
        }
        response->error_number = 500; //  Low memory
        return response;
    }

    response->elements = get_byte_stream_elements(response->stream);
    if (response->elements == NULL)
    {
        release_byte_stream(response->stream);
        response->stream = NULL;
        if (response->error_number == 501 && response->error_string != NULL)
        {
            free(response->error_string);
            response->error_string = NULL;
        }
        response->error_number = 500; //  Low memory
        return response;
    }

    return response;
}

// will document later on
tcp_connection_request *create_tcp_connection_request(tcp_connection *connection)
{
    tcp_connection_request *request;

    if (connection == NULL)
        return NULL;

    request = (tcp_connection_request *)malloc(sizeof(tcp_connection_request));
    if (request == NULL)
        return NULL;

    request->error_number = 0;
    request->error_string = NULL;
    request->stream = create_byte_stream();
    if (request->stream == NULL)
    {
        request->error_number = 500;
        return request;
    }
    request->action_name = NULL;
    return request;
}

void tcp_connection_request_set_action_name(tcp_connection_request *request, const char *action)
{
    int result;
    if (request == NULL || action == NULL || *action == '\0')
        return;

    if (request->action_name != NULL)
    {
        if (request->error_number == 501 && request->error_string != NULL)
        {
            free(request->error_string);
            request->error_string = NULL;
        }
        request->error_number = 506;
        return;
    }

    request->action_name = (char *)malloc(sizeof(char) * (strlen(action) + 1));
    if (request->action_name == NULL)
    {
        if (request->error_number == 501 && request->error_string != NULL)
        {
            free(request->error_string);
            request->error_string = NULL;
        }
        request->error_number = 500;
        return;
    }
    strcpy(request->action_name, action);
    result = add_string_to_byte_stream(request->stream, "***ACTION_NAME***", request->action_name);
    if (!result)
    {
        free(request->action_name);
        request->action_name = NULL;
        if (request->error_number == 501 && request->error_string != NULL)
        {
            free(request->error_string);
            request->error_string = NULL;
        }
        request->error_number = 500;
        return;
    }
}

char *tcp_connection_request_get_action_name(tcp_connection_request *request)
{
    char *str;

    if (request == NULL || request->action_name == NULL)
        return NULL;

    str = (char *)malloc(sizeof(char) * (strlen(request->action_name) + 1));
    if (str == NULL)
    {
        if (request->error_number == 501 && request->error_string != NULL)
        {
            free(request->error_string);
            request->error_string = NULL;
        }
        request->error_number = 500;
        return NULL;
    }

    strcpy(str, request->action_name);
    return str;
}

void tcp_connection_request_set_string(tcp_connection_request *request, const char *name, const char *value)
{
    int result;

    if (request == NULL || name == NULL || value == NULL || *name == '\0' || *value == '\0')
        return;

    result = add_string_to_byte_stream(request->stream, name, value);
    if (!result)
    {
        if (request->error_number == 501 && request->error_string != NULL)
        {
            free(request->error_string);
            request->error_string = NULL;
        }
        request->error_number = 500;
    }
}
void tcp_connection_request_set_char(tcp_connection_request *request, const char *name, char value)
{
    int result;

    if (request == NULL || name == NULL || *name == '\0')
        return;

    result = add_char_to_byte_stream(request->stream, name, value);
    if (!result)
    {
        if (request->error_number == 501 && request->error_string != NULL)
        {
            free(request->error_string);
            request->error_string = NULL;
        }
        request->error_number = 500;
    }
}

void tcp_connection_request_set_int8(tcp_connection_request *request, const char *name, int8_t value)
{
    int result;

    if (request == NULL || name == NULL || *name == '\0')
        return;

    result = add_int8_to_byte_stream(request->stream, name, value);
    if (!result)
    {
        if (request->error_number == 501 && request->error_string != NULL)
        {
            free(request->error_string);
            request->error_string = NULL;
        }
        request->error_number = 500;
    }
}

void tcp_connection_request_set_int16(tcp_connection_request *request, const char *name, int16_t value)
{
    int result;

    if (request == NULL || name == NULL || *name == '\0')
        return;

    result = add_int16_to_byte_stream(request->stream, name, value);
    if (!result)
    {
        if (request->error_number == 501 && request->error_string != NULL)
        {
            free(request->error_string);
            request->error_string = NULL;
        }
        request->error_number = 500;
    }
}

void tcp_connection_request_set_int32(tcp_connection_request *request, const char *name, int32_t value)
{
    int result;

    if (request == NULL || name == NULL || *name == '\0')
        return;

    result = add_int32_to_byte_stream(request->stream, name, value);
    if (!result)
    {
        if (request->error_number == 501 && request->error_string != NULL)
        {
            free(request->error_string);
            request->error_string = NULL;
        }
        request->error_number = 500;
    }
}

void tcp_connection_request_set_int64(tcp_connection_request *request, const char *name, int64_t value)
{
    int result;

    if (request == NULL || name == NULL || *name == '\0')
        return;

    result = add_int64_to_byte_stream(request->stream, name, value);
    if (!result)
    {
        if (request->error_number == 501 && request->error_string != NULL)
        {
            free(request->error_string);
            request->error_string = NULL;
        }
        request->error_number = 500;
    }
}

void tcp_connection_request_set_uint8(tcp_connection_request *request, const char *name, uint8_t value)
{
    int result;

    if (request == NULL || name == NULL || *name == '\0')
        return;

    result = add_uint8_to_byte_stream(request->stream, name, value);
    if (!result)
    {
        if (request->error_number == 501 && request->error_string != NULL)
        {
            free(request->error_string);
            request->error_string = NULL;
        }
        request->error_number = 500;
    }
}

void tcp_connection_request_set_uint16(tcp_connection_request *request, const char *name, uint16_t value)
{
    int result;

    if (request == NULL || name == NULL || *name == '\0')
        return;

    result = add_uint16_to_byte_stream(request->stream, name, value);
    if (!result)
    {
        if (request->error_number == 501 && request->error_string != NULL)
        {
            free(request->error_string);
            request->error_string = NULL;
        }
        request->error_number = 500;
    }
}

void tcp_connection_request_set_uint32(tcp_connection_request *request, const char *name, uint32_t value)
{
    int result;

    if (request == NULL || name == NULL || *name == '\0')
        return;

    result = add_uint32_to_byte_stream(request->stream, name, value);
    if (!result)
    {
        if (request->error_number == 501 && request->error_string != NULL)
        {
            free(request->error_string);
            request->error_string = NULL;
        }
        request->error_number = 500;
    }
}

void tcp_connection_request_set_uint64(tcp_connection_request *request, const char *name, uint64_t value)
{
    int result;

    if (request == NULL || name == NULL || *name == '\0')
        return;

    result = add_uint64_to_byte_stream(request->stream, name, value);
    if (!result)
    {
        if (request->error_number == 501 && request->error_string != NULL)
        {
            free(request->error_string);
            request->error_string = NULL;
        }
        request->error_number = 500;
    }
}

void tcp_connection_request_set_float(tcp_connection_request *request, const char *name, float value)
{
    int result;

    if (request == NULL || name == NULL || *name == '\0')
        return;

    result = add_float_to_byte_stream(request->stream, name, value);
    if (!result)
    {
        if (request->error_number == 501 && request->error_string != NULL)
        {
            free(request->error_string);
            request->error_string = NULL;
        }
        request->error_number = 500;
    }
}

void tcp_connection_request_set_double(tcp_connection_request *request, const char *name, double value)
{
    int result;

    if (request == NULL || name == NULL || *name == '\0')
        return;

    result = add_double_to_byte_stream(request->stream, name, value);
    if (!result)
    {
        if (request->error_number == 501 && request->error_string != NULL)
        {
            free(request->error_string);
            request->error_string = NULL;
        }
        request->error_number = 500;
    }
}

void tcp_connection_request_set_long_double(tcp_connection_request *request, const char *name, long double value)
{
    int result;

    if (request == NULL || name == NULL || *name == '\0')
        return;

    result = add_long_double_to_byte_stream(request->stream, name, value);
    if (!result)
    {
        if (request->error_number == 501 && request->error_string != NULL)
        {
            free(request->error_string);
            request->error_string = NULL;
        }
        request->error_number = 500;
    }
}

int tcp_connection_response_name_exists(tcp_connection_response *response, const char *name)
{
    uint32_t i;
    byte_stream_element *element;
    int found;
    uint32_t elements_count;

    if (response == NULL || name == NULL || *name == '\0' || response->elements == NULL)
        return 0; // for false / failure

    elements_count = get_byte_stream_elements_count(response->stream);
    found = 0;
    for (i = 0; i < elements_count; i++)
    {
        element = get_byte_stream_element(response->elements, i);
        if (element != NULL)
        {
            if (is_get_byte_stream_element_name(element, name))
            {
                found = 1;
                release_byte_stream_element(element);
                break;
            }
            release_byte_stream_element(element);
        }
    }
    return found;
}

// internal utility method , hence prototype not added to header file
byte_stream_element *get_tcp_connection_response_byte_stream_element_by_name(tcp_connection_response *response, const char *name)
{
    byte_stream_element *element;
    uint32_t i, elements_count;

    if (response == NULL || name == NULL || *name == '\0' || response->elements == NULL)
        return NULL;

    elements_count = get_byte_stream_elements_count(response->stream);
    for (i = 0; i < elements_count; i++)
    {
        element = get_byte_stream_element(response->elements, i);
        if (element == NULL)
            continue;
        if (is_get_byte_stream_element_name(element, name))
        {
            break;
        }
        release_byte_stream_element(element);
        element = NULL;
    }
    return element;
}

char *tcp_connection_response_get_string(tcp_connection_response *response, const char *name)
{
    char *value;
    byte_stream_element *element;

    if (response == NULL || name == NULL || *name == '\0' || response->elements == NULL)
        return NULL;

    element = get_tcp_connection_response_byte_stream_element_by_name(response, name);
    value = NULL;

    if (element == NULL)
    {
        if (response->error_number == 501 && response->error_string != NULL)
        {
            free(response->error_string);
            response->error_string = NULL;
        }
        response->error_number = 502; // name not present
        return value;
    }

    if (!is_byte_stream_element_string(element))
    {
        if (response->error_number == 501 && response->error_string != NULL)
        {
            free(response->error_string);
            response->error_string = NULL;
        }
        response->error_number = 503; // type is not as required
        return value;
    }

    if (get_byte_stream_element_string(element, &value))
    {
        return value;
    }
    else
    {
        if (response->error_number == 501 && response->error_string != NULL)
        {
            free(response->error_string);
            response->error_string = NULL;
        }
        response->error_number = 500;
        return NULL;
    }
}

char tcp_connection_response_get_char(tcp_connection_response *response, const char *name)
{
    char value;
    byte_stream_element *element;

    if (response == NULL || name == NULL || *name == '\0' || response->elements == NULL)
        return (char)0;

    element = get_tcp_connection_response_byte_stream_element_by_name(response, name);
    value = (char)0;

    if (element == NULL)
    {
        if (response->error_number == 501 && response->error_string != NULL)
        {
            free(response->error_string);
            response->error_string = NULL;
        }
        response->error_number = 502; // name not present
        return value;
    }

    if (!is_byte_stream_element_char(element))
    {
        if (response->error_number == 501 && response->error_string != NULL)
        {
            free(response->error_string);
            response->error_string = NULL;
        }
        response->error_number = 503; // type is not as required
        return value;
    }

    if (get_byte_stream_element_char(element, &value))
    {
        return value;
    }
    else
    {
        if (response->error_number == 501 && response->error_string != NULL)
        {
            free(response->error_string);
            response->error_string = NULL;
        }
        response->error_number = 500;
        return (char)0;
    }
}

int8_t tcp_connection_response_get_int8(tcp_connection_response *response, const char *name)
{
    int8_t value;
    byte_stream_element *element;

    if (response == NULL || name == NULL || *name == '\0' || response->elements == NULL)
        return 0;

    element = get_tcp_connection_response_byte_stream_element_by_name(response, name);
    value = 0;

    if (element == NULL)
    {
        if (response->error_number == 501 && response->error_string != NULL)
        {
            free(response->error_string);
            response->error_string = NULL;
        }
        response->error_number = 502; // name not present
        return value;
    }

    if (!is_byte_stream_element_int8(element))
    {
        if (response->error_number == 501 && response->error_string != NULL)
        {
            free(response->error_string);
            response->error_string = NULL;
        }
        response->error_number = 503; // type is not as required
        return value;
    }

    if (get_byte_stream_element_int8(element, &value))
    {
        return value;
    }
    else
    {
        if (response->error_number == 501 && response->error_string != NULL)
        {
            free(response->error_string);
            response->error_string = NULL;
        }
        response->error_number = 500;
        return 0;
    }
}

int16_t tcp_connection_response_get_int16(tcp_connection_response *response, const char *name)
{
    int16_t value;
    byte_stream_element *element;

    if (response == NULL || name == NULL || *name == '\0' || response->elements == NULL)
        return 0;

    element = get_tcp_connection_response_byte_stream_element_by_name(response, name);
    value = 0;

    if (element == NULL)
    {
        if (response->error_number == 501 && response->error_string != NULL)
        {
            free(response->error_string);
            response->error_string = NULL;
        }
        response->error_number = 502; // name not present
        return value;
    }

    if (!is_byte_stream_element_int16(element))
    {
        if (response->error_number == 501 && response->error_string != NULL)
        {
            free(response->error_string);
            response->error_string = NULL;
        }
        response->error_number = 503; // type is not as required
        return value;
    }

    if (get_byte_stream_element_int16(element, &value))
    {
        return value;
    }
    else
    {
        if (response->error_number == 501 && response->error_string != NULL)
        {
            free(response->error_string);
            response->error_string = NULL;
        }
        response->error_number = 500;
        return 0;
    }
}

int32_t tcp_connection_response_get_int32(tcp_connection_response *response, const char *name)
{
    int32_t value;
    byte_stream_element *element;

    if (response == NULL || name == NULL || *name == '\0' || response->elements == NULL)
        return 0;

    element = get_tcp_connection_response_byte_stream_element_by_name(response, name);
    value = 0;

    if (element == NULL)
    {
        if (response->error_number == 501 && response->error_string != NULL)
        {
            free(response->error_string);
            response->error_string = NULL;
        }
        response->error_number = 502; // name not present
        return value;
    }

    if (!is_byte_stream_element_int32(element))
    {
        if (response->error_number == 501 && response->error_string != NULL)
        {
            free(response->error_string);
            response->error_string = NULL;
        }
        response->error_number = 503; // type is not as required
        return value;
    }

    if (get_byte_stream_element_int32(element, &value))
    {
        return value;
    }
    else
    {
        if (response->error_number == 501 && response->error_string != NULL)
        {
            free(response->error_string);
            response->error_string = NULL;
        }
        response->error_number = 500;
        return 0;
    }
}

int64_t tcp_connection_response_get_int64(tcp_connection_response *response, const char *name)
{
    int64_t value;
    byte_stream_element *element;

    if (response == NULL || name == NULL || *name == '\0' || response->elements == NULL)
        return 0;

    element = get_tcp_connection_response_byte_stream_element_by_name(response, name);
    value = 0;

    if (element == NULL)
    {
        if (response->error_number == 501 && response->error_string != NULL)
        {
            free(response->error_string);
            response->error_string = NULL;
        }
        response->error_number = 502; // name not present
        return value;
    }

    if (!is_byte_stream_element_int64(element))
    {
        if (response->error_number == 501 && response->error_string != NULL)
        {
            free(response->error_string);
            response->error_string = NULL;
        }
        response->error_number = 503; // type is not as required
        return value;
    }

    if (get_byte_stream_element_int64(element, &value))
    {
        return value;
    }
    else
    {
        if (response->error_number == 501 && response->error_string != NULL)
        {
            free(response->error_string);
            response->error_string = NULL;
        }
        response->error_number = 500;
        return 0;
    }
}

uint8_t tcp_connection_response_get_uint8(tcp_connection_response *response, const char *name)
{
    uint8_t value;
    byte_stream_element *element;

    if (response == NULL || name == NULL || *name == '\0' || response->elements == NULL)
        return 0;

    element = get_tcp_connection_response_byte_stream_element_by_name(response, name);
    value = 0;

    if (element == NULL)
    {
        if (response->error_number == 501 && response->error_string != NULL)
        {
            free(response->error_string);
            response->error_string = NULL;
        }
        response->error_number = 502; // name not present
        return value;
    }

    if (!is_byte_stream_element_uint8(element))
    {
        if (response->error_number == 501 && response->error_string != NULL)
        {
            free(response->error_string);
            response->error_string = NULL;
        }
        response->error_number = 503; // type is not as required
        return value;
    }

    if (get_byte_stream_element_uint8(element, &value))
    {
        return value;
    }
    else
    {
        if (response->error_number == 501 && response->error_string != NULL)
        {
            free(response->error_string);
            response->error_string = NULL;
        }
        response->error_number = 500;
        return 0;
    }
}

uint16_t tcp_connection_response_get_uint16(tcp_connection_response *response, const char *name)
{
    uint16_t value;
    byte_stream_element *element;

    if (response == NULL || name == NULL || *name == '\0' || response->elements == NULL)
        return 0;

    element = get_tcp_connection_response_byte_stream_element_by_name(response, name);
    value = 0;

    if (element == NULL)
    {
        if (response->error_number == 501 && response->error_string != NULL)
        {
            free(response->error_string);
            response->error_string = NULL;
        }
        response->error_number = 502; // name not present
        return value;
    }

    if (!is_byte_stream_element_uint16(element))
    {
        if (response->error_number == 501 && response->error_string != NULL)
        {
            free(response->error_string);
            response->error_string = NULL;
        }
        response->error_number = 503; // type is not as required
        return value;
    }

    if (get_byte_stream_element_uint16(element, &value))
    {
        return value;
    }
    else
    {
        if (response->error_number == 501 && response->error_string != NULL)
        {
            free(response->error_string);
            response->error_string = NULL;
        }
        response->error_number = 500;
        return 0;
    }
}

uint32_t tcp_connection_response_get_uint32(tcp_connection_response *response, const char *name)
{
    uint32_t value;
    byte_stream_element *element;

    if (response == NULL || name == NULL || *name == '\0' || response->elements == NULL)
        return 0;

    element = get_tcp_connection_response_byte_stream_element_by_name(response, name);
    value = 0;

    if (element == NULL)
    {
        if (response->error_number == 501 && response->error_string != NULL)
        {
            free(response->error_string);
            response->error_string = NULL;
        }
        response->error_number = 502; // name not present
        return value;
    }

    if (!is_byte_stream_element_uint32(element))
    {
        if (response->error_number == 501 && response->error_string != NULL)
        {
            free(response->error_string);
            response->error_string = NULL;
        }
        response->error_number = 503; // type is not as required
        return value;
    }

    if (get_byte_stream_element_uint32(element, &value))
    {
        return value;
    }
    else
    {
        if (response->error_number == 501 && response->error_string != NULL)
        {
            free(response->error_string);
            response->error_string = NULL;
        }
        response->error_number = 500;
        return 0;
    }
}

uint64_t tcp_connection_response_get_uint64(tcp_connection_response *response, const char *name)
{
    uint64_t value;
    byte_stream_element *element;

    if (response == NULL || name == NULL || *name == '\0' || response->elements == NULL)
        return 0;

    element = get_tcp_connection_response_byte_stream_element_by_name(response, name);
    value = 0;

    if (element == NULL)
    {
        if (response->error_number == 501 && response->error_string != NULL)
        {
            free(response->error_string);
            response->error_string = NULL;
        }
        response->error_number = 502; // name not present
        return value;
    }

    if (!is_byte_stream_element_uint64(element))
    {
        if (response->error_number == 501 && response->error_string != NULL)
        {
            free(response->error_string);
            response->error_string = NULL;
        }
        response->error_number = 503; // type is not as required
        return value;
    }

    if (get_byte_stream_element_uint64(element, &value))
    {
        return value;
    }
    else
    {
        if (response->error_number == 501 && response->error_string != NULL)
        {
            free(response->error_string);
            response->error_string = NULL;
        }
        response->error_number = 500;
        return 0;
    }
}

float tcp_connection_response_get_float(tcp_connection_response *response, const char *name)
{
    float value;
    byte_stream_element *element;

    if (response == NULL || name == NULL || *name == '\0' || response->elements == NULL)
        return 0.0f;

    element = get_tcp_connection_response_byte_stream_element_by_name(response, name);
    value = 0.0f;

    if (element == NULL)
    {
        if (response->error_number == 501 && response->error_string != NULL)
        {
            free(response->error_string);
            response->error_string = NULL;
        }
        response->error_number = 502; // name not present
        return value;
    }

    if (!is_byte_stream_element_float(element))
    {
        if (response->error_number == 501 && response->error_string != NULL)
        {
            free(response->error_string);
            response->error_string = NULL;
        }
        response->error_number = 503; // type is not as required
        return value;
    }

    if (get_byte_stream_element_float(element, &value))
    {
        return value;
    }
    else
    {
        if (response->error_number == 501 && response->error_string != NULL)
        {
            free(response->error_string);
            response->error_string = NULL;
        }
        response->error_number = 500;
        return 0.0f;
    }
}

double tcp_connection_response_get_double(tcp_connection_response *response, const char *name)
{
    double value;
    byte_stream_element *element;

    if (response == NULL || name == NULL || *name == '\0' || response->elements == NULL)
        return 0.0;

    element = get_tcp_connection_response_byte_stream_element_by_name(response, name);
    value = 0.0;

    if (element == NULL)
    {
        if (response->error_number == 501 && response->error_string != NULL)
        {
            free(response->error_string);
            response->error_string = NULL;
        }
        response->error_number = 502; // name not present
        return value;
    }

    if (!is_byte_stream_element_double(element))
    {
        if (response->error_number == 501 && response->error_string != NULL)
        {
            free(response->error_string);
            response->error_string = NULL;
        }
        response->error_number = 503; // type is not as required
        return value;
    }

    if (get_byte_stream_element_double(element, &value))
    {
        return value;
    }
    else
    {
        if (response->error_number == 501 && response->error_string != NULL)
        {
            free(response->error_string);
            response->error_string = NULL;
        }
        response->error_number = 500;
        return 0.0;
    }
}

long double tcp_connection_response_get_long_double(tcp_connection_response *response, const char *name)
{
    long double value;
    byte_stream_element *element;

    if (response == NULL || name == NULL || *name == '\0' || response->elements == NULL)
        return 0.0L;

    element = get_tcp_connection_response_byte_stream_element_by_name(response, name);
    value = 0.0L;

    if (element == NULL)
    {
        if (response->error_number == 501 && response->error_string != NULL)
        {
            free(response->error_string);
            response->error_string = NULL;
        }
        response->error_number = 502; // name not present
        return value;
    }

    if (!is_byte_stream_element_long_double(element))
    {
        if (response->error_number == 501 && response->error_string != NULL)
        {
            free(response->error_string);
            response->error_string = NULL;
        }
        response->error_number = 503; // type is not as required
        return value;
    }

    if (get_byte_stream_element_long_double(element, &value))
    {
        return value;
    }
    else
    {
        if (response->error_number == 501 && response->error_string != NULL)
        {
            free(response->error_string);
            response->error_string = NULL;
        }
        response->error_number = 500;
        return 0.0L;
    }
}

void release_tcp_connection_request(tcp_connection_request *request)
{
    if (request == NULL)
        return;

    release_byte_stream(request->stream);
    if (request->action_name)
        free(request->action_name);

    if (request->error_number == 501 && request->error_string != NULL)
    {
        free(request->error_string);
        request->error_string = NULL;
    }

    free(request);
}

int tcp_connection_request_failed(tcp_connection_request *request)
{
    if (request == NULL || request->error_number != 0)
        return 1; // yes
    return 0;     // no
}

void tcp_connection_request_error(tcp_connection_request *request, char **error_string)
{
    char *error500 = "Low Memory";
    char *error504 = "Request is Empty";
    char *error505 = "Action name not set";
    char *error506 = "Action name already set";

    if (request == NULL || error_string == NULL)
        return;
    if (request->error_number == 0)
    {
        *error_string = NULL;
        return;
    }
    if (request->error_number == 501)
    {
        *error_string = request->error_string;
        request->error_string = NULL;
    }
    else if (request->error_number == 500)
    {
        *error_string = (char *)malloc(sizeof(char) * (strlen(error500) + 1));
        if (*error_string != NULL)
        {
            strcpy(*error_string, error500);
        }
    }
    else if (request->error_number == 504)
    {
        *error_string = (char *)malloc(sizeof(char) * (strlen(error504) + 1));
        if (*error_string != NULL)
        {
            strcpy(*error_string, error504);
        }
    }
    else if (request->error_number == 505)
    {
        *error_string = (char *)malloc(sizeof(char) * (strlen(error505) + 1));
        if (*error_string != NULL)
        {
            strcpy(*error_string, error505);
        }
    }
    else if (request->error_number == 506)
    {
        *error_string = (char *)malloc(sizeof(char) * (strlen(error506) + 1));
        if (*error_string != NULL)
        {
            strcpy(*error_string, error506);
        }
    }
    else
    {
        *error_string = NULL; // ideally this should not happen
    }
}

void release_tcp_connection_response(tcp_connection_response *response)
{
    if (response == NULL)
        return;

    release_byte_stream_elements(response->elements);
    release_byte_stream(response->stream);
    if (response->error_number == 501 && response->error_string != NULL)
    {
        free(response->error_string);
        response->error_string = NULL;
    }
    free(response);
}

int tcp_connection_response_failed(tcp_connection_response *response)
{
    if (response == NULL || response->error_number != 0)
        return 1; // yes
    return 0;     // no
}

void tcp_connection_response_error(tcp_connection_response *response, char **error_string)
{
    char *error500 = "Low Memory";
    char *error502 = "Name not present in response";
    char *error503 = "Type not as desired in response";

    if (response == NULL || error_string == NULL)
        return;
    if (response->error_number == 0)
    {
        *error_string = NULL;
        return;
    }
    if (response->error_number == 501)
    {
        *error_string = response->error_string;
        response->error_string = NULL;
    }
    else if (response->error_number == 500)
    {
        *error_string = (char *)malloc(sizeof(char) * (strlen(error500) + 1));
        if (*error_string != NULL)
        {
            strcpy(*error_string, error500);
        }
    }
    else if (response->error_number == 502)
    {
        *error_string = (char *)malloc(sizeof(char) * (strlen(error502) + 1));
        if (*error_string != NULL)
        {
            strcpy(*error_string, error502);
        }
    }
    else if (response->error_number == 503)
    {
        *error_string = (char *)malloc(sizeof(char) * (strlen(error503) + 1));
        if (*error_string != NULL)
        {
            strcpy(*error_string, error503);
        }
    }
    else
    {
        *error_string = NULL; // ideally this should not happen
    }
}

// the following code is written by the commond layer library user frmo client side of an application , it is just a sample code , later on the following will be removed and library from cscl.c created

void print_byte_stream(byte_stream *stream)
{
    if (!stream)
        return;

    byte_stream_elements *elements = get_byte_stream_elements(stream);
    if (!elements)
        return;

    uint32_t count = get_byte_stream_elements_count(stream);
    for (uint32_t i = 0; i < count; i++)
    {
        byte_stream_element *elem = get_byte_stream_element(elements, i);
        if (!elem)
            continue;

        char *name = get_byte_stream_element_name(elem);
        if (!name)
            continue;

        printf("Element %u: Name = %s, ", i + 1, name);

        if (is_byte_stream_element_string(elem))
        {
            char *val;
            if (get_byte_stream_element_string(elem, &val))
            {
                printf("Type = STRING, Value = %s", val);
                free(val);
            }
        }
        else if (is_byte_stream_element_char(elem))
        {
            char c;
            if (get_byte_stream_element_char(elem, &c))
            {
                printf("Type = CHAR, Value = %c", c);
            }
        }
        else if (is_byte_stream_element_int32(elem))
        {
            int32_t val;
            if (get_byte_stream_element_int32(elem, &val))
            {
                printf("Type = INT32, Value = %d", val);
            }
        }
        else if (is_byte_stream_element_uint32(elem))
        {
            uint32_t val;
            if (get_byte_stream_element_uint32(elem, &val))
            {
                printf("Type = UINT32, Value = %u", val);
            }
        }
        // Add other types similarly if needed (float, double, int64, etc.)
        printf("\n");

        release_byte_stream_element(elem);
        free(name);
    }

    release_byte_stream_elements(elements);
}

void add_student()
{
    uint32_t succeeded;
    unsigned int rollNumber;
    char name[22];
    unsigned int age;
    char gender;
    char m;
    tcp_connection *connection;
    char *err_str;
    tcp_connection_request *request;
    tcp_connection_response *response;

    printf("Student Master (Add)\n");
    printf("Enter roll number : ");
    scanf("%u", &rollNumber);
    fpurge(stdin);

    printf("Enter name : ");
    fgets(name, 22, stdin);
    fpurge(stdin);
    name[strlen(name) - 1] = '\0';

    printf("Enter gender : ");
    gender = getchar();
    fpurge(stdin);

    printf("Enter age : ");
    scanf("%u", &age);
    fpurge(stdin);

    printf("Save (Y/N) : ");
    m = getchar();
    fpurge(stdin);

    if (m != 'Y' && m != 'y')
    {
        printf("Student not saved\n");
        return;
    }

    connection = tcp_connect("localhost", 6060);

    if (tcp_connection_failed(connection))
    {
        tcp_connection_error(connection, &err_str);
        if (err_str)
        {
            printf("Unable to connect server reason : %s", err_str);
            free(err_str);
        }
        else
        {
            printf("Unable to connect server\n");
        }
        release_tcp_connection(connection);
        return;
    }

    request = create_tcp_connection_request(connection);
    if (tcp_connection_request_failed(request))
    {
        tcp_connection_request_error(request, &err_str);
        if (err_str)
        {
            printf("Unablt to send request : %s\n", err_str);
            free(err_str);
        }
        else
        {
            printf("Unable to send request\n");
        }

        disconnect_tcp_connection(connection);
        release_tcp_connection(connection);
        release_tcp_connection_request(request);
        return;
    }

    tcp_connection_request_set_action_name(request, "AddStudent");
    tcp_connection_request_set_uint32(request, "RollNumber", rollNumber);
    tcp_connection_request_set_string(request, "Name", name);
    tcp_connection_request_set_char(request, "Gender", gender);
    tcp_connection_request_set_uint32(request, "Age", age);

    // after above 5 calls , ideally failure should be checked by calling tcp_connection_request_failed
    tcp_connection_send_request(connection, request);
    if (tcp_connection_request_failed(request))
    {

        tcp_connection_request_error(request, &err_str);
        if (err_str)
        {
            printf("Unable to send request : %s\n", err_str);
            free(err_str);
        }
        else
        {
            printf("Unable to send request\n");
        }

        disconnect_tcp_connection(connection);
        release_tcp_connection(connection);
        release_tcp_connection_request(request);
        return;
    }

    release_tcp_connection_request(request);
    // request send , now lets process the response

    response = tcp_connection_receive_response(connection);
    if (tcp_connection_response_failed(response))
    {
        tcp_connection_response_error(response, &err_str);
        if (err_str)
        {
            printf("Unable to receive response , reason : %s\n", err_str);
        }
        else
        {
            printf("Unable to receive response\n");
        }
        disconnect_tcp_connection(connection);
        release_tcp_connection(connection);
        release_tcp_connection_response(response);
        return;
    }

    if (tcp_connection_response_name_exists(response, "succeeded"))
    {
        succeeded = tcp_connection_response_get_int32(response, "succeeded");
        if (succeeded)
        {
            printf("Student Added\n");
        }
        else
        {
            err_str = tcp_connection_response_get_string(response, "exception");
            if (err_str)
            {
                printf("Unable to add student , reason : %s\n", err_str);
                free(err_str);
            }
            else
            {
                printf("Unable to add student\n");
            }
        }
    }
    else
    {
        printf("Invalid response\n");
    }
    disconnect_tcp_connection(connection);
    release_tcp_connection(connection);
    release_tcp_connection_response(response);
}

void edit_student()
{

    // ask for roll number
    // connect
    //  prepare request will roll number and action name as GetStudetn
    // send request and receive response
    // if succeeded is 0 the print exception and story ends
    // if succeeded is 1 then
    //              extract anme , age and gender from response
    //              print name , age and gender
    //              ask if want to edit , if no then story ends
    //              if yes then procceds
    //              ask for new name , new age and new gender
    //              prepare request with rollnumber , name , gender ,age . Set the action name to UpdateStudent
    //              send request
    //              receive response and parse the succeeded part as done earlier
    //      Note : dont forget to disconnect and release resources
}
void delete_student()
{
    // ask for roll number
    // connect
    //  prepare request will roll number and action name as GetStudetn
    // send request and receive response
    // if succeeded is 0 the print exception and story ends
    // if succeeded is 1 then
    //              extract anme , age and gender from response
    //              print name , age and gender
    //              ask if want to delete , if no then story ends
    //              if yes then procceds
    //              prepare request with rollnumber . Set the action name to DeleteStudent
    //              send request
    //              receive response and parse the succeeded part as done earlier
    //      Note : dont forget to disconnect and release resources
}
void search_student()
{
    uint32_t succeeded;
    unsigned int rollNumber;
    char *name;
    uint32_t age;
    char gender;
    char m;
    tcp_connection *connection;
    char *err_str;
    tcp_connection_request *request;
    tcp_connection_response *response;

    printf("Student Master (Search)\n");
    printf("Enter roll number of the student to search for : ");
    scanf("%u", &rollNumber);
    fpurge(stdin);

    connection = tcp_connect("localhost", 6060);

    if (tcp_connection_failed(connection))
    {
        tcp_connection_error(connection, &err_str);
        if (err_str)
        {
            printf("Unable to connect server reason : %s", err_str);
            free(err_str);
        }
        else
        {
            printf("Unable to connect server\n");
        }
        release_tcp_connection(connection);
        return;
    }

    request = create_tcp_connection_request(connection);
    if (tcp_connection_request_failed(request))
    {
        tcp_connection_request_error(request, &err_str);
        if (err_str)
        {
            printf("Unablt to send request : %s\n", err_str);
            free(err_str);
        }
        else
        {
            printf("Unable to send request\n");
        }

        disconnect_tcp_connection(connection);
        release_tcp_connection(connection);
        release_tcp_connection_request(request);
        return;
    }

    tcp_connection_request_set_action_name(request, "GetStudent");
    tcp_connection_request_set_uint32(request, "RollNumber", rollNumber);

    tcp_connection_send_request(connection, request);
    if (tcp_connection_request_failed(request))
    {
        tcp_connection_request_error(request, &err_str);
        if (err_str)
        {
            printf("Unable to send request : %s\n", err_str);
            free(err_str);
        }
        else
        {
            printf("Unable to send request\n");
        }

        disconnect_tcp_connection(connection);
        release_tcp_connection(connection);
        release_tcp_connection_request(request);
        return;
    }

    release_tcp_connection_request(request);
    // request send , now lets process the response

    response = tcp_connection_receive_response(connection);
    if (tcp_connection_response_failed(response))
    {
        tcp_connection_response_error(response, &err_str);
        if (err_str)
        {
            printf("Unable to receive response , reason : %s\n", err_str);
        }
        else
        {
            printf("Unable to receive response\n");
        }
        disconnect_tcp_connection(connection);
        release_tcp_connection(connection);
        release_tcp_connection_response(response);
        return;
    }

    if (tcp_connection_response_name_exists(response, "succeeded"))
    {
        succeeded = tcp_connection_response_get_int32(response, "succeeded");
        if (succeeded)
        {
            name = tcp_connection_response_get_string(response, "name");
            gender = tcp_connection_response_get_char(response, "gender");
            age = tcp_connection_response_get_uint32(response, "age");
            printf("Name : %s\n", name);
            if (gender == 'M')
                printf("Gender : Male\n");
            if (gender == 'F')
                printf("Gender : Female\n");
            printf("Age : %" PRIu32 "\n", age);
            free(name);
            printf("Press Enter to continue\n");
            getchar();
            fpurge(stdin);
        }
        else
        {
            err_str = tcp_connection_response_get_string(response, "exception");
            if (err_str)
            {
                printf("Unable to get student , reason : %s\n", err_str);
                free(err_str);
                printf("Press Enter to continue\n");
                getchar();
                fpurge(stdin);
            }
            else
            {
                printf("Unable to get student\n");
            }
        }
    }
    else
    {
        printf("Invalid response\n");
    }
    disconnect_tcp_connection(connection);
    release_tcp_connection(connection);
    release_tcp_connection_response(response);
}

void display_list_of_student()
{
    tcp_connection *connection;
    char *error_string;
    tcp_connection_request *request;
    tcp_connection_response *response;
    uint32_t succeeded;
    uint32_t number_of_student;
    uint32_t roll_number;
    char *name;
    char gender;
    uint32_t age;
    char field_name[21];
    int x;

    connection = tcp_connect("localhost", 6060);

    if (tcp_connection_failed(connection))
    {
        tcp_connection_error(connection, &error_string);
        if (error_string)
        {
            printf("Unable to connect server reason : %s", error_string);
            free(error_string);
        }
        else
        {
            printf("Unable to connect server\n");
        }
        release_tcp_connection(connection);
        return;
    }

    request = create_tcp_connection_request(connection);
    if (tcp_connection_request_failed(request))
    {

        tcp_connection_request_error(request, &error_string);
        if (error_string)
        {
            printf("Unable to send request : %s\n", error_string);
            free(error_string);
        }
        else
        {
            printf("Unable to send request\n");
        }

        disconnect_tcp_connection(connection);
        release_tcp_connection(connection);
        release_tcp_connection_request(request);
        return;
    }

    tcp_connection_request_set_action_name(request, "GetAllStudents");
    tcp_connection_send_request(connection, request);

    if (tcp_connection_request_failed(request))
    {

        tcp_connection_request_error(request, &error_string);
        if (error_string)
        {
            printf("Unable to send request : %s\n", error_string);
            free(error_string);
        }
        else
        {
            printf("Unable to send request\n");
        }

        disconnect_tcp_connection(connection);
        release_tcp_connection(connection);
        release_tcp_connection_request(request);
        return;
    }

    release_tcp_connection_request(request);

    response = tcp_connection_receive_response(connection);
    // printf("\n");
    // print_byte_stream(request->stream);
    if (tcp_connection_response_failed(response))
    {
        tcp_connection_response_error(response, &error_string);
        if (error_string)
        {
            printf("Unable to receive response , reason : %s\n", error_string);
        }
        else
        {
            printf("Unable to receive response\n");
        }
        disconnect_tcp_connection(connection);
        release_tcp_connection(connection);
        release_tcp_connection_response(response);
        return;
    }

    succeeded = tcp_connection_response_get_int32(response, "succeeded");
    // printf("Correct %d\n", succeeded);
    if (succeeded)
    {
        number_of_student = tcp_connection_response_get_int32(response, "number_of_student");
        x = 1;
        while (x <= number_of_student)
        {
            sprintf(field_name, "roll_number_%d", x);
            roll_number = tcp_connection_response_get_uint32(response, field_name);
            sprintf(field_name, "name_%d", x);
            name = tcp_connection_response_get_string(response, field_name);
            sprintf(field_name, "gender_%d", x);
            gender = tcp_connection_response_get_char(response, field_name);
            sprintf(field_name, "age_%d", x);
            age = tcp_connection_response_get_uint32(response, field_name);
            printf("%d", x);
            printf(",%" PRIu32, roll_number);
            printf(",%s", name);
            printf(",%c", gender);
            printf(",%" PRIu32, age);
            printf("\n");
            x++;
        }
        if (number_of_student == 0)
        {
            printf("No student added\n");
        }
    }
    else
    {
        error_string = tcp_connection_response_get_string(response, "exception");
        printf("Unable to fetch students, reason : %s\n", error_string);
        free(error_string);
    }

    disconnect_tcp_connection(connection);
    release_tcp_connection(connection);
    release_tcp_connection_response(response);
}

int main()
{
    int choice;
    while (1)
    {
        printf("\nStudent Master\n");
        printf("----------------------------\n");
        printf("1. Add\n");
        printf("2. Edit\n");
        printf("3. Delete\n");
        printf("4. Search\n");
        printf("5. List\n");
        printf("6. Exit\n");

        printf("Enter your choice : ");
        scanf("%d", &choice);
        fpurge(stdin);

        if (choice == 1)
            add_student();
        else if (choice == 2)
            edit_student();
        else if (choice == 3)
            delete_student();
        else if (choice == 4)
            search_student();
        else if (choice == 5)
            display_list_of_student();
        else if (choice == 6)
            break;
        else
        {
            printf("Invalid Choice\n");
            continue;
        }
    }
    return 0;
}