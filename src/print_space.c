#include <stdio.h>
#include <stdlib.h>
#include <tarantool/tarantool.h>
#include <tarantool/tnt_net.h>
#include <tarantool/tnt_opt.h>
#include <stdint.h>

#include <msgpuck.h>


#include "../include/msgpuck_print.h"
#include "../include/msgpuck_sprint.h"
#include "../include/improved_select.h"

const uint32_t NumTuplesInOneRequest = 100;

void main(int argc, char** argv) {
    if (argc < 3) {
        printf ("ERROR: need space id as first argument and host_port string as second\n");
        exit (1);
    }
    
    uint32_t space_id = atoi (argv[1]);
    char* host_port   = argv[2];

    struct tnt_stream *tnt = tnt_net (NULL);          
    tnt_set (tnt, TNT_OPT_URI, host_port);               
    if (tnt_connect (tnt) < 0) {                         
        printf("Connection refused\n");
        exit (1);
    }
    
    struct tnt_reply reply = {};
    struct tnt_stream* cur_tuple = select_some_first (tnt, space_id, NumTuplesInOneRequest, &reply);

    print_array (&reply.data, 0, 0);

    while(true) {

        if (cur_tuple == NULL) {
            break;
        }

        struct tnt_reply reply = {};
        cur_tuple = select_some_after_key (tnt, space_id, NumTuplesInOneRequest, cur_tuple, &reply);

        print_array (&reply.data, 0, 0);
    }
    printf ("\n");
    
    tnt_close (tnt);
    tnt_stream_free (tnt);
}
