#include <clencoder.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdio.h> // to be removed

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
    uint8_t len;
} byte_stream;

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

byte_stream *create_byte_stream()
{
    byte_stream *stream;
    stream = (byte_stream *)malloc(sizeof(byte_stream));
    if (stream == NULL)
        return NULL;
    stream->len = 0;
    stream->buffer = NULL;
    return stream;
}

char *get_byte_stream(byte_stream *stream)
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
    int i;
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
    stream->len = required_len;
    return 1; // for true
}

int add_string_to_byte_stream(byte_stream *stream, const char *name, const char *value)
{
    uint32_t name_len;
    uint32_t value_len;
    int i;
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
    stream->len = required_len;
    return 1;
}

int add_int8_to_byte_stream(byte_stream *stream, const char *name, int8_t value)
{
    uint32_t name_len;
    int i;
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
    stream->len = required_len;
    return 1; // for true
}

int add_int16_to_byte_stream(byte_stream *stream, const char *name, int16_t value)
{
    uint32_t name_len;
    int i;
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
    stream->len = required_len;
    return 1; // for true
}

int add_int32_to_byte_stream(byte_stream *stream, const char *name, int32_t value)
{
    uint32_t name_len;
    int i;
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
    stream->len = required_len;
    return 1; // for true
}

int add_int64_to_byte_stream(byte_stream *stream, const char *name, int64_t value)
{
    uint32_t name_len;
    int i;
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
    stream->len = required_len;
    return 1; // for true
}

int add_uint8_to_byte_stream(byte_stream *stream, const char *name, uint8_t value)
{
    uint32_t name_len;
    int i;
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
    stream->len = required_len;
    return 1; // for true
}

int add_uint16_to_byte_stream(byte_stream *stream, const char *name, uint16_t value)
{
    uint32_t name_len;
    int i;
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
    stream->len = required_len;
    return 1; // for true
}

int add_uint32_to_byte_stream(byte_stream *stream, const char *name, uint32_t value)
{
    uint32_t name_len;
    int i;
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
    stream->len = required_len;
    return 1; // for true
}

int add_uint64_to_byte_stream(byte_stream *stream, const char *name, uint64_t value)
{
    uint32_t name_len;
    int i;
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
    stream->len = required_len;
    return 1; // for true
}

int add_float_to_byte_stream(byte_stream *stream, const char *name, float value)
{
}

int add_double_to_byte_stream(byte_stream *stream, const char *name, double value)
{
}

int add_long_double_to_byte_stream(byte_stream *stream, const char *name, long double value)
{
}

int main()
{
    byte_stream *stream;
    int i;
    char *str;
    uint32_t stream_len;

    uint64_t num;
    uint64_t norder_num;
    uint64_t decoded_num;
    char *p;

    stream = create_byte_stream();
    add_char_to_byte_stream(stream, "gender", 'M');

    add_string_to_byte_stream(stream, "first name of student", "Sammer Gupta");

    add_int8_to_byte_stream(stream, "AGE", 66);

    add_uint8_to_byte_stream(stream, "Total distance to cover before halting for lunch", 122);

    add_int64_to_byte_stream(stream, "Total height to cover before resting for lunch", 90);

    add_uint64_to_byte_stream(stream, "Total depth to cover before going back up", 57);

    str = get_byte_stream(stream);
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

    num = 5334535;
    norder_num = htonl64(num);
    printf("Initial : ");
    p = (char *)&num;
    for (i = 0; i < sizeof(uint64_t); i++, p++)
    {
        printf("%d    ", *p);
    }

    printf("\n");
    p = (char *)&norder_num;
    for (i = 0; i < sizeof(uint64_t); i++, p++)
    {
        printf("%d    ", *p);
    }
    printf("\n");

    decoded_num = ntohl64(norder_num);
    p = (char *)&decoded_num;
    for (i = 0; i < sizeof(uint64_t); i++, p++)
    {
        printf("%d    ", *p);
    }
    printf("\n");

    return 0;
}