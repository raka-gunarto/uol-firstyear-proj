add_executable(library_mgmt_sys_lib_tests
	test_book_management.c
	test_loans_management.c
	test_user_management.c
	test_libmgmt_runner.c
)

target_link_libraries(library_mgmt_sys_lib_tests
	library_management
	Unity
)

# add automated tests, run using 'ctest', add --verbose option for more details
add_test(library_lib_test library_mgmt_sys_lib_tests)
