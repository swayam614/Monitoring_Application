#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sscl.h>
#include <pthread.h>
#include <clencoder.h>
#include <ssnl.h>

struct _tcp_action_handler;

typedef struct _tcp_action_server
{
    tcp_server *server;
    int error_number;
    char *error_string;
    struct _tcp_action_handler **action_handlers;
    uint32_t action_handlers_count;
} tcp_action_server;

typedef struct _tcp_action_request
{
    tcp_client *client;
    tcp_action_server *server;
    char *action_name;
    int error_number;
    char *error_string;
    byte_stream *stream;
    byte_stream_elements *elements;
} tcp_action_request;

typedef struct _tcp_action_response
{
    tcp_client *client;
    int error_number;
    char *error_string;
    byte_stream *stream;
} tcp_action_response;

typedef struct _tcp_action_handler
{
    char *name;
    void (*action_handler)(tcp_action_request *, tcp_action_response *);

} tcp_action_handler;

tcp_action_server *global_action_server; // if everything goes well , this can be
                                         // changed to a data structure that will enable to support
                                         // for multiple servers in one application

tcp_action_server *allocate_tcp_action_server(unsigned short int port)
{
    tcp_action_server *action_server;
    action_server = (tcp_action_server *)malloc(sizeof(tcp_action_server));
    if (action_server == NULL)
        return NULL;

    global_action_server = action_server; // this will change when we will add support for enabling
                                          // multiple servers in one applicaiton
    action_server->server = NULL;
    action_server->error_number = 0;
    action_server->error_string = NULL;
    action_server->action_handlers = NULL;
    action_server->action_handlers_count = 0;

    action_server->server = allocate_tcp_server(port);
    if (tcp_server_failed(action_server->server))
    {
        action_server->error_number = 601; //  the source of error is not on commond layer
                                           // the source is on the underlying layer
        tcp_server_error(action_server->server, &(action_server->error_string));
    }

    return action_server;
}

void on_tcp_action_server_started(tcp_action_server *action_server, void (*handler)(unsigned short int))
{
    if (action_server == NULL || handler == NULL)
        return;
    on_tcp_server_started(action_server->server, handler);
}

void on_tcp_action_server_stopped(tcp_action_server *action_server, void (*handler)(unsigned short int))
{
    if (action_server == NULL || handler == NULL)
        return;

    on_tcp_server_stoppped(action_server->server, handler);
}

void tcp_action_server_add_action_mapping(tcp_action_server *action_server, const char *commond, void (*action)(tcp_action_request *, tcp_action_response *))
{
    tcp_action_handler *new_handler;
    tcp_action_handler **temp;
    int i;

    if (action_server == NULL || commond == NULL || *commond == '\0' || action == NULL)
    {
        return;
    }

    for (i = 0; i < action_server->action_handlers_count; i++)
    {
        if (strcmp(action_server->action_handlers[i]->name, commond) == 0)
        {
            action_server->action_handlers[i]->action_handler = action;
            return;
        }
    }

    new_handler = (tcp_action_handler *)malloc(sizeof(tcp_action_handler));
    if (new_handler == NULL)
    {
        // 601 is very very important part
        if (action_server->error_number == 601 && action_server->error_string != NULL)
        {
            free(action_server->error_string);
            action_server->error_string = NULL;
        }
        action_server->error_number = 600; // Low memory
        return;
    }

    new_handler->name = (char *)malloc(sizeof(char) * (strlen(commond) + 1));
    if (new_handler->name == NULL)
    {
        free(new_handler);
        // 601 is very very important part
        if (action_server->error_number == 601 && action_server->error_string != NULL)
        {
            free(action_server->error_string);
            action_server->error_string = NULL;
        }
        action_server->error_number = 600; // Low memory
        return;
    }

    strcpy(new_handler->name, commond);
    new_handler->action_handler = action;

    temp = (tcp_action_handler **)realloc(action_server->action_handlers, sizeof(tcp_action_handler *) * (action_server->action_handlers_count + 1));

    if (temp == NULL)
    {
        free(new_handler->name);
        free(new_handler);
        // 601 is very very important part
        if (action_server->error_number == 601 && action_server->error_string != NULL)
        {
            free(action_server->error_string);
            action_server->error_string = NULL;
        }
        action_server->error_number = 600; // Low memory
        return;
    }

    temp[action_server->action_handlers_count] = new_handler;
    action_server->action_handlers = temp;
    action_server->action_handlers_count++;
}

void action_client_connected_handler(unsigned short int port, tcp_server *server, tcp_client *client)
{
    tcp_action_handler *action_handler;
    int i;
    tcp_action_request *request;
    tcp_action_response *response;
    char *str;
    uint32_t str_len;

    if (global_action_server == NULL || server == NULL || client == NULL)
        return;

    str = tcp_client_receive(client, &str_len);
    if (str == NULL || tcp_client_failed(client))
    {
        disconnect_tcp_client(client);
        release_tcp_client(client);
        return;
    }

    request = (tcp_action_request *)malloc(sizeof(tcp_action_request));
    if (request == NULL)
    {
        // later on add something to server log
        disconnect_tcp_client(client);
        release_tcp_client(client);
        return;
    }

    response = (tcp_action_response *)malloc(sizeof(tcp_action_response));
    if (response == NULL)
    {
        // later on add something to server log
        free(request);
        disconnect_tcp_client(client);
        release_tcp_client(client);
        return;
    }

    response->client = client;
    response->error_number = 0;
    response->error_string = NULL;
    response->stream = create_byte_stream();
    if (response->stream == NULL)
    {
        free(request);
        free(response);
        disconnect_tcp_client(client);
        release_tcp_client(client);
        return;
    }

    request->client = client;
    request->error_number = 0;
    request->error_string = NULL;
    request->server = global_action_server; // this will change when support to enable
                                            // mulitple servers is added
    request->stream = create_byte_stream_from_bytes(str, str_len);
    if (request->stream == NULL)
    {
        free(str);
        free(request);
        release_byte_stream(response->stream);
        free(response);
        disconnect_tcp_client(client);
        release_tcp_client(client);
        return;
    }

    request->elements = get_byte_stream_elements(request->stream);
    if (request->elements == NULL)
    {
        // do not call free on the basis of str as the owner of the string is request->stream
        release_byte_stream(request->stream);
        release_byte_stream(response->stream);
        free(request);
        free(response);
        disconnect_tcp_client(client);
        release_tcp_client(client);
        return;
    }

    // ***ACTION_NAME*** is reserved for setting up action name within the commond layer
    // this has to be documented so that the user of commond layer does not send a data
    // element with name as a ***ACTION_NAME***

    request->action_name = tcp_action_request_get_string(request, "***ACTION_NAME***");
    if (request->action_name == NULL)
    {
        release_byte_stream_elements(request->elements); // this we have to check it once
        release_byte_stream(request->stream); 
        release_byte_stream(response->stream);
        free(request);
        free(response);
        disconnect_tcp_client(client);
        release_tcp_client(client);
        return;
    }

    action_handler = NULL;
    for (i = 0; i < request->server->action_handlers_count; i++)
    {
        if (strcmp(request->server->action_handlers[i]->name, request->action_name))
        {
            action_handler = request->server->action_handlers[i];
            break;
        }
    }

    if (action_handler == NULL)
    {
        free(request->action_name);
        release_byte_stream_elements(request->elements); // this we have to check it once
        release_byte_stream(request->stream);
        release_byte_stream(response->stream);
        free(request);
        free(response);
        disconnect_tcp_client(client);
        release_tcp_client(client);
        return;
    }

    action_handler->action_handler(request, response);
    // is everything work as desired , we will be adding some code over here to
    // check if the connection to the client has been closed or not
    // if not then we will again make sure that we are supposed to recieve more data

    // to release request and response will be the responsibitlity of commond layer user
    // by placing call to release_tcp_action_request and release_tcp_action_response
}

void start_tcp_action_server(tcp_action_server *action_server)
{
    if (action_server == NULL)
        return;
    on_tcp_client_connected(action_server->server, action_client_connected_handler);
    tcp_start_server(action_server->server);
    if (tcp_server_failed(action_server->server))
    {
        action_server->error_number = 601;
        tcp_server_error(action_server->server, &(action_server->error_string));
    }
}

void stop_tcp_action_server(tcp_action_server *action_server)
{
    if (action_server == NULL)
        return;
    tcp_stop_server(action_server->server);
}
void release_tcp_action_server(tcp_action_server *action_server)
{
    int i;

    if (action_server == NULL)
        return;

    if (action_server->server == NULL)
        return;

    release_tcp_server(action_server->server);

    global_action_server = NULL; // this will change when support for enabling multiple server is added
    action_server->server = NULL;
    if (action_server->error_number == 601 && action_server->error_string != NULL)
    {
        free(action_server->error_string);
        action_server->error_string = NULL;
    }

    if (action_server->action_handlers == NULL)
        return;

    for (i = 0; i < action_server->action_handlers_count; i++)
    {
        free(action_server->action_handlers[i]->name);
        free(action_server->action_handlers[i]);
    }
    free(action_server->action_handlers);
    free(action_server);
}

int tcp_action_server_failed(tcp_action_server *action_server)
{
    if (action_server == NULL || action_server->error_number != 0)
        return 1; // yes something failed
    return 0;     // 0 for false
}

void tcp_action_server_error(tcp_action_server *action_server, char **error_string)
{
    if (action_server->server == NULL || error_string == NULL)
        return;
    if (action_server->server == 0)
    {
        *error_string = NULL;
        return;
    }
    if (action_server->error_number == 601)
    {
        *error_string = action_server->error_string;
        action_server->error_string = NULL;
    }
    else
    {
        *error_string = NULL; // ideally this should not happen
    }
}

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
