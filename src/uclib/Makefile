# makefile
VPATH=test:net

.PHONY: MultiTarget
MultiTarget: uclib.a t
	@echo OK

src_t = $(wildcard test/*.c)

t: $(src_t) uclib.a
	$(CC) -o t $^ -Iinc -ldl -rdynamic

src_uclib = $(wildcard net/*.c) $(wildcard tools/*.c)

uclib.a: $(src_uclib)
	$(CC) -c $^ -Iinc
	@-mkdir bin > /dev/null 2>&1
	mv *.o bin
	ar rcs $@ bin/*.o

.PHONY:clean
clean:
	rm -rf *.o
	rm -rf bin
	rm -rf t uclib.a
