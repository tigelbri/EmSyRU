//## Copyright (c) 2015, Tristan Igelbrink
//## All rights reserved.
//## 
//## Redistribution and use in source and binary forms, with or without
//## modification, are permitted provided that the following conditions 
//## are met:
//##
//## 1. Redistributions of source code must retain the above copyright
//## notice, this list of conditions and the following disclaimer.
//##
//## 2. Redistributions in binary form must reproduce the above copyright
//## notice, this list of conditions and the following disclaimer in the
//## documentation and/or other materials provided with the distribution.
//## 
//## THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//## "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
//## LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
//## FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
//## COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT
//## , INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//## (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
//## SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
//## HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
//## STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
//## ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
//## OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <signal.h>
#include <stdio.h>
#include <stdlib.h> 
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

int SIGNALRECEIVED = 0;

/**
* Method for finding process id by its name
* @param const char* process name
* @return pid_t with process id if found else -1
*/
pid_t proc_find(const char* name) 
{
	DIR* dir;
	struct dirent* ent;
	char* endptr;
	char buf[512];
	
	if (!(dir = opendir("/proc")))
		return perror("can't open /proc"), -1;

	while((ent = readdir(dir)) != NULL)
	{
		/* if endptr is not a null character, the directory is not
		 * entirely numeric, so ignore it */
		long lpid = strtol(ent->d_name, &endptr, 10);
		if (*endptr != '\0')
			continue;
		/* try to open the cmdline file */
		snprintf(buf, sizeof(buf), "/proc/%ld/cmdline", lpid);
		FILE* fp = fopen(buf, "r");
		
		if (fp) 
		{
			if (fgets(buf, sizeof(buf), fp) != NULL)
			{
				/* check the first token in the file, the program name */
				char* first = strtok(buf, " ");
				size_t name_len = strlen(name); 
				size_t first_len = strlen(first);
				char* arr = new char[name_len];
				size_t i;
				for(i = 0; i < name_len; i++)
					arr[i] = first[first_len - name_len + i];
				if (!strcmp(arr, name))
				{
					fclose(fp);
					closedir(dir);
					return (pid_t)lpid;
				}
				free(arr);
			}
			fclose(fp);
		}
	}
	closedir(dir);
	return -1;
}

/**
* Reallocates memory
* @param void* pointer to old memory address
* @param size_t with new memory size
* @return void* to new memory address
*/
void* xrealloc (void *ptr, size_t size)
{
	void *value = realloc (ptr, size);
	if (value == 0)
		printf("Virtual memory exhausted");
	return value;
}

/**
* Allocates memory for file content and reads the given file
* @param const char* file name 
* @return char* file content
*/
char* readlink_malloc (const char *filename)
{
	int size = 100;
	char *buffer = NULL;

	while (1)
	{
		buffer = (char *) xrealloc (buffer, size);
		int nchars = readlink (filename, buffer, size);
		if (nchars < 0)
		{
			free (buffer);
			return NULL;
		}
		if (nchars < size)
			return buffer;
		size *= 2;
	}
}

/**
* Sanity Check method
* This method is crucial for the Sanity Check and the checked application must use it.
* It searches for the EmSyRU process ID and sends the SIGUSR1 signal to it.
*/
void sendOKSignal()
{
	const char* proc_name = "EmSyRU";
	pid_t pid = proc_find(proc_name);
	if(pid > 0)
		kill(pid, SIGUSR1);
}

/**
* Sanity Check handler
* This method is crucial for the Sanity Check because it handles the signal from the checked application
* If the signal is the SIGUSR1 signal an internal flag is set.
* @param int signale to handle
*/
void signalHandler(int signum)
{
    if (signum == SIGUSR1)
    {
        SIGNALRECEIVED = 1;
    }
}

/**
* Linux signal handler
* Connects the incoming signals to the signal handler.
*/
void registerHandler()
{
	signal(SIGUSR1, signalHandler);
}

#endif
