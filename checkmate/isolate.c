#include "isolate.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

struct cell *
isolate(isolate_fn func, const void *arg)
{
	struct cell *cell = malloc(sizeof(struct cell));
	int pipefd[2];

	if (pipe(pipefd) == -1) {
		perror("pipe");
		exit(EXIT_FAILURE);
	}

	switch (cell->pid = fork()) {
		case -1:
			perror("fork");
			exit(EXIT_FAILURE);
			break;
		case 0:
			/* child */
			close(pipefd[0]);
			exit(func(arg));
			break;
	}

	close(pipefd[1]);
	cell->pfd = pipefd[0];
	return cell;
}
