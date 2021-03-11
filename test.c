#define _GNU_SOURCE
#include <errno.h>	   // debugging
#include <string.h>	   // error to string
#include <sched.h>	   // flags
#include <unistd.h>	   // clone, getpid
#include <stdlib.h>	   // malloc
#include <stdio.h>	   // printf
#include <sys/types.h> // pid_t
#include <sys/mount.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <fcntl.h>
#define STACK 1024 * 1024

void print_err(char const *const reason)
{
	fprintf(stderr, "Error when %s: %s\n", reason, strerror(errno));
}

int shell()
{
	char *hostname = "container";
	if (mount("root mount", "root mount", NULL,
			  MS_REC | MS_BIND | MS_PRIVATE, NULL) != 0)
	{
		print_err("mounting proc");
		return 1;
	}
	mkdir("root mount/old", 0755);
	syscall(SYS_pivot_root, "root mount", "root mount/old");
	chdir("/");
	if (mount("proc", "/proc", "proc", 0, "") != 0)
	{
		print_err("mounting proc");
		return 1;
	}
	umount2("/old", MNT_DETACH);
	sethostname(hostname, strlen(hostname));
	system("/bin/sh");
	return 0;
}

int main()
{
	int flags = CLONE_NEWPID | CLONE_NEWNS | CLONE_NEWUSER | CLONE_NEWUTS | CLONE_VFORK;
	void *stack;
	stack = malloc(STACK);
	pid_t pid = clone(shell, (char *)stack + STACK, flags, 0);
	if (pid < 0)
	{
		print_err("calling clone");
		return 1;
	}
	free(stack);
	return 0;
}
