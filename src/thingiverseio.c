//	Copyright (c) 2017 Joern Weissenborn
//
//	This file is part of libthingiverseio.
//
//	libthingiverseio is free software: you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	libthingiverseio is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with libthingiverseio.  If not, see <http://www.gnu.org/licenses/>.

#include "tvio.h"

int tvio_version(int* major, int* minor, int* fix) {
	version(major, minor, fix);
}

void tvio_check_descriptor(char* descriptor, char** result, int* result_size) {
	check_descriptor(descriptor, result, result_size);
}

void tvio_error_message(int error, char** msg_p, int* msg_size){
	error_message(error, msg_p, msg_size);
}

int tvio_new_input(char* descriptor){
	return new_input(descriptor);
}

int tvio_input_uuid(int input, char** uuid_p, int* uuid_size) {
	return input_uuid(input, uuid_p, uuid_size);
}

int tvio_input_interface(int input, char** iface_p, int* iface_size) {
	return input_interface(input, iface_p, iface_size);
}

int tvio_input_remove(int input) {
	return input_remove(input);
}

int tvio_input_connected(int input, int* is) {
	return input_connected(input, is);
}

int tvio_input_call(int input, char* function, void* params, int params_size, char** id, int* id_size){
	return input_call(input, function, params, params_size, id, id_size);
}

int tvio_input_call_all(int input, char* function, void* params, int params_size, char** id, int* id_size){
	return input_call_all(input, function, params, params_size, id, id_size);
}

int tvio_input_trigger(int input, char* function, void* params, int params_size) {
	return input_trigger(input, function, params, params_size);
}

int tvio_input_trigger_all(int input, char* function, void* params, int params_size){
	return input_trigger_all(input, function, params, params_size);
}

int tvio_input_call_result_available(int input, char* id, int* ready){
	return input_call_result_available(input, id, ready);
}

int tvio_input_call_result_params(int input, char* id, void** params, int* params_size){
	return input_call_result_params(input, id, params, params_size);
}

int tvio_input_listen_start(int input, char* function){
	return input_listen_start(input, function);
}

int tvio_input_listen_stop(int input, char* function){
	return input_listen_stop(input, function);
}

int tvio_input_listen_result_available(int input, int* is){
	return input_listen_result_available(input, is);
}

int tvio_input_listen_result_id(int input, char** id, int* id_size) {
	return input_listen_result_id(input, id, id_size);
}

int tvio_input_listen_result_function(int input, char** function, int* function_size){
	return input_listen_result_function(input, function, function_size);
}

int tvio_input_listen_result_params(int input, void** params, int* params_size){
	return input_listen_result_params(input, params, params_size);
}

int tvio_input_listen_result_request_params(int input, void** params, int* params_size){
	return input_listen_result_request_params(input, params, params_size);
}

int tvio_input_listen_result_clear(int input) {
	return input_listen_result_clear(input);
}

int tvio_input_call_all_next_result_available(int input, int* is) {
	return tvio_input_call_all_next_result_available(input, is);
}

int tvio_input_call_all_next_result_params(int input, char* id, void** params, int* params_size){
	return input_call_all_next_result_params(input, id , params, params_size);
}

int tvio_input_call_all_next_result_clear(int input, char* id) {
	return input_call_all_next_result_clear(input, id);
}

int tvio_input_call_all_request_clear(int input, char* id) {
	return input_call_all_request_clear(input, id);
}

int tvio_input_property_get(int input, char* property, void** value, int* value_size) {
	return input_property_get(input, property, value, value_size);
}

int tvio_input_property_update(int input, char* property){
	return input_property_update(input, property);
}

int tvio_input_property_update_available(int input, char* property, int* property_size){
	return input_property_update_available(input, property, property_size);
}

int tvio_input_property_update_get(int input, char* property, void** value, int* value_size){
	return input_property_update_get(input, property, value, value_size);
}

int tvio_input_change_start_observe(int input, char* property){
	return input_change_start_observe(input, property);
}

int tvio_input_change_stop_observe(int input, char* property){
	return input_change_stop_observe(input, property);
}

int tvio_input_change_available(int input, int* is){
	return input_change_available(input, is);
}

int tvio_input_change_property(int input, char** property, int* property_size){
	return input_change_property(input, property, property_size);
}

int tvio_input_change_value(int input, void** value, int* value_size){
	return input_change_value(input, value, value_size);
}

int tvio_input_change_clear(int input){
	return input_change_clear(input);
}

int tvio_new_output(char* descriptor){
	return new_output(descriptor);
}

int tvio_output_remove(int output) {
	return output_remove(output);
}

int tvio_output_uuid(int output, char** uuid_p, int* uuid_size) {
	return output_uuid(output, uuid_p, uuid_size);
}

int tvio_output_interface(int output, char** iface_p, int* iface_size) {
	return output_interface(output, iface_p, iface_size);
}

int tvio_output_connected(int output, int* is) {
	return output_connected(output, is);
}

int tvio_output_request_available(int output, int* is){
  return output_request_available(output, is);
}

int tvio_output_request_id(int output, char** id, int* id_size){
	return output_request_id(output, id, id_size);
}

int tvio_output_request_function(int output, char* id, char** function, int* function_size){
	return output_request_function(output, id ,function, function_size);
}

int tvio_output_request_params(int output, char* id, void** params, int* params_size){
	return output_request_params(output, id ,params, params_size);
}

int tvio_output_reply(int output, char* id, void* params, int params_size){
	return output_reply(output, id , params, params_size);
}

int tvio_output_emit(int output, char* function, void* in_params, int in_params_size, void* params, int params_size){
	return output_emit(output, function, in_params, in_params_size, params, params_size);
}

int tvio_output_property_set(int output, char* property, void* value, int value_size){
	return output_property_set(output, property, value, value_size);
}

int main(){}
