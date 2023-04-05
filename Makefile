all: libthingiverseio.so

.PHONY: all test clean doc

test:
	mkdir -p _test
	gcc test/test_shared.c -Iinclude -Lbin -lpthread -ltvio -o _test/test
	./_test/test
	rm -rf _test

libtvio.so:
	go build -a --buildmode="c-shared" -o bin/libtvio.so src/input.go src/output.go src/error.go src/main.go
	mv bin/libtvio.h include/tvio.h

install:
	cp bin/libtvio.so /usr/lib/
	cp bin/libtvio.h /usr/include/
	ldconfig

doc:
	rm -rf doc/html/*
	doxygen

clean:
	rm -rf bin
