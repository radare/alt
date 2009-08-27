CFLAGS+=-Wall -g -O2
OBJ=main.o parser.o tree.o script.o
BIN=alt

all: ${OBJ}
	${CC} -o ${BIN} ${OBJ}
	./${BIN} t/hello.alt

loc:
	@cat *.c *.h | wc -l

clean:
	rm -f ${OBJ} ${BIN}
