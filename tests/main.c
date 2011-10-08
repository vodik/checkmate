#include <stdlib.h>
#include <stdio.h>

#include <checkmate/easy/easy.h>
#include "mytests.h"

int
main(int argc, char *argv[])
{
	easy_run_testset(&mytests);
}
