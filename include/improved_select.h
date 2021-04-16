#ifndef IMPROVED_SELECT_H
#define IMPROVED_SELECT_H



#include <stdint.h>

struct tnt_stream* select_some_after_key (struct tnt_stream* s, uint32_t space_id,
                                            uint32_t limit, struct tnt_stream* key, struct tnt_reply* reply);

struct tnt_stream* select_some_first (struct tnt_stream* s, uint32_t space_id, uint32_t limit, struct tnt_reply* reply);


struct tnt_stream* get_new_key_by_reply (struct tnt_stream* s, uint32_t space_id, struct tnt_reply* reply);



#endif // IMPROVED_SELECT_H