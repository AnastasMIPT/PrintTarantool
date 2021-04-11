#ifndef MSPCK_PRINT
#define MSPCK_PRINT


#include <stdio.h>
#include <stdlib.h>


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
void print_bool    (const char** data, unsigned int spaces_num);
void print_spaces  (unsigned int num);

#endif