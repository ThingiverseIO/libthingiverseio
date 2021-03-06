all: libthingiverseio.so

.PHONY: all test clean doc

test:
	mkdir -p _test
	gcc test/test_shared.c -Iinclude -Lbin -lpthread -lthingiverseio -ltvio -o _test/test
	./_test/test
	rm -rf _test

libtvio.so:
	go get github.com/ThingiverseIO/thingiverseio
	go build -a --buildmode="c-shared" -o bin/libtvio.so src/input.go src/output.go src/error.go src/main.go
	mv bin/libtvio.h include/tvio.h

tvio.o: libtvio.so
	mkdir -p build
	gcc -c src/thingiverseio.c -L./bin/ -I./include/ -fPIC -lpthread -ltvio -o build/tvio.o

libthingiverseio.so: tvio.o
	gcc -shared -fPIC build/tvio.o -o bin/libthingiverseio.so -lpthread -L./bin -ltvio

install:
	cp bin/lib* /usr/lib/
	cp include/* /usr/include/
	ldconfig

doc:
	rm -rf doc/html/*
	doxygen

clean:
	rm -rf build
	rm -rf bin
	rm -f include/tvio.h
