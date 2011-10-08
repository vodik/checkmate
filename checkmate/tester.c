#include "tester.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void
fail(const char msg[])
{
	write(pfds[1], msg, strlen(msg));
	exit(EXIT_FAILURE);
}

void
fail_if(bool cond, const char msg[])
{
	if (cond) {
		write(pfds[1], msg, strlen(msg));
		exit(EXIT_FAILURE);
	}
}

void
fail_unless(bool cond, const char msg[])
{
	if (!cond) {
		write(pfds[1], msg, strlen(msg));
		exit(EXIT_FAILURE);
	}
}
