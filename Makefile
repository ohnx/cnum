INCLUDES=-Iinclude/ -Ilib/include/
CFLAGS+=$(INCLUDES) -Wall -Werror -ansi -pedantic

OBJ=objs/data.o objs/main.o objs/perceptron.o
LIBS=lib/libcblas.a
OUTPUT=cnum

.PHONY: debug
default: $(OUTPUT)
debug: CFLAGS += -g -O0 -D__DEBUG
debug: $(OUTPUT)

objs/%.o: src/%.c
	@mkdir -p objs/
	$(CC) -c -o $@ $< $(CFLAGS) $(EXTRA)

$(OUTPUT): $(OBJ) $(LIBS)
	$(CC) -o $@ $^ $(CFLAGS) $(EXTRA)

lib/libcblas.a:
	mkdir -p lib/cblas/lib;
	mkdir -p lib/include;
	cd lib/cblas; make alllib;
	cp lib/cblas/lib/cblas_LINUX.a lib/libcblas.a
	cp lib/cblas/include/cblas.h lib/include/

.PHONY: clean
clean:
	-rm -f $(OBJ)
	-rm -f $(OUTPUT)

.PHONY: test
test:
	valgrind ./$(OUTPUT) train
	valgrind ./$(OUTPUT) test
