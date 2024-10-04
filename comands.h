#define REGISTER_URL "/api/v1/tema/auth/register"
#define LOGIN_URL "/api/v1/tema/auth/login"
#define ENTER_LIBRARY "/api/v1/tema/library/access"
#define ADD_BOOK "/api/v1/tema/library/books"
#define LOGOUT "/api/v1/tema/auth/logout"
#define MAX_URL_SIZE 100
#define CONTENT_TYPE_SIZE 20
#define SUCCESS "HTTP/1.1 2"
#define ERROR "HTTP/1.1 4"
#define CONTENT_TYPE "application/json"

typedef struct book {
    char title[100];
    char author[100];
    char genre[100];
    int page_count;
    char publisher[100];
} book;

// function to register a user to the server
void register_to_server(int sockfd);

// function to login a user to the server
char* login_to_server(int sockfd);

// function to enter the library
char* enter_library(char *cookie, int sockfd);

// function to add a book to the library
void add_book(int sockfd, char *token);

// function to logout a user from the server
void logout(int sockfd, char *cookie);

// function to get all the books from the library
void get_books(int sockfd, char *token);

// function to get a book from the library
void get_book(int sockfd, char *token);

// function to delete a book from the library
void delete_book(int sockfd, char *token);
