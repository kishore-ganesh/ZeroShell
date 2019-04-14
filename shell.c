#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <dirent.h>
#include<fcntl.h>
#include<errno.h>
#define MAXARGS 128

//Should search in path, if present, then it would execute the program

char *defaultPaths[] = {"/usr/bin/", "/bin/"};
int numberOfPaths = 2;
int logComm = 0, logInt = 0;
int checkIfFileExists(char *path, char *name)
{
	DIR *directory = opendir(path);
	struct dirent *p = readdir(directory);
	while (p != NULL)
	{
		// printf("FILES in %s path are: %s\n", path, p->d_name);
		if (strcmp(p->d_name, name) == 0)
		{
			return 1;
		}

		p = readdir(directory);
	}

	return -1;
}

char *findFullPath(char *name, char **sources, int n)
{
	for (int i = 0; i < n; i++)
	{
		if (checkIfFileExists(sources[i], name) == 1)
		{
			char *fullPath = (char *)malloc((strlen(sources[i]) + strlen(name)) * sizeof(char *));
			strcpy(fullPath, sources[i]);
			// printf("%s\n", strcat(fullPath, name));
			return strcat(fullPath, name);
		}
	}

	return NULL;
}

//Support arguments for each program separately

void recursiveExecutor(char *programs[], int index, int numberOfPrograms, int fd[][2], char ***argumentList)
{

	//Have base case
	//If number of programs = 2, then rediredct outer to inner
	//If more than two, then execute recursive executor again inside
	//Add support for arguments for inner programs
	// pipe(fd[index]);
	// printf("%d\n", fd[index][0]);

	if (fork() == 0)
	{
		int logIntFD = open("LogInt.txt", O_APPEND|O_CREAT|O_WRONLY);
		// printf("LOGGING FD IS: %d\n", logIntFD);
		if (index < numberOfPrograms - 1)
		{
			close(1);
			dup(fd[index][1]);
			// close(fd[index][1]);
		}

		if (index > 0)
		{

			recursiveExecutor(programs, index - 1, numberOfPrograms, fd, argumentList);
			if(logInt)
			{
				
				char* s = (char*)malloc(128*sizeof(char));
				s[0]='\0';
				s = strcat(s, "Output till ");
				for(int i=0; i<=index; i++)
				{
					
					if(i>0)
					{
						s=strcat(s, " | ");
					}
					s=strcat(s, programs[i]);
					
				}

				//Wherever >> replace it by program
				//This printing should be along with arguments
				s = strcat(s, " is:");
				s=strcat(s, "\n");
				// printf("%s\n", s);
				int k = write(logIntFD, s, strlen(s));

				// printf("WRITTEN %d\n", k);
				// printf("ERROR IS: %d\n", errno);
				char b;
				int t_p[2];
				pipe(t_p);
				close(fd[index - 1][1]);
				while(read(fd[index-1][0], &b, sizeof(b))>0)
				{
					
					write(logIntFD, &b, sizeof(b));
					printf("%c", b);
					write(t_p[1], &b, sizeof(b));
				}

				printf("ENDED READING PIPE\n");

				fd[index-1][0] = t_p[0];
				fd[index-1][1] = t_p[1];

			}
			close(0);
			dup(fd[index - 1][0]);
			close(fd[index - 1][1]);
			close(fd[index - 1][0]);
			//also log last output
		}

		// execlp(findFullPath(programs[index], defaultPaths, numberOfPaths), programs[index], NULL);
		close(logIntFD);
		execve(findFullPath(programs[index], defaultPaths, numberOfPaths), argumentList[index], NULL);
	}

	else
	{
		// if(index<numberOfPrograms-1)
		// {
		for (int i = 0; i < index; i++)
		//Fix this
		{
			close(fd[i][0]);
			close(fd[i][1]);
		}
		// close(fd[index+1][0]);
		// }

		wait(NULL);
	}
}

//Need to fix the piping which is occuring due to the fact that in the parent process the pipe is still open

void executeProgramList(char *programs[], int index, int numberOfPrograms, char ***argumentList)
{
	int fd[numberOfPrograms][2];
	for (int i = 0; i < numberOfPrograms; i++)
	{
		pipe(fd[i]);
		// printf("%d %d\n", fd[i][0], fd[i][1]);
	}

	recursiveExecutor(programs, index, numberOfPrograms, fd, argumentList);
}

int getTokens(char *tokens[256], char *command)
{
	tokens[0] = strtok(command, " ");
	int i = 1;
	while ((tokens[i++] = strtok(NULL, " ")) != NULL)
		;
	i--;
	return i;
}

void processCommand(char *command)
{
	char *tokens[256] = {0};
	int i = getTokens(tokens, command);
	int argscount = 0, pipedcount = 0, filecount = 0;

	for (int j = 1; j < i; j++)
	{
		if (tokens[j][0] == '-')
		{
			argscount++;
		}

		else if (tokens[j][0] == '|')
		{
			pipedcount++;
		}

		else if (strcmp(tokens[j], ">>") == 0)
		{
			filecount++;
		}

		else
		{
			argscount++;
		}
	}

	//have separate argument list for each
	char **args[pipedcount + 1];
	char **pipedprograms = (char **)malloc((pipedcount + 1) * sizeof(char *));
	char **redirectedfiles = (char **)malloc(filecount * sizeof(char *));
	for (int i = 0; i < pipedcount + 1; i++)
	{
		args[i] = (char **)malloc((MAXARGS) * sizeof(char *));
		// memcpy(args[i], NULL, MAXARGS * sizeof(char *));
		for(int j=0; j< MAXARGS; j++)
		{
			args[i][j] = NULL;
		}
	}

	int pipedindex = 0, fileindex = 0, currentProgramIndex = 0;
	int argsindex[pipedcount + 1];

	for (int i = 0; i < pipedcount + 1; i++)
	{
		argsindex[i] = 0;
	}

	args[0][argsindex[0]++] = tokens[0];
	pipedprograms[pipedindex++] = tokens[0];
	for (int j = 1; j < i; j++)
	{
		if (tokens[j][0] == '-')
		{
			args[currentProgramIndex][argsindex[currentProgramIndex]++] = tokens[j];
		}
		else if (tokens[j][0] == '|')
		{
			pipedprograms[pipedindex] = tokens[j + 1];
			currentProgramIndex = pipedindex;
			args[currentProgramIndex][argsindex[currentProgramIndex]++] = tokens[j + 1];
			pipedindex++;
			j++;
		}

		else if (strcmp(tokens[j], ">>") == 0)
		{
			redirectedfiles[fileindex++] = tokens[j + 1];
			j++;
		}

		else
		{
			args[currentProgramIndex][argsindex[currentProgramIndex]++] = tokens[j];
		}
	}

	executeProgramList(pipedprograms, pipedindex - 1, pipedindex, args);
}

int main()
{

	while (1)
	{
		char command[256];
		printf("ENTER COMMAND\n");
		scanf(" %[^\n]s", command);
		if (strcmp("EXIT", command) == 0)
		{
			break;
		}

		else if (strcmp("LOGINT", command) == 0)
		{
			printf("STARTED LOGGING\n");
			logInt = 1;
		}

		else if (strcmp("UNLOGINT", command) == 0)
		{
			printf("ENDED LOGGING\n");
			logInt = 0;
		}
		else if (strcmp("UNLOGCOMM", command) == 0)
		{
		}

		else
		{
			processCommand(command);
			// if (fork() == 0)
			// {

			// 	execve(findFullPath(tokens[0], defaultPaths, numberOfPaths), args, NULL);
			// }

			// else
			// {
			// 	wait(NULL); //Use wait for status
			// }
		}
	}

	//Should we store relative order?
	//Choose arguments properly
	//Handle signals
	//Is the
	//Fix gcc error
	//Grep not working currently because we aren't passing arguments
	//Improve tokenization
	//Process both pipe and redirection together
	//Fix input bug
}