/**
 * @author Joern Weissenborn
 * @brief Shared library for ThingiverseIO.
 */


/**
 * Error Codes:
 *   - ERR_INVALID_DESCRIPTOR  C.int = 1
 *   - ERR_INVALID_INPUT       C.int = 2
 *   - ERR_INVALID_OUTPUT      C.int = 3
 *   - ERR_INVALID_RESULT_ID   C.int = 4
 *   - ERR_INVALID_REQUEST_ID  C.int = 5
 *   - ERR_NO_RESULT_AVAILABLE C.int = 6
 *   - ERR_RESULT_NOT_ARRIVED  C.int = 7
 */


#ifdef __cplusplus
extern "C" {
#endif

	/**
	 * @brief Gets the version of ThingiverseIO. Useful to check if the shared library is linked correctly.
	 *
	 * @param major Pointer to an integer that will be set to version major.
	 * @param minor Pointer to an integer that will be set to version minor.
	 * @param fix Pointer to an integer that will be set to version fix.
	 *
	 * @return error
	 */
extern int tvio_version(int* major, int* minor, int* fix);

/**
 * @brief Checks the given descriptor for semantic errors. The result be a human readable string describing the semantic errors. If the result pointer is empty after check, the descriptor is OK.
 *
 * @param descriptor The descriptor to check.
 * @param result A pointer which will be set to result string.
 * @param result_size A pointer to int which will be set to size of the result string.
 *
 * @return error
 */
extern void tvio_check_descriptor(char* descriptor, char** result, int* result_size);

/**
 * @brief Creates a new thingiverseio input.
 *
 * @param descriptor The descriptor of the input.
 *
 * @return A int reference to the created input. -1 if an error occured.
 */
extern int tvio_new_input(char* descriptor);

/**
 * @brief Gets an inputs UUID.
 *
 * @param input The input reference.
 * @param uuid A pointer which will be set to the UUID.
 * @param uuid_size The size of the UUID.
 *
 * @return error
 */
extern int tvio_get_input_uuid(int input, char** uuid, int* uuid_size);

/**
 * @brief Removes an input.
 *
 * @param input The input reference.
 *
 * @return error.
 */
extern int tvio_remove_input(int input);

/**
 * @brief Gives the connection status of an input.
 *
 * @param input The input reference.
 * @param is 1 if at least one Output is connected, 0 otherwise.
 *
 * @return error
 */
extern int tvio_connected(int input, int* is);

/**
 * @brief Makes the an input listen to the given function.
 *
 * @param input The input reference.
 * @param function Name of the function to listen.
 *
 * @return error
 */
extern int tvio_start_listen(int input, char* function);

/**
 * @brief Makes an input stop listening to the given function.
 *
 * @param input The input reference.
 * @param function Name of the function to stop listening.
 *
 * @return error
 */
extern int tvio_stop_listen(int input, char* function);

/**
 * @brief Executes a ThingiverseIO CALL.
 *
 * @param input
 * @param function Name of the function.
 * @param params A pointer to the MsgPack serialized parameters.
 * @param params_size Size of the serialized parameters.
 * @param id A pointer which will be set to requests UUID.
 * @param id_size Size of the requests UUID.
 *
 * @return error
 */
extern int tvio_call(int input, char* function, void* params, int params_size, char** id, int* id_size);

/**
 * @brief Executes a ThingiverseIO CALL-ALL. WARNING: Function implementation is inclomete.
 *
 * @param input The input reference.
 * @param function Name of the function.
 * @param params A pointer to the MsgPack serialized parameters.
 * @param params_size Size of the serialized parameters.
 * @param id A pointer which will be set to requests UUID.
 * @param id_size Size of the requests UUID.
 *
 * @return error
 */
extern int tvio_call_all(int input, char* function, void* params, int params_size, char** id, int* id_size);

/**
 * @brief Executes a ThingiverseIO TRIGGER.
 *
 * @param input The input reference.
 * @param function Name of the function.
 * @param params A pointer to the MsgPack serialized parameters.
 * @param params_size Size of the serialized parameters.
 *
 * @return error
 */
extern int tvio_trigger(int input, char* function, void* params, int params_size);

/**
 * @brief Executes a ThingiverseIO TRIGGER-ALL.
 *
 * @param input The input reference.
 * @param function Name of the function.
 * @param params A pointer to the MsgPack serialized parameters.
 * @param params_size Size of the serialized parameters.
 *
 * @return error
 */
extern int tvio_trigger_all(int input, char* function, void* params, int params_size);

/**
 * @brief Checks if the result for an request has arrived.
 *
 * @param input The input reference.
 * @param id The UUID of the request.
 * @param ready A pointer which will be set to 1 if the result is ready, 0 otherwise.
 *
 * @return error
 */
extern int tvio_result_ready(int input, char* id, int* ready);

/**
 * @brief Retrieves the MsgPack serialized parameters of a result.
 *
 * @param input The input reference.
 * @param id The UUID of the request.
 * @param params A pointer which will be set to serialized parameters.
 * @param params_size The size of the serialized parameters.
 *
 * @return error
 */
extern int tvio_retrieve_result_params(int input, char* id, void** params, int* params_size);

/**
 * @brief Checks if a new result from function which an input listening to is available.
 *
 * @param input The input reference.
 * @param is A pointer which will be set to 1 if a result is available, 0 otherwise.
 *
 * @return error
 */
extern int tvio_listen_result_available(int input, int* is);

/**
 * @brief Retrieves UUID of the next available listen result.
 *
 * @param input The input reference.
 * @param id A pointer which will be set to the results UUID.
 * @param id_size An int pointer which will be set to the size of the results UUID.
 *
 * @return error
 */
extern int tvio_retrieve_listen_result_id(int input, char** id, int* id_size);

/**
 * @brief Retrieves the function name the next available listen result.
 *
 * @param input The input reference.
 * @param function A pointer which will be set to the results function name.
 * @param function_size A pointer which will be set to the size of the results function name.
 *
 * @return error
 */
extern int tvio_retrieve_listen_result_function(int input, char** function, int* function_size);

/**
 * @brief Retrieves the MsgPack serialized parameters of the next available listen result.
 *
 * @param input The input reference.
 * @param params A pointer which will be set to the parameters of the listen result.
 * @param params_size A pointer which will be set to size of the result parameters.
 *
 * @return error
 */
extern int tvio_retrieve_listen_result_params(int input, void** params, int* params_size);

/**
 * @brief Retrieves the next available parameters of a CALL-ALL result.
 *
 * @param input The input reference.
 * @param id The UUID of the request.
 * @param params A pointer which will be set to the parameters of the listen result.
 * @param params_size A pointer which will be set to size of the result parameters.
 *
 * @return
 */
extern int tvio_retrieve_next_call_all_result_params(int input, char* id, void** params, int* params_size);

/**
 * @brief Creates a new thingiverseio output.
 *
 * @param descriptor The descriptor of the output.
 *
 * @return A int reference to the created output. -1 if an error occured.
 */
extern int tvio_new_output(char* descriptor);

/**
 * @brief Gets an outputs UUID.
 *
 * @param output The output reference.
 * @param uuid A pointer which will be set to the UUID.
 * @param uuid_size The size of the UUID.
 *
 * @return error
 */
extern int tvio_get_output_uuid(int output, char** uuid, int* uuid_size);

/**
 * @brief Removes an output.
 *
 * @param output The output reference.
 *
 * @return error.
 */
extern int tvio_remove_output(int output);

/**
 * @brief Checks wether a new request is available.
 *
 * @param output The output reference.
 * @param is A pointer which will be set to 1 if a request is available, 0 otherwise.
 *
 * @return error
 */
extern int tvio_request_available(int output, int* is);

/**
 * @brief Retrieves the UUID of the next available request.
 *
 * @param output The output reference.
 * @param uuid A pointer which will be set to the requests UUID.
 * @param uuid_size The size of the UUID.
 *
 * @return error
 */
extern int tvio_get_next_request_id(int output, char** id, int* id_size);

/**
 * @brief Retrieves the function name of a request.
 *
 * @param output The output reference.
 * @param id The request UUID.
 * @param function A pointer which will be set to the results function name.
 * @param function_size A pointer which will be set to the size of the results function name.
 * @return error
 */
extern int tvio_retrieve_request_function(int output, char* id, char** function, int* function_size);

/**
 * @brief Retrieves the MsgPack serialized parameters of a request.
 *
 * @param output The output reference.
 * @param params A pointer which will be set to the parameters of the request.
 * @param params_size A pointer which will be set to size of the request parameters.
 *
 * @return error
 */
extern int tvio_retrieve_request_params(int output, char* id, void** params, int* params_size);

/**
 * @brief Replies to a request and sends the result to all concerned peers.
 *
 * @param output The output reference.
 * @param id The UUID of the request to reply to.
 * @param params A pointer to the MsgPack serialized parameters.
 * @param params_size Size of the serialized parameters.
 *
 * @return error
 */
extern int tvio_reply(int output, char* id, void* params, int params_size);

/**
 * @brief Executes ThingiverseIO EMIT.
 *
 * @param output The output reference.
 * @param function The of the emitted function.
 * @param in_params The MsgPack serialized input parameters of the emitted function.
 * @param in_params_size The size of the serialized input parameters.
 * @param params The MsgPack serialized output parameters of the emitted function.
 * @param params_size The size of the serialized output parameters.
 *
 * @return error
 */
extern int tvio_emit(int output, char* function, void* in_params, int in_params_size, void* params, int params_size);

#ifdef __cplusplus
}
#endif
