#ifndef __CLENCODER__
#define __CLENCODER__
#include <stdint.h>

struct _byte_stream;
typedef struct _byte_stream byte_stream;

int is_big_endian();
int is_little_endian();

byte_stream *create_byte_stream();

char *get_byte_stream(byte_stream *stream);

void release_byte_stream(byte_stream *stream);

int add_char_to_byte_stream(byte_stream *stream, const char *name, char value);

int add_string_to_byte_stream(byte_stream *stream, const char *name, const char *value);

int add_int8_to_byte_stream(byte_stream *stream, const char *name, int8_t value);

int add_int16_to_byte_stream(byte_stream *stream, const char *name, int16_t value);

int add_int32_to_byte_stream(byte_stream *stream, const char *name, int32_t value);

int add_int64_to_byte_stream(byte_stream *stream, const char *name, int64_t value);

int add_uint8_to_byte_stream(byte_stream *stream, const char *name, uint8_t value);

int add_uint16_to_byte_stream(byte_stream *stream, const char *name, uint16_t value);

int add_uint32_to_byte_stream(byte_stream *stream, const char *name, uint32_t value);

int add_uint64_to_byte_stream(byte_stream *stream, const char *name, uint64_t value);

int add_float_to_byte_stream(byte_stream *stream, const char *name, float value);

int add_double_to_byte_stream(byte_stream *stream, const char *name, double value);

int add_long_double_to_byte_stream(byte_stream *stream, const char *name, long double value);

#endif