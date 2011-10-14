#include "checkmate.h"

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/wait.h>

#include "tester.h"
#include "testset.h"
#include "ipc.h"
#include "isolate.h"

/* static volatile sig_atomic_t got_SIGCHLD = 0; */
/*  */
/* static void */
/* child_sig_handler(int sig) */
/* { */
/* 	printf("SIGCHLD!"); */
/* 	got_SIGCHLD = 1; */
/* } */

void
dump_msg(struct test_msg *msg, const struct score_cb *cb)
{
	switch (msg->event) {
		case TEST_INIT_OK:
		case TEST_TESTS_OK:
		case TEST_FREE_OK:
			cb->on_testset_end(msg->event, 0, 0, NULL);
			break;

		case TEST_START:
			cb->on_test_start(msg->name);
			break;
		case TEST_PASS:
			cb->on_test_end(msg->name, msg->event, 0, NULL);
			break;
		case TEST_FAIL:
			cb->on_test_end(msg->name, msg->event, 0, msg->msg);
			break;
		case TEST_TERM:
		case TEST_STOPPED:
			cb->on_test_end(msg->name, msg->event, msg->signal, msg->msg);
			break;
	}
}

void
monitor(int ipc, const struct score_cb *cb)
{
	sigset_t sigmask, empty_mask;
	/* struct sigaction sa; */
	fd_set readfds;
	int r;

	struct test_msg msg;

	sigemptyset(&sigmask);
	sigaddset(&sigmask, SIGCHLD);
	if (sigprocmask(SIG_BLOCK, &sigmask, NULL) == -1) {
		perror("sigprocmask");
		exit(EXIT_FAILURE);
	}

	sigemptyset(&empty_mask);

	for (;;) {
		FD_ZERO(&readfds);
		FD_SET(ipc, &readfds);

		r = pselect(ipc + 1, &readfds, NULL, NULL, NULL, &empty_mask);

		if (r == -1 && errno != EINTR) {
			perror("pselect");
			exit(EXIT_FAILURE);
		}

		if (FD_ISSET(ipc, &readfds)) {
			memset(&msg, 0, sizeof(struct test_msg));

			r = read(ipc, &msg, sizeof(struct test_msg));

			if (r == 0)
				return;
			else if (r == sizeof(struct test_msg))
				dump_msg(&msg, cb);
			else {
#if __LP64__
				printf("%d != %lu?\n", r, sizeof(struct test_msg));
#else
				printf("%d != %u?\n", r, sizeof(struct test_msg));
#endif
				exit(EXIT_FAILURE);
			}
		}

		/* if (got_SIGCHLD) { */
		/* 	got_SIGCHLD = 0; */
		/* 	return; */
		/* } */
	}
}

int
wait_for_child(pid_t child, int *action)
{
	pid_t w;
	int status;

	do {
		w = waitpid(child, &status, WUNTRACED | WCONTINUED);
		if (w == -1) {
			perror("waitpid");
			exit(EXIT_FAILURE);
		}

		if (WIFEXITED(status)) {
			if (action)
				*action = 0;
			return WEXITSTATUS(status);
		} else if (WIFSIGNALED(status)) {
			if (action)
				*action = 1;
			return WTERMSIG(status);
		} else if (WIFSTOPPED(status)) {
			if (action)
				*action = 2;
			return WSTOPSIG(status);
		}
	} while (!WIFEXITED(status) && !WIFSIGNALED(status));

	return 0;
}

int pipefd[2];

int
_test_set(const void *arg)
{
	const struct testset *testset = arg;

	/* child */
	close(pipefd[0]);
	_run_testset(testset, pipefd[1]);

	return 0;
}

int
run_testset(const struct testset *testset, const struct score_cb *cb)
{
	pid_t set;

	if (pipe(pipefd) == -1) {
		perror("pipe");
		exit(EXIT_FAILURE);
	}

	setsid();
    set = isolate(_test_set, testset);

	/* parent */
	close(pipefd[1]);

	monitor(pipefd[0], cb);
	close(pipefd[0]);

	int r, action = 0;
	r = wait_for_child(set, &action);

	printf("\n----------------------\n");
	if (r == 0)
		printf("Everything went OK!\n");
	else {
		printf("Something went wrong...\n");
		switch (action) {
			case 0:
				printf("Non zero return code: %d?\n", r);
				break;
			case 1:
				printf("Terminated on signal %d\n", r);
				break;
			case 2:
				printf("Stopped on signal %d\n", r);
				break;
		}
	}

	return 0;
}
