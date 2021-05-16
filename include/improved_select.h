#ifndef IMPROVED_SELECT_H
#define IMPROVED_SELECT_H
#include <stdint.h>


/*! Function for efficiently selecting a slice from space.
*   The fragment is selected using the "GT" iterator by the "key" with a "limit" tuple constraint.
*	\param stream     stream object 
*	\param space_id   space's id
*	\param limit      limit of tuples to select
*	\param key        key object. Tuples will be selected by this key using the "GT" iterator.
*	\param reply      pointer to reply object for storing result
*   
*   \returns primary key from the last tuple from the reply or NULL if the reply is empty
*/
struct tnt_stream* select_some_after_key (struct tnt_stream* s, uint32_t space_id,
                                          uint32_t limit, struct tnt_stream* key,
                                          struct tnt_reply* reply);

/*! Function for efficient selection of the first 'limit' tuples.
*	\param stream     stream object 
*	\param space_id   space's id
*	\param limit      limit of tuples to select
*	\param reply      pointer to reply object for storing result
*   
*   \returns primary key from the last tuple from the reply or NULL if the reply is empty
*/
struct tnt_stream* select_some_first     (struct tnt_stream* s, uint32_t space_id,
                                          uint32_t limit, struct tnt_reply* reply);


/*! Function to retrieve the primary key from the last reply tuple.
*	\param stream     stream object 
*	\param space_id   space's id
*	\param limit      limit of tuples to select
*	\param reply      pointer to reply object for storing result
*   
*   \returns primary key from the last tuple from the reply or NULL if the reply is empty
*/
struct tnt_stream* get_new_key_by_reply  (struct tnt_stream* s, uint32_t space_id,
                                          struct tnt_reply* reply);


#endif // IMPROVED_SELECT_H