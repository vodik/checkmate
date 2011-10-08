#include "mytests.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>

static int *value = NULL;

static void
mytests_init(void)
{
	/* printf("initializing testset\n"); */

	if (!value)
		value = malloc(sizeof *value);

	*value = 9;
}

static void
mytests_cleanup(void)
{
	/* printf("cleanup testset\n"); */

	if (value) {
		free(value);
		value = NULL;
	}

	value = (void *)1;
	printf("LETS TRIGGER A SEGFAULT! value=%d\n", *value);
}

static void
mytest1(void)
{
	printf("MyTest1! value=%d\n", *value);
	fail("fuck this im bailing");
}

static void
mytest2(void)
{
	value = (void *)1;
	printf("MyTest2! value=%d\n", *value);
}

static void
mytest3(void)
{
	fail_unless(*value == 9, "*value is not 9");

	*value = 6;
	fail_if(*value == 6, "*value is 6");

	printf("THIS SHOULD NOT BE PRINTED: MyTest3! value=%d\n", *value);
}

static void
mytest4(void)
{
	fail_if(*value == 6, "*value is 6");
	printf("MyTest4! value=%d\n", *value);
}

const struct testset mytests = {
	.name = "mytestset",

	.init = mytests_init,
	.free = mytests_cleanup,

	.tests = {
		{ "mytest1", mytest1 },
		{ "mytest2", mytest2 },
		{ "mytest3", mytest3 },
		{ "mytest4", mytest4 },
		{ NULL },
	},
};

