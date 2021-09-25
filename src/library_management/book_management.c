#include <library_management/book_management.h>

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Constants */
const char* BOOK_MGMT_DB = "database/book_db.txt";

/* Book Database */
#ifdef TESTING_FLAG
struct Book* book_db; // expose for testing purposes
long book_db_size;
#else
static struct Book* book_db;
static long book_db_size;
#endif

int store_books(struct FILE *file) {
    // check if file is valid
    if (file == NULL)
        return 1;

    // save books into file
    for(int i = 0; i < book_db_size; i++) {
        // get book in array
        struct Book* curBookPtr = &book_db[i];
        
        // check if valid book
        if (curBookPtr->id == -1)
            continue;

        // save book
        fprintf((FILE*) file, "%u;%s;%s;%u;%u\n", curBookPtr->id, curBookPtr->title, curBookPtr->authors, curBookPtr->year, curBookPtr->copies);
    }

    return 0;
}

int load_books(struct FILE *file) {
    // pre-check file not null
    if (file == NULL)
        return 1;

    // firstly check if book_db is not null, if yes, free it (will only happen during testing)
    if(book_db)
        free(book_db);
    book_db = NULL;
    book_db_size = 0;

    // load the books
    char* buf = 0;
    size_t buf_size = 0;
    while(getline(&buf, &buf_size, file) != -1) {
        // create book
        struct Book book;

        // populate the book
        char* token = strtok(buf, ";");
        book.id = atoi(token);

        token = strtok(NULL, ";");
        book.title = malloc(sizeof(char) * (strlen(token) + 1));
        strcpy(book.title, token);
        
        token = strtok(NULL, ";");
        book.authors = malloc(sizeof(char) * (strlen(token) + 1));
        strcpy(book.authors, token);

        token = strtok(NULL, ";");
        book.year = atoi(token);

        token = strtok(NULL, ";");
        book.copies = atoi(token);

        // add the book
        add_book(book);
    }

    return 0;
}

static struct Book* expand_bookdb() {
    long last_size = book_db_size;
    book_db_size += 10;

    if (last_size > 0)
        book_db = realloc(book_db, book_db_size * sizeof(struct Book));
    else
        book_db = malloc(book_db_size * sizeof(struct Book));
    for(long i = last_size; i < book_db_size; ++i)
        book_db[i].id = -1;

    return book_db + last_size;
}

static struct Book* get_free_bookdb_space() {
    for (int i = 0; i < book_db_size; i++) {
        if (book_db[i].id == -1)
            return book_db + i;
    }

    return expand_bookdb();
}

static void copy_book(struct Book* dest, struct Book src) {
    dest->title = malloc(sizeof(char) * (strlen(src.title) + 1));
    dest->authors = malloc(sizeof(char) * (strlen(src.authors) + 1));

    dest->id = src.id;
    strcpy(dest->title, src.title);
    strcpy(dest->authors, src.authors);
    dest->year = src.year;
    dest->copies = src.copies;
}

int add_book(struct Book book) {
    // return 1 if id exists
    int book_idx;
    for(book_idx = 0; book_idx < book_db_size && book_db[book_idx].id != book.id; ++book_idx);
    if (book_idx < book_db_size) return 1;

    // get next free space
    struct Book* dest = get_free_bookdb_space();

    // copy it over
    copy_book(dest, book);

    return 0;
}

int remove_book(unsigned int id) {
    // find the book. or return 1 if it doesn't exist
    int book_idx;
    for(book_idx = 0; book_idx < book_db_size && book_db[book_idx].id != id; ++book_idx);
    if (book_idx == book_db_size) return 1;

    // free the book!
    book_db[book_idx].id = -1;
    free(book_db[book_idx].title);
    free(book_db[book_idx].authors);
    return 0;
}

struct BookArray find_book_by_id (unsigned int book_ID) { // we should only find 1, not sure why this returns BookArray, but whatever
    struct BookArray b_arr = {NULL, 0};
    
    for(int i = 0; i < book_db_size; ++i) {
        // check if valid book
        if (book_db[i].id == -1)
            continue;
        
        // is this the id we're looking for?
        if (book_db[i].id == book_ID) {
            b_arr.arr = malloc(sizeof(struct Book)); 
            b_arr.size = 1;
            copy_book(b_arr.arr, book_db[i]);
            break;
        }
    }

    return b_arr;
}

struct BookArray find_book_by_title (const char *title) {
    struct BookArray b_arr = {NULL, 0};

    for(int i = 0; i < book_db_size; ++i) {
        // check if valid book
        if (book_db[i].id == -1)
            continue;
        
        // is this the book we're looking for?
        if (strcmp(book_db[i].title, title) == 0) {
            if (b_arr.size == 0) 
                b_arr.arr = malloc(sizeof(struct Book)); 
            else
                b_arr.arr = realloc(b_arr.arr, sizeof(struct Book) * (b_arr.size + 1)); 
            
            copy_book(b_arr.arr + b_arr.size, book_db[i]);
            b_arr.size += 1;
        }
    }
    
    return b_arr;
}

static int authors_contains(const char* authors, const char* author) {
    char* authors_cpy = malloc(sizeof(char) * (strlen(authors) + 1));
    strcpy(authors_cpy, authors);

    int found = 0;
    char* token = strtok(authors_cpy, ",");
    do {
        if (strcmp(author, token) == 0)
            found = 1;
        token = strtok(NULL, ",");
    } while(token != NULL);

    free(authors_cpy);
    return found;
}

struct BookArray find_book_by_author (const char *author) {
    struct BookArray b_arr = {NULL, 0};

    for(int i = 0; i < book_db_size; ++i) {
        // check if valid book
        if (book_db[i].id == -1)
            continue;
        
        // is this the book we're looking for?
        if (authors_contains(book_db[i].authors, author)) {
            if (b_arr.size == 0) 
                b_arr.arr = malloc(sizeof(struct Book)); 
            else
                b_arr.arr = realloc(b_arr.arr, sizeof(struct Book) * (b_arr.size + 1)); 
            
            copy_book(b_arr.arr + b_arr.size, book_db[i]);
            b_arr.size += 1;
        }
    }

    return b_arr;
}

struct BookArray find_book_by_year (unsigned int year) {
    struct BookArray b_arr = {NULL, 0};

    for(int i = 0; i < book_db_size; ++i) {
        // check if valid book
        if (book_db[i].id == -1)
            continue;
        
        // is this the book we're looking for?
        if (book_db[i].year == year) {
            if (b_arr.size == 0) 
                b_arr.arr = malloc(sizeof(struct Book)); 
            else
                b_arr.arr = realloc(b_arr.arr, sizeof(struct Book) * (b_arr.size + 1)); 
            
            copy_book(b_arr.arr + b_arr.size, book_db[i]);
            b_arr.size += 1;
        }
    }

    return b_arr;
}

void free_BookArray(struct BookArray b_arr) {
    for(int i = 0; i < b_arr.size; ++i) {
        free(b_arr.arr[i].title);
        free(b_arr.arr[i].authors);
    }

    free(b_arr.arr);
}
