OBJ=main.o parser.o tree.o
BIN=alt

all: ${OBJ}
	${CC} -o ${BIN} ${OBJ}
	#gdb --args ./${BIN} test.alt
	./${BIN} test.alt
