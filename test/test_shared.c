#include<stdio.h>
#include "thingiverseio.h"

  char * const DESCRIPTOR = "function SayHello(Greeting string) (Answer string)\n"
  			    "property testprop: Mood string";

  int main() {


	  printf("descriptor:\n\n");
	  printf(DESCRIPTOR);
	  printf("\n\n");

	printf("Testing Input Creation...\n");

	int input = tvio_new_input(DESCRIPTOR);

	if (input < 0) {
		printf("FAIL create input err %d\n", input);
		return 1;
	};
	printf("SUCCESS, Input id is %d\n", input);

	printf("Testing Output Creation...\n");

	int output = tvio_new_output(DESCRIPTOR);

	if (output == -1) {
		printf("FAIL\n");
		return 1;
	};

	printf("SUCCESS\n");

	sleep(5);
	int is;
	int err = tvio_input_connected(input, &is);
	if (err != 0) {
		printf("FAIL input connected err %d\n", err);
		return 1;
	};
	if (is != 1) {
		printf("FAIL, input did not connect \n");
		return 1;
	};

	printf("SUCCESS\n");

	printf("Testing Call...\n");

	char * uuid;
	int uuid_size;

	char * fun = "SayHello";
	char * params = "HELLO";
	int params_size = 5;

	err = tvio_input_call(input, fun,params,params_size, &uuid, &uuid_size);
	if (err != 0) {
		printf("FAIL input call err %d\n", err);
		return 1;
	};
	if (uuid_size != 36) {
		printf("FAIL, uuid_size is %d, want 36\n");
		return 1;
	};

	sleep(1);

	char * req_uuid;
	int req_uuid_size;
	err = tvio_output_request_id(output, &req_uuid, &req_uuid_size);
	if (err != 0) {
		printf("FAIL, err not 0\n");
		return 1;
	};
	if (req_uuid_size != 36) {
		printf("FAIL, req_uuid_size is %d, want 36\n", req_uuid_size);
		return 1;
	};

	char * rfun;
	int rfun_size;
	err = tvio_output_request_function(output, uuid, &rfun, &rfun_size);
	if (err != 0) {
		printf("FAIL, err not 0\n");
		return 1;
	};
	if (rfun_size == 0) {
		printf("FAIL, fun_size is 0\n");
		return 1;
	};
	void * rparams;
	int rparams_size;
	err = tvio_output_request_params(output, uuid, &rparams, &rparams_size);
	if (err != 0) {
		printf("FAIL, err not 0\n");
		return 1;
	};
	if (rparams_size != 5) {
		printf("FAIL, rparams_size is 0\n");
		return 1;
	};

	void * resparams = "HELLO_BACK";
	int resparams_size = 10;

	err = tvio_output_reply(output, uuid, resparams, resparams_size);
	if (err != 0) {
		printf("FAIL, err not 0\n");
		return 1;
	};

	sleep(1);

	int ready;
	err = tvio_input_call_result_available(input, uuid, &ready);
	if (err != 0) {
		printf("FAIL, err not 0\n");
		return 1;
	};
	if (ready != 1) {
		printf("FAIL, result hasnt arrived\n");
		return 1;
	}

	void * resultparams;
	int resultparams_size;
	err = tvio_input_call_result_params(input, uuid, &resultparams, &resultparams_size);
	if (err != 0) {
		printf("FAIL, err not 0\n");
		return 1;
	};
	if (resultparams_size != 10) {
		printf("FAIL, rparams_size is 0\n");
		return 1;
	};

	printf("SUCCESS\n");

	printf("Testing Trigger...\n");

	err = tvio_input_listen_start(input, fun);
	if (err != 0) {
		printf("FAIL, tvio_start_listen err not 0\n");
		return 1;
	};

	sleep(1);

	err = tvio_input_trigger(input, fun,params,params_size);
	if (err != 0) {
		printf("FAIL, tvio_trigger err not 0\n");
		return 1;
	};
	sleep(1);

	err = tvio_output_request_id(output, &req_uuid, &req_uuid_size);
	if (err != 0) {
		printf("FAIL, get_gext_req err not 0\n");
		return 1;
	};
	if (req_uuid_size == 0) {
		printf("FAIL, req_uuid_size is 0\n");
		return 1;
	};

	err = tvio_output_reply(output, req_uuid, resparams, resparams_size);
	if (err != 0) {
		printf("FAIL, tvio_reply err not 0, is \n");
		return 1;
	};

	sleep(1);

	err = tvio_input_listen_result_available(input, &ready);
	if (err != 0) {
		printf("FAIL,tvio_listen_result_available err not 0\n");
		return 1;
	};
	if (ready != 1) {
		printf("FAIL,tvio_listen_result_available result hasnt arrived\n");
		return 1;
	}

	err = tvio_input_listen_result_params(input, &resultparams, &resultparams_size);
	if (err != 0) {
		printf("FAIL,tvio_retrieve_listen_result_params err not 0\n");
		return 1;
	};
	if (resultparams_size != 10) {
		printf("FAIL, rparams_size is 0\n");
		return 1;
	};

	printf("SUCCESS\n");

	printf("Testing Observe...\n");

	char* prop = "testprop";

	err = tvio_output_property_set(output, prop, params,params_size);
	if (err != 0) {
		printf("FAIL, tvio_trigger err not 0\n");
		return 1;
	};
	err = tvio_input_change_start_observe(input, prop);
	if (err != 0) {
		printf("FAIL, tvio_input_change_start_observe err %d\n", err);
		return 1;
	};

	sleep(1);

	err = tvio_input_change_available(input, &ready);
	if (err != 0) {
		printf("FAIL,tvio_change_available err not 0\n");
		return 1;
	};
	if (ready != 1) {
		printf("FAIL,tvio_change_available hasnt arrived\n");
		return 1;
	}
	err = tvio_input_change_value(input, &resultparams, &resultparams_size);
	if (err != 0) {
		printf("FAIL,tvio_change_value err not 0\n");
		return 1;
	};
	if (resultparams_size != 5) {
		printf("FAIL, property value size is %d, not %d\n", resultparams_size, 5);
		return 1;
	};
	err = tvio_input_remove(input);
	if (err != 0) {
		printf("FAIL, tvio_remove_input err %d\n", err);
		return 1;
	};

	err = tvio_output_remove(output);
	if (err != 0) {
		printf("FAIL, tvio_remove_input err not 0\n");
		return 1;
	};
	printf("SUCCESS\n");
	return 0;
}
