#ifndef ISOLATE_X4T8IEI4
#define ISOLATE_X4T8IEI4

#include <unistd.h>

typedef int (*isolate_fn)(const void *arg);

pid_t isolate(isolate_fn func, const void *arg);

#endif
