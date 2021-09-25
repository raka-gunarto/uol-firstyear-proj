/*
 * test_book_management.c
 * 
 * AUTHOR: Raka Gunarto
 * DESC: Runs tests for the book_management module
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
#include <library_management/book_management.h>

/* constants -- static to restrict scope to this file */
static const char* LOAD_BOOK_TEST = "testfile/bookdb_loadtest.txt";
static const struct Book LOAD_BOOK_TEST_CONTENTS[] = {
	{.id = 0, .title = "test book 0\0", .authors = "author 0", .year = 2000, .copies = 2},
	{.id = 1, .title = "test book 1\0", .authors = "author 1", .year = 2010, .copies = 5},
	{.id = 2, .title = "test book 2\0", .authors = "author 2", .year = 2020, .copies = 3},
	{.id = 3, .title = "test book 3\0", .authors = "author 3", .year = 2030, .copies = 7},
	{.id = 4, .title = "test book 4\0", .authors = "author 4", .year = 2040, .copies = 6}
};

/*
 * Tests saving the library to a file
 *
 * Expects:
 * - 0 and books stored, for valid book database and file ptr
 * - 1, for null file ptr 
*/
void test_store_books() {
	// setup book_db
	book_db = malloc(sizeof(struct Book) * 5);
	book_db_size = 5;

	// populate book_db
	struct Book b0 = {0, "store 0", "storeauthor 0", 1990, 4};	
	struct Book b1 = {1, "store 1", "storeauthor 1", 1940, 3};		
	struct Book b2 = {2, "store 2", "storeauthor 2", 1960, 7};		
	struct Book b3 = {3, "store 3", "storeauthor 3", 1980, 3};		
	struct Book b4 = {4, "store 4", "storeauthor 4", 1910, 4};

	book_db[0] = b0;
	book_db[1] = b1;
	book_db[2] = b2;
	book_db[3] = b3;	
	book_db[4] = b4;

	// test store books invalid file
	TEST_ASSERT_EQUAL_INT_MESSAGE(1, store_books((struct FILE*) NULL), "store_books() should fail given a NULL ptr to a file");

	// test store books valid
	FILE* f = fopen(BOOK_MGMT_DB,"w");
	int result = store_books((struct FILE*) f);

	TEST_ASSERT_EQUAL_INT_MESSAGE(0, result, "store_books() failed to read a valid file");
	
	// additionally test that files actually has correct books
	fclose(f);
	f = fopen(BOOK_MGMT_DB,"r");
	char* buf = NULL;
	size_t bufSize = 0;
	int i = 0;
	for (i = 0; i < 5; i++)
	{
		int len = getdelim(&buf, &bufSize, ';', f);
		buf[strlen(buf)-1] = '\0'; // remove delim
		TEST_ASSERT(len != -1); // no line
		TEST_ASSERT_EQUAL_INT(book_db[i].id, atoi(buf));
		
		len = getdelim(&buf, &bufSize, ';', f);
		buf[strlen(buf)-1] = '\0'; // remove delim
		TEST_ASSERT(len != -1); // no line
		TEST_ASSERT_EQUAL_STRING(book_db[i].title, buf);
	
		len = getdelim(&buf, &bufSize, ';', f);
		buf[strlen(buf)-1] = '\0'; // remove delim
		TEST_ASSERT(len != -1); // no line
		TEST_ASSERT_EQUAL_STRING(book_db[i].authors, buf);
	
		len = getdelim(&buf, &bufSize, ';', f);
		buf[strlen(buf)-1] = '\0'; // remove delim
		TEST_ASSERT(len != -1); // no line
		TEST_ASSERT_EQUAL_INT(book_db[i].year, atoi(buf));
	
		len = getdelim(&buf, &bufSize, '\n', f);
		buf[strlen(buf)-1] = '\0'; // remove delim
		TEST_ASSERT(len != -1); // no line
		TEST_ASSERT_EQUAL_INT(book_db[i].copies, atoi(buf));
	}

	fclose(f);
}

void test_load_books() {
	// test invalid file
	TEST_ASSERT_EQUAL_INT_MESSAGE(1, load_books((struct FILE*) NULL), "load_books() should return 1 (fail) if given a NULL file pointer");
	
	// open test file
	FILE* f = fopen(LOAD_BOOK_TEST, "r");
	if (f == NULL) 
	{
		TEST_IGNORE_MESSAGE("[WARN] test_load_books ignored, test file not found!");
	}

	// load books and expect pass
	int result = load_books((struct FILE*) f);
	TEST_ASSERT_EQUAL_INT_MESSAGE(0, result, "load_books() non-zero return val when given a valid file");

	// test files read properly
	for (int i = 0; i < 5; i++) 
	{
		TEST_ASSERT_EQUAL_INT(LOAD_BOOK_TEST_CONTENTS[i].id, book_db[i].id);
		TEST_ASSERT_EQUAL_STRING(LOAD_BOOK_TEST_CONTENTS[i].title, book_db[i].title);
		TEST_ASSERT_EQUAL_STRING(LOAD_BOOK_TEST_CONTENTS[i].authors, book_db[i].authors);
		TEST_ASSERT_EQUAL_INT(LOAD_BOOK_TEST_CONTENTS[i].year, book_db[i].year);
		TEST_ASSERT_EQUAL_INT(LOAD_BOOK_TEST_CONTENTS[i].copies, book_db[i].copies);
	}	
}

void test_add_book() { 
	// create book
	struct Book book_to_add = {5, "test_add_book()'s test book", "test_book_management.c", 2020, 1};

	// add the book and expect pass
	int result = add_book(book_to_add);
	TEST_ASSERT_EQUAL_INT(0, result);

	// linear search to find the book
	int found = 0;
	for (int i = 0; i < book_db_size; i++) {
		if (book_db[i].id == 5) {
			TEST_ASSERT_EQUAL_STRING(book_db[i].title, book_to_add.title);
			TEST_ASSERT_EQUAL_STRING(book_db[i].authors, book_to_add.authors);
			TEST_ASSERT_EQUAL_INT(book_db[i].year, book_to_add.year);
			TEST_ASSERT_EQUAL_INT(book_db[i].copies, book_to_add.copies);

			found = 1; // we didn't fail!
		}
	}

	if (!found)
		TEST_FAIL_MESSAGE("added book not found in book_db");
}

void test_remove_book() {
	// remove book with id 0 
	int result = remove_book(0);
	TEST_ASSERT_EQUAL_INT_MESSAGE(0, result, "remove_book() non-zero return val given a valid book id to remove");

	// check book removed (id doesn't exist)
	int found = 0;
	for (int i = 0; i < book_db_size; i++) {
		if (book_db[i].id == 0) {
			TEST_FAIL_MESSAGE("remove_book() failed to remove book with specified id");
		}
	}

	// remove book with id 0 again (should fail)
	TEST_ASSERT_EQUAL_INT_MESSAGE(1, remove_book(0), "remove_book() did not fail given an invalid book id to remove");
}

void test_find_book_by_id() {
	// find id 2
	struct BookArray result = find_book_by_id(2);
	TEST_ASSERT_NOT_NULL_MESSAGE(result.arr, "test_find_book_by_id() failed to find valid book");
	TEST_ASSERT_EQUAL_INT_MESSAGE(1, result.size, "too many books was found?");

	// test the book contents
	TEST_MESSAGE("Testing returned book == book in db");
	TEST_ASSERT_EQUAL_STRING("test book 2", result.arr[0].title);
	TEST_ASSERT_EQUAL_STRING("author 2", result.arr[0].authors);
	TEST_ASSERT_EQUAL_INT(2020, result.arr[0].year);
	TEST_ASSERT_EQUAL_INT(3, result.arr[0].copies);

	// free the array inside
	free_BookArray(result);

	// find "test book 10" (should fail) (sanity check, very edge case)
	result = find_book_by_id(10);
	TEST_ASSERT_EQUAL_INT_MESSAGE(0, result.size, "test_find_book_by_id() found a non-existent book");
}

void test_find_book_by_title() {
	// find "test book 3"
	struct BookArray result = find_book_by_title("test book 3");
	TEST_ASSERT_NOT_NULL_MESSAGE(result.arr, "test_find_book_by_title() failed to find valid book");
	TEST_ASSERT_EQUAL_INT_MESSAGE(1, result.size, "too many books was found?");

	// test the book contents
	TEST_MESSAGE("Testing returned book == book in db");
	TEST_ASSERT_EQUAL_INT(3, result.arr[0].id);
	TEST_ASSERT_EQUAL_STRING("author 3", result.arr[0].authors);
	TEST_ASSERT_EQUAL_INT(2030, result.arr[0].year);
	TEST_ASSERT_EQUAL_INT(7, result.arr[0].copies);

	// free the array inside
	free_BookArray(result);

	// find "test book 10" (should fail) (sanity check, very edge case)
	result = find_book_by_title("test book 10");
	TEST_ASSERT_EQUAL_INT_MESSAGE(0, result.size, "test_find_book_by_authors() found a non-existent book");
}

void test_find_book_by_authors() {
	// find "author 4"
	struct BookArray result = find_book_by_author("author 4");
	TEST_ASSERT_NOT_NULL_MESSAGE(result.arr, "test_find_book_by_authors() failed to find valid book");
	TEST_ASSERT_EQUAL_INT_MESSAGE(1, result.size, "too many books was found?");

	// test the book contents
	TEST_MESSAGE("Testing returned book == book in db");
	TEST_ASSERT_EQUAL_INT(4, result.arr[0].id);
	TEST_ASSERT_EQUAL_STRING("test book 4", result.arr[0].title);
	TEST_ASSERT_EQUAL_INT(2040, result.arr[0].year);
	TEST_ASSERT_EQUAL_INT(6, result.arr[0].copies);

	// free the array inside
	free_BookArray(result);

	// find "test book 10" (should fail) (sanity check, very edge case)
	result = find_book_by_title("author 10");
	TEST_ASSERT_EQUAL_INT_MESSAGE(0, result.size, "test_find_book_by_authors() found a non-existent book");
}

void test_find_book_by_year() { // TODO: test find_book_by_year
	// find "test book 3"
	struct BookArray result = find_book_by_year(2040);
	TEST_ASSERT_NOT_NULL_MESSAGE(result.arr, "find_book_by_year() failed to find valid book");
	TEST_ASSERT_EQUAL_INT_MESSAGE(1, result.size, "too many books was found?");

	// test the book contents
	TEST_MESSAGE("Testing returned book == book in db");
	TEST_ASSERT_EQUAL_INT(4, result.arr[0].id);
	TEST_ASSERT_EQUAL_STRING("test book 4", result.arr[0].title);
	TEST_ASSERT_EQUAL_STRING("author 4", result.arr[0].authors);
	TEST_ASSERT_EQUAL_INT(6, result.arr[0].copies);

	// free the array inside
	free_BookArray(result);

	// find year 1000 (should fail) (sanity check, very edge case)
	result = find_book_by_year(1000);
	TEST_ASSERT_EQUAL_INT_MESSAGE(0, result.size, "find_book_by_year() found a non-existent book");
}

void test_book_management() { // TODO: test book_management
	// run tests
	RUN_TEST(test_store_books);
	RUN_TEST(test_load_books);
	RUN_TEST(test_add_book);
	RUN_TEST(test_remove_book);
	RUN_TEST(test_find_book_by_id);
	RUN_TEST(test_find_book_by_title);
	RUN_TEST(test_find_book_by_year);
	RUN_TEST(test_find_book_by_authors);
}
