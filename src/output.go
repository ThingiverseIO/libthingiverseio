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

package main

import "C"

import (
	"sync"
	"unsafe"

	"github.com/ThingiverseIO/thingiverseio/config"
	"github.com/ThingiverseIO/thingiverseio/core"
	"github.com/ThingiverseIO/thingiverseio/descriptor"
	"github.com/ThingiverseIO/thingiverseio/message"
	"github.com/ThingiverseIO/uuid"
)

type output struct {
	m             *sync.RWMutex
	c             core.OutputCore
	requests      *message.RequestCollector
	request_cache map[uuid.UUID]*message.Request
}

func newOutput(desc string) (o *output, err C.int) {
	d, derr := descriptor.Parse(desc)
	if derr != nil {
		err = ERR_INVALID_DESCRIPTOR.asInt()
		return
	}
	cfg := config.Configure()
	tracker, provider := core.DefaultBackends()
	c, nerr := core.NewOutputCore(d, cfg, tracker, provider...)
	if nerr != nil {
		err = ERR_NETWORK.asInt()
		return
	}
	o = &output{
		m:             &sync.RWMutex{},
		c:             c,
		requests:      message.NewRequestCollector(),
		request_cache: map[uuid.UUID]*message.Request{},
	}
	o.requests.AddStream(c.RequestStream())
	o.c.Run()
	return
}

type outputRegister struct {
	n        int
	m        *sync.RWMutex
	register map[C.int]*output
}

func (o *outputRegister) next() (n int) {
	n = o.n
	o.n++
	return
}

func (o *outputRegister) new(desc string) (idOrErr C.int) {
	o.m.Lock()
	defer o.m.Unlock()
	out, idOrErr := newOutput(desc)
	if idOrErr != NO_ERR.asInt() {
		return
	}
	idOrErr = C.int(o.next())
	o.register[idOrErr] = out
	return
}

func (o *outputRegister) connected(id C.int) (is bool, err C.int) {
	o.m.Lock()
	defer o.m.Unlock()
	out, ok := o.register[id]
	if !ok {
		err = ERR_INVALID_OUTPUT.asInt()
		return
	}
	is = out.c.Connected()
	return
}

func (o *outputRegister) iface(id C.int) (iface string, err C.int) {
	o.m.Lock()
	defer o.m.Unlock()
	out, ok := o.register[id]
	if !ok {
		err = ERR_INVALID_OUTPUT.asInt()
		return
	}
	iface = out.c.Interface()
	return
}

func (o *outputRegister) uuid(id C.int) (iID uuid.UUID, err C.int) {
	o.m.Lock()
	defer o.m.Unlock()
	out, ok := o.register[id]
	if !ok {
		err = ERR_INVALID_OUTPUT.asInt()
		return
	}
	iID = out.c.UUID()
	return
}

func (o *outputRegister) remove(id C.int) (err C.int) {
	o.m.Lock()
	defer o.m.Unlock()
	out, ok := o.register[id]
	if !ok {
		err = ERR_INVALID_OUTPUT.asInt()
		return
	}
	out.c.Shutdown()
	delete(o.register, id)
	return
}

func (o *outputRegister) requestAvailable(id C.int) (is bool, err C.int) {

	o.m.Lock()
	defer o.m.Unlock()
	out, ok := o.register[id]
	if !ok {
		err = ERR_INVALID_OUTPUT.asInt()
		return
	}

	is = !out.requests.Empty()
	return

}

func (o *outputRegister) nextRequestUUID(id C.int) (reqID uuid.UUID, err C.int) {

	o.m.RLock()
	defer o.m.RUnlock()
	out, ok := o.register[id]
	if !ok {
		err = ERR_INVALID_OUTPUT.asInt()
		return
	}

	if out.requests.Empty() {
		err = ERR_NO_REQUEST_AVAILABLE.asInt()
		return
	}
	out.m.Lock()
	defer out.m.Unlock()
	req := out.requests.Get()
	reqID = req.UUID
	out.request_cache[reqID] = req
	return
}

func (o *outputRegister) nextRequestFunction(id C.int, reqID uuid.UUID) (function string, err C.int) {
	o.m.RLock()
	defer o.m.RUnlock()
	out, ok := o.register[id]
	if !ok {
		err = ERR_INVALID_OUTPUT.asInt()
		return
	}
	out.m.RLock()
	defer out.m.RUnlock()
	req, ok := out.request_cache[reqID]
	if !ok {
		err = ERR_INVALID_REQUEST_ID.asInt()
		return
	}
	function = req.Function
	return
}

func (o *outputRegister) nextRequestParameter(id C.int, reqID uuid.UUID) (params []byte, err C.int) {
	o.m.RLock()
	defer o.m.RUnlock()
	out, ok := o.register[id]
	if !ok {
		err = ERR_INVALID_OUTPUT.asInt()
		return
	}
	out.m.RLock()
	defer out.m.RUnlock()
	req, ok := out.request_cache[reqID]
	if !ok {
		err = ERR_INVALID_REQUEST_ID.asInt()
		return
	}
	params = req.Parameter()
	return
}

func (o *outputRegister) reply(id C.int, reqID uuid.UUID, params []byte) (err C.int) {
	o.m.RLock()
	defer o.m.RUnlock()
	out, ok := o.register[id]
	if !ok {
		err = ERR_INVALID_OUTPUT.asInt()
		return
	}
	out.m.Lock()
	defer out.m.Unlock()
	req, ok := out.request_cache[reqID]
	if !ok {
		err = ERR_INVALID_REQUEST_ID.asInt()
		return
	}
	out.c.Reply(req, params)
	delete(out.request_cache, reqID)
	return
}

func (o *outputRegister) emit(id C.int, function string, in_params []byte, out_params []byte) (err C.int) {
	o.m.RLock()
	defer o.m.RUnlock()
	out, ok := o.register[id]
	if !ok {
		err = ERR_INVALID_OUTPUT.asInt()
		return
	}
	ferr := out.c.Emit(function, in_params, out_params)
	if ferr != nil {
		err = ERR_INVALID_FUNCTION.asInt()
		return
	}
	return
}

func (o *outputRegister) setProperty(id C.int, property string, value []byte) (err C.int) {
	o.m.RLock()
	defer o.m.RUnlock()
	out, ok := o.register[id]
	if !ok {
		err = ERR_INVALID_OUTPUT.asInt()
		return
	}
	perr := out.c.SetProperty(property, value)
	if perr != nil {
		err = ERR_INVALID_PROPERTY.asInt()
		return
	}
	return
}

var outputs = outputRegister{
	m:        &sync.RWMutex{},
	register: map[C.int]*output{},
}

//export new_output
func new_output(descriptor *C.char) C.int {
	d := C.GoString(descriptor)
	return outputs.new(d)
}

//export output_remove
func output_remove(o C.int) C.int {
	return outputs.remove(o)
}

//export output_connected
func output_connected(o C.int, is_p *C.int) C.int {
	is, err := outputs.connected(o)
	boolToIntPtr(is, is_p)
	return err
}

//export output_uuid
func output_uuid(o C.int, uuid_p **C.char, uuid_size *C.int) C.int {
	uuid, err := outputs.uuid(o)
	if err == NO_ERR.asInt() {
		*uuid_p = C.CString(string(uuid))
		*uuid_size = C.int(len(uuid))
	}
	return err
}

//export output_interface
func output_interface(o C.int, iface_p **C.char, iface_size *C.int) C.int {

	iface, err := outputs.iface(o)
	if err == 0 {
		*iface_p = C.CString(iface)
		*iface_size = C.int(len(iface))
	}
	return err
}

//export output_request_id
func output_request_id(o C.int, req_id **C.char, req_id_size *C.int) C.int {
	reqID, err := outputs.nextRequestUUID(o)
	if err == NO_ERR.asInt() {
		*req_id = C.CString(string(reqID))
		*req_id_size = C.int(len(reqID))
	}
	return err
}

//export output_request_available
func output_request_available(o C.int, is_p *C.int) C.int {
	is, err := outputs.requestAvailable(o)
	boolToIntPtr(is, is_p)
	return err
}

//export output_request_function
func output_request_function(o C.int, req_id *C.char, function **C.char, function_size *C.int) C.int {
	reqID := uuid.UUID(C.GoString(req_id))
	fun, err := outputs.nextRequestFunction(o, reqID)
	if err == NO_ERR.asInt() {
		*function = C.CString(fun)
		*function_size = C.int(len(fun))
	}
	return err
}

//export output_request_params
func output_request_params(o C.int, req_id *C.char, params *unsafe.Pointer, params_size *C.int) C.int {
	reqID := uuid.UUID(C.GoString(req_id))
	p, err := outputs.nextRequestParameter(o, reqID)
	if err == NO_ERR.asInt() {
		*params = unsafe.Pointer(C.CBytes(p))
		*params_size = C.int(len(p))
	}
	return err
}

//export output_reply
func output_reply(o C.int, req_id *C.char, params unsafe.Pointer, params_size C.int) C.int {
	reqID := uuid.UUID(C.GoString(req_id))
	parameter := getParams(params, params_size)
	err := outputs.reply(o, reqID, parameter)
	return err
}

//export output_emit
func output_emit(o C.int, function *C.char, in_params unsafe.Pointer, in_params_size C.int, out_params unsafe.Pointer, out_params_size C.int) C.int {
	fun := C.GoString(function)
	inParams := getParams(in_params, in_params_size)
	outParams := getParams(out_params, out_params_size)
	err := outputs.emit(o, fun, inParams, outParams)
	return err
}

//export output_property_set
func output_property_set(o C.int, property *C.char, value_p unsafe.Pointer, value_size C.int) C.int {
	prop := C.GoString(property)
	value := getParams(value_p, value_size)
	err := outputs.setProperty(o, prop, value)
	return err
}
