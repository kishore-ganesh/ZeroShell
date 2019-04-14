# ZeroShell
A work in progress attempt to make a shell that supports piping and redirection in Linux

Current status:

1. ZeroShell is able to execute most Linux commands, and piping is working fine. Just like the traditional Bash shell, you can pipe any number of programs any number of times.

2. File redirection is currently being worked on. Logging support will also be added soon.

3. There are also plans to integrate signals into the system to allow Ctrl-C'ing and killing processes.

Usage:

Compile with gcc shell.c
Start with ./a.out
And then you can proceed to give commands to the interface
To exit type EXIT
