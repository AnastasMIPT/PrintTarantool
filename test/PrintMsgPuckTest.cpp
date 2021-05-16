#include <msgpuck.h>
#include <string.h>
#include "./Utils/Helpers.hpp"

extern "C" void sprint_str (const char** data, unsigned int spaces_num, char** sprint_buf);
extern "C" void sprint_uint (const char** data, unsigned int spaces_num, char** sprint_buf);
extern "C" void sprint_array   (const char** data, unsigned int spaces_num, char** sprint_buf, uint8_t outer_brackets_are);

void TestPrintStr () {
    TEST_INIT (0);
    char data[BUFSIZ] = {};
    mp_encode_str (data, "testing string", 15);

    char printed_data[BUFSIZ] = {};
    char* printed_data_p = printed_data;
    const char* data_p   = data;
    sprint_str (&data_p, 0, &printed_data_p);
    printf ("prited data = *%s*\n", printed_data);
    fail_if (strncmp (printed_data, "\"testing string\"", 15) != 0);
}

void TestPrintUint () {
    TEST_INIT (0);
    char data[BUFSIZ] = {};
    mp_encode_uint (data, 12345);

    char printed_data[BUFSIZ] = {};
    char* printed_data_p = printed_data;
    const char* data_p   = data;
    sprint_uint (&data_p, 0, &printed_data_p);
    printf ("prited data = *%s*\n", printed_data);
    

    fail_if (strncmp (printed_data, "12345", 5) != 0);
}

void TestPrintArray () {
    TEST_INIT (0);
    char data[BUFSIZ]         = {};
    char printed_data[BUFSIZ] = {};
    
    TEST_CASE ("Simple array");
    {
        char* ptr = data;
        ptr = mp_encode_array (ptr, 3);
        ptr = mp_encode_str   (ptr, "test str", 9);
        ptr = mp_encode_str   (ptr, "somth", 6);
        ptr = mp_encode_uint  (ptr, 27);

        const char* data_p = data;
        char* printed_data_p = printed_data;
        sprint_array (&data_p, 0, &printed_data_p, 1);
        printf ("prited data = *%s*\n", printed_data);

        fail_if (strncmp (printed_data, "[\"test str\", \"somth\", 27]", 26) != 0);
    }
    TEST_CASE ("With inner uint array");
    {   
        memchr (data, 0, BUFSIZ);
        char* ptr = data;
        ptr = mp_encode_array (ptr, 2);
        ptr = mp_encode_uint  (ptr, 27);
        ptr = mp_encode_array (ptr, 2);
            ptr = mp_encode_uint (ptr, 24);
            ptr = mp_encode_uint (ptr, 123);
        
        const char* data_p   = data;
        char* printed_data_p = printed_data;
        sprint_array (&data_p, 0, &printed_data_p, 1);
    
        printf ("prited data = *%s*\n", printed_data);
        fail_if (strncmp (printed_data, "[27, [24, 123]]", 16) != 0);
    }
    TEST_CASE ("With inner uint/str array"); {
        memchr (data, 0, BUFSIZ);
        char* ptr = data;
        ptr = mp_encode_array (ptr, 4);
        ptr = mp_encode_str   (ptr, "test str", 9);
        ptr = mp_encode_array (ptr, 3);
            ptr = mp_encode_uint (ptr, 265);
            ptr = mp_encode_uint (ptr, 38);
            ptr = mp_encode_str  (ptr, "dog", 4);
        ptr = mp_encode_str  (ptr, "some str", 9);
        ptr = mp_encode_uint (ptr, 98);
        
        const char* data_p   = data;
        char* printed_data_p = printed_data;
        sprint_array (&data_p, 0, &printed_data_p, 1);
    
        printf ("prited data = *%s*\n", printed_data);
        fail_if (strncmp (printed_data, "[\"test str\", [265, 38, \"dog\"], \"some str\", 98]", 47) != 0);
    }
}

int main () {

    TestPrintStr   ();
    TestPrintUint  ();
    TestPrintArray ();
    return 0;
}