#ifndef USER_MANAGEMENT_GUARD__H 
#define USER_MANAGEMENT_GUARD__H

#ifdef TESTING_FLAG
// expose User struct for testing
struct User {
    char* username; // unique
    unsigned int PIN;

    int permissions; // 0 = user
                     // 1 = librarian
};

// expose internals
extern struct User* user_db;
extern long user_db_size;
#else
struct User;
#endif

struct FILE;
extern const char* USER_MGMT_DB;

// stores the current db of users in memory to specified file
// return codes:
// 0 = success
// 1 = fail
int store_users(struct FILE* file);

// loads user details from specified file to memory
// return codes:
// 0 = success
// 1 = fail
int load_users(struct FILE* file);

// authenticates a user given a username and PIN
// return codes:
// 0 = success
// 1 = user does not exist (fail)
// 2 = incorrect PIN (fail)
int authenticate_user(const char* username, unsigned int PIN);

// registers a user given username and PIN
// return codes:
// 0 = success
// 1 = user already exists (fail)
// 2 = PIN incorrect length, expects 4 (fail)
int register_user(const char* username, unsigned int PIN, int permissions);

// returns the username of the currently authenticated user
// NULL if no one is logged in
char* get_authenticated_user_username();

// returns the permissions level of the currently authenticated user
// -1 if no one is logged in
int get_authenticated_user_permissions_level();

// logs out current logged in user
// return codes:
// 0 = success
// 1 = no user logged in (fail)
int logout_user();

#endif