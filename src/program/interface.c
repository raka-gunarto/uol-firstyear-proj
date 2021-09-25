#include "interface.h"

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <library_management/book_management.h>
#include <library_management/loans_management.h>
#include <library_management/user_management.h>

static int stop_flag = 0;

// interface utilities

static char* prompt(const char* prompt_str) {
    printf("%s", prompt_str);

    char* buf = NULL;
    size_t len = 0;
    getline(&buf, &len, stdin);
    buf[strlen(buf) - 1] = '\0'; // remove newline

    return buf;
}

static void display_books(struct Book* book_arr, long len, const char* list_header) {
    printf("%s\n", list_header);
    for(int i = 0; i < len; ++i) {
        printf("ID: %i\n", book_arr[i].id);
        printf("Title: %s\n", book_arr[i].title);
        printf("Authors: %s\n", book_arr[i].authors);
        printf("Year of Publication: %u\n", book_arr[i].year);
        printf("Copies: %u\n\n", book_arr[i].copies);
    }
}

static void display_loans(struct LoansArray loans, int display_book) {
    printf("Loans:\n");
    for(int i = 0; i < loans.size; ++i) {
        printf("Loaned to: %s\n", loans.arr[i].username);
        if (display_book) {
            struct Book* book = find_book_by_id(loans.arr[i].book_ID).arr;
            display_books(book,1,"Book: ");
        }
        else {
            printf("Book ID: %u\n\n", loans.arr[i].book_ID);
        }
    }
}

// menu handlers

static void interface_quit() {
    stop_flag = 1;
}

static void interface_logout() {
    if (logout_user() == 0) {
        printf("Logged out successfully!\n\n");
    }
    else {
        printf("Not logged in.\n\n");
    }
}

static void interface_login() {
    char* username = prompt("Enter username: ");
    char* PIN = prompt("Enter PIN: ");

    int PIN_conv = atoi(PIN);
    int result = authenticate_user(username, PIN_conv);
    if(result == 0)
        printf("Logged in successfully!\n\n");
    else if(result == 1)
        printf("That user does not exist.\n\n");
    else if(result == 2)
        printf("Incorrect credentials.\n\n");
    
    free(username);
    free(PIN);
}

static void interface_register() {
    char* username = prompt("Enter username: ");
    char* PIN = prompt("Enter PIN: ");

    int result = register_user(username, atoi(PIN), 0);
    if(result == 0)
        printf("User registered!\n\n");
    else if(result == 1)
        printf("That user already exists.\n\n");
    else if(result == 2)
        printf("Incorrect PIN length.\n\n");

    free(username);
    free(PIN);
}

static void interface_search() {
    printf("1. Search by title\n");
    printf("2. Search by author\n");
    printf("3. Search by year of publication\n\n");

    char* buf = prompt("Enter choice: ");
    int choice = atoi(buf);
    free(buf);

    struct BookArray result = {NULL, 0};

    if (choice == 1) {
        char* search_str = prompt("Enter title (case-sensitive): ");

        result = find_book_by_title(search_str);
        free(search_str);
    }
    else if (choice == 2) {
        char* search_str = prompt("Enter author (case-sensitive): ");

        result = find_book_by_author(search_str);
        free(search_str);
    }
    else if (choice == 3) {
        char* search_str = prompt("Enter year: ");

        result = find_book_by_year(atoi(search_str));
        free(search_str);
    }
    else {
        printf("Invalid choice!\n\n");
    }

    if (result.arr != NULL) {
        display_books(result.arr, result.size, "Search results:");
    }
    else {
        printf("No results found.\n\n");
    }

    free_BookArray(result);
}

static void interface_borrow_book() {
    char* input = prompt("Enter book ID: ");
    int book_ID = atoi(input);
    free(input);

    struct Book* book = find_book_by_id(book_ID).arr;

    if (book == NULL) {
        printf("That book does not exist!");
        return;
    }

    display_books(book, 1, "Book:");

    char* yn = prompt("Confirm loan (y/n)? ");
    if(yn[0] != 'y' && yn[0] != 'Y') return;

    int result = borrow_book(get_authenticated_user_username(), book_ID);
    if (result == 0) {
        printf("Book loaned successfully!\n\n");
    }
    else if (result == 1) {
        printf("You already borrowed a copy of that book!\n\n");
    }
}

static void interface_return_book() {
    char* input = prompt("Enter book ID: ");
    int book_ID = atoi(input);
    free(input);

    int result = return_book(get_authenticated_user_username(), book_ID);
    if (result == 0) {
        printf("Book returned successfully!\n\n");
    }
    else if (result == 1) {
        printf("You don't have a copy of that book!\n\n");
    }
}

static void interface_add_book() {
    char* id = prompt("Enter ID: ");
    char* title = prompt("Enter Title: ");
    char* authors = prompt("Enter Authors (comma seperated, no spaces): ");
    char* year = prompt("Enter Year of Publication: ");
    char* copies = prompt("Enter Copies: ");

    int year_atoi = atoi(year);
    int copies_atoi = atoi(copies);
    int id_atoi = atoi(id);

    struct Book book = {id_atoi, title, authors, year_atoi, copies_atoi};
    int result = add_book(book);

    if (result == 0) {
        printf("Book added successfully!\n\n");
    }
    else {
        printf("Book with that ID already exists.\n\n");
    }

    free(id);
    free(title);
    free(authors);
    free(year);
    free(copies);
}

static void interface_remove_book() {
    char* id = prompt("Enter book ID: ");
    int id_atoi = atoi(id);
    free(id);

    struct Book* book = find_book_by_id(id_atoi).arr;
    if (book == NULL) {
        printf("That book does not exist.\n\n");
        return;
    }
    
    display_books(book, 1, "Book to remove:");

    free(book->title);
    free(book->authors);
    free(book);

    char* yn = prompt("Confirm book removal (y/n)? ");
    if(yn[0] != 'y' && yn[0] != 'Y') return;

    int result = remove_book(id_atoi);
    if (result != 0) {
        printf("There was an issue during removal.\n\n");
    }
    else {
        printf("Book removed successfully!\n\n");
    }
}

static void interface_show_userloans() {
    struct LoansArray loans = find_loans_by_username(get_authenticated_user_username());
    display_loans(loans, 1);
    free_LoansArray(loans);
}

static void interface_show_bookloans() {
    char* id = prompt("Enter book ID: ");
    int id_atoi = atoi(id);
    free(id);
    
    struct Book* book = find_book_by_id(id_atoi).arr;
    if (book == NULL) {
        printf("That book does not exist.\n\n");
        return;
    }
    
    display_books(book, 1, "Showing loans for book:");

    free(book->title);
    free(book->authors);
    free(book);

    struct LoansArray loans = find_loans_by_id(id_atoi);
    display_loans(loans, 0);
    free_LoansArray(loans);
}

// menu items and their respective handlers

static const char* MAIN_MENU_ITEMS[] = {
    "1. Login\n",
    "2. Register\n",
    "3. Quit\n"
};

static void (*MAIN_MENU_HANDLERS[])() = {
    interface_login,
    interface_register,
    interface_quit
};

static const char* USER_MENU_ITEMS[] = {
    "1. Search for book\n",
    "2. Borrow a book\n",
    "3. Return a book\n",
    "4. Display current loans\n",
    "5. Logout\n"
};

static void (*USER_MENU_HANDLERS[])() = {
    interface_search,
    interface_borrow_book,
    interface_return_book,
    interface_show_userloans,
    interface_logout
};

static const char* LIBRARIAN_MENU_ITEMS[] = {
    "1. Search for a book\n",
    "2. Borrow a book\n",
    "3. Return a book\n",
    "4. Display current loans\n",
    "5. List loans for book\n",
    "6. Add a book\n",
    "7. Remove a book\n",
    "8. Logout\n"
};

static void (*LIBRARIAN_MENU_HANDLERS[])() = {
    interface_search,
    interface_borrow_book,
    interface_return_book,
    interface_show_userloans,
    interface_show_bookloans,
    interface_add_book,
    interface_remove_book,
    interface_logout
};

static void do_menu(const char* menu_items[], void (*menu_handlers[])(), int menu_len) {
    for (int i = 0; i < menu_len; ++i) {
        printf("%s", menu_items[i]);
    }

    char* buf = prompt("Choose an option: ");
    int choice = atoi(buf) - 1;
    if(choice >= 0 && choice < menu_len) {
        (*menu_handlers[choice])();
    }
    else {
        printf("Invalid choice!\n\n");
    }

    free(buf);
}

// exposed public functions

int save_library() {
    return save_library_custom(BOOK_MGMT_DB, USER_MGMT_DB, LOAN_MGMT_DB);
}

#pragma GCC diagnostic push // make GCC shut up about struct FILE*, because technically FILE is _IO_FILE
#pragma GCC diagnostic ignored "-Wincompatible-pointer-types"

int save_library_custom(const char* bookdbname, const char* userdbname, const char* loandbname) {
    int flag = 0;
    FILE* hbookdb = fopen(bookdbname, "w");
    FILE* huserdb = fopen(userdbname, "w");
    FILE* hloandb = fopen(loandbname, "w");

    flag += store_books(hbookdb);
    flag += store_users(huserdb);
    flag += store_loans(hloandb);

    if (hbookdb != NULL)
        fclose(hbookdb);
    if (huserdb != NULL)
        fclose(huserdb);
    if (hloandb != NULL)
        fclose(hloandb);

    return flag;
}

int load_library() {
    int flag = 0;
    FILE* hbookdb = fopen(BOOK_MGMT_DB, "r");
    FILE* huserdb = fopen(USER_MGMT_DB, "r");
    FILE* hloandb = fopen(LOAN_MGMT_DB, "r");

    flag += load_books(hbookdb);
    flag += load_users(huserdb);
    flag += load_loans(hloandb);

    if (hbookdb != NULL)
        fclose(hbookdb);
    if (huserdb != NULL)
        fclose(huserdb);
    if (hloandb != NULL)
        fclose(hloandb);

    return flag;
}

#pragma GCC diagnostic pop

void run_interface() {
    printf("+-----------------------------+\n");
    printf("|  Library Management System  |\n");
    printf("|   Written by: Raka Gunarto  |\n");
    printf("+-----------------------------+\n");

    while(!stop_flag) {
        char* username = get_authenticated_user_username();
        if(username == NULL) {
            do_menu(MAIN_MENU_ITEMS, MAIN_MENU_HANDLERS, 3);
            free(username);
            continue;
        }

        printf("Welcome %s!\n", username);

        if(get_authenticated_user_permissions_level() == 0)
            do_menu(USER_MENU_ITEMS, USER_MENU_HANDLERS, 5);
        else if(get_authenticated_user_permissions_level() == 1)
            do_menu(LIBRARIAN_MENU_ITEMS, LIBRARIAN_MENU_HANDLERS, 8);
        free(username);
    }
}
