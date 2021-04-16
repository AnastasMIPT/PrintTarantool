#ifndef ADD_UTILITIES_H
#define ADD_UTILITIES_H

#include <stdint.h>


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




void mp_copy_current (char** dest, const char** src);


/*! Function for getting mp_array that is subarray of other
*	\param mp_array object 
*	\param key_fields pointer to array for storing result
*	\param space space no
*   
*   \returns number of key fields
*/
void get_subarr_from_mp_array (const char* mp_array, char* result, uint32_t* fields_indexes,
                                        uint32_t fields_num, uint32_t* byte_num);


#endif // ADD_UTILITIES_H