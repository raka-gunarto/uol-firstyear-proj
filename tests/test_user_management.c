/*
 * test_user_management.c
 * 
 * AUTHOR: Raka Gunarto
 * DESC: Runs tests for the user_management module
 *
*/

/* includes for testing */
#define _GNU_SOURCE
#include "lib_mgmt_testsuite.h"
#include <unity.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* modules to test */
#include <library_management/user_management.h>

/* constants -- static to restrict scope to this file */
static const char* LOAD_USER_TEST = "testfile/userdb_loadtest.txt";
static const struct User LOAD_USER_TEST_CONTENTS[] = {
	{.username = "testlibrarian\0", .PIN = 1337, .permissions = 1},
	{.username = "testuser1\0", .PIN = 1234, .permissions = 0},
	{.username = "testuser2\0", .PIN = 6789, .permissions = 0},
};

void test_store_users() {
    // setup user_db
	user_db = malloc(sizeof(struct User) * 2);
	user_db_size = 2;

	// populate user_db -- puts stack pointers in a global object, should reconsider later
	struct User u0 = {"thelibrarian", 1000, 1};	
	struct User u1 = {"rakag", 2222, 0};				

	user_db[0] = u0;
    user_db[1] = u1;

	// test store users invalid file
	TEST_ASSERT_EQUAL_INT_MESSAGE(1, store_users((struct FILE*) NULL), "store_users() should fail given a NULL ptr to a file");

	// test store users valid
	FILE* f = fopen(USER_MGMT_DB,"w");
	int result = store_users((struct FILE*) f);

	TEST_ASSERT_EQUAL_INT_MESSAGE(0, result, "store_users() failed to read a valid file");
	
	// additionally test that db has correct users
	fclose(f);
	f = fopen(USER_MGMT_DB,"r");
	char* buf = NULL;
	size_t bufSize = 0;
	int i = 0;
	TEST_MESSAGE("Testing the database file has the saved users");
	for (i = 0; i < 2; i++)
	{
		int len = getdelim(&buf, &bufSize, ';', f);
		buf[strlen(buf)-1] = '\0'; // remove delim
		TEST_ASSERT(len != -1); // no line
		TEST_ASSERT_EQUAL_STRING(user_db[i].username, buf);
	
		len = getdelim(&buf, &bufSize, ';', f);
		buf[strlen(buf)-1] = '\0'; // remove delim
		TEST_ASSERT(len != -1); // no line
		TEST_ASSERT_EQUAL_INT(user_db[i].PIN, atoi(buf));
	
		len = getdelim(&buf, &bufSize, '\n', f);
		buf[strlen(buf)-1] = '\0'; // remove delim
		TEST_ASSERT(len != -1); // no line
		TEST_ASSERT_EQUAL_INT(user_db[i].permissions, atoi(buf));
	}

    if(buf != NULL)
        free(buf);
    
	fclose(f);
}

void test_load_users() {
	// test invalid file
	TEST_ASSERT_EQUAL_INT_MESSAGE(1, load_users((struct FILE*) NULL), "load_users() should return 1 (fail) if given a NULL file pointer");
	
	// open test file
	FILE* f = fopen(LOAD_USER_TEST, "r");
	if (f == NULL) 
	{
		TEST_IGNORE_MESSAGE("[WARN] load_users ignored, test file not found!");
	}

	// load users and expect pass
	int result = load_users((struct FILE*) f);
	TEST_ASSERT_EQUAL_INT_MESSAGE(0, result, "load_users() non-zero return val when given a valid file");

	// test users read properly
	TEST_MESSAGE("Testing user_db has the intended users");
	for (int i = 0; i < 3; i++) 
	{
		TEST_ASSERT_EQUAL_STRING(LOAD_USER_TEST_CONTENTS[i].username, user_db[i].username);
		TEST_ASSERT_EQUAL_INT(LOAD_USER_TEST_CONTENTS[i].PIN, user_db[i].PIN);
		TEST_ASSERT_EQUAL_INT(LOAD_USER_TEST_CONTENTS[i].permissions, user_db[i].permissions);
	}
}

void test_authenticate_user() {
	// test wrong password
	int result = authenticate_user("testlibrarian",0000);
	TEST_ASSERT_EQUAL_INT_MESSAGE(2, result, "authenticate_user() should return 1 on incorrect password");

	// test user doesn't exist
	result = authenticate_user("testidontexist", 1111);
	TEST_ASSERT_EQUAL_INT_MESSAGE(1, result, "authenticate_user() should return 2 on user not found");

	// test valid user
	result = authenticate_user("testlibrarian", 1337);
	TEST_ASSERT_EQUAL_INT_MESSAGE(0, result, "authenticate_user() failed to authenticate user with valid credentials");
}

void test_get_authenticated_user_username() {
	// logout user and attempt to get username, expect NULL
	logout_user();
	char* username = get_authenticated_user_username();
	TEST_ASSERT_NULL_MESSAGE(username, "should not be getting a username when no user is logged in, but could be a logout_user() issue");

	// login user and attempt to get username, expect !NULL and username
	authenticate_user("testlibrarian", 1337);
	free(username);
	username = get_authenticated_user_username();
	TEST_ASSERT_NOT_NULL_MESSAGE(username, "should be getting a username when a user is logged in, but could be an authenticate_user() issue");
	TEST_ASSERT_EQUAL_STRING_MESSAGE("testlibrarian", username, "retrieved the wrong username for logged in user");
}

void test_get_authenticated_user_permissions_level() {
	// logout user and attempt to get username, expect -1
	logout_user();
	int perms = get_authenticated_user_permissions_level();
	TEST_ASSERT_EQUAL_INT_MESSAGE(-1, perms, "should not be getting a perms level, but could be a logout_user() issue");

	// login librarian and attempt to get perms, expect 0
	authenticate_user("testlibrarian", 1337);
	perms = get_authenticated_user_permissions_level();
	TEST_ASSERT_EQUAL_INT_MESSAGE(1, perms, "retrieved the wrong perms for logged in user");

	// login librarian and attempt to get perms, expect 1
	authenticate_user("testuser1", 1234);
	perms = get_authenticated_user_permissions_level();
	TEST_ASSERT_EQUAL_INT_MESSAGE(0, perms, "retrieved the wrong perms for logged in user");
}

void test_logout_user() {
	// log out
	logout_user();

	// expect NULL and -1 on username and perms respectively
	TEST_ASSERT_NULL_MESSAGE(get_authenticated_user_username(), "username is not null after logout");
	TEST_ASSERT_EQUAL_INT_MESSAGE(-1, get_authenticated_user_permissions_level(), "perms level != -1 after logout");
}

void test_user_management() {
    RUN_TEST(test_store_users);
	RUN_TEST(test_load_users);
	RUN_TEST(test_authenticate_user);
	RUN_TEST(test_get_authenticated_user_username);
	RUN_TEST(test_get_authenticated_user_permissions_level);
	RUN_TEST(test_logout_user);
}
