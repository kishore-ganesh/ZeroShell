# ZeroShell
A work in progress attempt to make a shell that supports piping and redirection in Linux

Current status:

1. ZeroShell is able to execute most Linux commands, and piping is working fine. Just like the traditional Bash shell, you can pipe any number of programs any number of times.

2. File Redirection support has been added, you can now redirect to files just like in Bash. One thing to note, however, is that while in Bash (and other compatible shells), in a command like ls >> t1.txt, the file is first created and then the ls command is run. Here, the ls command is run first and then the file is created. This is a trivial difference, and can easily be adapted to go either way.

3. Logging support has been added, and will save the intermediate values of pipe commands (except for last one) to a file named LogInt.txt. If the command is "ls | wc | wc", it will save the output of ls and ls | wc, but not ls | wc | wc. This small bug will be fixed soon.

4. There are also plans to integrate signals into the system to allow Ctrl-C'ing and killing processes.

Usage:

Compile with gcc shell.c
Start with ./a.out
And then you can proceed to give commands to the interface
To exit type EXIT

Supported built in commands: 
LOGINT: Start Logging
UNLOGINT: Stop logging
EXIT: Stop the shell interface

