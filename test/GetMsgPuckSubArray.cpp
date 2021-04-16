#include <msgpuck.h>
#include <string.h>
#include "./Utils/Helpers.hpp"

extern "C" void sprint_array   (const char** data, unsigned int spaces_num, char** sprint_buf);

extern "C" void get_subarr_from_mp_array (const char* mp_array, char* result, uint32_t* fields_indexes,
                                          uint32_t fields_num, uint32_t* byte_num);

void TestSimpleArray () {
    TEST_INIT (0);
    char data[BUFSIZ] = {};
    char result[BUFSIZ] = {};
    char printed_data[BUFSIZ] = {};
    
    char* ptr = data;
    ptr = mp_encode_array (ptr, 5);
        ptr = mp_encode_str (ptr, "test str", 9);
        ptr = mp_encode_str (ptr, "somth", 6);
        ptr = mp_encode_uint (ptr, 27);
        ptr = mp_encode_str (ptr, "some str", 9);
        ptr = mp_encode_uint (ptr, 98);


    TEST_CASE ("fields 0 2 3"); {
        uint32_t filed_no[] = {0, 2, 3};

        const char* data_p = data;

        get_subarr_from_mp_array (data_p, result, filed_no, 3, NULL);
        const char* result_p = result;
        char* printed_data_p = printed_data;
        sprint_array (&result_p, 0, &printed_data_p);
        printf ("prited data = *%s*\n", printed_data);

        fail_if (strncmp (printed_data, "[\"test str\", 27, \"some str\"]", 29) != 0);
    }
    

    TEST_CASE ("fields 0 1 4"); {
        uint32_t filed_no[] = {0, 1, 4};

        const char* data_p = data;

        get_subarr_from_mp_array (data_p, result, filed_no, 3, NULL);
        const char* result_p = result;
        char* printed_data_p = printed_data;
        sprint_array (&result_p, 0, &printed_data_p);
        printf ("prited data = *%s*\n", printed_data);

        fail_if (strncmp (printed_data, "[\"test str\", \"somth\", 98]", 26) != 0);
    }
    
}

int main () {
    TestSimpleArray ();

    return 0;
}