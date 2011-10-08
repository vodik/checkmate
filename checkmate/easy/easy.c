#include "easy.h"

#include <stdlib.h>
#include <stdio.h>

#include "../checkmate.h"

void
testset_start(int step)
{
	switch (step) {
	}
}

void
testset_end(int step, int result, int signal, const char *msg)
{
	switch (step) {
		case TEST_INIT_OK:
			printf(">>  init done!\n");
			break;
		case TEST_TESTS_OK:
			printf(">>  tests done!\n");
			break;
		case TEST_FREE_OK:
			printf(">>  free done!\n");
			break;
	}
}

void
test_start(const char *testname)
{
	printf(":: Starting test %s\n", testname);
}

void
test_end(const char *testname, int result, int signal, const char *msg)
{
	switch (result) {
		case TEST_PASS:
			printf(":: Ending test %s\n", testname);
			printf("  PASSED\n");
			break;
		case TEST_FAIL:
			printf(":: Stopping test %s\n", testname);
			printf("  FAILED: %s\n", msg);
			break;
		case TEST_TERM:
		case TEST_STOPPED:
			printf(":: Stopping test %s\n", testname);
			printf("  ENDED on signal %d\n", signal);
			break;
		default:
			printf(":: unknown result type\n");
			break;
	}
}

static const struct score_cb easy_cb = {
	.on_testset_start	= testset_start,
	.on_testset_end		= testset_end,
	.on_test_start		= test_start,
	.on_test_end		= test_end,
};

void
easy_run_testset(const struct testset *set)
{
	run_testset(set, &easy_cb);
}
