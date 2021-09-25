# Coursework 1
### IMPORTANT - This program works using relative paths from the working directory (wherever it was executed from, not the executable path of the program). 
## Building the project
### Steps 
 1. Clone **recursively** (as Unity testing framework is a submodule) using `git clone --recursive git@gitlab.com:comp1921/20/sc19rag.git`
 2. Run the `build.sh` script
	 - What `build.sh` does:
	 - Creates a build folder `build/` and `cd` to it
	 - Create makefiles using CMake `cmake ..`
	 - Make the project using `make`
 3. That's it!

## Testing the project
### WARNING 
#### Running without `test.sh`
The test program expects a `testfiles/` directory with the test files in them. This directory is provided as `testfiles/`
#### `database/`  directory
The test program will also expect a `database/` subdirectory to write to.

Additionally, the test program will attempt to save dummy data into the `database/` directory. Any saved state in there will be deleted, `test.sh` automatically tries to make a copy of `database/` if it exists. 

### Steps -- with `test.sh`
1. Run `test.sh`
2. That's it!
### Steps -- without `test.sh`
1. Ensure a `testfiles/` subdirectory exists with the testfiles in them.
2. Ensure a `database/` subdirectory exists.
3. Run the program.
4. That's it!

## Running the project
### Steps -- with `run.sh`
1. Run `run.sh`
2. That's it!
### Steps -- without `run.sh`
1. Ensure a `database/` directory exists 
2. Run the program
3. That's it! 
