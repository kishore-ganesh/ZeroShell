#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <dirent.h>

//Should search in path, if present, then it would execute the program

char *defaultPaths[] = {"/usr/bin/", "/bin/"};
int numberOfPaths = 2;
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

void recursiveExecutor(char *programs[], int index, int numberOfPrograms, int fd[][2])
{

	//Have base case
	//If number of programs = 2, then rediredct outer to inner
	//If more than two, then execute recursive executor again inside
	//Add support for arguments for inner programs
	// pipe(fd[index]);
	// printf("%d\n", fd[index][0]);

	if (fork() == 0)
	{
		if (index < numberOfPrograms - 1)
		{
			close(1);
			dup(fd[index][1]);
			// close(fd[index][1]);
		}

		if (index > 0)
		{

			recursiveExecutor(programs, index - 1, numberOfPrograms, fd);
			close(0);	
			dup(fd[index - 1][0]);
			close(fd[index - 1][1]);
			close(fd[index - 1][0]);
			
		}

		execlp(findFullPath(programs[index], defaultPaths, numberOfPaths), programs[index], NULL);
	}

	else
	{
		// if(index<numberOfPrograms-1)
		// {
			for(int i=0; i<index; i++)
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

void executeProgramList(char *programs[], int index, int numberOfPrograms)
{
	int fd[numberOfPrograms][2];
	for (int i = 0; i < numberOfPrograms; i++)
	{
		pipe(fd[i]);
		// printf("%d %d\n", fd[i][0], fd[i][1]);
	}

	recursiveExecutor(programs, index, numberOfPrograms, fd);
}

// else
// {
// 	// close(0);
// 	// dup(fd[index][0]);

// 	// 	//check indexe
// 	// 	execlp(findFullPath(programs[index+1], sources, numberOfPaths));
// }

// else
// {

// 	if (numberOfPrograms >= 2)
// 	{
// 		execlp(findFullPath(programs[index + 1]))
// 	}
// }

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
		}

		else if (strcmp("UNLOGINT", command) == 0)
		{
		}
		else if (strcmp("UNLOGCOMM", command) == 0)
		{
		}

		else
		{
			char *tokens[256] = {0};
			tokens[0] = strtok(command, " ");
			int i = 1, argscount = 0, pipedcount = 0, filecount = 0;
			while ((tokens[i++] = strtok(NULL, " ")) != NULL)
				;
			i--;
			
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
			char **args = (char **)malloc((argscount + 2) * sizeof(char *));
			args[argscount + 1] = NULL;
			char **pipedprograms = (char **)malloc((pipedcount + 1) * sizeof(char *));
			char **redirectedfiles = (char **)malloc(filecount * sizeof(char *));
			int argsindex = 0, pipedindex = 0, fileindex = 0;
			args[argsindex++] = tokens[0];
			pipedprograms[pipedindex++] = tokens[0];
			for (int j = 1; j < i; j++)
			{
				if (tokens[j][0] == '-')
				{
					args[argsindex++] = tokens[j];
				}
				else if (tokens[j][0] == '|')
				{
					pipedprograms[pipedindex++] = tokens[j + 1];
					j++;
				}

				else if (strcmp(tokens[j], ">>") == 0)
				{
					redirectedfiles[fileindex++] = tokens[j + 1];
					j++;
				}

				else
				{
					args[argsindex++] = tokens[j];
				}
			}

			// printf("PATH IS: %s\n", findFullPath(tokens[0], defaultPaths, numberOfPaths));
			executeProgramList(pipedprograms, pipedindex - 1, pipedindex);
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
}