#include <library_management/user_management.h>

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// User struct declaration
#ifndef TESTING_FLAG
struct User {
    char* username; // unique
    unsigned int PIN;

    int permissions; // 0 = user
                     // 1 = librarian
}
#endif

/* Constants */
const char* USER_MGMT_DB = "database/user_db.txt";

/* User Database */
#ifdef TESTING_FLAG
struct User* user_db;
long user_db_size;
#else
static struct User* user_db;
static long user_db_size;
#endif

static struct User authenticated_user; // currently logged in user

int store_users(struct FILE* file) {
    // check if file is valid
    if (file == NULL)
        return 1;

    // save books into file
    for(int i = 0; i < user_db_size; i++) {
        // get user in array
        struct User* curUserPtr = &user_db[i];

        // check if user is valid
        if (curUserPtr->username == NULL) continue;

        // save book
        fprintf((FILE*) file, "%s;%u;%u\n", curUserPtr->username, curUserPtr->PIN, curUserPtr->permissions);
    }

    return 0;
}

int load_users(struct FILE* file) {
    // pre-check file not null
    if (file == NULL)
        return 1;

    // firstly check if user_db is not null, if yes, free it (will only happen during testing)
    if(user_db)
        free(user_db);
    user_db = NULL;
    user_db_size = 0;

    // load the books
    char* buf = 0;
    size_t buf_size = 0;
    while(getline(&buf, &buf_size, file) != -1) {
        char* username = strtok(buf, ";");
        unsigned int PIN = atoi(strtok(NULL, ";"));
        int perms = atoi(strtok(NULL, ";"));

        register_user(username, PIN, perms);
    }

    return 0;
}

static struct User* expand_userdb() {
    long last_size = user_db_size;
    user_db_size += 10;

    if (last_size > 0)
        user_db = realloc(user_db, user_db_size * sizeof(struct User));
    else
        user_db = malloc(user_db_size * sizeof(struct User));
    for(long i = last_size; i < user_db_size; ++i)
        user_db[i].username = NULL;

    return user_db + last_size;
}

static struct User* get_free_userdb_space() {
    for (int i = 0; i < user_db_size; i++) {
        if (user_db[i].username == NULL)
            return user_db + i;
    }

    return expand_userdb();
}

static void copy_user(struct User* dest, struct User src) {
    dest->username = malloc(sizeof(char) * (strlen(src.username) + 1));

    strcpy(dest->username, src.username);
    dest->PIN = src.PIN;
    dest->permissions = src.permissions;
}

int authenticate_user(const char* username, unsigned int PIN) {
    for (int i = 0; i < user_db_size; ++i) {
        if(user_db[i].username == NULL) continue;
        if (strcmp(user_db[i].username, username) == 0)
            if (user_db[i].PIN == PIN) {
                if (authenticated_user.username)
                    free(authenticated_user.username);

                copy_user(&authenticated_user, user_db[i]);
                return 0;
            } else return 2;
    }
    return 1;
}

int register_user(const char* username, unsigned int PIN, int permissions) {
    struct User* freebook = get_free_userdb_space();
    freebook->username = malloc(sizeof(char) * (strlen(username) + 1));
    strcpy(freebook->username, username);
    freebook->PIN = PIN;
    freebook->permissions = permissions;
    return 0;
}

char* get_authenticated_user_username() {
    if(authenticated_user.username == NULL) return NULL;

    char* user = malloc(sizeof(char) * (strlen(authenticated_user.username) + 1));
    strcpy(user, authenticated_user.username);
    return user;
}

int get_authenticated_user_permissions_level() {
    if(authenticated_user.username == NULL) return -1;

    return authenticated_user.permissions;
}

int logout_user() {
   if(authenticated_user.username == NULL) return -1;
   free(authenticated_user.username);
   authenticated_user.username = NULL;
   return 0;
}