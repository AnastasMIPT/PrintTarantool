#include <stdio.h>
#include <stdlib.h>
#include <tarantool/tarantool.h>
#include <tarantool/tnt_net.h>
#include <tarantool/tnt_opt.h>
#include <stdint.h>
#include "../include/msgpuck_print.h"
#include <msgpuck.h>

const uint32_t NumTuplesInOneRequest = 2;
const uint32_t MaxKeyFieldsInTuple   = 10;
const uint32_t IdOfIndexSpace = 288;


// TODO unit tests with complicated primary key, small array key_fields. Add errors handling connected to 
// key_fields array size.
/*! Function for getting numbers of key fileds in tuple that primary key contains
*	\param stream object 
*	\param key_fields pointer to array for storing result
*	\param space space no
*   
*   \returns number of key fields
*/
uint32_t get_key_fields (struct tnt_stream* s, uint32_t* key_fields, uint32_t space);


/*! Function for getting mp_array that is subarray of other
*	\param mp_array object 
*	\param key_fields pointer to array for storing result
*	\param space space no
*   
*   \returns number of key fields
*/
void get_subarr_from_mp_array (const char* mp_array, char* result, uint32_t* fields_indexes,
                                        uint32_t fields_num, uint32_t* byte_num);


struct tnt_stream* select_some_after_key (struct tnt_stream* s, uint32_t space_id,
                                            uint32_t limit, struct tnt_stream* key, struct tnt_reply* reply);

struct tnt_stream* select_some_first (struct tnt_stream* s, uint32_t space_id, uint32_t limit, struct tnt_reply* reply);

struct tnt_stream* get_new_key_by_reply (struct tnt_stream* s, uint32_t space_id, struct tnt_reply* reply);

void mp_next_fast (const char** data, int k);

void mp_copy_current (char** dest, const char** src);



void main(int argc, char** argv) {
    setbuf(stdout, NULL);

    // printf ("From %s, %d\n", __func__, __LINE__);
    
    if (argc < 2) {
        printf ("ERROR: need space id\n");
        exit (1);
    }
    uint32_t space_id = atoi (argv[1]);

    // printf ("From %s, %d\n", __func__, __LINE__);
    
    struct tnt_stream *tnt = tnt_net (NULL);             // Создание нового потока
    tnt_set (tnt, TNT_OPT_URI, "localhost:3301");        // Установка порта
    if (tnt_connect (tnt) < 0) {                         // обработка ошибок
        printf("Connection refused\n");
        exit (1);
    }

    // printf ("From %s, %d\n", __func__, __LINE__);
    
    struct tnt_reply reply;
    struct tnt_stream* cur_tuple = select_some_first (tnt, space_id, NumTuplesInOneRequest, &reply);
    print_element (&reply.data, 0);
    printf ("\n");

    // const char* data_key = TNT_SBUF_DATA (cur_tuple);
    // assert (mp_typeof(*data_key) == MP_ARRAY);
    // uint32_t num_el = mp_decode_array(&data_key);
    // printf ("num_el = %u\n", num_el);
   
    // printf ("From %s, %d\n", __func__, __LINE__);
    
    while(true) {
        // printf ("From %s, %d\n", __func__, __LINE__);
        struct tnt_reply reply;
        cur_tuple = select_some_after_key (tnt, space_id, NumTuplesInOneRequest, cur_tuple, &reply);
        // printf ("From %s, %d\n", __func__, __LINE__);
    
        if (cur_tuple == NULL) {
            // printf ("From %s, %d\n", __func__, __LINE__);
            break;
        }

        print_element (&reply.data, 0);
        printf("\n");
    }
    
    tnt_close (tnt);
    tnt_stream_free (tnt);
}

uint32_t get_key_fields (struct tnt_stream* s, uint32_t* key_fields, uint32_t space) {
    assert (s);
    assert (key_fields);

    // printf ("From %s, %d\n", __func__, __LINE__);
    
    struct tnt_stream *tuple = tnt_object (NULL); 
    tnt_object_format (tuple, "[%d,%d]", space, 0);                     
    // printf ("From %s, %d\n", __func__, __LINE__);
    
    tnt_select (s, IdOfIndexSpace,  0, 1, 0, TNT_ITER_ALL, tuple);
    tnt_flush (s);
    
        
    struct tnt_reply reply; tnt_reply_init (&reply);
    s->read_reply (s, &reply);
    // printf ("From %s, %d\n", __func__, __LINE__);
    
    mp_decode_array  (&reply.data);
    mp_decode_array  (&reply.data);
    mp_next_slowpath (&reply.data, 5);
    
    // printf ("From %s, %d\n", __func__, __LINE__);
    
    uint32_t fields_num = mp_decode_array (&reply.data);
    // printf ("From %s, %d\n", __func__, __LINE__);
    
    for (int i = 0; i < fields_num; ++i) {
        assert (2 == mp_decode_array (&reply.data));
        key_fields[i] = mp_decode_uint (&reply.data);
        mp_next (&reply.data);
    }

    // printf ("From %s, %d\n", __func__, __LINE__);
    tnt_stream_free (tuple);
    return fields_num;
}

struct tnt_stream* select_some_after_key (struct tnt_stream* s, uint32_t space_id,
                                            uint32_t limit, struct tnt_stream* key, struct tnt_reply* reply) {
    
    // printf ("From %s, %d\n", __func__, __LINE__);
    ssize_t result = tnt_select (s, space_id, 0, NumTuplesInOneRequest, 0, TNT_ITER_GT, key);  // TODO interator GT after first request?  
    tnt_flush (s);

    const char* key_data = TNT_SBUF_DATA(key);
    assert (mp_typeof (*key_data) == MP_ARRAY);

    tnt_stream_free(key);

    tnt_reply_init (reply);
    s->read_reply (s, reply);
    if (reply->code != 0) {
        printf ("Select failed.\n");
        exit (1);
    }
    // printf ("From %s, %d\n", __func__, __LINE__);
    
    return get_new_key_by_reply (s, space_id, reply);
}

struct tnt_stream* select_some_first (struct tnt_stream* s, uint32_t space_id, uint32_t limit,
                                                                        struct tnt_reply* reply) {
    assert (s);

    // printf ("From %s, %d\n", __func__, __LINE__);
    
    struct tnt_stream* tuple = tnt_object (NULL);        
    tnt_object_format (tuple, "[]");            
    ssize_t result = tnt_select (s, space_id, 0, limit, 0, TNT_ITER_GT, tuple);
    tnt_flush (s);
    tnt_stream_free (tuple);

    // printf ("From %s, %d\n", __func__, __LINE__);
    
    tnt_reply_init (reply);
    s->read_reply (s, reply);
    if (reply->code != 0) {
        printf ("Select failed.\n");
        exit (1);
    }

    return get_new_key_by_reply (s, space_id, reply);
}

void get_subarr_from_mp_array (const char* mp_array, char* result, uint32_t* fields_indexes,
                                        uint32_t fields_num, uint32_t* byte_num) {
    assert (mp_array);
    assert (result);
    assert (fields_indexes);
    assert (mp_typeof(*mp_array) == MP_ARRAY);

    // printf ("From %s, %d\n", __func__, __LINE__);
    char* res_ptr = result;
    const char* arr_ptr = mp_array;
    printf ("res_p %p\n", res_ptr);
    res_ptr = mp_encode_array(res_ptr, fields_num);
    printf ("field_indexes[0] = %u\n", fields_indexes[0]);
    printf ("arr_ptr_was = %p\n", arr_ptr);
    
    mp_decode_array (&arr_ptr);
    mp_next_slowpath (&arr_ptr, fields_indexes[0]);
    mp_copy_current (&res_ptr, &arr_ptr);
    
    for (int i = 1; i < fields_num; ++i) {
        mp_next_slowpath (&arr_ptr, fields_indexes[i] - fields_indexes[i - 1] - 1);
        mp_copy_current (&res_ptr, &arr_ptr);
    }

    if (byte_num != NULL) *byte_num = res_ptr - result;
    // printf ("From %s, %d\n", __func__, __LINE__);
    
    assert (mp_typeof(*result) == MP_ARRAY);
}

void mp_copy_current (char** dest, const char** src) {
    const char* ptr_was = *src;
    mp_next (src);
    int offset = *src - ptr_was;
    memcpy (*dest, ptr_was, offset);
    *dest += offset; 
}

struct tnt_stream* get_new_key_by_reply (struct tnt_stream* s, uint32_t space_id, struct tnt_reply* reply) {
    assert (reply);
    assert (s);
    assert (reply->data);

    uint32_t key_fields[MaxKeyFieldsInTuple];
    uint32_t fields_num = get_key_fields (s, key_fields, space_id);

    // printf ("From %s, %d\n", __func__, __LINE__);
    uint32_t byte_num = 0;
    char last_tuple[BUFSIZ]  = {};
    char new_key_buf[BUFSIZ] = {};
    const char* ptr = reply->data;
    // printf ("From %s, %d\n", __func__, __LINE__);
    printf ("%p\n", reply->data);
    int len = mp_decode_array (&ptr) - 1;
    if (len == 0) return NULL;
    printf ("len = %d\n", len);
    // printf ("From %s, %d\n", __func__, __LINE__);
    
    get_subarr_from_mp_array (reply->data, last_tuple, &len, 1, NULL);
    assert (mp_typeof (*last_tuple) == MP_ARRAY);
    // printf ("From %s, %d\n", __func__, __LINE__);

    get_subarr_from_mp_array (last_tuple, new_key_buf, key_fields, fields_num, &byte_num);
    // printf ("From %s, %d\n", __func__, __LINE__);
    printf ("byte_num = %d\n", byte_num);

    struct tnt_stream* new_key = tnt_object_as (NULL, new_key_buf, byte_num);
    // printf ("From %s, %d\n", __func__, __LINE__);

    return new_key;
}


void mp_next_fast (const char** data, int k) {
    const char** copy = data;
    printf ("\n");
    print_element (copy, 0);
    printf ("\n\n\n");
    while (k > 0)
    {
        printf ("k = %d\n", k);
        uint32_t type = mp_typeof (**data);
        if (type != MP_ARRAY && type != MP_MAP) {
            printf ("not array, type = %u\n", type);
            //printf ("uint = %lu\n", mp_decode_uint(data));
    
            mp_next (data);
        } else {
            printf ("array\n");
            printf ("data = %p\n", *data);
            uint32_t size = (type == MP_ARRAY? mp_decode_array(data) : mp_decode_map(data));
            
            printf ("data = %p\n", *data);
            printf ("type = %d\n", mp_typeof (**data));
            mp_next_fast (data, size);
            printf ("size = %u\n", size);
        }
        k--;
    }
}