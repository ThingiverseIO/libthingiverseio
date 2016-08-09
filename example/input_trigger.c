/** @example input_trigger.c
 * Example showing a ThingiverseIO TRIGGER.
 */
#include "thingiverseio.h"
#include <stdio.h>
#include <unistd.h>
#include <msgpack.h>

char * const DESCRIPTOR = "\
func SayHello(Greeting string) (Answer string)\n\
tag example_tag\
";

int main() {
	int input = tvio_new_input(DESCRIPTOR);
	if (input == -1) {
		printf("Failed to create input\n");
		return 1;
	};

	tvio_start_listen(input,"SayHello");

	int err;
	int connected = 0;

	printf("Waiting for connection...");

	while (connected == 0) {
		err = tvio_connected(input, &connected);
		if (err != 0) {
			printf("ERROR");
			return err;
		}
		sleep(1);

		printf(".");

	}

	printf("\n Connected\n");

	printf("Triggering SayHello with Greeting 'Hello'\n");

	msgpack_sbuffer sbuf;
	msgpack_packer pk;

	msgpack_sbuffer_init(&sbuf);
	msgpack_packer_init(&pk, &sbuf, msgpack_sbuffer_write);

	msgpack_pack_map(&pk,1);
	msgpack_pack_str(&pk,8);
	msgpack_pack_str_body(&pk,"Greeting",8);
	msgpack_pack_str(&pk,5);
	msgpack_pack_str_body(&pk,"Hello",5);


	tvio_trigger(input, "SayHello", sbuf.data, sbuf.size);
	msgpack_sbuffer_destroy(&sbuf);

	printf("Waiting for listen result...");

	int available = 0;

	while (available == 0){
		tvio_listen_result_available(input, &available);
		printf(".");
		sleep(1);
	}

	char * res_uuid;
	int res_uuid_size;
	tvio_retrieve_listen_result_id(input, &res_uuid, &res_uuid_size);

	printf("\nReceived listen result with id %s.\n", res_uuid);

	char * res_fun;
	int res_fun_size;
	tvio_retrieve_listen_result_function(input, &res_fun, &res_fun_size);

	printf("Result function name is %s\n",res_fun);

	void * rparams;
	int rparams_size;

	tvio_retrieve_listen_result_params(input, &rparams, &rparams_size);

	msgpack_zone mempool;
	msgpack_object deserialized;

	msgpack_zone_init(&mempool, 2048);

	msgpack_unpack(rparams, rparams_size, NULL, &mempool, &deserialized);
	msgpack_zone_destroy(&mempool);

	printf("Result parameters are: \n");
	msgpack_object_print(stdout, deserialized);


}
