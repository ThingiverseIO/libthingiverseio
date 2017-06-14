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

package main

import "C"

import (
	"unsafe"

	"github.com/ThingiverseIO/thingiverseio"
	"github.com/ThingiverseIO/thingiverseio/descriptor"
)

func main() {
}

//export version
func version(maj, min, fix *C.int) {
	*maj = C.int(thingiverseio.CurrentVersion.Major)
	*min = C.int(thingiverseio.CurrentVersion.Minor)
	*fix = C.int(thingiverseio.CurrentVersion.Fix)
}

//export check_descriptor
func check_descriptor(desc *C.char, result **C.char, result_size *C.int) {
	err := descriptor.Check(C.GoString(desc))
	if err != nil {
		*result = C.CString(err.Error())
		*result_size = C.int(len(err.Error()))
	} else {
		*result = C.CString("")
		*result_size = C.int(0)
	}
}

//export error_message
func error_message(err C.int, msg_p **C.char, msg_size *C.int) {
	msg := tvio_err(err).String()
	*msg_p = C.CString(msg)
	*msg_size = C.int(len(msg))
}

func getParams(parameter unsafe.Pointer, parameter_size C.int) (params []byte) {
	params = C.GoBytes(parameter, parameter_size)
	return
}

func boolToIntPtr(b bool, ptr *C.int) {
	if b {
		*ptr = 1
	} else {
		*ptr = 0
	}
}
