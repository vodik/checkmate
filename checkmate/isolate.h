#ifndef ISOLATE_X4T8IEI4
#define ISOLATE_X4T8IEI4

#include <unistd.h>

struct cell {
	pid_t pid;
	int pfd;
};

typedef int (*isolate_fn)(const void *arg);

struct cell *isolate(isolate_fn func, const void *arg);

#endif
