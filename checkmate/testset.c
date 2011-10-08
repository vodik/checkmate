#include "testset.h"

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/wait.h>

#include "ipc.h"

char buf[1024];
static struct test_msg msg;

/* static volatile sig_atomic_t got_SIGCHLD = 0; */
/*  */
/* static void */
/* child_sig_handler(int sig) */
/* { */
/* 	printf("SIGCHLD!"); */
/* 	got_SIGCHLD = 1; */
/* } */
/*  */
/*  */
void
watch_pipe(void)
{
	sigset_t sigmask, empty_mask;
	/* struct sigaction sa; */
	fd_set readfds;
	int r;

	sigemptyset(&sigmask);
	sigaddset(&sigmask, SIGCHLD);
	if (sigprocmask(SIG_BLOCK, &sigmask, NULL) == -1) {
		perror("sigprocmask");
		exit(EXIT_FAILURE);
	}

	sigemptyset(&empty_mask);

	buf[0] = '\0';

	for (;;) {
		FD_ZERO(&readfds);
		FD_SET(pfds[0], &readfds);

		r = pselect(pfds[0] + 1, &readfds, NULL, NULL, NULL, &empty_mask);

		if (r == -1 && errno != EINTR) {
			perror("pselect");
			exit(EXIT_FAILURE);
		}

		if (FD_ISSET(pfds[0], &readfds)) {
			size_t len;
			len = read(pfds[0], buf, 1024);

			if (len) {
				buf[len] = '\0';
			} else {
				return;
			}
		}

		/* if (got_SIGCHLD) { */
		/* 	got_SIGCHLD = 0; */
		/* 	return; */
		/* } */
	}
}

static int
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

void
run_test(const struct test *test)
{
	/* int i; */

	/* drop privileges */
	/* if (setgid(test->gid) != 0) { */
	/* 	perror("setgid"); */
	/* 	exit(EXIT_FAILURE); */
	/* } */
	/* if (setuid(test->uid) != 0) { */
	/* 	perror("setuid"); */
	/* 	exit(EXIT_FAILURE); */
	/* } */

	/* close(STDIN_FILENO); */
	/* close(STDOUT_FILENO); */
	/* close(STDERR_FILENO); */

	test->test();
	exit(EXIT_SUCCESS);
}

void
send_msg(int ipc)
{
	/* printf("SENDING IPC MESSAGE\n"); */
	int r;

	r = write(ipc, &msg, sizeof(struct test_msg));
	if (r == -1) {
		perror("write");
		exit(EXIT_FAILURE);
	}
}

void
_run_test(const struct test *test, int ipc)
{
	pid_t child;
	int r, action = 0;

	if (pipe(pfds) == -1) {
		perror("pipe");
		exit(EXIT_FAILURE);
	}

	msg.name = test->name;
	msg.event = TEST_START;
	send_msg(ipc);

	switch (child = fork()) {
		case -1:
			perror("fork");
			exit(EXIT_FAILURE);
			break;
		case 0:
			/* child */
			close(pfds[0]);
			run_test(test);

			/* should not get here */
			exit(EXIT_FAILURE);
			break;
	}

	/* parent */
	close(pfds[1]);
	watch_pipe();
	r = wait_for_child(child, &action);

	if (r == 0) {
		msg.event = TEST_PASS;
		msg.signal = 0;
		send_msg(ipc);
	} else {
		switch (action) {
			case 0:
				msg.event = TEST_FAIL;
				strcpy(msg.msg, buf);
				break;
			case 1:
				msg.event = TEST_TERM;
				msg.signal = r;
				break;
			case 2:
				msg.event = TEST_STOPPED;
				msg.signal = r;
				break;
		}
		send_msg(ipc);
	}

	close(pfds[0]);
	++test;
}

int
_run_testset(const struct testset *testset, int ipc)
{
	/* int i, ret; */
	/* int action; */

	if (testset->init)
		testset->init();

	msg.event = TEST_INIT_OK;
	send_msg(ipc);

	const struct test *test = testset->tests;
	while (test && test->test)
		_run_test(test++, ipc);

	msg.event = TEST_TESTS_OK;
	send_msg(ipc);

	if (testset->free)
		testset->free();

	msg.event = TEST_FREE_OK;
	send_msg(ipc);

	return 0;
}
