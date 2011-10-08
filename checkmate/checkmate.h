#ifndef SCORE_PMMZGP95
#define SCORE_PMMZGP95

#include "tester.h"
#include "ipc.h"

struct score_cb {
	void (*on_testset_start)(int step);
	void (*on_testset_end)(int step, int result, int signal, const char *msg);

	void (*on_test_start)(const char *testname);
	void (*on_test_end)(const char *testname, int result, int signal, const char *msg);
};

int run_testset(const struct testset *testset, const struct score_cb *cb);

#endif
