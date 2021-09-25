#ifndef INTERFACE_GUARD__H // wouldn't expect this file to be included more than once but eh, to be safe?
#define INTERFACE_GUARD__H

// saves library 
// return codes:
// 0 = success
// 1 = fail
int save_library();
int save_library_custom(const char* bookdbname, const char* userdbname, const char* loandbname);

// loads library
// return codes:
// 0 = success
// 1 = fail
int load_library();

// runs the interface
void run_interface();

#endif