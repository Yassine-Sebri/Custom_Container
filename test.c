#define _GNU_SOURCE
#include <errno.h>	   // debugging
#include <string.h>	   // error to string
#include <sched.h>	   // flags
#include <unistd.h>	   // clone, getpid
#include <stdlib.h>	   // malloc
#include <stdio.h>	   // printf
#include <sys/types.h> // pid_t
#include <sys/mount.h>
#define STACK 1024 * 1024

void print_err(char const *const reason)
{
	fprintf(stderr, "Error when %s: %s\n", reason, strerror(errno));
}

int shell()
{
	if (mount("proc", "/proc", "proc", 0, "") != 0)
	{
		print_err("mounting proc");
		return 1;
	}
	system("/bin/zsh");
	return 0;
}

int main()
{
	void *stack;
	stack = malloc(STACK);
	pid_t pid = clone(shell, (char *)stack + STACK, CLONE_NEWPID | CLONE_NEWNS | CLONE_VFORK, 0);
	if (pid < 0)
	{
		print_err("calling clone");
		return 1;
	}
	free(stack);
	if (mount("proc", "/proc", "proc", 0, "") != 0)
	{
		print_err("mounting proc");
		return 1;
	}
	return 0;
}
