#define _POSIX_C_SOURCE 
#define _GNU_SOURCE
#ifndef DEBUG
#define NDEBUG
#endif

#include "Util.h"

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <limits.h>
