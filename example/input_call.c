/** @example input_call.c
 * Example showing a ThingiverseIO CALL.
 */
#include "tvio.h"
#include <stdio.h>
#include <unistd.h>
#include <msgpack.h>

char * const DESCRIPTOR = "\
func SayHello(Greeting string) (Answer string)\n\
tag example_tag\
";

int main() {
	int input = new_input(DESCRIPTOR);
	if (input == -1) {
		printf("Failed to create input\n");
		return 1;
	};

	int err;
	int connected;

	//printf("Calling SayHello with Greeting 'Hello'\n");

	msgpack_sbuffer sbuf;
	msgpack_packer pk;

	msgpack_sbuffer_init(&sbuf);
	msgpack_packer_init(&pk, &sbuf, msgpack_sbuffer_write);

	msgpack_pack_map(&pk,1);
	msgpack_pack_str(&pk,8);
	msgpack_pack_str_body(&pk,"Greeting",8);
	msgpack_pack_str(&pk,5);
	msgpack_pack_str_body(&pk,"Hello",5);
	char * req_uuid;
	int req_uuid_size;

	call(input, "SayHello", sbuf.data, sbuf.size, &req_uuid, &req_uuid_size);

	printf("Waiting for Answer...");

	int received = 0;

	while (received == 0){
		result_ready(input, req_uuid, &received);
		printf(".");
		sleep(1);
	}

	printf("\nReceived Answer.\n");
	void * rparams;
	int rparams_size;

	err = retrieve_result_params(input, req_uuid, &rparams, &rparams_size);

	msgpack_zone mempool;
	msgpack_object deserialized;

	msgpack_zone_init(&mempool, 2048);

	msgpack_unpack(rparams, rparams_size, NULL, &mempool, &deserialized);
	msgpack_zone_destroy(&mempool);

	printf("Result parameters are: \n");
	msgpack_object_print(stderr, deserialized);
	msgpack_sbuffer_destroy(&sbuf);
	msgpack_zone_destroy(&mempool);
}
