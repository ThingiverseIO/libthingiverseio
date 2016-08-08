#include "thingiverseio.h"
#include <unistd.h>
#include <stdio.h>
#include <msgpack.h>

char * const DESCRIPTOR = "\
func SayHello(Greeting string) (Answer string)\n\
tag example_tag\
";

int main() {

	int output = tvio_new_output(DESCRIPTOR);
	if (output == -1) {
		printf("Failed to create output\n");
		return 1;
	};

	int res_available;
	int err;

	while (1) {

		err = tvio_request_available(output, &res_available);
		if (err != 0) {
			printf("ERROR");
			return err;
		}

		if (res_available) {

			char * req_uuid;
			int req_uuid_size;
			err = tvio_get_next_request_id(output, &req_uuid, &req_uuid_size);
			if (err != 0) {
				printf("ERROR");
				return err;
			}

			printf("Got new request with UUID %s\n", req_uuid);

			char * rfun;
			int rfun_size;
			err = tvio_retrieve_request_function(output, req_uuid, &rfun, &rfun_size);
			if (err != 0) {
				printf("ERROR");
				return err;
			}

			printf("Request function name is %s\n", rfun);
			if (strcmp(rfun,"SayHello") != 0) {
				printf("Can't deal with this function name\n");
				// Reply with empty params to flush the request.
				tvio_reply(output, req_uuid, NULL, 0);
				continue;
			}

			void * rparams;
			int rparams_size;
			err = tvio_retrieve_request_params(output, req_uuid, &rparams, &rparams_size);
			if (err != 0) {
				printf("ERROR");
				return err;
			}

			msgpack_zone mempool;
			msgpack_object deserialized;

			msgpack_zone_init(&mempool, 2048);

			err = msgpack_unpack(rparams, rparams_size, NULL, &mempool, &deserialized);
			
			free(rparams);
			printf("Request parameters are: \n");
			msgpack_object_print(stderr, deserialized);


			msgpack_sbuffer sbuf;
			msgpack_packer pk;

			msgpack_sbuffer_init(&sbuf);
			msgpack_packer_init(&pk, &sbuf, msgpack_sbuffer_write);

			msgpack_pack_map(&pk,1);
			msgpack_pack_str(&pk,6);
			msgpack_pack_str_body(&pk,"Answer",6);
			msgpack_pack_str(&pk,15);
			msgpack_pack_str_body(&pk,"Greetings back!",15);

			err = tvio_reply(output, req_uuid, sbuf.data, sbuf.size);
			if (err != 0) {
				printf("ERROR");
				return err;
			}
			msgpack_sbuffer_destroy(&sbuf);

			msgpack_zone_destroy(&mempool);
		} else{
			sleep(1);
		}
	}
}
