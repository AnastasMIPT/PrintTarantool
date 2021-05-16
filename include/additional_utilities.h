#ifndef ADD_UTILITIES_H
#define ADD_UTILITIES_H

#include <stdint.h>


// TODO unit tests with complicated primary key, small array key_fields. Add errors handling connected to 
// key_fields array size.
/*! Function for retrieving the field numbers in a tuple defined by the primary key
*	\param stream     stream object 
*	\param key_fields pointer to array for storing result
*	\param space      space's id
*   
*   \returns number of key fields
*/
uint32_t get_key_fields (struct tnt_stream* s, uint32_t* key_fields, uint32_t space);



/*! Function to copy an element of an MessagePack array.
*	\param dest pointer to element of MessagePack array that is destination
*	\param src  pointer to element of MessagePack array that is source
*/
void mp_copy_current (char** dest, const char** src);


/*! Function for getting  MessagePack array that is subarray of other
*	\param mp_array       pointer to MessagePack array
*   \param result         pointer to store result array
*   \param fields_indexes array with indexes of source array
*   \param fields_num     length of fields_indexes array
*   \param byte_num       pointer to store number of bytes written. May be NULL
*   
*/
void get_subarr_from_mp_array (const char* mp_array, char* result, uint32_t* fields_indexes,
                                        uint32_t fields_num, uint32_t* byte_num);


#endif // ADD_UTILITIES_H