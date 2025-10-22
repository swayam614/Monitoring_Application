#include <stdint.h>
#include <cscl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include <_stdio.h>

typedef struct _tcp_connection_request
{

} tcp_connection_request;

typedef struct _tcp_connection_response
{

} tcp_connection_response;

void tcp_connection_send_request(tcp_connection *connection, tcp_connection_request *request)
{
}

tcp_connection_response *tcp_connection_receive_response(tcp_connection *connection)
{
}

// will document later on
tcp_connection_request *create_tcp_connection_request(tcp_connection *connection)
{
}

void tcp_connection_request_set_action_name(tcp_connection_request *request, const char *action)
{
}
char *tcp_connection_request_get_action_name(tcp_connection_request *request)
{
}

void tcp_connection_request_set_string(tcp_connection_request *request, const char *name, const char *value) {}
void tcp_connection_request_set_char(tcp_connection_request *request, const char *name, char value) {}
void tcp_connection_request_set_int8(tcp_connection_request *request, const char *name, int8_t value) {}
void tcp_connection_request_set_int16(tcp_connection_request *request, const char *name, int16_t value) {}
void tcp_connection_request_set_int32(tcp_connection_request *request, const char *name, int32_t value) {}
void tcp_connection_request_set_int64(tcp_connection_request *request, const char *name, int64_t value) {}
void tcp_connection_request_set_uint8(tcp_connection_request *request, const char *name, uint8_t value) {}
void tcp_connection_request_set_uint16(tcp_connection_request *request, const char *name, uint16_t value) {}
void tcp_connection_request_set_uint32(tcp_connection_request *request, const char *name, uint32_t value) {}
void tcp_connection_request_set_uint64(tcp_connection_request *request, const char *name, uint64_t value) {}
void tcp_connection_request_set_float(tcp_connection_request *request, const char *name, float value) {}
void tcp_connection_request_set_double(tcp_connection_request *request, const char *name, double value) {}
void tcp_connection_request_set_long_double(tcp_connection_request *request, const char *name, long double value) {}

int tcp_connection_response_name_exists(tcp_connection_response *response, const char *name) {}
char *tcp_connection_response_get_string(tcp_connection_response *response, const char *name) {}
char tcp_connection_response_get_char(tcp_connection_response *response, const char *name) {}
int8_t tcp_connection_response_get_int8(tcp_connection_response *response, const char *name) {}
int16_t tcp_connection_response_get_int16(tcp_connection_response *response, const char *name) {}
int32_t tcp_connection_response_get_int32(tcp_connection_response *response, const char *name) {}
int64_t tcp_connection_response_get_int64(tcp_connection_response *response, const char *name) {}
uint8_t tcp_connection_response_get_uint8(tcp_connection_response *response, const char *name) {}
uint16_t tcp_connection_response_get_uint16(tcp_connection_response *response, const char *name) {}
uint32_t tcp_connection_response_get_uint32(tcp_connection_response *response, const char *name) {}
uint64_t tcp_connection_response_get_uint64(tcp_connection_response *response, const char *name) {}
float tcp_connection_response_get_float(tcp_connection_response *response, const char *name) {}
double tcp_connection_response_get_double(tcp_connection_response *response, const char *name) {}
long double tcp_connection_response_get_long_double(tcp_connection_response *response, const char *name) {}

void release_tcp_connection_request(tcp_connection_request *request) {}

int tcp_connection_request_failed(tcp_connection_request *request) {}
void tcp_connection_request_error(tcp_connection_request *request, char **ptr) {}

void release_tcp_connection_response(tcp_connection_response *response) {}

int tcp_connection_response_failed(tcp_connection_response *response) {}
void tcp_connection_response_error(tcp_connection_response *response, char **ptr) {}

// the following code is written by the commond layer library user frmo client side of an application , it is just a sample code , later on the following will be removed and library from cscl.c created

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

    tcp_connection_request_set_action_name(request, "Add Student");
    tcp_connection_request_set_uint32(request, "Roll Number", rollNumber);
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
        succeeded = tcp_connection_response_get_uint32(response, "succeeded");
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
    // ask for roll number
    // connect
    //  prepare request will roll number and action name as GetStudetn
    // send request and receive response
    // if succeeded is 0 the print exception and story ends
    // if succeeded is 1 then
    //              extract anme , age and gender from response
    //              print name , age and gender
    //              receive response and parse the succeeded part as done earlier
    //      Note : dont forget to disconnect and release resources
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
            printf("Unablt to send request : %s\n", error_string);
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

    succeeded = tcp_connection_response_get_uint32(response, "succeeded");
    if (succeeded)
    {
        number_of_student = tcp_connection_response_get_uint32(response, "number_of_student");
        x = 1;
        while (x <= number_of_student)
        {
            sprintf(field_name, "roll_number_%d", x);
            roll_number = tcp_connection_response_get_uint32(response, field_name);
            sprintf(field_name, "name_%d", x);
            name = tcp_connection_response_get_string(response, field_name);
            sprintf(field_name, "gender_%d", x);
            gender = tcp_connection_response_get_char(response, field_name);
            sprintf(field_name, "age_ %d", x);
            age = tcp_connection_response_get_uint32(response, field_name);
            printf("%d", x);
            printf(",%" PRIu32, roll_number);
            printf(",%s", name);
            printf(",%c", gender);
            printf(",%" PRIu32, age);
            x++;
        }
    }
    else
    {
        error_string = tcp_connection_response_get_string(response, "exception");
        printf("Unable to fetch students , reason : %s\n", error_string);
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
        printf("Student Master\n");
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