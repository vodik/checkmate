#include "isolate.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

pid_t
isolate(isolate_fn func, const void *arg)
{
	pid_t child;

	switch (child = fork()) {
		case -1:
			perror("fork");
			exit(EXIT_FAILURE);
			break;
		case 0:
			/* child */
			/* close(pfds[0]); */
			/* run_test(test); */
			exit(func(arg));
			break;
	}

	return child;
}
