
#include "../include/msgpuck_sprint.h"
#include <assert.h>
#include <msgpuck.h>


const uint32_t NumElemInTupleOnOneLine = 5;
const uint32_t NumElemInMapOnOneLine = 2;


void sprint_element (const char** data, unsigned int spaces_num, char** sprint_buf) {
    char element_type = mp_typeof (**data);
    switch (element_type)
    {
    case MP_ARRAY:
        sprint_array (data, spaces_num, sprint_buf);       
        break;
    case MP_UINT:
        sprint_uint  (data, spaces_num, sprint_buf);       
        break;
    case MP_STR:
        sprint_str   (data, spaces_num, sprint_buf);
        break;
    case MP_BOOL:
        sprint_bool  (data, spaces_num, sprint_buf);
        break;
    case MP_DOUBLE:
        sprintf (*sprint_buf, "double\n");
        break;
    case MP_NIL:
        sprintf (*sprint_buf, "nill");
        break;
    case MP_EXT:
        sprintf (*sprint_buf, "ext\n");
        break;
    case MP_MAP:
        sprint_map(data, spaces_num, sprint_buf);
        break;
    default:
        sprintf (*sprint_buf, "PARSE ERROR: Unknown type = %d\n", element_type);
        exit(1);
        break;
    }
}

void sprint_array   (const char** data, unsigned int spaces_num, char** sprint_buf) {
    uint32_t elem_count = mp_decode_array(data);
    
    unsigned int i = 0;
    sprintf (*sprint_buf, "[");
    *sprint_buf += 1;
    
    if (elem_count > NumElemInTupleOnOneLine)  {
        sprintf (*sprint_buf, "\n");
        *sprint_buf += 1;
    }
    spaces_num += 2;
    for (i = 0; i < elem_count; ++i) {
        if (elem_count > NumElemInTupleOnOneLine) sprint_spaces (spaces_num, sprint_buf);
        sprint_element (data, spaces_num, sprint_buf);
        if (i != elem_count - 1)  {
            sprintf (*sprint_buf, ", ");
            *sprint_buf += 2;
        }
        if (elem_count > NumElemInTupleOnOneLine) {
            sprintf (*sprint_buf, "\n");
            *sprint_buf += 1;
        } else if (mp_typeof (**data) == MP_MAP) {
            sprintf (*sprint_buf, "\n");
            *sprint_buf += 1;
            if (i != elem_count - 1) sprint_spaces (spaces_num - 1, sprint_buf);
        }
    }
    spaces_num -= 2;
    if (elem_count > NumElemInTupleOnOneLine) sprint_spaces (spaces_num, sprint_buf);
    sprintf (*sprint_buf, "]");
    *sprint_buf += 1;
}

void sprint_uint    (const char** data, unsigned int spaces_num, char** sprint_buf) {
    uint64_t num_value = mp_decode_uint (data);
    uint32_t printed_num = 0;
    sprintf (*sprint_buf, "%lu%n", num_value, &printed_num);
    *sprint_buf += printed_num;
}

void sprint_str     (const char** data, unsigned int spaces_num, char** sprint_buf) {
    const char * str_value = NULL;
    uint32_t str_value_length = 0;
    str_value = mp_decode_str (data, &str_value_length);
    sprintf (*sprint_buf, "\"%.*s\"", str_value_length, str_value);
    *sprint_buf += str_value_length + 1;
}

void sprint_map     (const char** data, unsigned int spaces_num, char** sprint_buf) {
    uint32_t map_size = 0;
    map_size = mp_decode_map (data);
    unsigned i = 0;

    sprintf (*sprint_buf, "{");
    *sprint_buf += 1;
    
    if (map_size > NumElemInMapOnOneLine) {
        sprintf (*sprint_buf, "\n");
        *sprint_buf += 1;
    }

    spaces_num += 2;
    for (i = 0; i < map_size; ++i) {
        if (map_size > NumElemInMapOnOneLine) sprint_spaces (spaces_num, sprint_buf);
        sprint_element(data, spaces_num, sprint_buf);
        sprintf (*sprint_buf, ": ");
        *sprint_buf += 2; 
        sprint_element(data, spaces_num, sprint_buf);
        if (i != map_size - 1) {
            sprintf (*sprint_buf, ", ");
            *sprint_buf += 2;
        }
        if (map_size > NumElemInMapOnOneLine) {
            sprintf (*sprint_buf, "\n");
            *sprint_buf += 1;
        }
    }
    spaces_num -= 2; 
    if (map_size > NumElemInMapOnOneLine) sprint_spaces (spaces_num, sprint_buf);
    sprintf (*sprint_buf, "}");
    *sprint_buf += 1;
}

void sprint_bool (const char** data, unsigned int spaces_num, char** sprint_buf) {
    if (mp_decode_bool(data)) {
        sprintf (*sprint_buf, "true");
        *sprint_buf += 4;
    } else {
        sprintf (*sprint_buf, "false");
        *sprint_buf += 5;
    }
}

void sprint_spaces (unsigned int num, char** sprint_buf) {
    for (unsigned int i = 0; i < num; ++i) {
        sprintf (*sprint_buf, " ");
        *sprint_buf += 1;
    }
}