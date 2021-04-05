#include <stdio.h>
#include <stdlib.h>
#include <tarantool/tarantool.h>
#include <tarantool/tnt_net.h>
#include <tarantool/tnt_opt.h>
#include <stdint.h>

#define MP_SOURCE 1
#include <msgpuck.h>


const uint32_t NumElemInTupleOnOneLine = 5;
const uint32_t NumElemInMapOnOneLine = 2;
const uint32_t NumTuplesInOneRequest = 10;
const uint32_t MaxKeyFieldsInTuple   = 10;
const uint32_t IdOfIndexSpace = 288;

/*! Function for printing arbitrary MsgPack object
*	\param reply pointer to buffer with reply form commands like select 
*	\param spaces_num number of spaces to indent
*/
void print_element (const char** data, unsigned int spaces_num);


/*! Function for printing arbitrary MsgPack array
*	\param reply pointer to buffer with reply form commands like select 
*	\param spaces_num number of spaces to indent
*/
void print_array   (const char** data, unsigned int spaces_num);

/*! Function for printing arbitrary MsgPack unsigned int
*	\param reply pointer to buffer with reply form commands like select 
*	\param spaces_num number of spaces to indent
*/
void print_uint    (const char** reply, unsigned int spaces_num);

/*! Function for printing arbitrary MsgPack string
*	\param reply pointer to buffer with reply form commands like select 
*	\param spaces_num number of spaces to indent
*/
void print_str     (const char** data, unsigned int spaces_num);
void print_map     (const char** data, unsigned int spaces_num);
void print_reply   (const char** data, unsigned int spaces_num);
void print_bool    (const char** data, unsigned int spaces_num);
void print_spaces  (unsigned int num);


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

    const char* data_key = TNT_SBUF_DATA (cur_tuple);
    assert (mp_typeof(*data_key) == MP_ARRAY);
    uint32_t num_el = mp_decode_array(&data_key);
    printf ("num_el = %u\n", num_el);
    assert (mp_typeof(*data_key) == MP_UINT);
    printf ("key = %lu\n", mp_decode_uint (&data_key));

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
    
    mp_next(&arr_ptr);
    mp_next_fast (&arr_ptr, fields_indexes[0]);
    
    printf ("arr_ptr_is = %p\n", arr_ptr);
    // printf ("From %s, %d\n", __func__, __LINE__);
    
    const char* ptr_was = arr_ptr;
    printf ("type = %d\n", mp_typeof(*arr_ptr));
    printf ("uint = %lu\n", mp_decode_uint(&arr_ptr));
    //assert (mp_typeof(*arr_ptr) == MP_ARRAY);
    mp_next (&arr_ptr);
    printf ("arr_ptr_is = %p\n", arr_ptr);
    
    // printf ("From %s, %d\n", __func__, __LINE__);
    
    int offset = arr_ptr - ptr_was; //TODO function
    // printf ("From %s, %d\n", __func__, __LINE__);
    
    printf ("res_ptr = %p, %p, %d\n", res_ptr, ptr_was, offset);

    assert (mp_typeof(*result) == MP_ARRAY);
    //printf ("decoded = %lu\n", mp_decode_uint(&ptr_was));
    memcpy (res_ptr, ptr_was, offset);
    assert (mp_typeof(*result) == MP_ARRAY);
    //assert (mp_typeof(*res_ptr) == MP_ARRAY);
    //printf ("decoded = %lu\n", mp_decode_uint(&res_ptr));    

    res_ptr += offset; 

    // printf ("From %s, %d\n", __func__, __LINE__);
    

    assert (mp_typeof(*result) == MP_ARRAY);
    for (int i = 1; i < fields_num; ++i) {
        mp_next_fast (&arr_ptr, fields_indexes[i] - fields_indexes[i - 1] - 1);
        ptr_was = arr_ptr;
        mp_next (&arr_ptr);
        offset = ptr_was - arr_ptr;
        memcpy (res_ptr, ptr_was, offset);
        res_ptr += offset;
    }

    if (byte_num != NULL) *byte_num = res_ptr - result;
    // printf ("From %s, %d\n", __func__, __LINE__);
    
    assert (mp_typeof(*result) == MP_ARRAY);
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