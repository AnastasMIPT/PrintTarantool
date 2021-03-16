#ifndef MSPCK_PRINT
#define MSPCK_PRINT


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
void print_uint    (struct tnt_reply* reply, unsigned int spaces_num);  
void print_str     (struct tnt_reply* reply, unsigned int spaces_num);
void print_map     (struct tnt_reply* reply, unsigned int spaces_num);
void print_reply   (struct tnt_reply* reply, unsigned int spaces_num);
void print_bool    (struct tnt_reply* reply, unsigned int spaces_num);
void print_spaces  (unsigned int num);

#endif