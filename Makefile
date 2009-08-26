CFLAGS+=-Wall -g
OBJ=main.o parser.o tree.o script.o
BIN=alt

all: ${OBJ}
	${CC} -o ${BIN} ${OBJ}
	#gdb --args ./${BIN} test.alt
	./${BIN} t/hello.alt
	#./${BIN} t/hello.alt

loc:
	@cat *.c *.h | wc -l

clean:
	rm -f ${OBJ} ${BIN}
