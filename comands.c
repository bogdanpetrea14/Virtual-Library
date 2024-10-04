#include <stdio.h>
#include <stdlib.h>   
#include <unistd.h>    
#include <string.h>    
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <netdb.h>      
#include <arpa/inet.h>
#include <errno.h>
#include "helpers.h"
#include "requests.h"
#include <stdbool.h>
#include "buffer.h"
#include "comands.h"
#include "parson.h"

char* get_token(char *response) {
    char *token = calloc(LINELEN, sizeof(char));

    token = strstr(response, ":\"");
    token = strtok(token, "\n");
    token = strtok(token, ":}\"");

    return token;
}

char* get_cookie(char *response) {
    char *cookie = calloc(LINELEN, sizeof(char));

    cookie = strstr(response, "connect.sid");
    cookie = strtok(cookie, "\n");
    cookie = strtok(cookie, ";");

    return cookie;
}

void register_to_server(int sockfd) {
    char username[MAXUSERNAME], password[MAXPASSWORD];

    while (getchar() != '\n');
    printf("username=");
    fgets(username, MAXUSERNAME, stdin);
    username[strlen(username) - 1] = '\0';

    printf("password=");
    fgets(password, MAXPASSWORD, stdin);
    password[strlen(password) - 1] = '\0';

    if (strchr(username, ' ') != NULL) {
        printf("[ERROR]Username cannot contain whitespaces\n");
        return;
    }

    char **body_data = (char**)calloc(1, sizeof(char*));
    body_data[0] = (char*)calloc(LINELEN, sizeof(char));

    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);
    json_object_set_string(root_object, "username", username);
    json_object_set_string(root_object, "password", password);

    char *serialized_string = json_serialize_to_string_pretty(root_value);
    
    strcpy(body_data[0], serialized_string);

    char *message = compute_post_request(IP, REGISTER_URL, CONTENT_TYPE, body_data, 1, NULL, 1, NULL, 1);
    send_to_server(sockfd, message);
    char* response = receive_from_server(sockfd);
    
    if (strstr(response, SUCCESS) != NULL) {
        printf("[SUCCESS]Successfully registered\n");
    } else if (strstr(response, ERROR) != NULL) {
        printf("[ERROR]Error registering. There might be another user witht the same username. Please try another one\n");
    } else {
        printf("Unknown error\n");
    }

    free(body_data[0]);
    free(body_data);
    free(message);
    free(response);
    json_value_free(root_value);
    json_free_serialized_string(serialized_string);
}

char* login_to_server(int sockfd) {
    char username[MAXUSERNAME], password[MAXPASSWORD];

    printf("username=");
    scanf("%s", username);
    printf("password=");
    scanf("%s", password);

    char **body_data = (char**)calloc(1, sizeof(char*));
    body_data[0] = (char*)calloc(LINELEN, sizeof(char));

    char *cookie_final = NULL;

    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);

    json_object_set_string(root_object, "username", username);
    json_object_set_string(root_object, "password", password);
    char *serialized_string = json_serialize_to_string_pretty(root_value);

    strcpy(body_data[0], serialized_string);

    char *message = compute_post_request(IP, LOGIN_URL, CONTENT_TYPE, body_data, 1, NULL, 0, NULL, 0);
    send_to_server(sockfd, message);
    char* response = receive_from_server(sockfd);

    if (strstr(response, SUCCESS) != NULL) {
        printf("[SUCCESS]Successfully logged in\n");
        cookie_final = (char*)calloc(LINELEN, sizeof(char));
        strcpy(cookie_final, get_cookie(response));

    } else if (strstr(response, ERROR) != NULL) {
        printf("[ERROR]Error logging in, no user with the given username or password wrong\n");
    } else {
        printf("Unknown error\n");
    }

    free(body_data[0]);
    free(body_data);
    json_value_free(root_value);
    json_free_serialized_string(serialized_string);
    return cookie_final;
}

char* enter_library(char *cookie, int sockfd) {
    char **cookies = (char**)calloc(1, sizeof(char*));
    cookies[0] = (char*)calloc(LINELEN, sizeof(char));
    int no_cookies = 0;
    if (cookie) {
        strcpy(cookies[0], cookie);
        no_cookies = 1;
    }

    char *message = compute_get_request(IP, ENTER_LIBRARY, NULL, cookies, no_cookies, NULL, 0);
    char *token = calloc(LINELEN, sizeof(char));

    send_to_server(sockfd, message);
    char* response = receive_from_server(sockfd);

    if (strstr(response, SUCCESS) != NULL) {
        printf("[SUCCESS]Successfully entered library\n");
        strcpy(token, get_token(response));
    } else if (strstr(response, ERROR) != NULL) {
        printf("[ERROR]Error entering library\n");
    } else {
        printf("Unknown error\n");
    }

    free(message);
    free(response);
    free(cookies[0]);
    free(cookies);

    return token;
}

void add_book(int sockfd, char *token) {
    char title[100], author[100], genre[100], publisher[100], page_count_string[100];
    int page_count;

    // clear the stdin buffer
    while (getchar() != '\n');

    printf("title=");
    fgets(title, 100, stdin);
    title[strlen(title) - 1] = '\0';

    printf("author=");
    fgets(author, 100, stdin);
    author[strlen(author) - 1] = '\0';

    printf("genre=");
    fgets(genre, 100, stdin);
    genre[strlen(genre) - 1] = '\0';

    printf("publisher=");
    fgets(publisher, 100, stdin);
    publisher[strlen(publisher) - 1] = '\0';

    printf("page_count=");
    fgets(page_count_string, 100, stdin);
    page_count_string[strlen(page_count_string) - 1] = '\0';
    page_count = atoi(page_count_string);

    char **body_data = (char**)calloc(1, sizeof(char*));
    body_data[0] = (char*)calloc(LINELEN, sizeof(char));

    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);

    json_object_set_string(root_object, "title", title);
    json_object_set_string(root_object, "author", author);
    json_object_set_string(root_object, "genre", genre);
    json_object_set_string(root_object, "publisher", publisher);
    json_object_set_number(root_object, "page_count", page_count);

    char *serialized_string = json_serialize_to_string_pretty(root_value);
    strcpy(body_data[0], serialized_string);

    char **tokens = (char**)calloc(1, sizeof(char*));
    tokens[0] = (char*)calloc(LINELEN, sizeof(char));
    strcpy(tokens[0], token);

    char *message = compute_post_request(IP, ADD_BOOK, CONTENT_TYPE, body_data, 1, NULL, 0, tokens, 1);
    send_to_server(sockfd, message);
    char* response = receive_from_server(sockfd);

    if (strstr(response, SUCCESS) != NULL) {
        printf("[SUCCESS]Successfully added book\n");
    } else if (strstr(response, ERROR) != NULL) {
        printf("[ERROR]Error adding book\n");
    } else {
        printf("Unknown error\n");
    }

    free(body_data[0]);
    free(body_data);
    free(message);
    free(response);
    json_value_free(root_value);
    json_free_serialized_string(serialized_string);
}

void logout(int sockfd, char *cookie) {
    char **cookies = (char**)calloc(1, sizeof(char*));
    cookies[0] = (char*)calloc(LINELEN, sizeof(char));
    strcpy(cookies[0], cookie);

    char *message = compute_get_request(IP, LOGOUT, NULL, cookies, 1, NULL, 0);
    send_to_server(sockfd, message);
    char* response = receive_from_server(sockfd);

    if (strstr(response, SUCCESS) != NULL) {
        printf("[SUCCESS]Successfully logged out\n");
    } else if (strstr(response, ERROR) != NULL) {
        printf("[ERROR]Error logging out\n");
    } else {
        printf("Unknown error\n");
    }

    free(message);
    free(response);
    free(cookies[0]);
    free(cookies);
}

void get_books(int sockfd, char *token) {
    char **tokens = (char**)calloc(1, sizeof(char*));
    tokens[0] = (char*)calloc(LINELEN, sizeof(char));
    strcpy(tokens[0], token);

    char *message = compute_get_request(IP, ADD_BOOK, NULL, NULL, 0, tokens, 1);

    send_to_server(sockfd, message);
    char* response = receive_from_server(sockfd);

    if (strstr(response, SUCCESS) != NULL) {
        printf("[SUCCESS]Successfully got books\n");
        char *books = strstr(response, "[");
        printf("%s\n", books);
    } else if (strstr(response, ERROR) != NULL) {
        printf("[ERROR]Error getting books\n");
    } else {
        printf("Unknown error\n");
    }

    free(message);
    free(response);
    free(tokens[0]);
    free(tokens);
}

void get_book(int sockfd, char *token) {
    int id;
    printf("id=");
    scanf("%d", &id);
    char **tokens = (char**)calloc(1, sizeof(char*));
    tokens[0] = (char*)calloc(LINELEN, sizeof(char));
    strcpy(tokens[0], token);

    char url[100];
    sprintf(url, "%s/%d", ADD_BOOK,id);

    char *message = compute_get_request(IP, url, NULL, NULL, 0, tokens, 1);

    send_to_server(sockfd, message);
    char* response = receive_from_server(sockfd);

    if (strstr(response, SUCCESS) != NULL) {
        printf("[SUCCESS]Successfully got book\n");
        char *book = strstr(response, "{");
        printf("%s\n", book);
    } else if (strstr(response, ERROR) != NULL) {
        printf("[ERROR]Error getting book\n");
    } else {
        printf("Unknown error\n");
    }

    free(message);
    free(response);
    free(tokens[0]);
    free(tokens);
}

void delete_book(int sockfd, char *token) {
    int id;
    printf("id=");
    scanf("%d", &id);
    char **tokens = (char**)calloc(1, sizeof(char*));
    tokens[0] = (char*)calloc(LINELEN, sizeof(char));
    strcpy(tokens[0], token);

    char url[100];
    sprintf(url, "%s/%d", ADD_BOOK,id);

    char *message = compute_delete_request(IP, url, NULL, tokens, 1);

    send_to_server(sockfd, message);
    char* response = receive_from_server(sockfd);

    if (strstr(response, SUCCESS) != NULL) {
        printf("[SUCCESS]Successfully deleted book\n");
    } else if (strstr(response, ERROR) != NULL) {
        printf("[ERROR]Error deleting book\n");
    } else {
        printf("Unknown error\n");
    }

    free(message);
    free(response);
    free(tokens[0]);
    free(tokens);
}