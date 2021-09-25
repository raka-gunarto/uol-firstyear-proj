#ifndef LOANS_MANAGEMENT_GUARD__H 
#define LOANS_MANAGEMENT_GUARD__H

/* externals -- for testing */
#ifdef TESTING_FLAG
extern struct Loan* loan_db;
extern long loan_db_size;
#endif

struct Loan {
    char* username;
    unsigned int book_ID;
};

struct LoansArray {
    struct Loan* arr;
    long size;
};

struct FILE;
extern const char* LOAN_MGMT_DB;

// saves loans in given file
// return codes:
// 0 = success
// 1 = fail
int store_loans(struct FILE *file);

// loads loans from given file
// return codes:
// 0 = success
// 1 = fail
int load_loans(struct FILE *file);

// loan a book to specified user
// return codes:
// 0 = success
// 1 = user already borrowed the book
int borrow_book(const char* username, unsigned int book_ID);

// loan a book to specified user
// return codes:
// 0 = success
// 1 = user has no loaned books with that book ID
int return_book(const char* username, unsigned int book_ID);

// finds loans of a specified user
// LoansArray has a null pointer if none found
struct LoansArray find_loans_by_username(const char* username);

// finds loans of specified book
// LoansArray has a null pointer if none found
struct LoansArray find_loans_by_id(unsigned int book_ID);

// frees a LoansArray
void free_LoansArray(struct LoansArray l_arr);

#endif