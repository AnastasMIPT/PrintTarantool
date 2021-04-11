#include <msgpuck.h>
#include <string.h>
#include "../include/msgpuck_sprint.h"
#include "./Utils/Helpers.hpp"

void TestPrintStr () {
    TEST_INIT (0);
    char data[BUFSIZ] = {};
    mp_encode_str (data, "testing string", 15);

    char printed_data[BUFSIZ] = {};
    const char* data_p = data;
    //sprint_str (&data_p, 0, printed_data);
    int g = 9;
    some_func (&g);

    fail_if (g != 10);
    //fail_if (strncmp (printed_data, "testing string", 15) != 0);
}

int main () {

    TestPrintStr ();
    return 0;
}