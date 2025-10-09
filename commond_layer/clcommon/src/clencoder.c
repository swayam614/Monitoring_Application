#include <clencoder.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h> // to be removed
#include <inttypes.h>

#define char_e 'A'
#define string_e 'B'
#define int8_e 'C'
#define int16_e 'D'
#define int32_e 'E'
#define int64_e 'F'
#define uint8_e 'G'
#define uint16_e 'H'
#define uint32_e 'I'
#define uint64_e 'J'
#define float_e 'K'
#define double_e 'L'
#define long_double_e 'M'

typedef struct _byte_stream
{
    char *buffer;
    uint32_t len;
    uint32_t elements_count;
} byte_stream;

typedef struct _byte_stream_elements
{
    byte_stream *stream;
    char **elements_ptr;
} byte_stream_elements;

typedef struct _byte_stream_element
{
    char type;
    uint32_t name_len;
    char *name;
    uint32_t value_len;
    char *value;
} byte_stream_element;

int is_big_endian()
{
    return __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__;
}
int is_little_endian()
{
    return __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__;
}

uint64_t htonl64(uint64_t v)
{
    if (is_big_endian())
        return v;
    return (((uint64_t)htonl(v & 0xFFFFFFFF)) << 32) | htonl(v >> 32);
}

uint64_t ntohl64(uint64_t v)
{
    if (is_big_endian())
        return v;
    return (((uint64_t)htonl(v & 0xFFFFFFFF)) << 32) | htonl(v >> 32);
}

uint32_t get_type_size(char type)
{
    if (type == char_e)
        return sizeof(char);
    else if (type == int8_e)
        return sizeof(int8_t);
    else if (type == int16_e)
        return sizeof(int16_t);
    else if (type == int32_e)
        return sizeof(int32_t);
    else if (type == int64_e)
        return sizeof(int64_t);
    else if (type == uint8_e)
        return sizeof(uint8_t);
    else if (type == uint16_e)
        return sizeof(uint16_t);
    else if (type == uint32_e)
        return sizeof(uint32_t);
    else if (type == uint64_e)
        return sizeof(uint64_t);
    else
        return 0;
}

int is_byte_stream_type_valid(char type)
{
    return type >= 'A' && type <= 'M';
}

void update_byte_stream_header(byte_stream *stream, uint32_t len, uint32_t elements_count)
{
    uint32_t norder_len;
    uint32_t norder_elements_count;

    if (stream == NULL)
        return;

    norder_len = htonl(len);
    norder_elements_count = htonl(elements_count);
    memcpy(stream->buffer, &norder_len, sizeof(uint32_t));
    memcpy(stream->buffer + sizeof(uint32_t), &norder_elements_count, sizeof(uint32_t));
    stream->len = len;
    stream->elements_count = elements_count;
}

byte_stream *create_byte_stream()
{
    uint32_t norder_len;
    byte_stream *stream;
    stream = (byte_stream *)malloc(sizeof(byte_stream));
    if (stream == NULL)
        return NULL;

    stream->buffer = (char *)malloc(sizeof(uint32_t) + sizeof(uint32_t));
    if (stream->buffer == NULL)
    {
        free(stream);
        return NULL;
    }

    stream->len = sizeof(uint32_t) + sizeof(uint32_t);
    norder_len = htonl(stream->len);
    memset(stream->buffer, 0, stream->len);
    memcpy(stream->buffer, &norder_len, sizeof(uint32_t));
    return stream;
}

byte_stream *create_byte_stream_from_bytes(char *bytes, uint32_t length)
{
    byte_stream *stream;
    uint32_t norder_len;
    uint32_t norder_elements_count;
    uint32_t len;
    uint32_t elements_count;

    memcpy(&norder_len, bytes, sizeof(uint32_t));
    len = ntohl(norder_len);
    if (len != length)
        return NULL; // something is invalid about the stream

    memcpy(&norder_elements_count, bytes + sizeof(uint32_t), sizeof(uint32_t));
    elements_count = ntohl(norder_elements_count);
    if (elements_count < 0)
        return NULL;
    if (elements_count == 0 && length > (sizeof(uint32_t) + sizeof(uint32_t)))
    {
        return NULL;
    } // more code to check the valiadity of the stream , implement it later

    stream = (byte_stream *)malloc(sizeof(byte_stream));
    if (stream == NULL)
        return NULL;

    stream->buffer = bytes; // Now the owner the library creator the user should not free bytes
    stream->len = length;
    stream->elements_count = elements_count;
    return stream;
}

char *get_byte_stream_bytes(byte_stream *stream)
{
    if (stream == NULL)
        return NULL;
    return stream->buffer;
}

uint32_t get_byte_stream_length(byte_stream *stream)
{
    if (stream == NULL)
        return 0;
    return stream->len;
}

void release_byte_stream(byte_stream *stream)
{
    if (stream == NULL)
        return;
    if (stream->buffer != NULL)
        free(stream->buffer);
    free(stream);
}

int add_char_to_byte_stream(byte_stream *stream, const char *name, char value)
{
    uint32_t name_len;
    uint32_t i;
    uint32_t required_len;
    uint32_t norder_name_len;
    uint32_t name_index;
    uint32_t value_index;
    char *ptr;

    if (stream == NULL || name == NULL)
        return 0; // 0 for false
    i = stream->len;
    name_len = strlen(name);
    if (name_len == 0)
        return 0; // 0 for false
    required_len = i + sizeof(char) + sizeof(uint32_t) + name_len + sizeof(char);

    ptr = (char *)realloc(stream->buffer, required_len);
    if (ptr == NULL)
        return 0; // for false

    norder_name_len = htonl(name_len);
    name_index = i + sizeof(char) + sizeof(uint32_t);
    value_index = name_index + name_len;
    stream->buffer = ptr;
    stream->buffer[i] = char_e;
    memcpy(stream->buffer + i + sizeof(char), &norder_name_len, sizeof(uint32_t));
    memcpy(stream->buffer + name_index, name, name_len);
    stream->buffer[value_index] = value;
    update_byte_stream_header(stream, required_len, stream->elements_count + 1);
    return 1; // for true
}

int add_string_to_byte_stream(byte_stream *stream, const char *name, const char *value)
{
    uint32_t name_len;
    uint32_t value_len;
    uint32_t i;
    uint32_t required_len;
    uint32_t norder_name_len;
    uint32_t norder_value_len;
    uint32_t name_index;
    uint32_t value_index;
    char *ptr;

    if (stream == NULL || name == NULL || value == NULL)
        return 0; // for false
    i = stream->len;
    name_len = strlen(name);
    if (name_len == 0)
        return 0; // for false;
    value_len = strlen(value);

    required_len = i + sizeof(char) + sizeof(uint32_t) + name_len + sizeof(uint32_t) + value_len;

    ptr = (char *)realloc(stream->buffer, required_len);
    if (ptr == NULL)
        return 0; // for false

    norder_name_len = htonl(name_len);
    norder_value_len = htonl(value_len);

    name_index = i + sizeof(char) + sizeof(uint32_t);
    value_index = name_index + name_len + sizeof(uint32_t);

    stream->buffer = ptr;
    stream->buffer[i] = string_e;
    memcpy(stream->buffer + i + sizeof(char), &norder_name_len, sizeof(uint32_t));
    memcpy(stream->buffer + name_index, name, name_len);
    memcpy(stream->buffer + name_index + name_len, &norder_value_len, sizeof(uint32_t));
    memcpy(stream->buffer + value_index, value, value_len);
    update_byte_stream_header(stream, required_len, stream->elements_count + 1);
    return 1;
}

int add_int8_to_byte_stream(byte_stream *stream, const char *name, int8_t value)
{
    uint32_t name_len;
    uint32_t i;
    uint32_t required_len;
    uint32_t norder_name_len;
    uint32_t name_index;
    uint32_t value_index;
    char *ptr;

    if (stream == NULL || name == NULL)
        return 0; // 0 for false
    i = stream->len;
    name_len = strlen(name);
    if (name_len == 0)
        return 0; // 0 for false
    required_len = i + sizeof(char) + sizeof(uint32_t) + name_len + sizeof(int8_t);

    ptr = (char *)realloc(stream->buffer, required_len);
    if (ptr == NULL)
        return 0; // for false

    norder_name_len = htonl(name_len);
    name_index = i + sizeof(char) + sizeof(uint32_t);
    value_index = name_index + name_len;
    stream->buffer = ptr;
    stream->buffer[i] = int8_e;
    memcpy(stream->buffer + i + sizeof(char), &norder_name_len, sizeof(uint32_t));
    memcpy(stream->buffer + name_index, name, name_len);
    memcpy(stream->buffer + value_index, &value, sizeof(int8_t));
    update_byte_stream_header(stream, required_len, stream->elements_count + 1);
    return 1; // for true
}

int add_int16_to_byte_stream(byte_stream *stream, const char *name, int16_t value)
{
    uint32_t name_len;
    uint32_t i;
    uint32_t required_len;
    uint32_t norder_name_len;
    uint32_t name_index;
    uint32_t value_index;
    uint16_t norder_value;
    char *ptr;

    if (stream == NULL || name == NULL)
        return 0; // 0 for false
    i = stream->len;
    name_len = strlen(name);
    if (name_len == 0)
        return 0; // 0 for false
    required_len = i + sizeof(char) + sizeof(uint32_t) + name_len + sizeof(int16_t);

    ptr = (char *)realloc(stream->buffer, required_len);
    if (ptr == NULL)
        return 0; // for false

    norder_name_len = htonl(name_len);
    norder_value = htons(value);
    name_index = i + sizeof(char) + sizeof(uint32_t);
    value_index = name_index + name_len;
    stream->buffer = ptr;
    stream->buffer[i] = int16_e;
    memcpy(stream->buffer + i + sizeof(char), &norder_name_len, sizeof(uint32_t));
    memcpy(stream->buffer + name_index, name, name_len);
    memcpy(stream->buffer + value_index, &norder_value, sizeof(int16_t));
    update_byte_stream_header(stream, required_len, stream->elements_count + 1);
    return 1; // for true
}

int add_int32_to_byte_stream(byte_stream *stream, const char *name, int32_t value)
{
    uint32_t name_len;
    uint32_t i;
    uint32_t required_len;
    uint32_t norder_name_len;
    uint32_t name_index;
    uint32_t value_index;
    uint32_t norder_value;
    char *ptr;

    if (stream == NULL || name == NULL)
        return 0; // 0 for false
    i = stream->len;
    name_len = strlen(name);
    if (name_len == 0)
        return 0; // 0 for false
    required_len = i + sizeof(char) + sizeof(uint32_t) + name_len + sizeof(int32_t);

    ptr = (char *)realloc(stream->buffer, required_len);
    if (ptr == NULL)
        return 0; // for false

    norder_name_len = htonl(name_len);
    norder_value = htonl(value);
    name_index = i + sizeof(char) + sizeof(uint32_t);
    value_index = name_index + name_len;
    stream->buffer = ptr;
    stream->buffer[i] = int32_e;
    memcpy(stream->buffer + i + sizeof(char), &norder_name_len, sizeof(uint32_t));
    memcpy(stream->buffer + name_index, name, name_len);
    memcpy(stream->buffer + value_index, &norder_value, sizeof(int32_t));
    update_byte_stream_header(stream, required_len, stream->elements_count + 1);
    return 1; // for true
}

int add_int64_to_byte_stream(byte_stream *stream, const char *name, int64_t value)
{
    uint32_t name_len;
    uint32_t i;
    uint32_t required_len;
    uint32_t norder_name_len;
    uint32_t name_index;
    uint32_t value_index;
    uint64_t norder_value;
    char *ptr;

    if (stream == NULL || name == NULL)
        return 0; // 0 for false
    i = stream->len;
    name_len = strlen(name);
    if (name_len == 0)
        return 0; // 0 for false
    required_len = i + sizeof(char) + sizeof(uint32_t) + name_len + sizeof(int64_t);

    ptr = (char *)realloc(stream->buffer, required_len);
    if (ptr == NULL)
        return 0; // for false

    norder_name_len = htonl(name_len);
    norder_value = htonl64(value);
    name_index = i + sizeof(char) + sizeof(uint32_t);
    value_index = name_index + name_len;
    stream->buffer = ptr;
    stream->buffer[i] = int64_e;
    memcpy(stream->buffer + i + sizeof(char), &norder_name_len, sizeof(uint32_t));
    memcpy(stream->buffer + name_index, name, name_len);
    memcpy(stream->buffer + value_index, &norder_value, sizeof(int64_t));
    update_byte_stream_header(stream, required_len, stream->elements_count + 1);
    return 1; // for true
}

int add_uint8_to_byte_stream(byte_stream *stream, const char *name, uint8_t value)
{
    uint32_t name_len;
    uint32_t i;
    uint32_t required_len;
    uint32_t norder_name_len;
    uint32_t name_index;
    uint32_t value_index;
    char *ptr;

    if (stream == NULL || name == NULL)
        return 0; // 0 for false
    i = stream->len;
    name_len = strlen(name);
    if (name_len == 0)
        return 0; // 0 for false
    required_len = i + sizeof(char) + sizeof(uint32_t) + name_len + sizeof(uint8_t);

    ptr = (char *)realloc(stream->buffer, required_len);
    if (ptr == NULL)
        return 0; // for false

    norder_name_len = htonl(name_len);
    name_index = i + sizeof(char) + sizeof(uint32_t);
    value_index = name_index + name_len;
    stream->buffer = ptr;
    stream->buffer[i] = uint8_e;
    memcpy(stream->buffer + i + sizeof(char), &norder_name_len, sizeof(uint32_t));
    memcpy(stream->buffer + name_index, name, name_len);
    memcpy(stream->buffer + value_index, &value, sizeof(uint8_t));
    update_byte_stream_header(stream, required_len, stream->elements_count + 1);
    return 1; // for true
}

int add_uint16_to_byte_stream(byte_stream *stream, const char *name, uint16_t value)
{
    uint32_t name_len;
    uint32_t i;
    uint32_t required_len;
    uint32_t norder_name_len;
    uint32_t name_index;
    uint32_t value_index;
    uint16_t norder_value;
    char *ptr;

    if (stream == NULL || name == NULL)
        return 0; // 0 for false
    i = stream->len;
    name_len = strlen(name);
    if (name_len == 0)
        return 0; // 0 for false
    required_len = i + sizeof(char) + sizeof(uint32_t) + name_len + sizeof(uint16_t);

    ptr = (char *)realloc(stream->buffer, required_len);
    if (ptr == NULL)
        return 0; // for false

    norder_name_len = htonl(name_len);
    norder_value = htons(value);
    name_index = i + sizeof(char) + sizeof(uint32_t);
    value_index = name_index + name_len;
    stream->buffer = ptr;
    stream->buffer[i] = uint16_e;
    memcpy(stream->buffer + i + sizeof(char), &norder_name_len, sizeof(uint32_t));
    memcpy(stream->buffer + name_index, name, name_len);
    memcpy(stream->buffer + value_index, &norder_value, sizeof(uint16_t));
    update_byte_stream_header(stream, required_len, stream->elements_count + 1);
    return 1; // for true
}

int add_uint32_to_byte_stream(byte_stream *stream, const char *name, uint32_t value)
{
    uint32_t name_len;
    uint32_t i;
    uint32_t required_len;
    uint32_t norder_name_len;
    uint32_t name_index;
    uint32_t value_index;
    uint32_t norder_value;
    char *ptr;

    if (stream == NULL || name == NULL)
        return 0; // 0 for false
    i = stream->len;
    name_len = strlen(name);
    if (name_len == 0)
        return 0; // 0 for false
    required_len = i + sizeof(char) + sizeof(uint32_t) + name_len + sizeof(uint32_t);

    ptr = (char *)realloc(stream->buffer, required_len);
    if (ptr == NULL)
        return 0; // for false

    norder_name_len = htonl(name_len);
    norder_value = htonl(value);
    name_index = i + sizeof(char) + sizeof(uint32_t);
    value_index = name_index + name_len;
    stream->buffer = ptr;
    stream->buffer[i] = uint32_e;
    memcpy(stream->buffer + i + sizeof(char), &norder_name_len, sizeof(uint32_t));
    memcpy(stream->buffer + name_index, name, name_len);
    memcpy(stream->buffer + value_index, &norder_value, sizeof(uint32_t));
    update_byte_stream_header(stream, required_len, stream->elements_count + 1);
    return 1; // for true
}

int add_uint64_to_byte_stream(byte_stream *stream, const char *name, uint64_t value)
{
    uint32_t name_len;
    uint32_t i;
    uint32_t required_len;
    uint32_t norder_name_len;
    uint32_t name_index;
    uint32_t value_index;
    uint64_t norder_value;
    char *ptr;

    if (stream == NULL || name == NULL)
        return 0; // 0 for false
    i = stream->len;
    name_len = strlen(name);
    if (name_len == 0)
        return 0; // 0 for false
    required_len = i + sizeof(char) + sizeof(uint32_t) + name_len + sizeof(uint64_t);

    ptr = (char *)realloc(stream->buffer, required_len);
    if (ptr == NULL)
        return 0; // for false

    norder_name_len = htonl(name_len);
    norder_value = htonl64(value);
    name_index = i + sizeof(char) + sizeof(uint32_t);
    value_index = name_index + name_len;
    stream->buffer = ptr;
    stream->buffer[i] = uint64_e;
    memcpy(stream->buffer + i + sizeof(char), &norder_name_len, sizeof(uint32_t));
    memcpy(stream->buffer + name_index, name, name_len);
    memcpy(stream->buffer + value_index, &norder_value, sizeof(uint64_t));
    update_byte_stream_header(stream, required_len, stream->elements_count + 1);
    return 1; // for true
}

int add_float_to_byte_stream(byte_stream *stream, const char *name, float value)
{
    uint32_t name_len;
    uint32_t value_len;
    uint32_t i;
    uint32_t required_len;
    uint32_t norder_name_len;
    uint32_t norder_value_len;
    uint32_t name_index;
    uint32_t value_index;
    uint32_t value_len_index;
    char *ptr;
    char fstr[2001];

    if (stream == NULL || name == NULL)
        return 0; // 0 for false
    i = stream->len;
    name_len = strlen(name);
    if (name_len == 0)
        return 0; // 0 for false

    sprintf(fstr, "%f", value); // string form representation of float type value
    value_len = strlen(fstr);
    required_len = i + sizeof(char) + sizeof(uint32_t) + name_len + sizeof(uint32_t) + value_len;

    ptr = (char *)realloc(stream->buffer, required_len);
    if (ptr == NULL)
        return 0; // for false

    norder_name_len = htonl(name_len);
    norder_value_len = htonl(value_len);
    name_index = i + sizeof(char) + sizeof(uint32_t);
    value_len_index = name_index + name_len;
    value_index = value_len_index + sizeof(uint32_t);

    stream->buffer = ptr;
    stream->buffer[i] = float_e;
    memcpy(stream->buffer + i + sizeof(char), &norder_name_len, sizeof(uint32_t));
    memcpy(stream->buffer + name_index, name, name_len);
    memcpy(stream->buffer + value_len_index, &norder_value_len, sizeof(uint32_t));
    memcpy(stream->buffer + value_index, fstr, value_len);
    update_byte_stream_header(stream, required_len, stream->elements_count + 1);
    return 1; // for true
}

int add_double_to_byte_stream(byte_stream *stream, const char *name, double value)
{
    uint32_t name_len;
    uint32_t value_len;
    uint32_t i;
    uint32_t required_len;
    uint32_t norder_name_len;
    uint32_t norder_value_len;
    uint32_t name_index;
    uint32_t value_index;
    uint32_t value_len_index;
    char *ptr;
    char dstr[2001];

    if (stream == NULL || name == NULL)
        return 0; // 0 for false
    i = stream->len;
    name_len = strlen(name);
    if (name_len == 0)
        return 0; // 0 for false

    sprintf(dstr, "%f", value); // string form representation of double type value
    value_len = strlen(dstr);
    required_len = i + sizeof(char) + sizeof(uint32_t) + name_len + sizeof(uint32_t) + value_len;

    ptr = (char *)realloc(stream->buffer, required_len);
    if (ptr == NULL)
        return 0; // for false

    norder_name_len = htonl(name_len);
    norder_value_len = htonl(value_len);
    name_index = i + sizeof(char) + sizeof(uint32_t);
    value_len_index = name_index + name_len;
    value_index = value_len_index + sizeof(uint32_t);

    stream->buffer = ptr;
    stream->buffer[i] = double_e;
    memcpy(stream->buffer + i + sizeof(char), &norder_name_len, sizeof(uint32_t));
    memcpy(stream->buffer + name_index, name, name_len);
    memcpy(stream->buffer + value_len_index, &norder_value_len, sizeof(uint32_t));
    memcpy(stream->buffer + value_index, dstr, value_len);
    update_byte_stream_header(stream, required_len, stream->elements_count + 1);
    return 1; // for true
}

int add_long_double_to_byte_stream(byte_stream *stream, const char *name, long double value)
{
    uint32_t name_len;
    uint32_t value_len;
    uint32_t i;
    uint32_t required_len;
    uint32_t norder_name_len;
    uint32_t norder_value_len;
    uint32_t name_index;
    uint32_t value_index;
    uint32_t value_len_index;
    char *ptr;
    char ldstr[2001];

    if (stream == NULL || name == NULL)
        return 0; // 0 for false
    i = stream->len;
    name_len = strlen(name);
    if (name_len == 0)
        return 0; // 0 for false

    sprintf(ldstr, "%Lf", value); // string form representation of double type value
    value_len = strlen(ldstr);
    required_len = i + sizeof(char) + sizeof(uint32_t) + name_len + sizeof(uint32_t) + value_len;

    ptr = (char *)realloc(stream->buffer, required_len);
    if (ptr == NULL)
        return 0; // for false

    norder_name_len = htonl(name_len);
    norder_value_len = htonl(value_len);
    name_index = i + sizeof(char) + sizeof(uint32_t);
    value_len_index = name_index + name_len;
    value_index = value_len_index + sizeof(uint32_t);

    stream->buffer = ptr;
    stream->buffer[i] = long_double_e;
    memcpy(stream->buffer + i + sizeof(char), &norder_name_len, sizeof(uint32_t));
    memcpy(stream->buffer + name_index, name, name_len);
    memcpy(stream->buffer + value_len_index, &norder_value_len, sizeof(uint32_t));
    memcpy(stream->buffer + value_index, ldstr, value_len);
    update_byte_stream_header(stream, required_len, stream->elements_count + 1);
    return 1; // for true
}

byte_stream_elements *get_byte_stream_elements(byte_stream *stream)
{
    byte_stream_elements *elements;
    char type;
    uint32_t name_len;
    uint32_t norder_name_len;
    char *str;
    uint32_t value_len;
    uint32_t norder_value_len;
    int i;

    if (stream == NULL || stream->elements_count == 0)
        return NULL;

    elements = (byte_stream_elements *)malloc(sizeof(byte_stream_elements));
    if (elements == NULL)
        return NULL;
    elements->stream = stream;
    elements->elements_ptr = (char **)malloc(sizeof(char *) * stream->elements_count);
    if (elements->elements_ptr == NULL)
    {
        free(elements);
        return NULL;
    }
    i = 0;
    str = stream->buffer + sizeof(uint32_t) + sizeof(uint32_t); // str now points to next to header
    while (i < stream->elements_count)
    {
        elements->elements_ptr[i] = str;
        type = *str;
        str++; // skipped the type part
        memcpy(&norder_name_len, str, sizeof(uint32_t));
        name_len = ntohl(norder_name_len);

        str += sizeof(uint32_t);
        str += name_len;

        if (type == string_e || type == float_e || type == double_e || type == long_double_e)
        {
            memcpy(&norder_value_len, str, sizeof(uint32_t));
            value_len = ntohl(norder_value_len);
            str += sizeof(uint32_t);
            str += value_len;
        }
        else
        {
            str += get_type_size(type);
        }

        i++;
    }

    return elements;
}

uint32_t get_byte_stream_elements_length(byte_stream_elements *elements)
{
    if (elements == NULL)
        return 0;

    return elements->stream->elements_count;
}

byte_stream_element *get_byte_stream_element(byte_stream_elements *elements, uint32_t index)
{
    byte_stream_element *element;
    uint32_t norder_name_len;
    uint32_t norder_value_len;
    char *str;
    char type;
    uint32_t name_len;
    uint32_t value_len;

    if (elements == NULL)
        return NULL;
    if (index < 0 || index >= elements->stream->elements_count)
        return NULL;

    element = (byte_stream_element *)malloc(sizeof(byte_stream_element));
    if (element == NULL)
        return NULL;

    str = elements->elements_ptr[index];
    type = *str;
    element->type = type;
    str++; // skipped the type part
    memcpy(&norder_name_len, str, sizeof(uint32_t));
    name_len = ntohl(norder_name_len);
    element->name_len = name_len;
    str += sizeof(uint32_t);
    element->name = str;
    str += name_len;

    if (type == string_e || type == float_e || type == double_e || type == long_double_e)
    {
        memcpy(&norder_value_len, str, sizeof(uint32_t));
        value_len = ntohl(norder_value_len);
        element->value_len = value_len;
        str += sizeof(uint32_t);
        element->value = str;
    }
    else
    {
        element->value_len = get_type_size(type);
        element->value = str;
    }
    return element;
}

void release_byte_stream_elements(byte_stream_elements *elements)
{
    if (elements == NULL)
        return;

    free(elements->elements_ptr);
    free(elements);
}

void release_byte_stream_element(byte_stream_element *element)
{
    if (element == NULL)
        return;
    free(element);
}

char *get_byte_stream_element_name(byte_stream_element *element)
{
    char *str;
    if (element == NULL)
        return NULL;
    str = (char *)malloc(sizeof(char) * ((element->name_len) + 1));
    if (str == NULL)
        return NULL;
    memcpy(str, element->name, element->name_len);
    str[element->name_len] = '\0';
    return str;
}

int is_byte_stream_element_char(byte_stream_element *element)
{
    if (element == NULL)
        return 0; // for false;

    return element->type == char_e;
}

int is_byte_stream_element_string(byte_stream_element *element)
{
    if (element == NULL)
        return 0; // for false;

    return element->type == string_e;
}
int is_byte_stream_element_int8(byte_stream_element *element)
{
    if (element == NULL)
        return 0; // for false;

    return element->type == int8_e;
}
int is_byte_stream_element_int16(byte_stream_element *element)
{
    if (element == NULL)
        return 0; // for false;

    return element->type == int16_e;
}
int is_byte_stream_element_int32(byte_stream_element *element)
{
    if (element == NULL)
        return 0; // for false;

    return element->type == int32_e;
}
int is_byte_stream_element_int64(byte_stream_element *element)
{
    if (element == NULL)
        return 0; // for false;

    return element->type == int64_e;
}
int is_byte_stream_element_uint8(byte_stream_element *element)
{
    if (element == NULL)
        return 0; // for false;

    return element->type == uint8_e;
}
int is_byte_stream_element_uint16(byte_stream_element *element)
{
    if (element == NULL)
        return 0; // for false;

    return element->type == uint16_e;
}
int is_byte_stream_element_uint32(byte_stream_element *element)
{
    if (element == NULL)
        return 0; // for false;

    return element->type == uint32_e;
}
int is_byte_stream_element_uint64(byte_stream_element *element)
{
    if (element == NULL)
        return 0; // for false;

    return element->type == uint64_e;
}
int is_byte_stream_element_float(byte_stream_element *element)
{
    if (element == NULL)
        return 0; // for false;

    return element->type == float_e;
}
int is_byte_stream_element_double(byte_stream_element *element)
{
    if (element == NULL)
        return 0; // for false;

    return element->type == double_e;
}
int is_byte_stream_element_long_double(byte_stream_element *element)
{
    if (element == NULL)
        return 0; // for false;

    return element->type == long_double_e;
}

// 0 for false
// 1 for true or success
int get_byte_stream_element_char(byte_stream_element *element, char *c)
{
    if (element == NULL || c == NULL)
        return 0; // 0 for false

    if (element->type != char_e)
        return 0;

    *c = *(element->value);
    return 1; // 1 for true or success
}

int get_byte_stream_element_string(byte_stream_element *element, char **ptr2str)
{
    char *str;

    if (element == NULL || ptr2str == NULL)
        return 0;

    if (element->type != string_e)
        return 0;

    str = (char *)malloc(sizeof(char) * (element->value_len + 1));
    if (str == NULL)
    {
        *ptr2str = NULL;
        return 0;
    }

    memcpy(str, element->value, element->value_len);
    str[element->name_len] = '\0';
    *ptr2str = str;

    return 1;
}

int get_byte_stream_element_int8(byte_stream_element *element, int8_t *ptr)
{
    if (element == NULL || ptr == NULL)
        return 0;

    if (element->type != int8_e)
        return 0;

    memcpy(ptr, element->value, sizeof(int8_t));
    return 1;
}

int get_byte_stream_element_int16(byte_stream_element *element, int16_t *ptr)
{
    uint16_t norder_value;
    uint16_t value;

    if (element == NULL || ptr == NULL)
        return 0;

    if (element->type != int16_e)
        return 0;

    memcpy(&norder_value, element->value, element->value_len);
    value = ntohs(norder_value);
    memcpy(ptr, &value, sizeof(uint16_t));
    return 1;
}

int get_byte_stream_element_int32(byte_stream_element *element, int32_t *ptr)
{
    uint32_t norder_value;
    uint32_t value;

    if (element == NULL || ptr == NULL)
        return 0;

    if (element->type != int32_e)
        return 0;

    memcpy(&norder_value, element->value, element->value_len);
    value = ntohl(norder_value);
    memcpy(ptr, &value, sizeof(uint32_t));
    return 1;
}

int get_byte_stream_element_int64(byte_stream_element *element, int64_t *ptr)
{
    uint64_t norder_value;
    uint64_t value;

    if (element == NULL || ptr == NULL)
        return 0;

    if (element->type != int64_e)
        return 0;

    memcpy(&norder_value, element->value, element->value_len);
    value = ntohl64(norder_value);
    memcpy(ptr, &value, sizeof(uint64_t));
    return 1;
}

int get_byte_stream_element_uint8(byte_stream_element *element, uint8_t *ptr)
{
    if (element == NULL || ptr == NULL)
        return 0;

    if (element->type != uint8_e)
        return 0;

    memcpy(ptr, element->value, sizeof(uint8_t));
    return 1;
}

int get_byte_stream_element_uint16(byte_stream_element *element, uint16_t *ptr)
{
    uint16_t norder_value;
    uint16_t value;

    if (element == NULL || ptr == NULL)
        return 0;

    if (element->type != uint16_e)
        return 0;

    memcpy(&norder_value, element->value, element->value_len);
    value = ntohs(norder_value);
    memcpy(ptr, &value, sizeof(uint16_t));
    return 1;
}

int get_byte_stream_element_uint32(byte_stream_element *element, uint32_t *ptr)
{
    uint32_t norder_value;
    uint32_t value;

    if (element == NULL || ptr == NULL)
        return 0;

    if (element->type != uint32_e)
        return 0;

    memcpy(&norder_value, element->value, element->value_len);
    value = ntohl(norder_value);
    memcpy(ptr, &value, sizeof(uint32_t));
    return 1;
}

int get_byte_stream_element_uint64(byte_stream_element *element, uint64_t *ptr)
{
    uint64_t norder_value;
    uint64_t value;

    if (element == NULL || ptr == NULL)
        return 0;

    if (element->type != uint64_e)
        return 0;

    memcpy(&norder_value, element->value, element->value_len);
    value = ntohl64(norder_value);
    memcpy(ptr, &value, sizeof(uint64_t));
    return 1;
}

int get_byte_stream_element_float(byte_stream_element *element, float *ptr)
{
    char *str;
    if (element == NULL || ptr == NULL)
        return 0;

    if (element->type != float_e)
        return 0;

    str = (char *)malloc(sizeof(char) * (element->value_len + 1));
    if (str == NULL)
        return 0;

    memcpy(str, element->value, element->value_len);
    str[element->value_len] = '\0';
    errno = 0;

    *ptr = strtof(str, NULL);
    if (errno != 0)
        return 0;
    return 1;
}

int get_byte_stream_element_double(byte_stream_element *element, double *ptr)
{
    char *str;
    if (element == NULL || ptr == NULL)
        return 0;

    if (element->type != double_e)
        return 0;

    str = (char *)malloc(sizeof(char) * (element->value_len + 1));
    if (str == NULL)
        return 0;

    memcpy(str, element->value, element->value_len);
    str[element->value_len] = '\0';
    errno = 0;

    *ptr = strtod(str, NULL);
    if (errno != 0)
        return 0;
    return 1;
}

int get_byte_stream_element_long_double(byte_stream_element *element, long double *ptr)
{
    char *str;
    if (element == NULL || ptr == NULL)
        return 0;

    if (element->type != long_double_e)
        return 0;

    str = (char *)malloc(sizeof(char) * (element->value_len + 1));
    if (str == NULL)
        return 0;

    memcpy(str, element->value, element->value_len);
    str[element->value_len] = '\0';
    errno = 0;

    *ptr = strtold(str, NULL);
    if (errno != 0)
        return 0;
    return 1;
}

int main()
{
    int success;
    byte_stream *stream;
    int i;
    char *str;
    uint32_t stream_len;

    // -- other side
    byte_stream *other_stream;
    byte_stream_elements *elements;
    byte_stream_element *element;
    uint32_t str2len;
    uint32_t number_of_elements;

    char v0;
    char *v1;
    int8_t v2;
    int16_t v3;
    int32_t v4;
    int64_t v5;
    uint8_t v6;
    uint16_t v7;
    uint32_t v8;
    uint64_t v9;
    float v10;
    double v11;
    long double v12;
    char *str2;
    char *name;

    // ------------------------
    uint64_t num;
    uint64_t norder_num;
    uint64_t decoded_num;
    char *p;

    double rr = 4345.339930;

    long double uu = 435345.623230;

    stream = create_byte_stream();
    add_char_to_byte_stream(stream, "gender", 'M');

    add_string_to_byte_stream(stream, "first name of student", "Sammer Gupta");

    add_int32_to_byte_stream(stream, "AGE", 66);

    add_uint32_to_byte_stream(stream, "Total distance to cover before halting for lunch", 122);

    add_int64_to_byte_stream(stream, "Total height to cover before resting for lunch", 90);

    add_uint64_to_byte_stream(stream, "Total depth to cover before going back up", 57);

    add_float_to_byte_stream(stream, "Basic Salary : ", 4545.33f);

    add_double_to_byte_stream(stream, "Total PPF : ", rr);

    add_long_double_to_byte_stream(stream, "Total accumalated amount : ", uu);

    str = get_byte_stream_bytes(stream);
    stream_len = get_byte_stream_length(stream);
    for (i = 0; i < stream_len; i++)
    {
        printf("%c", str[i]);
    }
    printf("\n---------------------------------------------\n");

    for (i = 0; i < stream_len; i++)
    {
        printf("(%c,%d)", str[i], str[i]);
    }

    printf("\n---------------------------------------------\n");

    // num = 5334535;
    // norder_num = htonl64(num);
    // printf("Initial : ");
    // p = (char *)&num;
    // for (i = 0; i < sizeof(uint64_t); i++, p++)
    // {
    //     printf("%d    ", *p);
    // }

    // printf("\n");
    // p = (char *)&norder_num;
    // for (i = 0; i < sizeof(uint64_t); i++, p++)
    // {
    //     printf("%d    ", *p);
    // }
    // printf("\n");

    // decoded_num = ntohl64(norder_num);
    // p = (char *)&decoded_num;
    // for (i = 0; i < sizeof(uint64_t); i++, p++)
    // {
    //     printf("%d    ", *p);
    // }
    // printf("\n");

    // Assumne that the above code got executed on one side of the network
    // on the other end , a stream is received and str2 is the storing the base address

    str2 = (char *)malloc(sizeof(char) * stream->len);
    str2len = stream->len;
    memcpy(str2, str, stream->len);
    release_byte_stream(stream);
    // Assumne that the following code is running on the other  side of network

    other_stream = create_byte_stream_from_bytes(str2, str2len);

    // now the owner of the sequence of bytes is bytes stream

    elements = get_byte_stream_elements(other_stream);

    number_of_elements = get_byte_stream_elements_length(elements);

    for (i = 0; i < number_of_elements; i++)
    {
        printf("Prcoessing element with index %d\n", i);

        element = get_byte_stream_element(elements, i);
        if (element == NULL)
        {
            printf("Unable to extract %dth element\n", i);
            continue;
        }

        name = get_byte_stream_element_name(element);
        if (name != NULL)
            printf("Name : (%s)\n", name);
        else
        {
            printf("Unable to extract name\n");
            continue;
        }

        if (is_byte_stream_element_char(element))
        {

            success = get_byte_stream_element_char(element, &v0); // will return true or false (1 or 0)
            if (success)
                printf("Char value (%c)\n", v0);
            else
                printf("Unable to get char value\n");
        }
        else if (is_byte_stream_element_string(element))
        {
            success = get_byte_stream_element_string(element, &v1); // will return true or false (1 or 0)
            if (success)
                printf("String value (%s)\n", v1);
            else
                printf("Unable to get string value\n");
        }
        else if (is_byte_stream_element_int8(element))
        {
            success = get_byte_stream_element_int8(element, &v2); // will return true or false (1 or 0)
            if (success)
                printf("int8 value (%" PRId8 ")\n", v2);
            else
                printf("Unable to get int8 value\n");
        }
        else if (is_byte_stream_element_int16(element))
        {
            success = get_byte_stream_element_int16(element, &v3); // will return true or false (1 or 0)
            if (success)
                printf("int16 value (%" PRId16 ")\n", v2);
            else
                printf("Unable to get int16 value\n");
        }
        else if (is_byte_stream_element_int32(element))
        {
            success = get_byte_stream_element_int32(element, &v4); // will return true or false (1 or 0)
            if (success)
                printf("int32 value (%" PRId32 ")\n", v4);
            else
                printf("Unable to get int32 value\n");
        }
        else if (is_byte_stream_element_int64(element))
        {

            success = get_byte_stream_element_int64(element, &v5); // will return true or false (1 or 0)
            if (success)
                printf("int64 value (%" PRId64 ")\n", v5);
            else
                printf("Unable to get int64 value\n");
        }
        else if (is_byte_stream_element_uint8(element))
        {
            success = get_byte_stream_element_uint8(element, &v6); // will return true or false (1 or 0)
            if (success)
                printf("uint8 value (%" PRIu8 ")\n", v6);
            else
                printf("Unable to get uint8 value\n");
        }
        else if (is_byte_stream_element_uint16(element))
        {
            success = get_byte_stream_element_uint16(element, &v7); // will return true or false (1 or 0)
            if (success)
                printf("uint16 value (%" PRIu16 ")\n", v7);
            else
                printf("Unable to get uint16 value\n");
        }
        else if (is_byte_stream_element_uint32(element))
        {
            success = get_byte_stream_element_uint32(element, &v8); // will return true or false (1 or 0)
            if (success)
                printf("uint32 value (%" PRIu32 ")\n", v8);
            else
                printf("Unable to get uint32 value\n");
        }
        else if (is_byte_stream_element_uint64(element))
        {
            success = get_byte_stream_element_uint64(element, &v9); // will return true or false (1 or 0)
            if (success)
                printf("uint64 value (%" PRIu64 ")\n", v9);
            else
                printf("Unable to get uint64 value\n");
        }
        else if (is_byte_stream_element_float(element))
        {
            success = get_byte_stream_element_float(element, &v10); // will return true or false (1 or 0)
            if (success)
                printf("float value : (%f)\n", v10);
            else
                printf("Unable to get float value\n");
        }
        else if (is_byte_stream_element_double(element))
        {
            success = get_byte_stream_element_double(element, &v11); // will return true or false (1 or 0)
            if (success)
                printf("double value : (%f)\n", v11);
            else
                printf("Unable to get double value\n");
        }
        else if (is_byte_stream_element_long_double(element))
        {
            success = get_byte_stream_element_long_double(element, &v12); // will return true or false (1 or 0)
            if (success)
                printf("long double value : (%Lf)\n", v12);
            else
                printf("Unable to get long double value\n");
        }
        release_byte_stream_element(element);
    }
    release_byte_stream_elements(elements);
    release_byte_stream(other_stream);

    return 0;
}