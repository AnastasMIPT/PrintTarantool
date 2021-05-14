#ifndef IMPROVED_SELECT_H
#define IMPROVED_SELECT_H
#include <stdint.h>


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
                                          uint32_t limit, struct tnt_stream* key,
                                          struct tnt_reply* reply);

/*! Function for efficient select first limit elements.
*	\param stream     stream object 
*	\param space_id   space no
*	\param limit      limit of tuples to select
*	\param reply      pointer to reply object for storing result
*   
*   \returns primary key from last tuple in reply
*/
struct tnt_stream* select_some_first     (struct tnt_stream* s, uint32_t space_id,
                                          uint32_t limit, struct tnt_reply* reply);


/*! Function getting last key from reply
*	\param stream     stream object 
*	\param space_id   space no
*	\param limit      limit of tuples to select
*	\param reply      pointer to reply object for storing result
*   
*   \returns primary key from last tuple in reply
*/
struct tnt_stream* get_new_key_by_reply  (struct tnt_stream* s, uint32_t space_id,
                                          struct tnt_reply* reply);


#endif // IMPROVED_SELECT_H