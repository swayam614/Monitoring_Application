#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sscl.h>
#include <pthread.h>

typedef struct _tcp_action_server
{
} tcp_action_server;
typedef struct _tcp_action_request
{
} tcp_action_request;
typedef struct _tcp_action_response
{
} tcp_action_response;

tcp_action_server *allocate_tcp_action_server(unsigned short int port) {}
void on_tcp_action_server_started(tcp_action_server *server, void (*handler)(unsigned short int)) {}
void on_tcp_action_server_stopped(tcp_action_server *server, void (*handler)(unsigned short int)) {}
void tcp_action_server_add_action_mapping(tcp_action_server *server, const char *commond, void (*action)(tcp_action_request *, tcp_action_response *)) {}

void start_tcp_action_server(tcp_action_server *server) {}

void stop_tcp_action_server(tcp_action_server *server) {}
void release_tcp_action_server(tcp_action_server *server) {}

int tcp_action_server_failed(tcp_action_server *server) {}
void tcp_action_server_error(tcp_action_server *server, char **ptr) {}

char *tcp_action_request_get_action_name(tcp_action_request *request) {}
char *tcp_action_request_get_local_ip(tcp_action_request *request) {}
char *tcp_action_request_get_remote_ip(tcp_action_request *request) {}
unsigned short int tcp_action_request_get_local_port(tcp_action_request *request) {}
unsigned short int tcp_action_request_get_remote_port(tcp_action_request *request) {}

int tcp_action_request_name_exists(tcp_action_request *request, const char *name) {}
char *tcp_action_request_get_string(tcp_action_request *request, const char *name) {}
char tcp_action_request_get_char(tcp_action_request *request, const char *name) {}
int8_t tcp_action_request_get_int8(tcp_action_request *request, const char *name) {}
int16_t tcp_action_request_get_int16(tcp_action_request *request, const char *name) {}
int32_t tcp_action_request_get_int32(tcp_action_request *request, const char *name) {}
int64_t tcp_action_request_get_int64(tcp_action_request *request, const char *name) {}
uint8_t tcp_action_request_get_uint8(tcp_action_request *request, const char *name) {}
uint16_t tcp_action_request_get_uint16(tcp_action_request *request, const char *name) {}
uint32_t tcp_action_request_get_uint32(tcp_action_request *request, const char *name) {}
uint64_t tcp_action_request_get_uint64(tcp_action_request *request, const char *name) {}
float tcp_action_request_get_float(tcp_action_request *request, const char *name) {}
double tcp_action_request_get_double(tcp_action_request *request, const char *name) {}
long double tcp_action_request_get_long_double(tcp_action_request *request, const char *name) {}

void tcp_action_response_set_string(tcp_action_response *response, const char *name, const char *value) {}
void tcp_action_response_set_char(tcp_action_response *response, const char *name, char value) {}
void tcp_action_response_set_int8(tcp_action_response *response, const char *name, int8_t value) {}
void tcp_action_response_set_int16(tcp_action_response *response, const char *name, int16_t value) {}
void tcp_action_response_set_int32(tcp_action_response *response, const char *name, int32_t value) {}
void tcp_action_response_set_int64(tcp_action_response *response, const char *name, int64_t value) {}
void tcp_action_response_set_uint8(tcp_action_response *response, const char *name, uint8_t value) {}
void tcp_action_response_set_uint16(tcp_action_response *response, const char *name, uint16_t value) {}
void tcp_action_response_set_uint32(tcp_action_response *response, const char *name, uint32_t value) {}
void tcp_action_response_set_uint64(tcp_action_response *response, const char *name, uint64_t value) {}
void tcp_action_response_set_float(tcp_action_response *response, const char *name, float value) {}
void tcp_action_response_set_double(tcp_action_response *response, const char *name, double value) {}
void tcp_action_response_set_long_double(tcp_action_response *response, const char *name, long double value) {}

void send_tcp_action_response(tcp_action_response *response) {}

void tcp_action_request_close(tcp_action_request *request) {}
void release_tcp_action_request(tcp_action_request *request) {}

int tcp_action_request_failed(tcp_action_request *request) {}
void tcp_action_request_error(tcp_action_request *request, char **ptr) {}

void release_tcp_action_response(tcp_action_response *response) {}

int tcp_action_response_failed(tcp_action_response *response) {}
void tcp_action_response_error(tcp_action_response *response, char **ptr) {}

tcp_action_server *get_tcp_action_server(tcp_action_request *request) {}

// the following code will be for sample application

void server_started_event_handler(unsigned short int port)
{
    printf("Server on port %u started\n", port);
}

void server_stopped_event_handler(unsigned short int port)
{
    printf("Server on port %u stopped\n", port);
}

struct student
{
    uint32_t roll_number;
    char name[21];
    char gender;
    uint32_t age;

} student;

void add_student_action_hnadler(tcp_action_request *request, tcp_action_response *response)
{
    uint32_t roll_number;
    char *name;
    char gender;
    uint32_t age;
    char *error_string;
    struct student s;
    FILE *f;
    struct student t;

    if (!tcp_action_request_name_exists(request, "RollNumber"))
    {
        tcp_action_response_set_int32(response, "succeeded", 0); // 0 for failure
        tcp_action_response_set_string(response, "exception", "RollNumber is missing");
        send_tcp_action_response(response);
        tcp_action_request_close(request);
        release_tcp_action_request(request);
        release_tcp_action_response(response);
        return;
    }

    if (!tcp_action_request_name_exists(request, "Name"))
    {
        tcp_action_response_set_int32(response, "succeeded", 0); // 0 for failure
        tcp_action_response_set_string(response, "exception", "Name is missing");
        send_tcp_action_response(response);
        tcp_action_request_close(request);
        release_tcp_action_request(request);
        release_tcp_action_response(response);
        return;
    }

    if (!tcp_action_request_name_exists(request, "Gender"))
    {
        tcp_action_response_set_int32(response, "succeeded", 0); // 0 for failure
        tcp_action_response_set_string(response, "exception", "Gender is missing");
        send_tcp_action_response(response);
        tcp_action_request_close(request);
        release_tcp_action_request(request);
        release_tcp_action_response(response);
        return;
    }

    if (!tcp_action_request_name_exists(request, "Age"))
    {
        tcp_action_response_set_int32(response, "succeeded", 0); // 0 for failure
        tcp_action_response_set_string(response, "exception", "Age is missing");
        send_tcp_action_response(response);
        tcp_action_request_close(request);
        release_tcp_action_request(request);
        release_tcp_action_response(response);
        return;
    }

    roll_number = tcp_action_request_get_uint32(request, "RollNumber");
    if (tcp_action_request_failed(request))
    {
        tcp_action_request_error(request, &error_string);
        if (error_string)
        {
            // this is just for debugging , later on instead of printing , it can be logged in a file
            printf("Error extracting RollNumber : %s\n", error_string);
            free(error_string);
        }
        else
        {
            // this is just for debugging , later on instead of printing , it can be logged in a file
            printf("Error extracting RollNumber\n");
        }
        tcp_action_response_set_int32(response, "succeeded", 0); // 0 for failure
        tcp_action_response_set_string(response, "exception", "Unable to extract RollNumber");
        send_tcp_action_response(response);
        tcp_action_request_close(request);
        release_tcp_action_request(request);
        release_tcp_action_response(response);
        return;
    }

    name = tcp_action_request_get_string(request, "Name");
    if (tcp_action_request_failed(request))
    {
        tcp_action_request_error(request, &error_string);
        if (error_string)
        {
            // this is just for debugging , later on instead of printing , it can be logged in a file
            printf("Error extracting Name : %s\n", error_string);
            free(error_string);
        }
        else
        {
            // this is just for debugging , later on instead of printing , it can be logged in a file
            printf("Error extracting Name\n");
        }
        tcp_action_response_set_int32(response, "succeeded", 0); // 0 for failure
        tcp_action_response_set_string(response, "exception", "Unable to extract Name ");
        send_tcp_action_response(response);
        tcp_action_request_close(request);
        release_tcp_action_request(request);
        release_tcp_action_response(response);
        return;
    }

    gender = tcp_action_request_get_char(request, "Gender");
    if (tcp_action_request_failed(request))
    {
        tcp_action_request_error(request, &error_string);
        if (error_string)
        {
            // this is just for debugging , later on instead of printing , it can be logged in a file
            printf("Error extracting Gender : %s\n", error_string);
            free(error_string);
        }
        else
        {
            // this is just for debugging , later on instead of printing , it can be logged in a file
            printf("Error extracting Gender\n");
        }
        tcp_action_response_set_int32(response, "succeeded", 0); // 0 for failure
        tcp_action_response_set_string(response, "exception", "Unable to extract Gender");
        send_tcp_action_response(response);
        tcp_action_request_close(request);
        release_tcp_action_request(request);
        release_tcp_action_response(response);
        return;
    }

    age = tcp_action_request_get_uint32(request, "Age");
    if (tcp_action_request_failed(request))
    {
        tcp_action_request_error(request, &error_string);
        if (error_string)
        {
            // this is just for debugging , later on instead of printing , it can be logged in a file
            printf("Error extracting Age : %s\n", error_string);
            free(error_string);
        }
        else
        {
            // this is just for debugging , later on instead of printing , it can be logged in a file
            printf("Error extracting Age\n");
        }
        tcp_action_response_set_int32(response, "succeeded", 0); // 0 for failure
        tcp_action_response_set_string(response, "exception", "Unable to extract Age");
        send_tcp_action_response(response);
        tcp_action_request_close(request);
        release_tcp_action_request(request);
        release_tcp_action_response(response);
        return;
    }

    f = fopen("student.data", "rb");
    if (f != NULL)
    {
        while (1)
        {
            fread(&t, sizeof(struct student), 1, f);
            if (feof(f))
                break;
            if (t.roll_number == roll_number)
            {
                fclose(f);
                tcp_action_response_set_int32(response, "succeeded", 0); // 0 for failure
                tcp_action_response_set_string(response, "exception", "Roll Number exists!");
                send_tcp_action_response(response);
                tcp_action_request_close(request);
                release_tcp_action_request(request);
                release_tcp_action_response(response);
                return;
            }
        }
        fclose(f);
    } // logic to search ends here
    s.roll_number = roll_number;
    strcpy(s.name, name);
    free(name);
    s.gender = gender;
    s.age = age;
    f = fopen("student.data", "ab");
    fwrite(&s, sizeof(struct student), 1, f);
    fclose(f);
    tcp_action_response_set_int32(response, "succeeded", 1); // 1 for task done
    send_tcp_action_response(response);
    tcp_action_request_close(request);
    release_tcp_action_request(request);
    release_tcp_action_response(response);
}

void get_all_students_aciton_handler(tcp_action_request *request, tcp_action_response *response)
{
    char field_name[21];
    uint32_t number_of_student;
    FILE *f;
    struct student s;
    int x;

    f = fopen("student.data", "rb");
    if (f == NULL)
    {
        tcp_action_response_set_int32(response, "succeeded", 1); // 1 for task done
        tcp_action_response_set_int32(response, "number_of_student", 0);
        send_tcp_action_response(response);
        tcp_action_request_close(request);
        release_tcp_action_request(request);
        release_tcp_action_response(response);
        return;
    }
    number_of_student = 0;
    x = 0;
    while (1)
    {
        fread(&s, sizeof(struct student), 1, f);
        if (feof(f))
            break;
        number_of_student++;
        x++;
        sprintf(field_name, "roll_number_%d", x);
        tcp_action_response_set_uint32(response, field_name, s.roll_number);
        // we should check  with tcp_action_response_failed , I am ignoring right now

        sprintf(field_name, "name_%d", x);
        tcp_action_response_set_string(response, field_name, s.name);

        sprintf(field_name, "gender_%d", x);
        tcp_action_response_set_char(response, field_name, s.gender);

        sprintf(field_name, "age_%d", x);
        tcp_action_response_set_uint32(response, field_name, s.age);
    }
    fclose(f);

    tcp_action_response_set_int32(response, "succeeded", 1); // 1 for task done
    tcp_action_response_set_int32(response, "number_of_student", number_of_student);
    send_tcp_action_response(response);
    tcp_action_request_close(request);
    release_tcp_action_request(request);
    release_tcp_action_response(response);
}

void stop_server_action_handler(tcp_action_request *request, tcp_action_response *response)
{
}

int main()
{
    unsigned short int port = 6060;
    tcp_action_server *action_server;
    char *error_string;

    action_server = allocate_tcp_action_server(port);
    if (tcp_action_server_failed(action_server))
    {
        tcp_action_server_error(action_server, &error_string);
        if (error_string)
        {
            printf("Erorr : %s\n", error_string);
            free(error_string);
        }
        else
        {
            printf("Unable to allocate resources for server\n");
        }
        release_tcp_action_server(action_server);
        exit(0);
    }

    on_tcp_action_server_started(action_server, server_started_event_handler);
    on_tcp_action_server_stopped(action_server, server_stopped_event_handler);

    tcp_action_server_add_action_mapping(action_server, "AddStudent", add_student_action_hnadler);
    tcp_action_server_add_action_mapping(action_server, "GetAllStudents", get_all_students_aciton_handler);
    tcp_action_server_add_action_mapping(action_server, "StopServer", stop_server_action_handler);

    start_tcp_action_server(action_server);
    if (tcp_action_server_failed(action_server))
    {
        tcp_action_server_error(action_server, &error_string);
        if (error_string)
        {
            printf("Erorr : %s\n", error_string);
            free(error_string);
        }
        else
        {
            printf("Unable to start server\n");
        }
        release_tcp_action_server(action_server);
        exit(0);
    }

    pthread_exit(0);
}
