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
const uint32_t NumTuplesInOneRequest = 100;
const uint32_t MaxKeyFieldsInTuple   = 10;
const uint32_t IdOfIndexSpace = 288;

/*! Function for printing arbitrary MsgPack object
*	\param reply pointer to buffer with reply form commands like select 
*	\param spaces_num number of spaces to indent
*/
void print_element (struct tnt_reply* reply, unsigned int spaces_num);


/*! Function for printing arbitrary MsgPack array
*	\param reply pointer to buffer with reply form commands like select 
*	\param spaces_num number of spaces to indent
*/
void print_array   (struct tnt_reply* reply, unsigned int spaces_num);

/*! Function for printing arbitrary MsgPack unsigned int
*	\param reply pointer to buffer with reply form commands like select 
*	\param spaces_num number of spaces to indent
*/
void print_uint    (struct tnt_reply* reply, unsigned int spaces_num);

/*! Function for printing arbitrary MsgPack string
*	\param reply pointer to buffer with reply form commands like select 
*	\param spaces_num number of spaces to indent
*/
void print_str     (struct tnt_reply* reply, unsigned int spaces_num);
void print_map     (struct tnt_reply* reply, unsigned int spaces_num);
void print_reply   (struct tnt_reply* reply, unsigned int spaces_num);
void print_bool    (struct tnt_reply* reply, unsigned int spaces_num);
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
                                            uint32_t limit, struct tnt_stream* key);

struct tnt_stream* select_some_first (struct tnt_stream* s, uint32_t space_id, uint32_t limit);


void main(int argc, char** argv) {
    setbuf(stdout, NULL);

    printf ("From %s, %d\n", __func__, __LINE__);
    
    if (argc < 2) {
        printf ("ERROR: need space id\n");
        exit (1);
    }
    uint32_t space_id = atoi (argv[1]);

    printf ("From %s, %d\n", __func__, __LINE__);
    
    struct tnt_stream *tnt = tnt_net (NULL);             // Создание нового потока
    tnt_set (tnt, TNT_OPT_URI, "localhost:3301");        // Установка порта
    if (tnt_connect (tnt) < 0) {                         // обработка ошибок
        printf("Connection refused\n");
        exit (1);
    }

    printf ("From %s, %d\n", __func__, __LINE__);
    
    struct tnt_stream* cur_tuple = select_some_first (tnt, space_id, NumTuplesInOneRequest);
    printf ("From %s, %d\n", __func__, __LINE__);
    
    while(true) {
        cur_tuple = select_some_after_key (tnt, space_id, NumTuplesInOneRequest, cur_tuple);
        
        if (cur_tuple == NULL) {
            break;
        }

        struct tnt_reply reply; tnt_reply_init (&reply);
        tnt->read_reply (tnt, &reply);
        if (reply.code != 0) {
            printf ("Select failed.\n");
            exit (1);
        }

        print_element (&reply, 0);
        printf("\n");
    }
    
    tnt_close (tnt);
    tnt_stream_free (tnt);
}


void print_element (struct tnt_reply* reply, unsigned int spaces_num) {
    char element_type = mp_typeof (*reply->data);
    switch (element_type)
    {
    case MP_ARRAY:
        print_array (reply, spaces_num);       
        break;
    case MP_UINT:
        print_uint  (reply, spaces_num);       
        break;
    case MP_STR:
        print_str   (reply, spaces_num);
        break;
    case MP_BOOL:
        print_bool  (reply, spaces_num);
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
    
    unsigned int i = 0;
    printf ("[");
    if (elem_count > NumElemInTupleOnOneLine) printf ("\n");
    spaces_num += 2;
    for (i = 0; i < elem_count; ++i) {
        if (elem_count > NumElemInTupleOnOneLine) print_spaces(spaces_num);
        print_element(reply, spaces_num);
        if (i != elem_count - 1) printf(", ");
        if (elem_count > NumElemInTupleOnOneLine) {
            printf ("\n");
        } else if ( mp_typeof (*reply->data) == MP_MAP) {
            printf ("\n");
            if (i != elem_count - 1) print_spaces(spaces_num - 1);
        }
    }
    spaces_num -= 2;
    if (elem_count > NumElemInTupleOnOneLine) print_spaces(spaces_num);
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
    printf("\"%.*s\"", str_value_length, str_value);
}

void print_map     (struct tnt_reply* reply, unsigned int spaces_num) {
    uint32_t map_size = 0;
    map_size = mp_decode_map (&reply->data);
    unsigned i = 0;
    printf("{");
    if (map_size > NumElemInMapOnOneLine) printf("\n");
    spaces_num += 2;
    for (i = 0; i < map_size; ++i) {
        if (map_size > NumElemInMapOnOneLine) print_spaces (spaces_num);
        print_element(reply, spaces_num);
        printf(": ");
        print_element(reply, spaces_num);
        if (i != map_size - 1) printf (", ");
        if (map_size > NumElemInMapOnOneLine) printf ("\n");
        
    }
    spaces_num -= 2; 
    if (map_size > NumElemInMapOnOneLine) print_spaces (spaces_num);
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

uint32_t get_key_fields (struct tnt_stream* s, uint32_t* key_fields, uint32_t space) {
    assert (s);
    assert (key_fields);

    printf ("From %s, %d\n", __func__, __LINE__);
    
    struct tnt_stream *tuple = tnt_object (NULL); 
    tnt_object_format (tuple, "[%d,%d]", space, 0);                     
    printf ("From %s, %d\n", __func__, __LINE__);
    
    tnt_select (s, IdOfIndexSpace,  0, 1, 0, TNT_ITER_ALL, tuple);
    tnt_flush (s);
    
        
    struct tnt_reply reply; tnt_reply_init (&reply);
    s->read_reply (s, &reply);
    printf ("From %s, %d\n", __func__, __LINE__);
    
    mp_decode_array  (&reply.data);
    mp_decode_array  (&reply.data);
    mp_next_slowpath (&reply.data, 5);
    
    printf ("From %s, %d\n", __func__, __LINE__);
    
    uint32_t fields_num = mp_decode_array (&reply.data);
    printf ("From %s, %d\n", __func__, __LINE__);
    
    for (int i = 0; i < fields_num; ++i) {
        assert (2 == mp_decode_array (&reply.data));
        key_fields[i] = mp_decode_uint (&reply.data);
        mp_next (&reply.data);
    }

    printf ("From %s, %d\n", __func__, __LINE__);
    tnt_stream_free (tuple);
    return fields_num;
}

struct tnt_stream* select_some_after_key (struct tnt_stream* s, uint32_t space_id,
                                            uint32_t limit, struct tnt_stream* key) {
    
    ssize_t result = tnt_select (s, space_id, 0, NumTuplesInOneRequest, 0, TNT_ITER_GT, key);  // TODO interator GT after first request?  
    tnt_flush (s);
    tnt_stream_free(key);

}

struct tnt_stream* select_some_first (struct tnt_stream* s, uint32_t space_id, uint32_t limit) {
    assert (s);

    printf ("From %s, %d\n", __func__, __LINE__);
    
    struct tnt_stream* tuple = tnt_object (NULL);        
    tnt_object_format (tuple, "[]");            
    ssize_t result = tnt_select (s, space_id, 0, limit, 0, TNT_ITER_GT, tuple);
    tnt_flush (s);
    tnt_stream_free (tuple);

    printf ("From %s, %d\n", __func__, __LINE__);
    
    struct tnt_reply reply; tnt_reply_init (&reply);
    s->read_reply (s, &reply);
    if (reply.code != 0) {
        printf ("Select failed.\n");
        exit (1);
    }


    uint32_t key_fields[MaxKeyFieldsInTuple];
    uint32_t fields_num = get_key_fields (s, key_fields, space_id);

    printf ("From %s, %d\n", __func__, __LINE__);
    uint32_t byte_num = 0;
    char last_tuple[BUFSIZ]  = {};
    char new_key_buf[BUFSIZ] = {};
    const char* ptr = reply.data;
    printf ("From %s, %d\n", __func__, __LINE__);
    int len = mp_decode_array (&ptr);
    printf ("From %s, %d\n", __func__, __LINE__);
    get_subarr_from_mp_array (reply.data, last_tuple, &len, 1, NULL);
    get_subarr_from_mp_array (last_tuple, new_key_buf, key_fields, fields_num, &byte_num);
    struct tnt_stream* new_key = tnt_object_as (new_key, new_key_buf, byte_num);
    return new_key;
}

void get_subarr_from_mp_array (const char* mp_array, char* result, uint32_t* fields_indexes,
                                        uint32_t fields_num, uint32_t* byte_num) {
    assert (mp_array);
    assert (result);
    assert (fields_indexes);

    printf ("From %s, %d\n", __func__, __LINE__);
    char* res_ptr = result;
    const char* arr_ptr = mp_array;
    mp_encode_array(res_ptr, fields_num);
    mp_next(&arr_ptr);
    
    printf ("From %s, %d\n", __func__, __LINE__);
    
    mp_next_slowpath(&arr_ptr, fields_indexes[0]);
    
    const char* ptr_was = arr_ptr;
    mp_next (&arr_ptr);
    int offset = ptr_was - arr_ptr; //TODO function
    memcpy (res_ptr, ptr_was, offset);
    res_ptr += offset; 

    printf ("From %s, %d\n", __func__, __LINE__);
    
    for (int i = 1; i < fields_num; ++i) {
        mp_next_slowpath (&arr_ptr, fields_indexes[i] - fields_indexes[i - 1] - 1);
        ptr_was = arr_ptr;
        mp_next (&arr_ptr);
        offset = ptr_was - arr_ptr;
        memcpy (res_ptr, ptr_was, offset);
        res_ptr += offset;
    }
    printf ("From %s, %d\n", __func__, __LINE__);
    
}