#include <msgpuck.h>
#include <tarantool/tarantool.h>
#include <tarantool/tnt_net.h>
#include <tarantool/tnt_opt.h>
#include "../include/improved_select.h"
#include "../include/additional_utilities.h"
#include "../include/msgpuck_print.h"

const uint32_t MaxKeyFieldsInTuple   = 10;



/*! Function for efficient select slice from space. Slice bounded by key of first tuple and limit
*	\param stream     stream object 
*	\param space_id   space no
*	\param limit      limit of tuples to select
*	\param key        key object. Tuples will be selected with this key with GT iterator.
*	\param reply      pointer to reply object for storing result
*   
*   \returns primary key from last tuple in reply
*/
struct tnt_stream* select_some_after_key (struct tnt_stream* s, uint32_t space_id,
                                            uint32_t limit, struct tnt_stream* key, struct tnt_reply* reply) {
    
    ssize_t result = tnt_select (s, space_id, 0, limit, 0, TNT_ITER_GT, key);  
    tnt_flush (s);

    const char* key_data = TNT_SBUF_DATA(key);
    assert (mp_typeof (*key_data) == MP_ARRAY);

    tnt_stream_free(key);

    tnt_reply_init (reply);
    s->read_reply (s, reply);
    if (reply->code != 0) {
        printf ("Select failed.\n");
        printf ("ERROR: %s\n", reply->error);
        exit(1);
    }

    return get_new_key_by_reply (s, space_id, reply);
}



/*! Function for efficient select first limit elements.
*	\param stream     stream object 
*	\param space_id   space no
*	\param limit      limit of tuples to select
*	\param reply      pointer to reply object for storing result
*   
*   \returns primary key from last tuple in reply
*/
struct tnt_stream* select_some_first (struct tnt_stream* s, uint32_t space_id, uint32_t limit,
                                                                        struct tnt_reply* reply) {
    assert (s);
   
    struct tnt_stream* tuple = tnt_object (NULL);        
    tnt_object_format (tuple, "[]");            
    ssize_t result = tnt_select (s, space_id, 0, limit, 0, TNT_ITER_GT, tuple);
    tnt_flush (s);
    tnt_stream_free (tuple);

    tnt_reply_init (reply);
    s->read_reply (s, reply);
    if (reply->code != 0) {
        printf ("Select failed.\n");
        printf ("ERROR: %s\n", reply->error);
        exit (1);
    }

    return get_new_key_by_reply (s, space_id, reply);
}


/*! Function getting last key from reply
*	\param stream     stream object 
*	\param space_id   space no
*	\param limit      limit of tuples to select
*	\param reply      pointer to reply object for storing result
*   
*   \returns primary key from last tuple in reply
*/
struct tnt_stream* get_new_key_by_reply (struct tnt_stream* s, uint32_t space_id, struct tnt_reply* reply) {
    assert (reply);
    assert (s);
    assert (reply->data);

    uint32_t key_fields[MaxKeyFieldsInTuple];
    uint32_t fields_num = get_key_fields (s, key_fields, space_id);

    char last_tuple[BUFSIZ]  = {};
    char new_key_buf[BUFSIZ] = {};
    const char* ptr = reply->data;
    
    int len = mp_decode_array (&ptr) - 1;
    if (len < 0) return NULL;
    
    get_subarr_from_mp_array (reply->data, last_tuple, &len, 1, NULL);
    assert (mp_typeof (*last_tuple) == MP_ARRAY);
    
    const char* last_tuple_p = last_tuple;
    mp_decode_array (&last_tuple_p);
    uint32_t byte_num = 0;
    get_subarr_from_mp_array (last_tuple_p, new_key_buf, key_fields, fields_num, &byte_num);

    struct tnt_stream* new_key = tnt_object_as (NULL, new_key_buf, byte_num);
    return new_key;
}
