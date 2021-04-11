
#include "../include/msgpuck_print.h"
#include <assert.h>




// #define MP_SOURCE 1
#include <msgpuck.h>


const uint32_t NumElemInTupleOnOneLine = 5;
const uint32_t NumElemInMapOnOneLine = 2;




void print_element (const char** data, unsigned int spaces_num) {
    char element_type = mp_typeof (**data);
    switch (element_type)
    {
    case MP_ARRAY:
        print_array (data, spaces_num);       
        break;
    case MP_UINT:
        print_uint  (data, spaces_num);       
        break;
    case MP_STR:
        print_str   (data, spaces_num);
        break;
    case MP_BOOL:
        print_bool  (data, spaces_num);
        break;
    case MP_DOUBLE:
        printf("double\n");
        break;
    case MP_NIL:
        printf("nill");
        break;
    case MP_EXT:
        printf("ext\n");
        break;
    case MP_MAP:
        print_map(data, spaces_num);
        break;
    default:
        printf("PARSE ERROR: Unknown type = %d\n", element_type);
        exit(1);
        break;
    }
}

void print_array   (const char** data, unsigned int spaces_num) {
    uint32_t elem_count = mp_decode_array(data);
    
    unsigned int i = 0;
    printf ("[");
    if (elem_count > NumElemInTupleOnOneLine) printf ("\n");
    spaces_num += 2;
    for (i = 0; i < elem_count; ++i) {
        if (elem_count > NumElemInTupleOnOneLine) print_spaces(spaces_num);
        print_element(data, spaces_num);
        if (i != elem_count - 1) printf(", ");
        if (elem_count > NumElemInTupleOnOneLine) {
            printf ("\n");
        } else if ( mp_typeof (**data) == MP_MAP) {
            printf ("\n");
            if (i != elem_count - 1) print_spaces(spaces_num - 1);
        }
    }
    spaces_num -= 2;
    if (elem_count > NumElemInTupleOnOneLine) print_spaces(spaces_num);
    printf ("]");
}

void print_uint    (const char** data, unsigned int spaces_num) {
    uint64_t num_value = mp_decode_uint(data);
    printf("%lu", num_value);
}

void print_str     (const char** data, unsigned int spaces_num) {
    const char * str_value = NULL;
    uint32_t str_value_length = 0;
    str_value = mp_decode_str(data, &str_value_length);
    printf("\"%.*s\"", str_value_length, str_value);
}

void print_map     (const char** data, unsigned int spaces_num) {
    uint32_t map_size = 0;
    map_size = mp_decode_map (data);
    unsigned i = 0;
    printf("{");
    if (map_size > NumElemInMapOnOneLine) printf("\n");
    spaces_num += 2;
    for (i = 0; i < map_size; ++i) {
        if (map_size > NumElemInMapOnOneLine) print_spaces (spaces_num);
        print_element(data, spaces_num);
        printf(": ");
        print_element(data, spaces_num);
        if (i != map_size - 1) printf (", ");
        if (map_size > NumElemInMapOnOneLine) printf ("\n");
        
    }
    spaces_num -= 2; 
    if (map_size > NumElemInMapOnOneLine) print_spaces (spaces_num);
    printf("}");
}

void print_bool (const char** data, unsigned int spaces_num) {
    if (mp_decode_bool(data)) {
        printf("true");
    } else {
        printf("false");
    }
}

void print_spaces (unsigned int num) {
    for (unsigned int i = 0; i < num; ++i) {
        printf(" ");
    }
}