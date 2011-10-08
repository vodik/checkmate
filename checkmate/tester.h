#ifndef TESTER_1Z3G609F
#define TESTER_1Z3G609F

#include <stdbool.h>

int pfds[2];

void fail(const char msg[]);

void fail_if(bool cond, const char msg[]);
void fail_unless(bool cond, const char msg[]);

struct test {
	const char *name;
	void (*test)(void);
};

struct testset {
	const char *name;
	int uid, gid;
	void (*init)(void);
	void (*free)(void);
	const struct test tests[];
};

#endif
