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
	"github.com/ThingiverseIO/thingiverseio/uuid"
	"github.com/joernweissenborn/eventual2go"
)

type propertyChange struct {
	name  string
	value []byte
}

func toPropertyChange(name string) eventual2go.Transformer {
	return func(d eventual2go.Data) eventual2go.Data {
		return propertyChange{
			name:  name,
			value: d.([]byte),
		}
	}
}

type input struct {
	m               *sync.RWMutex
	c               core.InputCore
	results         map[uuid.UUID]*message.ResultFuture
	callall         map[uuid.UUID]*message.ResultCollector
	listen          *message.ResultCollector
	propertyChanges *eventual2go.Collector
	propertyUpdates map[string]*eventual2go.Future
}

func newInput(desc string) (i *input, err C.int) {
	d, derr := descriptor.Parse(desc)
	if derr != nil {
		err = ERR_INVALID_DESCRIPTOR.asInt()
		return
	}
	cfg := config.Configure()
	tracker, provider := core.DefaultBackends()
	c, nerr := core.NewInputCore(d, cfg, tracker, provider...)
	if nerr != nil {
		err = ERR_NETWORK.asInt()
		return
	}
	i = &input{
		m:               &sync.RWMutex{},
		c:               c,
		results:         map[uuid.UUID]*message.ResultFuture{},
		callall:         map[uuid.UUID]*message.ResultCollector{},
		listen:          message.NewResultCollector(),
		propertyChanges: eventual2go.NewCollector(),
		propertyUpdates: map[string]*eventual2go.Future{},
	}
	for _, p := range c.Properties() {
		o, _ := c.GetProperty(p)
		i.propertyChanges.AddStream(o.Stream().Transform(toPropertyChange(p)))
	}
	i.listen.AddStream(c.ListenStream())
	i.c.Run()
	return
}

type inputRegister struct {
	n        int
	m        *sync.RWMutex
	register map[C.int]*input
}

func (i *inputRegister) next() (n int) {
	n = i.n
	i.n++
	return
}

func (i *inputRegister) new(desc string) (idOrErr C.int) {
	i.m.Lock()
	defer i.m.Unlock()
	in, idOrErr := newInput(desc)
	if idOrErr != NO_ERR.asInt() {
		return
	}
	idOrErr = C.int(i.next())
	i.register[idOrErr] = in
	return
}

func (i *inputRegister) connected(id C.int) (is bool, err C.int) {
	i.m.RLock()
	defer i.m.RUnlock()
	in, ok := i.register[id]
	if !ok {
		err = ERR_INVALID_INPUT.asInt()
		return
	}
	is = in.c.Connected()
	return
}

func (i *inputRegister) iface(id C.int) (iface string, err C.int) {
	i.m.RLock()
	defer i.m.RUnlock()
	in, ok := i.register[id]
	if !ok {
		err = ERR_INVALID_INPUT.asInt()
		return
	}
	iface = in.c.Interface()
	return
}

func (i *inputRegister) uuid(id C.int) (iID uuid.UUID, err C.int) {
	i.m.RLock()
	defer i.m.RUnlock()
	in, ok := i.register[id]
	if !ok {
		err = ERR_INVALID_INPUT.asInt()
		return
	}
	iID = in.c.UUID()
	return
}

func (i *inputRegister) remove(id C.int) (err C.int) {
	i.m.Lock()
	defer i.m.Unlock()
	in, ok := i.register[id]
	if !ok {
		err = ERR_INVALID_INPUT.asInt()
		return
	}
	in.c.Shutdown()
	delete(i.register, id)
	return
}

func (i *inputRegister) startListen(id C.int, function string) (err C.int) {
	i.m.RLock()
	defer i.m.RUnlock()
	in, ok := i.register[id]
	if !ok {
		err = ERR_INVALID_INPUT.asInt()
		return
	}
	ferr := in.c.StartListen(function)
	if ferr != nil {
		err = ERR_INVALID_FUNCTION.asInt()
	}
	return
}

func (i *inputRegister) stopListen(id C.int, function string) (err C.int) {
	i.m.RLock()
	defer i.m.RUnlock()
	in, ok := i.register[id]
	if !ok {
		err = ERR_INVALID_INPUT.asInt()
		return
	}
	ferr := in.c.StopListen(function)
	if ferr != nil {
		err = ERR_INVALID_FUNCTION.asInt()
	}
	return
}

func (i *inputRegister) startObserve(id C.int, property string) (err C.int) {
	i.m.RLock()
	defer i.m.RUnlock()
	in, ok := i.register[id]
	if !ok {
		err = ERR_INVALID_INPUT.asInt()
		return
	}
	ferr := in.c.StartObservation(property)
	if ferr != nil {
		err = ERR_INVALID_PROPERTY.asInt()
	}
	return
}

func (i *inputRegister) stopObserve(id C.int, property string) (err C.int) {
	i.m.RLock()
	defer i.m.RUnlock()
	in, ok := i.register[id]
	if !ok {
		err = ERR_INVALID_INPUT.asInt()
		return
	}
	ferr := in.c.StopObservation(property)
	if ferr != nil {
		err = ERR_INVALID_PROPERTY.asInt()
	}
	return
}

func (i *inputRegister) propertyChanged(id C.int) (is bool, err C.int) {
	i.m.RLock()
	defer i.m.RUnlock()
	in, ok := i.register[id]
	if !ok {
		err = ERR_INVALID_INPUT.asInt()
		return
	}
	in.m.Lock()
	defer in.m.Unlock()
	is = !in.propertyChanges.Empty()
	return
}

func (i *inputRegister) nextChangeProperty(id C.int) (property string, err C.int) {
	i.m.RLock()
	defer i.m.RUnlock()
	in, ok := i.register[id]
	if !ok {
		err = ERR_INVALID_INPUT.asInt()
		return
	}
	in.m.Lock()
	defer in.m.Unlock()
	if in.propertyChanges.Empty() {
		err = ERR_NO_UPDATE.asInt()
	}
	property = in.propertyChanges.Preview().(propertyChange).name
	return
}

func (i *inputRegister) nextChangeValue(id C.int) (value []byte, err C.int) {
	i.m.RLock()
	defer i.m.RUnlock()
	in, ok := i.register[id]
	if !ok {
		err = ERR_INVALID_INPUT.asInt()
		return
	}
	in.m.Lock()
	defer in.m.Unlock()
	if in.propertyChanges.Empty() {
		err = ERR_NO_UPDATE.asInt()
	}
	value = in.propertyChanges.Preview().(propertyChange).value
	return
}

func (i *inputRegister) clearNextChange(id C.int) (err C.int) {
	i.m.RLock()
	defer i.m.RUnlock()
	in, ok := i.register[id]
	if !ok {
		err = ERR_INVALID_INPUT.asInt()
		return
	}
	in.m.Lock()
	defer in.m.Unlock()
	if in.propertyChanges.Empty() {
		err = ERR_NO_UPDATE.asInt()
	}
	in.propertyChanges.Get()
	return
}

func (i *inputRegister) updateProperty(id C.int, property string) (err C.int) {
	i.m.RLock()
	defer i.m.RUnlock()
	in, ok := i.register[id]
	if !ok {
		err = ERR_INVALID_INPUT.asInt()
		return
	}
	in.m.Lock()
	defer in.m.Unlock()
	if f, ok := in.propertyUpdates[property]; ok {
		if !f.Completed() {
			return
		}
	}
	p, ferr := in.c.UpdateProperty(property)
	if ferr != nil {
		err = ERR_INVALID_PROPERTY.asInt()
	}
	in.propertyUpdates[property] = p
	return
}

func (i *inputRegister) propertyUpdateAvailable(id C.int, property string) (is bool, err C.int) {
	i.m.RLock()
	defer i.m.RUnlock()
	in, ok := i.register[id]
	if !ok {
		err = ERR_INVALID_INPUT.asInt()
		return
	}

	in.m.RLock()
	defer in.m.RUnlock()
	p, is := in.propertyUpdates[property]
	if !is {
		return
	}
	is = p.Completed()
	return
}

func (i *inputRegister) getPropertyUpdate(id C.int, property string) (value []byte, err C.int) {
	i.m.RLock()
	defer i.m.RUnlock()
	in, ok := i.register[id]
	if !ok {
		err = ERR_INVALID_INPUT.asInt()
		return
	}
	in.m.Lock()
	defer in.m.Unlock()
	p, is := in.propertyUpdates[property]
	if !is {
		err = ERR_NO_UPDATE.asInt()
		return
	}
	if !p.Completed() {
		err = ERR_NO_UPDATE.asInt()
		return
	}
	value = p.Result().([]byte)
	delete(in.propertyUpdates, property)
	return
}

func (i *inputRegister) getProperty(id C.int, property string) (value []byte, err C.int) {
	i.m.RLock()
	defer i.m.RUnlock()
	in, ok := i.register[id]
	if !ok {
		err = ERR_INVALID_INPUT.asInt()
		return
	}
	p, ferr := in.c.GetProperty(property)
	if ferr != nil {
		err = ERR_INVALID_PROPERTY.asInt()
	}
	value = p.Value().([]byte)
	return
}

func (i *inputRegister) call(id C.int, function string, params []byte) (resID uuid.UUID, err C.int) {
	i.m.RLock()
	defer i.m.RUnlock()
	in, ok := i.register[id]
	if !ok {
		err = ERR_INVALID_INPUT.asInt()
		return
	}
	res, _, resID, ferr := in.c.Request(function, message.CALL, params)
	if ferr != nil {
		err = ERR_INVALID_FUNCTION.asInt()
		return
	}
	in.m.Lock()
	defer in.m.Unlock()
	in.results[resID] = res
	return
}

func (i *inputRegister) callAll(id C.int, function string, params []byte) (resID uuid.UUID, err C.int) {
	i.m.RLock()
	defer i.m.RUnlock()
	in, ok := i.register[id]
	if !ok {
		err = ERR_INVALID_INPUT.asInt()
		return
	}
	_, res, resID, ferr := in.c.Request(function, message.CALLALL, params)
	if ferr != nil {
		err = ERR_INVALID_FUNCTION.asInt()
		return
	}
	in.m.Lock()
	defer in.m.Unlock()
	in.callall[resID] = message.NewResultCollector()
	in.callall[resID].AddStream(res)
	res.CloseOnFuture(in.callall[resID].Removed())
	return
}

func (i *inputRegister) trigger(id C.int, function string, params []byte) (err C.int) {
	i.m.RLock()
	defer i.m.RUnlock()
	in, ok := i.register[id]
	if !ok {
		err = ERR_INVALID_INPUT.asInt()
		return
	}
	_, _, _, ferr := in.c.Request(function, message.TRIGGER, params)
	if ferr != nil {
		err = ERR_INVALID_FUNCTION.asInt()
		return
	}
	return
}

func (i *inputRegister) triggerAll(id C.int, function string, params []byte) (err C.int) {
	i.m.RLock()
	defer i.m.RUnlock()
	in, ok := i.register[id]
	if !ok {
		err = ERR_INVALID_INPUT.asInt()
		return
	}
	_, _, _, ferr := in.c.Request(function, message.TRIGGERALL, params)
	if ferr != nil {
		err = ERR_INVALID_FUNCTION.asInt()
		return
	}
	return
}

func (i *inputRegister) resultReady(id C.int, resID uuid.UUID) (ready bool, err C.int) {

	i.m.RLock()
	defer i.m.RUnlock()
	in, ok := i.register[id]
	if !ok {
		err = ERR_INVALID_INPUT.asInt()
		return
	}

	in.m.RLock()
	defer in.m.RUnlock()
	req, ok := in.results[resID]
	if !ok {
		err = ERR_INVALID_RESULT_ID.asInt()
		return
	}
	ready = req.Completed()
	return

}

func (i *inputRegister) resultParameter(id C.int, resID uuid.UUID) (params []byte, err C.int) {

	i.m.RLock()
	defer i.m.RUnlock()
	in, ok := i.register[id]
	if !ok {
		err = ERR_INVALID_INPUT.asInt()
		return
	}

	in.m.Lock()
	defer in.m.Unlock()
	req, ok := in.results[resID]
	if !ok {
		err = ERR_INVALID_RESULT_ID.asInt()
		return
	}
	if !req.Completed() {
		err = ERR_RESULT_NOT_ARRIVED.asInt()
		return
	}
	params = req.Result().Parameter()
	delete(in.results, resID)
	return

}

func (i *inputRegister) listenResultAvailable(id C.int) (is bool, err C.int) {

	i.m.RLock()
	defer i.m.RUnlock()
	in, ok := i.register[id]
	if !ok {
		err = ERR_INVALID_INPUT.asInt()
		return
	}

	is = !in.listen.Empty()
	return

}

func (i *inputRegister) nextListenResultUUID(id C.int) (resID uuid.UUID, err C.int) {

	i.m.RLock()
	defer i.m.RUnlock()
	in, ok := i.register[id]
	if !ok {
		err = ERR_INVALID_INPUT.asInt()
		return
	}

	if in.listen.Empty() {
		err = ERR_NO_RESULT_AVAILABLE.asInt()
		return
	}
	resID = in.listen.Preview().Request.UUID
	return
}

func (i *inputRegister) nextListenResultFunction(id C.int) (function string, err C.int) {
	i.m.RLock()
	defer i.m.RUnlock()
	in, ok := i.register[id]
	if !ok {
		err = ERR_INVALID_INPUT.asInt()
		return
	}
	if in.listen.Empty() {
		err = ERR_NO_RESULT_AVAILABLE.asInt()
		return
	}
	function = in.listen.Preview().Request.Function
	return
}

func (i *inputRegister) nextListenResultRequestParameter(id C.int) (params []byte, err C.int) {
	i.m.RLock()
	defer i.m.RUnlock()
	in, ok := i.register[id]
	if !ok {
		err = ERR_INVALID_INPUT.asInt()
		return
	}
	if in.listen.Empty() {
		err = ERR_NO_RESULT_AVAILABLE.asInt()
		return
	}
	params = in.listen.Preview().Request.Parameter()
	return
}

func (i *inputRegister) nextListenResultParameter(id C.int) (params []byte, err C.int) {
	i.m.RLock()
	defer i.m.RUnlock()
	in, ok := i.register[id]
	if !ok {
		err = ERR_INVALID_INPUT.asInt()
		return
	}
	if in.listen.Empty() {
		err = ERR_NO_RESULT_AVAILABLE.asInt()
		return
	}
	params = in.listen.Preview().Parameter()
	return
}

func (i *inputRegister) clearListenResult(id C.int) (err C.int) {
	i.m.RLock()
	defer i.m.RUnlock()
	in, ok := i.register[id]
	if !ok {
		err = ERR_INVALID_INPUT.asInt()
		return
	}
	if in.listen.Empty() {
		err = ERR_NO_RESULT_AVAILABLE.asInt()
		return
	}
	in.listen.Get()
	return
}

func (i *inputRegister) callAllResultAvailable(id C.int, resID uuid.UUID) (is bool, err C.int) {

	i.m.RLock()
	defer i.m.RUnlock()
	in, ok := i.register[id]
	if !ok {
		err = ERR_INVALID_INPUT.asInt()
		return
	}
	in.m.RLock()
	defer in.m.RLock()

	res, ok := in.callall[resID]
	if !ok {
		err = ERR_INVALID_RESULT_ID.asInt()
		return
	}
	is = !res.Empty()
	return

}

func (i *inputRegister) callAllResultParameter(id C.int, resID uuid.UUID) (p []byte, err C.int) {

	i.m.RLock()
	defer i.m.RUnlock()
	in, ok := i.register[id]
	if !ok {
		err = ERR_INVALID_INPUT.asInt()
		return
	}
	in.m.RLock()
	defer in.m.RLock()

	res, ok := in.callall[resID]
	if !ok {
		err = ERR_INVALID_RESULT_ID.asInt()
		return
	}
	if res.Empty() {
		err = ERR_NO_RESULT_AVAILABLE.asInt()
	}
	p = res.Preview().Parameter()

	return

}

func (i *inputRegister) clearCallAllResult(id C.int, resID uuid.UUID) (err C.int) {

	i.m.RLock()
	defer i.m.RUnlock()
	in, ok := i.register[id]
	if !ok {
		err = ERR_INVALID_INPUT.asInt()
		return
	}
	in.m.RLock()
	defer in.m.RLock()

	res, ok := in.callall[resID]
	if !ok {
		err = ERR_INVALID_RESULT_ID.asInt()
		return
	}
	res.Get()
	return

}
func (i *inputRegister) clearCallAllRequest(id C.int, resID uuid.UUID) (err C.int) {

	i.m.RLock()
	defer i.m.RUnlock()
	in, ok := i.register[id]
	if !ok {
		err = ERR_INVALID_INPUT.asInt()
		return
	}
	in.m.Lock()
	defer in.m.Lock()

	res, ok := in.callall[resID]
	if !ok {
		err = ERR_INVALID_RESULT_ID.asInt()
		return
	}
	res.Remove()
	delete(in.callall, resID)
	return

}

var inputs = inputRegister{
	m:        &sync.RWMutex{},
	register: map[C.int]*input{},
}

//export new_input
func new_input(descriptor *C.char) (i C.int) {
	d := C.GoString(descriptor)
	return inputs.new(d)
}

//export input_remove
func input_remove(i C.int) C.int {
	return inputs.remove(i)
}

//export input_connected
func input_connected(i C.int, is_p *C.int) C.int {
	is, err := inputs.connected(i)
	boolToIntPtr(is, is_p)
	return err
}

//export input_uuid
func input_uuid(i C.int, uuid_p **C.char, uuid_size *C.int) C.int {
	uuid, err := inputs.uuid(i)
	if err == 0 {
		*uuid_p = C.CString(string(uuid))
		*uuid_size = C.int(len(uuid))
	}
	return err
}

//export input_interface
func input_interface(i C.int, iface_p **C.char, iface_size *C.int) C.int {
	iface, err := inputs.iface(i)
	if err == 0 {
		*iface_p = C.CString(iface)
		*iface_size = C.int(len(iface))
	}
	return err
}

//export input_call
func input_call(i C.int, function *C.char, params unsafe.Pointer, params_size C.int, request_id **C.char, request_id_size *C.int) C.int {
	fun := C.GoString(function)
	paramter := getParams(params, params_size)
	res_id, err := inputs.call(i, fun, paramter)
	if err == NO_ERR.asInt() {
		*request_id = C.CString(string(res_id))
		*request_id_size = C.int(len(res_id))
	}
	return err
}

//export input_call_all
func input_call_all(i C.int, function *C.char, params unsafe.Pointer, params_size C.int, request_id **C.char, request_id_size *C.int) C.int {
	fun := C.GoString(function)
	paramter := getParams(params, params_size)
	res_id, err := inputs.callAll(i, fun, paramter)
	if err == NO_ERR.asInt() {
		*request_id = C.CString(string(res_id))
		*request_id_size = C.int(len(res_id))
	}
	return err
}

//export input_trigger
func input_trigger(i C.int, function *C.char, params unsafe.Pointer, params_size C.int) C.int {
	fun := C.GoString(function)
	paramter := getParams(params, params_size)
	err := inputs.trigger(i, fun, paramter)
	return err
}

//export input_trigger_all
func input_trigger_all(i C.int, function *C.char, params unsafe.Pointer, params_size C.int) C.int {
	fun := C.GoString(function)
	paramter := getParams(params, params_size)
	err := inputs.triggerAll(i, fun, paramter)
	return err
}

//export input_call_result_available
func input_call_result_available(i C.int, res_id *C.char, ready *C.int) C.int {
	resID := uuid.UUID(C.GoString(res_id))
	r, err := inputs.resultReady(i, resID)
	boolToIntPtr(r, ready)
	return err
}

//export input_call_result_params
func input_call_result_params(i C.int, res_id *C.char, params *unsafe.Pointer, params_size *C.int) C.int {
	resID := uuid.UUID(C.GoString(res_id))
	p, err := inputs.resultParameter(i, resID)
	if err == NO_ERR.asInt() {
		*params = unsafe.Pointer(C.CBytes(p))
		*params_size = C.int(len(p))
	}
	return err
}

//export input_listen_start
func input_listen_start(i C.int, function *C.char) C.int {
	return inputs.startListen(i, C.GoString(function))
}

//export input_listen_stop
func input_listen_stop(i C.int, function *C.char) C.int {
	return inputs.stopListen(i, C.GoString(function))
}

//export input_listen_result_available
func input_listen_result_available(i C.int, is_p *C.int) C.int {
	is, err := inputs.listenResultAvailable(i)
	boolToIntPtr(is, is_p)
	return err
}

//export input_listen_result_id
func input_listen_result_id(i C.int, result_id **C.char, result_id_size *C.int) C.int {
	resID, err := inputs.nextListenResultUUID(i)
	if err == NO_ERR.asInt() {
		*result_id = C.CString(string(resID))
		*result_id_size = C.int(len(resID))
	}
	return err
}

//export input_listen_result_function
func input_listen_result_function(i C.int, function **C.char, function_size *C.int) C.int {
	fun, err := inputs.nextListenResultFunction(i)
	if err == NO_ERR.asInt() {
		*function = C.CString(fun)
		*function_size = C.int(len(fun))
	}
	return err
}

//export input_listen_result_request_params
func input_listen_result_request_params(i C.int, params *unsafe.Pointer, params_size *C.int) C.int {
	p, err := inputs.nextListenResultRequestParameter(i)
	if err == NO_ERR.asInt() {
		*params = unsafe.Pointer(C.CBytes(p))
		*params_size = C.int(len(p))
	}
	return err
}

//export input_listen_result_params
func input_listen_result_params(i C.int, params *unsafe.Pointer, params_size *C.int) C.int {
	p, err := inputs.nextListenResultParameter(i)
	if err == NO_ERR.asInt() {
		*params = unsafe.Pointer(C.CBytes(p))
		*params_size = C.int(len(p))
	}
	return err
}

//export input_listen_result_clear
func input_listen_result_clear(i C.int) C.int {
	err := inputs.clearListenResult(i)
	return err
}

//export input_call_all_next_result_available
func input_call_all_next_result_available(i C.int, res_id *C.char, is_p *C.int) C.int {
	resID := uuid.UUID(C.GoString(res_id))
	is, err := inputs.callAllResultAvailable(i, resID)
	boolToIntPtr(is, is_p)
	return err
}

//export input_call_all_next_result_params
func input_call_all_next_result_params(i C.int, res_id *C.char, params *unsafe.Pointer, params_size *C.int) C.int {
	resID := uuid.UUID(C.GoString(res_id))
	p, err := inputs.callAllResultParameter(i, resID)
	if err == NO_ERR.asInt() {
		*params = unsafe.Pointer(C.CBytes(p))
		*params_size = C.int(len(p))
	}
	return err
}

//export input_call_all_next_result_clear
func input_call_all_next_result_clear(i C.int, res_id *C.char) C.int {
	resID := uuid.UUID(C.GoString(res_id))
	err := inputs.clearCallAllResult(i, resID)
	return err
}

//export input_call_all_request_clear
func input_call_all_request_clear(i C.int, res_id *C.char) C.int {
	resID := uuid.UUID(C.GoString(res_id))
	err := inputs.clearCallAllRequest(i, resID)
	return err
}

//export input_property_get
func input_property_get(i C.int, property *C.char, value_p *unsafe.Pointer, value_size *C.int) C.int {
	prop := C.GoString(property)
	p, err := inputs.getProperty(i, prop)
	if err == NO_ERR.asInt() {
		*value_p = unsafe.Pointer(C.CBytes(p))
		*value_size = C.int(len(p))
	}
	return err
}

//export input_property_update
func input_property_update(i C.int, property *C.char) C.int {
	prop := C.GoString(property)
	err := inputs.updateProperty(i, prop)
	return err
}

//export input_property_update_available
func input_property_update_available(i C.int, property *C.char, is_p *C.int) C.int {
	prop := C.GoString(property)
	is, err := inputs.propertyUpdateAvailable(i, prop)
	boolToIntPtr(is, is_p)
	return err
}

//export input_property_update_get
func input_property_update_get(i C.int, property *C.char, value_p *unsafe.Pointer, value_size *C.int) C.int {
	prop := C.GoString(property)
	p, err := inputs.getPropertyUpdate(i, prop)
	if err == NO_ERR.asInt() {
		*value_p = unsafe.Pointer(C.CBytes(p))
		*value_size = C.int(len(p))
	}
	return err
}

//export input_change_start_observe
func input_change_start_observe(i C.int, property *C.char) C.int {
	return inputs.startObserve(i, C.GoString(property))
}

//export input_change_stop_observe
func input_change_stop_observe(i C.int, property *C.char) C.int {
	return inputs.stopObserve(i, C.GoString(property))
}

//export input_change_available
func input_change_available(i C.int, is_p *C.int) C.int {
	is, err := inputs.propertyChanged(i)
	boolToIntPtr(is, is_p)
	return err
}

//export input_change_property
func input_change_property(i C.int, property **C.char, property_size *C.int) C.int {
	p, err := inputs.nextChangeProperty(i)
	if err == NO_ERR.asInt() {
		*property = C.CString(p)
		*property_size = C.int(len(p))
	}
	return err
}

//export input_change_value
func input_change_value(i C.int, value_p *unsafe.Pointer, value_size *C.int) C.int {
	p, err := inputs.nextChangeValue(i)
	if err == NO_ERR.asInt() {
		*value_p = unsafe.Pointer(C.CBytes(p))
		*value_size = C.int(len(p))
	}
	return err
}

//export input_change_clear
func input_change_clear(i C.int) C.int {
	err := inputs.clearNextChange(i)
	return err
}
