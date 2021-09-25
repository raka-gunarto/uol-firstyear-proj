#include <library_management/loans_management.h>

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Constants */
const char* LOAN_MGMT_DB = "database/loan_db.txt";

/* Loan Database */
#ifdef TESTING_FLAG
struct Loan* loan_db; // expose for testing purposes
long loan_db_size;
#else
static struct Loan* loan_db;
static long loan_db_size;
#endif

int store_loans(struct FILE *file) {
    // check if file is valid
    if (file == NULL)
        return 1;

    // save loans into file
    for(int i = 0; i < loan_db_size; i++) {
        // get loan in array
        struct Loan* curLoanPtr = &loan_db[i];
        
        // check if valid loan
        if (curLoanPtr->username == NULL)
            continue;

        // save loan
        fprintf((FILE*) file, "%s;%u\n", curLoanPtr->username, curLoanPtr->book_ID);
    }
    return 0;
}

int load_loans(struct FILE *file) {
    // pre-check file not null
    if (file == NULL)
        return 1;

    // firstly check if book_db is not null, if yes, free it (will only happen during testing)
    if(loan_db)
        free(loan_db);
    loan_db = NULL;
    loan_db_size = 0;

    // load the books
    char* buf = 0;
    size_t buf_size = 0;
    while(getline(&buf, &buf_size, file) != -1) {
        // add the book
        char* username = strtok(buf, ";");
        unsigned int bookid = atoi(strtok(NULL, ";"));
        borrow_book(username, bookid);
    }

    return 0;
}

static struct Loan* expand_loandb() {
    long last_size = loan_db_size;
    loan_db_size += 10;

    if (loan_db > 0)
        loan_db = realloc(loan_db, loan_db_size * sizeof(struct Loan));
    else
        loan_db = malloc(loan_db_size * sizeof(struct Loan));
    for(long i = last_size; i < loan_db_size; ++i)
        loan_db[i].username = NULL;

    return loan_db + last_size;
}

static struct Loan* get_free_loandb_space() {
    for (int i = 0; i < loan_db_size; i++) {
        if (loan_db[i].username == NULL)
            return loan_db + i;
    }

    return expand_loandb();
}

static void copy_loan(struct Loan* dest, struct Loan src) {
    dest->username = malloc(sizeof(char) * (strlen(src.username) + 1));

    strcpy(dest->username, src.username);
    dest->book_ID = src.book_ID;
}

static int loan_equality_check(struct Loan* lhs, struct Loan* rhs) {
    if (lhs->username == NULL || rhs->username == NULL) return 0;
    return (strcmp(lhs->username, rhs->username) == 0) && (lhs->book_ID == rhs->book_ID);
}

int borrow_book(const char* username, unsigned int book_ID) {
    // make sure this user hasn't already have that book
    int idx;
    struct Loan l = {username, book_ID};
    for(idx = 0; idx < loan_db_size && !loan_equality_check(&l, &loan_db[idx]); ++idx);
    if (idx < loan_db_size) return 1;

    // get next free space
    struct Loan* dest = get_free_loandb_space();

    // add book
    copy_loan(dest, l);
    return 0;
}

int return_book(const char* username, unsigned int book_ID) {
    // find loan
    int idx;
    struct Loan l = {username, book_ID};
    for(idx = 0; idx < loan_db_size && !loan_equality_check(&l, &loan_db[idx]); ++idx);
    if (idx == loan_db_size) return 1;

    // "remove" loan
    free(loan_db[idx].username);
    loan_db[idx].username = NULL;
    return 0;
}

struct LoansArray find_loans_by_username(const char* username) {
    struct LoansArray l_arr = {NULL, 0};

    for(int i = 0; i < loan_db_size; ++i) {
        // check if valid loan
        if (loan_db[i].username == NULL)
            continue;
        
        // is this the user we're looking for?
        if (strcmp(username, loan_db[i].username) == 0) {
            if (l_arr.size == 0) 
                l_arr.arr = malloc(sizeof(struct Loan)); 
            else
                l_arr.arr = realloc(l_arr.arr, sizeof(struct Loan) * (l_arr.size + 1)); 
            
            copy_loan(l_arr.arr + l_arr.size, loan_db[i]);
            l_arr.size += 1;
        }
    }
    
    return l_arr;
}

struct LoansArray find_loans_by_id(unsigned int book_ID) {
    struct LoansArray l_arr = {NULL, 0};

    for(int i = 0; i < loan_db_size; ++i) {
        // check if valid loan
        if (loan_db[i].username == NULL)
            continue;
        
        // is this the id we're looking for?
        if (loan_db[i].book_ID == book_ID) {
            if (l_arr.size == 0) 
                l_arr.arr = malloc(sizeof(struct Loan)); 
            else
                l_arr.arr = realloc(l_arr.arr, sizeof(struct Loan) * (l_arr.size + 1)); 
            
            copy_loan(l_arr.arr + l_arr.size, loan_db[i]);
            l_arr.size += 1;
        }
    }

    return l_arr;
}

void free_LoansArray(struct LoansArray b_arr) {
    for(int i = 0; i < b_arr.size; i++) {
        free(b_arr.arr[i].username);
    }
    free(b_arr.arr);
}

