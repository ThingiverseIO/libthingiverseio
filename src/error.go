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

//go:generate stringer -type=tvio_err

type tvio_err C.int

const (
	NO_ERR tvio_err = iota
	ERR_NETWORK
	ERR_INVALID_DESCRIPTOR
	ERR_INVALID_INPUT
	ERR_INVALID_OUTPUT
	ERR_INVALID_RESULT_ID
	ERR_INVALID_REQUEST_ID
	ERR_NO_RESULT_AVAILABLE
	ERR_NO_REQUEST_AVAILABLE
	ERR_RESULT_NOT_ARRIVED
	ERR_INVALID_FUNCTION
	ERR_INVALID_PROPERTY
	ERR_NO_UPDATE
)

func (err tvio_err) String() (s string) {
	switch err {
	case ERR_NETWORK:
		s = "Network Fail"
	case ERR_INVALID_DESCRIPTOR:
		s = "Invalid Descriptor"
	case ERR_INVALID_INPUT:
		s = "Invalid Input"
	case ERR_INVALID_OUTPUT:
		s = "Invalid Output"
	case ERR_INVALID_RESULT_ID:
		s = "Invalid Result ID"
	case ERR_INVALID_REQUEST_ID:
		s = "Invalid Request ID"
	case ERR_NO_RESULT_AVAILABLE:
		s = "Network Fail"
	case ERR_RESULT_NOT_ARRIVED:
		s = "Result Not Arrived"
	case ERR_INVALID_FUNCTION:
		s = "Invalid Function"
	case ERR_INVALID_PROPERTY:
		s = "Invalid Property"
	case ERR_NO_UPDATE:
		s = "No Property Update Available"
	}
	return
}

func (err tvio_err) asInt() C.int{
	return C.int(err)
}
