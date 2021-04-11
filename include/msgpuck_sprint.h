#ifndef MSPCK_SPRINT
#define MSPCK_SPRINT


#include <stdio.h>
#include <stdlib.h>


void some_func (int* k);

/*! Function for printing arbitrary MsgPack object
*	\param reply pointer to buffer with reply form commands like select 
*	\param spaces_num number of spaces to indent
*	\param sprint_buf string to caontain resul, if NULL result will be print to the standart output
*/
void sprint_element (const char** data, unsigned int spaces_num, char* sprint_buf);


/*! Function for printing arbitrary MsgPack array
*	\param reply pointer to buffer with reply form commands like select 
*	\param spaces_num number of spaces to indent
*	\param sprint_buf string to caontain resul, if NULL result will be print to the standart output
*/
void sprint_array   (const char** data, unsigned int spaces_num, char* sprint_buf);

/*! Function for printing arbitrary MsgPack unsigned int
*	\param reply pointer to buffer with reply form commands like select 
*	\param spaces_num number of spaces to indent
*	\param sprint_buf string to caontain resul, if NULL result will be print to the standart output
*/
void sprint_uint    (const char** reply, unsigned int spaces_num, char* sprint_buf);

/*! Function for printing arbitrary MsgPack string
*	\param reply pointer to buffer with reply form commands like select 
*	\param spaces_num number of spaces to indent
*	\param sprint_buf string to caontain resul, if NULL result will be print to the standart output
*/
void sprint_str     (const char** data, unsigned int spaces_num, char* sprint_buf);
void sprint_map     (const char** data, unsigned int spaces_num, char* sprint_buf);
void sprint_bool    (const char** data, unsigned int spaces_num, char* sprint_buf);
void sprint_spaces  (unsigned int num, char* sprint_buf);

#endif