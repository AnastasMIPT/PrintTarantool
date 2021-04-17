#include <stdint.h>
#include <stdlib.h>
#include <tarantool/tarantool.h>
#include <tarantool/tnt_net.h>
#include <tarantool/tnt_opt.h>
#include <msgpuck.h>


#include "../include/additional_utilities.h"


const uint32_t IdOfIndexSpace = 288;


uint32_t get_key_fields (struct tnt_stream* s, uint32_t* key_fields, uint32_t space) {
    assert (s);
    assert (key_fields);

    struct tnt_stream *tuple = tnt_object (NULL); 
    tnt_object_format (tuple, "[%d,%d]", space, 0);                     
    
    tnt_select (s, IdOfIndexSpace,  0, 1, 0, TNT_ITER_ALL, tuple);
    tnt_flush (s);
    
        
    struct tnt_reply reply; tnt_reply_init (&reply);
    s->read_reply (s, &reply);
    
    mp_decode_array  (&reply.data);
    mp_decode_array  (&reply.data);
    mp_next_slowpath (&reply.data, 5);
    
    uint32_t fields_num = mp_decode_array (&reply.data);
    
    for (int i = 0; i < fields_num; ++i) {
        assert (2 == mp_decode_array (&reply.data));
        key_fields[i] = mp_decode_uint (&reply.data);
        mp_next (&reply.data);
    }

    tnt_stream_free (tuple);
    return fields_num;
}



void get_subarr_from_mp_array (const char* mp_array, char* result, uint32_t* fields_indexes,
                                        uint32_t fields_num, uint32_t* byte_num) {
    assert (mp_array);
    assert (result);
    assert (fields_indexes);
    assert (mp_typeof(*mp_array) == MP_ARRAY);

    char* res_ptr = result;
    const char* arr_ptr = mp_array;
    res_ptr = mp_encode_array(res_ptr, fields_num);
    
    mp_decode_array (&arr_ptr);
    mp_next_slowpath (&arr_ptr, fields_indexes[0]);
    mp_copy_current (&res_ptr, &arr_ptr);
    
    for (int i = 1; i < fields_num; ++i) {
        mp_next_slowpath (&arr_ptr, fields_indexes[i] - fields_indexes[i - 1] - 1);
        mp_copy_current (&res_ptr, &arr_ptr);
    }

    if (byte_num != NULL) *byte_num = res_ptr - result;
    
    assert (mp_typeof(*result) == MP_ARRAY);
}

void mp_copy_current (char** dest, const char** src) {
    const char* ptr_was = *src;
    mp_next (src);
    int offset = *src - ptr_was;
    memcpy (*dest, ptr_was, offset);
    *dest += offset; 
}