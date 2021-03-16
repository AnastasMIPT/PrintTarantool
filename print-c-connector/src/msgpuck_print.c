
#include "mspck_print.h"

void print_element (struct tnt_reply* reply, unsigned int spaces_num) {
    char element_type = mp_typeof(*reply->data);
    switch (element_type)
    {
    case MP_ARRAY:
        print_array(reply, spaces_num);       
        break;
    case MP_UINT:
        print_uint(reply, spaces_num);       
        break;
    case MP_STR:
        print_str(reply, spaces_num);
        break;
    case MP_BOOL:
        print_bool(reply, spaces_num);
        break;
    case MP_DOUBLE:
        printf("double\n");
        break;
    case MP_NIL:
        printf("nill\n");
        break;
    case MP_EXT:
        printf("ext\n");
        break;
    case MP_MAP:
        print_map(reply, spaces_num);
        break;
    default:
        printf("PARSE ERROR: Unknown type = %d\n", element_type);
        exit(1);
        break;
    }
}

void print_array   (struct tnt_reply* reply, unsigned int spaces_num) {
    uint32_t elem_count = mp_decode_array(&reply->data);
    //printf("tuple count=%u\n", elem_count);
    unsigned int i = 0;
    printf ("[\n");
    spaces_num += 2;
    for (i = 0; i < elem_count; ++i) {
        print_spaces(spaces_num);
        print_element(reply, spaces_num);
        printf(",\n");
    }
    spaces_num -= 2;
    print_spaces(spaces_num);
    printf ("]");
}

void print_uint    (struct tnt_reply* reply, unsigned int spaces_num) {
    uint64_t num_value = mp_decode_uint(&reply->data);
    printf("%lu", num_value);
}

void print_str     (struct tnt_reply* reply, unsigned int spaces_num) {
    const char * str_value = NULL;
    uint32_t str_value_length = 0;
    str_value = mp_decode_str(&reply->data, &str_value_length);
    printf("%.*s", str_value_length, str_value);
}

void print_map     (struct tnt_reply* reply, unsigned int spaces_num) {
    uint32_t map_size = 0;
    map_size = mp_decode_map (&reply->data);
    unsigned i = 0;
    print_spaces (spaces_num);
    printf("{\n");
    spaces_num += 2;
    for (i = 0; i < map_size; ++i) {
        //printf ("{");
        print_spaces (spaces_num);
        print_element(reply, spaces_num);
        printf(": ");
        print_element(reply, spaces_num);
        printf (",\n");
        
    }
    spaces_num -= 2; 
    print_spaces (spaces_num);
    printf("}");
}

void print_bool (struct tnt_reply* reply, unsigned int spaces_num) {
    if (mp_decode_bool(&reply->data)) {
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