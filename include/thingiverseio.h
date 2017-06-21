//	Copyright (c) 2015 Joern Weissenborn
//
//	This file is part of libthingiverseio.
//
//	Foobar is free software: you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	libaursir is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with libthingiverseio.  If not, see <http://www.gnu.org/licenses/>.

/**
 * @author Joern Weissenborn
 * @brief Shared library for ThingiverseIO.
 * @file thingiverseio.h
 */


/**
 * Error Codes:
 *	- NO_ERR			= 0
 *	- ERR_NETWORK			= -1
 * 	- ERR_INVALID_DESCRIPTOR	= -2
 * 	- ERR_INVALID_INPUT		= -3
 * 	- ERR_INVALID_OUTPUT		= -4
 * 	- ERR_INVALID_RESULT_ID		= -5
 * 	- ERR_INVALID_REQUEST_ID	= -6
 * 	- ERR_NO_RESULT_AVAILABLE	= -7
 * 	- ERR_NO_REQUEST_AVAILABLE	= -8
 * 	- ERR_RESULT_NOT_ARRIVED	= -9
 * 	- ERR_INVALID_FUNCTION		= -10
 * 	- ERR_INVALID_PROPERTY		= -11
 * 	- ERR_NO_UPDATE			= -12
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
	 */
extern void tvio_version(int* major, int* minor, int* fix);

/**
 * @brief Checks the given descriptor for semantic errors. The result be a human readable string describing the semantic errors. If the result pointer is empty after check, the descriptor is OK.
 *
 * @param descriptor The descriptor to check.
 * @param result A pointer which will be set to result string.
 * @param result_size A pointer to int which will be set to size of the result string.
 *
 */
extern void tvio_check_descriptor(char* descriptor, char** result, int* result_size);

/**
 * @brief Translates an error code to an error string.
 *
 * @param error The error code.
 * @param msg_p A pointer which will be set to the error string.
 * @param msg_size A pointer which will be set to the size of the error string.
 *
 */
extern void tvio_error_message(int error, char** msg_p, int* msg_size);

/**
 * @brief Creates a new thingiverseio input.
 *
 * @param descriptor The descriptor of the input.
 *
 * @return A int reference to the created input. -1 if an error occured.
 */
extern int tvio_new_input(char* descriptor);

/**
 * @brief Removes an input.
 *
 * @param input The input reference.
 *
 * @return error.
 */
extern int tvio_input_remove(int input);

/**
 * @brief Indicates the connection status of an input.
 *
 * @param input The input reference.
 * @param is will be set to 1 if at least one Output is connected, 0 otherwise.
 *
 * @return error
 */
extern int tvio_input_connected(int input, int* is);

/**
 * @brief Gets an inputs UUID.
 *
 * @param input The input reference.
 * @param uuid_p A pointer which will be set to the UUID.
 * @param uuid_size The size of the UUID.
 *
 * @return error
 */
extern int tvio_input_uuid(int input, char** uuid_p, int* uuid_size);

/**
 * @brief Gets an inputs interface.
 *
 * @param input The input reference.
 * @param iface_p A pointer which will be set to the interface.
 * @param iface_size The size of the interface.
 *
 * @return error
 */
extern int tvio_input_interface(int input, char** iface_p, int* iface_size);

/**
 * @brief Executes a ThingiverseIO CALL.
 *
 * @param input
 * @param function Name of the function.
 * @param fparams A pointer to the MsgPack serialized parameters.
 * @param fparams_size Size of the serialized parameters.
 * @param id A pointer which will be set to requests UUID.
 * @param id_size Size of the requests UUID.
 *
 * @return error
 */
extern int tvio_input_call(int input, char* function, void* fparams, int fparams_size, char** id, int* id_size);

/**
 * @brief Executes a ThingiverseIO CALL-ALL.
 *
 * @param input The input reference.
 * @param function Name of the function.
 * @param fparams A pointer to the MsgPack serialized parameters.
 * @param fparams_size Size of the serialized parameters.
 * @param id A pointer which will be set to requests UUID.
 * @param id_size Size of the requests UUID.
 *
 * @return error
 */
extern int tvio_input_call_all(int input, char* function, void* fparams, int fparams_size, char** id, int* id_size);

/**
 * @brief Executes a ThingiverseIO TRIGGER.
 *
 * @param input The input reference.
 * @param function Name of the function.
 * @param fparams A pointer to the MsgPack serialized parameters.
 * @param fparams_size Size of the serialized parameters.
 *
 * @return error
 */
extern int tvio_input_trigger(int input, char* function, void* fparams, int fparams_size);

/**
 * @brief Executes a ThingiverseIO TRIGGER-ALL.
 *
 * @param input The input reference.
 * @param function Name of the function.
 * @param fparams A pointer to the MsgPack serialized parameters.
 * @param fparams_size Size of the serialized parameters.
 *
 * @return error
 */
extern int tvio_input_trigger_all(int input, char* function, void* fparams, int fparams_size);

/**
 * @brief Checks if the result for an request has arrived.
 *
 * @param input The input reference.
 * @param id The UUID of the request.
 * @param ready A pointer which will be set to 1 if the result is ready, 0 otherwise.
 *
 * @return error
 */
extern int tvio_input_call_result_available(int input, char* id, int* ready);

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
extern int tvio_input_call_result_params(int input, char* id, void** params, int* params_size);

/**
 * @brief Makes the an input listen to the given function.
 *
 * @param input The input reference.
 * @param function Name of the function to listen.
 *
 * @return error
 */
extern int tvio_input_listen_start(int input, char* function);

/**
 * @brief Makes an input stop listening to the given function.
 *
 * @param input The input reference.
 * @param function Name of the function to stop listening.
 *
 * @return error
 */
extern int tvio_input_listen_stop(int input, char* function);

/**
 * @brief Checks if a new result from function which an input listening to is available.
 *
 * @param input The input reference.
 * @param is A pointer which will be set to 1 if a result is available, 0 otherwise.
 *
 * @return error
 */
extern int tvio_input_listen_result_available(int input, int* is);

/**
 * @brief Retrieves UUID of the next available listen result.
 *
 * @param input The input reference.
 * @param id A pointer which will be set to the results UUID.
 * @param id_size An int pointer which will be set to the size of the results UUID.
 *
 * @return error
 */
extern int tvio_input_listen_result_id(int input, char** id, int* id_size);

/**
 * @brief Retrieves the function name the next available listen result.
 *
 * @param input The input reference.
 * @param function A pointer which will be set to the results function name.
 * @param function_size A pointer which will be set to the size of the results function name.
 *
 * @return error
 */
extern int tvio_input_listen_result_function(int input, char** function, int* function_size);

/**
 * @brief Retrieves the MsgPack serialized request parameters of the next available listen result.
 *
 * @param input The input reference.
 * @param params A pointer which will be set to the parameters of the listen result.
 * @param params_size A pointer which will be set to size of the result parameters.
 *
 * @return error
 */
extern int tvio_input_listen_result_request_params(int input, void** params, int* params_size);

/**
 * @brief Retrieves the MsgPack serialized parameters of the next available listen result.
 *
 * @param input The input reference.
 * @param params A pointer which will be set to the parameters of the listen result.
 * @param params_size A pointer which will be set to size of the result parameters.
 *
 * @return error
 */
extern int tvio_input_listen_result_params(int input, void** params, int* params_size);

/**
 * @brief Clears the next listen result. Must be called to receive following results.
 *
 * @param input The input reference.
 *
 * @return error
 */
extern int tvio_input_listen_result_clear(int input);

/**
 * @brief Checks if a CALL-ALL result is available.
 *
 * @param input The input reference.
 * @param id The UUID of the request.
 * @param is A pointer which will be set to 1 if a result is available, 0 otherwise.
 *
 * @return error
 */
extern int tvio_input_call_all_next_result_available(int input, int* is);

/**
 * @brief Retrieves the next available parameters of a CALL-ALL result.
 *
 * @param input The input reference.
 * @param id The UUID of the request.
 * @param params A pointer which will be set to the parameters of the listen result.
 * @param params_size A pointer which will be set to size of the result parameters.
 *
 * @return error
 */
extern int tvio_input_call_all_next_result_params(int input, char* id, void** params, int* params_size);

/**
 * @brief Clears the next CALL-ALL result. Must be called to receive further results.
 *
 * @param input The input reference.
 * @param id The UUID of the request.
 *
 * @return error
 */
extern int tvio_input_call_all_next_result_clear(int input, char* id);

/**
 * @brief Clears the CALL-ALL request.
 *
 * @param input The input reference.
 * @param id The UUID of the request.
 *
 * @return error
 */
extern int tvio_input_call_all_request_clear(int input, char* id);

/**
 * @brief Gets the MsgPack serialized value of a property.
 *
 * @param input The input reference.
 * @param property The name of the property.
 * @param value Pointer which will be set to the serialized value.
 * @param value_size Pointer which will be set to the size of the serialized data.
 *
 * @return error
 */
extern int tvio_input_property_get(int input, char* property, void** value, int* value_size);

/**
 * @brief Initiates an update of a property.
 *
 * @param input The input reference.
 * @param property The name of the property.
 *
 * @return error
 */
extern int tvio_input_property_update(int input, char* property);

/**
 * @brief Checks if an update for a property is available.
 *
 * @param input The input reference.
 * @param property The name of the property.
 * @param is A pointer which will be set to 1 if an update is available, 0 otherwise.
 *
 * @return error
 */
extern int tvio_input_property_update_available(int input, int* is);

/**
 * @brief Gets the MsgPack serialized value of a property update.
 *
 * @param input The input reference.
 * @param property The name of the property.
 * @param value Pointer which will be set to the serialized value.
 * @param value_size Pointer which will be set to the size of the serialized data.
 *
 * @return error
 */
extern int tvio_input_property_update_get(int input, char* property, void** value, int* value_size);

/**
 * @brief Makes the an input start observing changes to a property.
 *
 * @param input The input reference.
 * @param property The name of the property.
 *
 * @return error
 */
extern int tvio_input_change_start_observe(int input, char* property);

/**
 * @brief Makes the an input stop observing changes to a property.
 *
 * @param input The input reference.
 * @param property The name of the property.
 *
 * @return error
 */
extern int tvio_input_change_stop_observe(int input, char* property);

/**
 * @brief Checks if a property change is available.
 *
 * @param input The input reference.
 * @param is A pointer which will be set to 1 if an update is available, 0 otherwise.
 *
 * @return error
 */
extern int tvio_input_change_available(int input, int* is);

/**
 * @brief Gets the the name of the changed property.
 *
 * @param input The input reference.
 * @param property Pointer which will be set to the name of the property.
 * @param property_size Pointer which will be set to the size of the name of the property.
 *
 * @return error
 */
extern int tvio_input_change_property(int input, char** property, int* property_size);

/**
 * @brief Gets the MsgPack serialized value of the changed property.
 *
 * @param input The input reference.
 * @param property The name of the property.
 * @param value Pointer which will be set to the serialized value.
 * @param value_size Pointer which will be set to the size of the serialized data.
 *
 * @return error
 */
extern int tvio_input_change_value(int input, void** value, int* value_size);

/**
 * @brief Clears the current property change. Must be called to receive further changes.
 *
 * @param input The input reference.
 *
 * @return error
 */
extern int tvio_input_change_clear(int input);

/**
 * @brief Creates a new thingiverseio output.
 *
 * @param descriptor The descriptor of the output.
 *
 * @return A int reference to the created output. -1 if an error occured.
 */
extern int tvio_new_output(char* descriptor);

/**
 * @brief Removes an output.
 *
 * @param output The output reference.
 *
 * @return error.
 */
extern int tvio_output_remove(int output);

/**
 * @brief Indicates the connection status of an output.
 *
 * @param output The output reference.
 * @param is will be set to 1 if at least one Output is connected, 0 otherwise.
 *
 * @return error
 */
extern int tvio_output_connected(int output, int* is);

/**
 * @brief Gets an outputs UUID.
 *
 * @param output The output reference.
 * @param uuid A pointer which will be set to the UUID.
 * @param uuid_size The size of the UUID.
 *
 * @return error
 */
extern int tvio_output_uuid(int output, char** uuid, int* uuid_size);


/**
 * @brief Gets an outputs interface.
 *
 * @param output The output reference.
 * @param iface_p A pointer which will be set to the interface.
 * @param iface_size The size of the interface.
 *
 * @return error
 */
extern int tvio_output_interface(int output, char** iface_p, int* iface_size);

/**
 * @brief Checks wether a new request is available.
 *
 * @param output The output reference.
 * @param is A pointer which will be set to 1 if a request is available, 0 otherwise.
 *
 * @return error
 */
extern int tvio_output_request_available(int output, int* is);

/**
 * @brief Retrieves the UUID of the next available request.
 *
 * @param output The output reference.
 * @param id A pointer which will be set to the requests UUID.
 * @param id_size The size of the UUID.
 *
 * @return error
 */
extern int tvio_output_request_id(int output, char** id, int* id_size);

/**
 * @brief Retrieves the function name of a request.
 *
 * @param output The output reference.
 * @param id The request UUID.
 * @param function A pointer which will be set to the results function name.
 * @param function_size A pointer which will be set to the size of the results function name.
 * @return error
 */
extern int tvio_output_request_function(int output, char* id, char** function, int* function_size);

/**
 * @brief Retrieves the MsgPack serialized parameters of a request.
 *
 * @param output The output reference.
 * @param id The request UUID.
 * @param params A pointer which will be set to the parameters of the request.
 * @param params_size A pointer which will be set to size of the request parameters.
 *
 * @return error
 */
extern int tvio_output_request_params(int output, char* id, void** params, int* params_size);

/**
 * @brief Replies to a request and sends the result to all concerned peers.
 *
 * @param output The output reference.
 * @param id The UUID of the request to reply to.
 * @param rparams A pointer to the MsgPack serialized parameters.
 * @param rparams_size Size of the serialized parameters.
 *
 * @return error
 */
extern int tvio_output_reply(int output, char* id, void* rparams, int rparams_size);

/**
 * @brief Executes a ThingiverseIO EMIT.
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
extern int tvio_output_emit(int output, char* function, void* in_params, int in_params_size, void* params, int params_size);

/**
 * @brief Sets the MsgPack serialized value of a property.
 *
 * @param input The input reference.
 * @param property The name of the property.
 * @param value The serialized value.
 * @param value_size The size of the serialized data.
 *
 * @return error
 */
extern int tvio_output_property_set(int output, char* property, void* value, int value_size);

#ifdef __cplusplus
}
#endif
