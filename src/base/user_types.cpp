// user_types.cpp

#include "user_types.h"

#include <stdlib.h>

#ifdef _WIN32
//const char* DIR_SEPARATOR = "\\";
const char* DIR_SEPARATOR = "/";
const char* HOME_DIRECTORY = getenv("USERPROFILE");
#else
const char* DIR_SEPARATOR = "/";
const char* HOME_DIRECTORY = getenv("HOME");
#endif

