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

/*int getProcIdByName(string procName)
{
    int pid = -1;

    // Open the /proc directory
    DIR *dp = opendir("/proc");
    if (dp != NULL)
    {
        // Enumerate all entries in directory until process found
        struct dirent *dirp;
        while (pid < 0 && (dirp = readdir(dp)))
        {
            // Skip non-numeric entries
            int id = atoi(dirp->d_name);
            if (id > 0)
            {
                // Read contents of virtual /proc/{pid}/cmdline file
                string cmdPath = string("/proc/") + dirp->d_name + "/cmdline";
                ifstream cmdFile(cmdPath.c_str());
                string cmdLine;
                getline(cmdFile, cmdLine);
                if (!cmdLine.empty())
                {
                    // Keep first cmdline item which contains the program path
                    size_t pos = cmdLine.find('\0');
                    if (pos != string::npos)
                        cmdLine = cmdLine.substr(0, pos);
                    // Keep program name only, removing the path
                    pos = cmdLine.rfind('/');
                    if (pos != string::npos)
                        cmdLine = cmdLine.substr(pos + 1);
                    // Compare against requested process name
                    if (procName == cmdLine)
                        pid = id;
                }
            }
        }
    }

    closedir(dp);

    return pid;
}*/

pid_t proc_find(const char* name) 
{
    DIR* dir;
    struct dirent* ent;
    char* endptr;
    char buf[512];
    
    if (!(dir = opendir("/proc"))) {
        perror("can't open /proc");
        return -1;
    }

    while((ent = readdir(dir)) != NULL) {
        /* if endptr is not a null character, the directory is not
         * entirely numeric, so ignore it */
        long lpid = strtol(ent->d_name, &endptr, 10);
        if (*endptr != '\0') {
            continue;
        }

        /* try to open the cmdline file */
        snprintf(buf, sizeof(buf), "/proc/%ld/cmdline", lpid);
        FILE* fp = fopen(buf, "r");
        
        if (fp) {
            if (fgets(buf, sizeof(buf), fp) != NULL) {
                /* check the first token in the file, the program name */
                char* first = strtok(buf, " ");
                if (!strcmp(first, name)) {
                    fclose(fp);
                    closedir(dir);
                    return (pid_t)lpid;
                }
            }
            fclose(fp);
        }
        
    }
    
    closedir(dir);
    return -1;
}


void* xrealloc (void *ptr, size_t size)
{
	void *value = realloc (ptr, size);
	if (value == 0)
		printf("Virtual memory exhausted");
	return value;
}

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


void sendOKSignal()
{
	const char* proc_name = "EmSyRU";
	pid_t pid = proc_find(proc_name);
	if(pid > 0)
		kill(pid, SIGUSR1);
}

void signalHandler(int signum)
{
    if (signum == SIGUSR1)
    {
        printf("Received SIGUSR1!\n");
        SIGNALRECEIVED = 1;
    }
}

void registerHandler()
{
	signal(SIGUSR1, signalHandler);
}

#endif
