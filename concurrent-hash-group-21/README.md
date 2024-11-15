# Concurrent Hash Table

This project implements a concurrent hash table with read-write locks in C.

## How to Compile

### Windows
1. Open the terminal from the project folder.
2. Run the command: `mingw32-make`
3. To run the compiled file, use: `chash.exe`

### Linux
1. Open the terminal from the project folder.
2. Run the command: `make`
3. To run the compiled file, use: `./chash`

## Usage

1. Ensure that `commands.txt` is in the same directory as the executable.
2. Run the executable (`chash.exe` on Windows or `./chash` on Linux).
3. The program will read the commands from `commands.txt` and output the results to `output.txt`.

## AI Assistance

GitHub Copilot was used to generate some of the thread and condition variable operations. The AI provided suggestions for implementing the read-write locks, condition variables, and thread synchronization mechanisms. Specifically, "implement thread synchronization using pthreads" and "generate code for condition variables in C" were such prompts used to get the results.

I used AI in some debugging for how to do the makefile. It provided some structure on how to do that, and it helped with any errors/warnings that occured while I tried to compile the makefile. I also used it to refresh my knowledge on some of the functions in .h files (string.h,stdio.h,etc) to ensure that I was using them correctly and that they could be used for the purposes that I needed them. -Noah Elder 
