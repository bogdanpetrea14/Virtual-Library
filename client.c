#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include <stdbool.h>
#include "buffer.h"
#include <errno.h>
#include "comands.h"
#include <stdbool.h>

int transform_command_into_int (char* command) {
    if (strcmp(command, "register") == 0)
        return 1;
    if (strcmp(command, "login") == 0)
        return 2;
    if (strcmp(command, "enter_library") == 0)
        return 3;
    if (strcmp(command, "get_books") == 0)
        return 4;
    if (strcmp(command, "get_book") == 0)
        return 5;
    if (strcmp(command, "add_book") == 0)
        return 6;
    if (strcmp(command, "delete_book") == 0)
        return 7;
    if (strcmp(command, "logout") == 0)
        return 8;
    if (strcmp(command, "exit") == 0)
        return 9;
    return 0;
}

int main(int argc, char *argv[])
{
    char *cookie = NULL;
    char *token = NULL;
    bool logged_in = false;
    bool in_library = false;
    int sockfd;

    char *command = (char*)calloc(MAXCOMMAND, sizeof(char));
    char *username = (char*)calloc(MAXUSERNAME, sizeof(char));
    char *password = (char*)calloc(MAXPASSWORD, sizeof(char));

    loop {
        scanf("%s", command);

        int command_code = transform_command_into_int(command);

        sockfd = open_connection(IP, PORT, AF_INET, SOCK_STREAM, 0);

        switch (command_code) {
            case 1:
                register_to_server(sockfd);
                break;
            case 2:
                if (!logged_in) {
                    cookie = login_to_server(sockfd);
                    logged_in = cookie != NULL ? true : false;
                } else {
                    printf("You are already logged in\n");
                }
                break;
            case 3:
                if (logged_in) {
                    token = enter_library(cookie, sockfd);
                    in_library = true;
                } else {
                    printf("You must be logged in to enter the library\n");
                }
                break;
            case 4:
                if (in_library) {
                    get_books(sockfd, token);
                } else {
                    printf("You must be in the library to get books\n");
                }
            break;
            case 5:
                if (in_library) {
                    get_book(sockfd, token);
                } else {
                    printf("You must be in the library to get a book\n");
                }
            break;
            case 6:
                if (in_library) {
                    add_book(sockfd, token);
                } else {
                    printf("You must be in the library to add a book\n");
                }
                break;
            case 7:
                if (in_library) {
                    delete_book(sockfd, token);
                } else {
                    printf("You must be in the library to delete a book\n");
                }
            break;
            case 8:
                if (logged_in) {
                    logout(sockfd, cookie);
                    logged_in = false;
                    in_library = false;
                    if (cookie != NULL) {
                        free(cookie);
                        cookie = NULL;
                    }
                    if (token != NULL) {
                        free(token);
                        token = NULL;
                    }
                } else {
                    printf("You must be logged in to logout\n");
                }
            break;
            case 9:
                free(command);
                free(username);
                free(password);

                if (cookie != NULL)
                    free(cookie);
                if (token != NULL)
                    free(token);
                return 0;
            break;
            default:
                printf("Invalid command\n");
        }
        // clear the username and password
        memset(username, 0, MAXUSERNAME);
        memset(password, 0, MAXPASSWORD);
        close_connection(sockfd);
    }

    return 0;
}
