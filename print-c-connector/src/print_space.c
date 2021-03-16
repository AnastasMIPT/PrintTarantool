#include <stdio.h>
#include <stdlib.h>
#include <tarantool/tarantool.h>
#include <tarantool/tnt_net.h>
#include <tarantool/tnt_opt.h>

#define MP_SOURCE 1
#include <msgpuck.h>


/*! Function for printing arbitrary MsgPack object
*	@param [in] reply pointer to buffer with reply form commands like select 
*	@param [in] spaces_num number of spaces to indent
*/
void print_element (struct tnt_reply* reply, unsigned int spaces_num);


/*! Function for printing arbitrary MsgPack array
*	@param [in] reply pointer to buffer with reply form commands like select 
*	@param [in] spaces_num number of spaces to indent
*/
void print_array   (struct tnt_reply* reply, unsigned int spaces_num);

/*! Function for printing arbitrary MsgPack unsigned int
*	@param [in] reply pointer to buffer with reply form commands like select 
*	@param [in] spaces_num number of spaces to indent
*/
void print_uint    (struct tnt_reply* reply, unsigned int spaces_num);

/*! Function for printing arbitrary MsgPack string
*	@param [in] reply pointer to buffer with reply form commands like select 
*	@param [in] spaces_num number of spaces to indent
*/
void print_str     (struct tnt_reply* reply, unsigned int spaces_num);
void print_map     (struct tnt_reply* reply, unsigned int spaces_num);
void print_reply   (struct tnt_reply* reply, unsigned int spaces_num);
void print_bool    (struct tnt_reply* reply, unsigned int spaces_num);
void print_spaces  (unsigned int num);

void main(int argc, char** argv) {

    if (argc < 2) {
        printf ("ERROR: need space id\n");
        exit(1);
    }

    struct tnt_stream *tnt = tnt_net(NULL);             // Создание нового потока
    tnt_set(tnt, TNT_OPT_URI, "localhost:3301");        // Установка порта
    if (tnt_connect(tnt) < 0) {                         // обработка ошибок
        printf("Connection refused\n");
        exit(1);
    }
    struct tnt_stream *tuple = tnt_object(NULL);        // Создание пустого MsgPuck объекта 
    tnt_object_format(tuple, "[%d]", 3); /* tuple = search key */
    tnt_select(tnt, atoi(argv[1])/*512*/, 0, (2^32) - 1, 0, TNT_ITER_ALL, tuple);  // TODO interator GT after first request?  
    //tnt_get_space(tnt);   
    //tnt_get_index(tnt);
    
    tnt_flush(tnt);
    struct tnt_reply reply; tnt_reply_init(&reply);
    tnt->read_reply(tnt, &reply);
    if (reply.code != 0) {
        printf("Select failed.\n");
        exit(1);
    }
    unsigned int spaces = 0;
    print_element (&reply, spaces);

    tnt_close(tnt);
    tnt_stream_free(tuple);
    tnt_stream_free(tnt);
}


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