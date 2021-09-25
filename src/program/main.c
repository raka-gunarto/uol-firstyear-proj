#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include "interface.h"

static const char* bookdb_failover = "bookdb.bak.txt";
static const char* loandb_failover = "loandb.bak.txt";
static const char* userdb_failover = "userdb.bak.txt";

int main(int argc, char** argv){
	// attempt to load library
	if(load_library() != 0) {
		printf("Loading library database failed!\n\n");
		printf("This is to be expected on first run if the database files don't exist.\n");
		printf("Would you still like to continue (y/n)? ");

		char* buf;
		scanf("%ms", &buf);
		if(buf[0] != 'y' && buf[0] != 'Y') {
			free(buf);
			return 0;
		}
		free(buf);
	}

	// run interface
	run_interface();

	// attempt to save library
	// TODO: maybe catch sigabrt (ctrl-c) to always allow saving?
	if(save_library() != 0) {
		printf("Saving library database failed!\n");
		printf("Attempting to save in current directory instead...");
		if(save_library_custom(bookdb_failover, userdb_failover, loandb_failover) != 0) {
			printf("Saving to failover files failed.");
		}
		else {
			printf("Files successfully saved at %s, %s, %s", bookdb_failover, userdb_failover, loandb_failover);
		}
	}

	return 0;
}
