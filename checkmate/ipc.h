#ifndef IPC_2XS1GLUC
#define IPC_2XS1GLUC

enum {
	TEST_START,
	TEST_PASS,
	TEST_FAIL,
	TEST_TERM,
	TEST_STOPPED,

	TEST_INIT_OK,
	TEST_TESTS_OK,
	TEST_FREE_OK,

};

struct test_msg {
	int event;
	int signal;
	const char *name;
	char msg[512];
};

#endif
