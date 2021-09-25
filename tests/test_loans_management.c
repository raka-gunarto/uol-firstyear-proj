/* 
 * test_loans_management.c
 * 
 * AUTHOR: Raka Gunarto
 * DESC: Runs tests for the loans management module
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
#include <library_management/loans_management.h>

/* constants -- static to restrict scope to this file */
static const char* LOAD_LOANS_TEST = "testfile/loandb_loadtest.txt";
static const struct Loan LOAD_LOANS_TEST_CONTENTS[] = {
	{.username = "test guy 0\0", .book_ID = 2},
    {.username = "test guy 1\0", .book_ID = 4},
    {.username = "test guy 2\0", .book_ID = 5},
    {.username = "test guy 2\0", .book_ID = 4}
};

#include "lib_mgmt_testsuite.h"

void test_store_loans() {
    // setup loan_db
	loan_db = malloc(sizeof(struct Loan) * 5);
	loan_db_size = 5;

	// populate loan_db - string literals but whatever
	struct Loan l0 = {"test store user 1", 1};	
	struct Loan l1 = {"test store user 2", 2};
	struct Loan l2 = {"test store user 3", 3};
	struct Loan l3 = {"test store user 4", 4};
	struct Loan l4 = {"test store user 5", 5};

	loan_db[0] = l0;
	loan_db[1] = l1;
	loan_db[2] = l2;
	loan_db[3] = l3;	
	loan_db[4] = l4;

	// test store loans invalid file
	TEST_ASSERT_EQUAL_INT_MESSAGE(1, store_loans((struct FILE*) NULL), "store_loans() should fail given a NULL ptr to a file");

	// test store loans valid
	FILE* f = fopen(LOAN_MGMT_DB,"w");
	int result = store_loans((struct FILE*) f);

	TEST_ASSERT_EQUAL_INT_MESSAGE(0, result, "store_loans() failed to read a valid file");
	
	// additionally test that files actually has correct loans
	fclose(f);
	f = fopen(LOAN_MGMT_DB,"r");
	char* buf = NULL;
	size_t bufSize = 0;
	int i = 0;
	for (i = 0; i < 5; i++)
	{ 
		int len = getdelim(&buf, &bufSize, ';', f);
		buf[strlen(buf)-1] = '\0'; // remove delim
		TEST_ASSERT(len != -1); // no line
		TEST_ASSERT_EQUAL_STRING(loan_db[i].username, buf);
		
		len = getdelim(&buf, &bufSize, '\n', f);
		buf[strlen(buf)-1] = '\0'; // remove delim
		TEST_ASSERT(len != -1); // no line
		TEST_ASSERT_EQUAL_INT(loan_db[i].book_ID, atoi(buf));
	}
	fclose(f);
}

void test_load_loans() {
    // test invalid file
	TEST_ASSERT_EQUAL_INT_MESSAGE(1, load_loans((struct FILE*) NULL), "load_loans() should return 1 (fail) if given a NULL file pointer");
	
	// open test file
	FILE* f = fopen(LOAD_LOANS_TEST, "r");
	if (f == NULL) 
	{
		TEST_IGNORE_MESSAGE("[WARN] test_load_loans ignored, test file not found!");
	}

	// load loans and expect pass
	int result = load_loans((struct FILE*) f);
	TEST_ASSERT_EQUAL_INT_MESSAGE(0, result, "load_loans() non-zero return val when given a valid file");

	// test files read properly
	for (int i = 0; i < 4; i++) 
	{
		TEST_ASSERT_EQUAL_STRING(LOAD_LOANS_TEST_CONTENTS[i].username, loan_db[i].username);
		TEST_ASSERT_EQUAL_INT(LOAD_LOANS_TEST_CONTENTS[i].book_ID, loan_db[i].book_ID);
	}	
}

void test_borrow_book() {
    // loan a book
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, borrow_book("someone", 2), "failed to borrow a book");

    // try to loan the same book
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, borrow_book("someone", 2), "failed to detect that user has already borrowed that book");

    // check the book is actually in the array
    int loan_idx;
    for(loan_idx = 0; loan_idx < 5; loan_idx++) {
        if((strcmp(loan_db[loan_idx].username, "someone") == 0) && loan_db[loan_idx].book_ID == 2)
            break;
    }
    TEST_ASSERT_LESS_THAN_INT_MESSAGE(5, loan_idx, "loan wasn't recorded");
}

void test_return_book() {
    // loan a book
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, return_book("someone", 2), "failed to return a book");

    // try to loan the same book
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, return_book("someone", 2), "somehow returned a book that user isn't borrowing");

    // check the book is actually not in the array
    int loan_idx;
    for(loan_idx = 0; loan_idx < 4; loan_idx++) {
        if((strcmp(loan_db[loan_idx].username, "someone") == 0) && loan_db[loan_idx].book_ID == 2)
            break;
    }
    TEST_ASSERT_EQUAL_INT_MESSAGE(4, loan_idx, "loan wasn't removed after return");
}

void test_find_loans_by_username() {
    // find "test guy 2"
	struct LoansArray result = find_loans_by_username("test guy 2");
	TEST_ASSERT_NOT_NULL_MESSAGE(result.arr, "find_loans_by_username() failed to find loan");
	TEST_ASSERT_EQUAL_INT_MESSAGE(2, result.size, "too many loans was found?");

	// test the loan array contents
	TEST_MESSAGE("Testing returned loans == loans in db");
	TEST_ASSERT_EQUAL_STRING("test guy 2", result.arr[0].username);
	TEST_ASSERT_EQUAL_INT(5, result.arr[0].book_ID);
	TEST_ASSERT_EQUAL_STRING("test guy 2", result.arr[1].username);
	TEST_ASSERT_EQUAL_INT(4, result.arr[1].book_ID);

	// free the array inside
	free_LoansArray(result);

	// find loans with user idonotexist (should fail) (sanity check, very edge case)
	result = find_loans_by_username("idonotexist");
	TEST_ASSERT_EQUAL_INT_MESSAGE(0, result.size, "find_loans_by_username() found a non-existent book");
}

void test_find_loans_by_id() {
    // find book 4
	struct LoansArray result = find_loans_by_id(4);
	TEST_ASSERT_NOT_NULL_MESSAGE(result.arr, "find_loans_by_id() failed to find loan");
	TEST_ASSERT_EQUAL_INT_MESSAGE(2, result.size, "too many loans was found?");

	// test the loan array contents
	TEST_MESSAGE("Testing returned loans == loans in db");
	TEST_ASSERT_EQUAL_STRING("test guy 1", result.arr[0].username);
	TEST_ASSERT_EQUAL_INT(4, result.arr[0].book_ID);
	TEST_ASSERT_EQUAL_STRING("test guy 2", result.arr[1].username);
	TEST_ASSERT_EQUAL_INT(4, result.arr[1].book_ID);

	// free the array inside
	free_LoansArray(result);

	// find loans with id 10 (should fail) (sanity check, very edge case)
	result = find_loans_by_id(10);
	TEST_ASSERT_EQUAL_INT_MESSAGE(0, result.size, "find_loans_by_id() found a non-existent book");
}

void test_loans_management()
{
    RUN_TEST(test_store_loans);
    RUN_TEST(test_load_loans);
    RUN_TEST(test_borrow_book);
    RUN_TEST(test_return_book);
    RUN_TEST(test_find_loans_by_username);
    RUN_TEST(test_find_loans_by_id);
}
