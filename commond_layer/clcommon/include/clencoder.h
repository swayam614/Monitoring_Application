#ifndef __CLENCODER__
#define __CLENCODER__
#include <stdint.h>

struct _byte_stream;
typedef struct _byte_stream byte_stream;

struct _byte_stream_elements;
typedef struct _byte_stream_elements byte_stream_elements;

struct _byte_stream_element;
typedef struct _byte_stream_element byte_stream_element;

int is_big_endian();
int is_little_endian();

byte_stream *create_byte_stream();

char *get_byte_stream_bytes(byte_stream *stream);

uint32_t get_byte_stream_length(byte_stream *stream);

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

byte_stream *create_byte_stream_from_bytes(char *bytes, uint32_t len);
byte_stream_elements *get_byte_stream_elements(byte_stream *stream);

uint32_t get_byte_stream_elements_length(byte_stream_elements *elements);

byte_stream_element *get_byte_stream_element(byte_stream_elements *elements, uint32_t index);

void release_byte_stream_elements(byte_stream_elements *elements);

void release_byte_stream_element(byte_stream_element *element);

char *get_byte_stream_element_name(byte_stream_element *element);

int is_byte_stream_element_char(byte_stream_element *element);

int is_byte_stream_element_string(byte_stream_element *element);
int is_byte_stream_element_int8(byte_stream_element *element);
int is_byte_stream_element_int16(byte_stream_element *element);
int is_byte_stream_element_int32(byte_stream_element *element);
int is_byte_stream_element_int64(byte_stream_element *element);
int is_byte_stream_element_uint8(byte_stream_element *element);
int is_byte_stream_element_uint16(byte_stream_element *element);
int is_byte_stream_element_uint32(byte_stream_element *element);
int is_byte_stream_element_uint64(byte_stream_element *element);
int is_byte_stream_element_float(byte_stream_element *element);
int is_byte_stream_element_double(byte_stream_element *element);
int is_byte_stream_element_long_double(byte_stream_element *element);

int get_byte_stream_element_char(byte_stream_element *element, char *c);

int get_byte_stream_element_string(byte_stream_element *element, char **ptr2str);

int get_byte_stream_element_int8(byte_stream_element *element, int8_t *ptr);

int get_byte_stream_element_int16(byte_stream_element *element, int16_t *ptr);

int get_byte_stream_element_int32(byte_stream_element *element, int32_t *ptr);

int get_byte_stream_element_int64(byte_stream_element *element, int64_t *ptr);

int get_byte_stream_element_uint8(byte_stream_element *element, uint8_t *ptr);

int get_byte_stream_element_uint16(byte_stream_element *element, uint16_t *ptr);

int get_byte_stream_element_uint32(byte_stream_element *element, uint32_t *ptr);

int get_byte_stream_element_uint64(byte_stream_element *element, uint64_t *ptr);

int get_byte_stream_element_float(byte_stream_element *element, float *ptr);

int get_byte_stream_element_double(byte_stream_element *element, double *ptr);

int get_byte_stream_element_long_double(byte_stream_element *element, long double *ptr);

#endif