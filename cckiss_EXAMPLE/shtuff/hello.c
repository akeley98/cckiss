#include "config.h"
#include "hello.h"

const char* get_hello()
{
    return config_shout ? "HELLO, CCKISS!!!" : "Hello, cckiss.";
}

