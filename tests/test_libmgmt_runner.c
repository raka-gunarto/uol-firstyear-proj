/* 
 * test_libmgmt_runner.c
 * 
 * AUTHOR: Raka Gunarto
 * DESC: Runs the whole test suite for the library management implementation lib.
 *	 using the Unity testing framework
 *
*/

#include <unity.h>
#include "lib_mgmt_testsuite.h"

/* Test initialisation - mainly for compatibility */
void setUp() {}
void tearDown() {}

int main() 
{
	UNITY_BEGIN();
	test_book_management();
	test_loans_management();
	test_user_management();
	
	return UNITY_END();
}



