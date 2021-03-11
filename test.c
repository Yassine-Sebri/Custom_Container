#define _GNU_SOURCE
#include <errno.h>	   // debugging
#include <string.h>	   // error to string
#include <sched.h>	   // flags
#include <unistd.h>	   // clone, getpid
#include <stdlib.h>	   // malloc
#include <stdio.h>	   // printf
#include <sys/types.h> // pid_t
#define STACK 8192

void print_err(char const *const reason)
{
	fprintf(stderr, "Error when %s: %s\n", reason, strerror(errno));
}

int print_pid()
{
	printf("The id of this process is %d.\n", getpid());
	return 0;
}

int main()
{
	void *stack;
	stack = malloc(STACK);
	print_pid();
	pid_t pid = clone(print_pid, (char *)stack + STACK, 0, 0);
	if (pid < 0)
	{
		print_err("calling clone");
		return 1;
	}
	free(stack);
	return 0;
}

