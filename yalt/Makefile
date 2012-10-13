CFLAGS=-g -ggdb
BIN=yalt

all: ${BIN}

${BIN}: lang.c
	${CC} -o ${BIN} ${CFLAGS} lang.c ../alt.c -I ..

test:
	./yalt test.yalt
