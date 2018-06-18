INCLUDES=-Iinclude/
CFLAGS+=$(INCLUDES) -Wall -Werror -ansi -pedantic

OBJ=objs/data.o objs/main.o objs/perceptron.o
OUTPUT=cnum

.PHONY: debug
default: $(OUTPUT)
debug: CFLAGS += -g -O0 -D__DEBUG
debug: $(OUTPUT)

objs/%.o: src/%.c
	@mkdir -p objs/
	$(CC) -c -o $@ $< $(CFLAGS) $(EXTRA)

$(OUTPUT): $(OBJ) $(LIBS)
	$(CC) -o $@ $^ $(CFLAGS) $(EXTRA) -L/usr/lib -lblas

.PHONY: clean
clean:
	-rm -f $(OBJ)
	-rm -f $(OUTPUT)

.PHONY: test
test:
	valgrind ./$(OUTPUT) train
	valgrind ./$(OUTPUT) test
