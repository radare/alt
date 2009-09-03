CFLAGS+=-Wall -g -O2
OBJ=main.o parser.o tree.o script.o
BIN=alt

all: ${OBJ}
	${CC} -o ${BIN} ${OBJ}
#	./${BIN} t/ops.alt

loc:
	@cat *.c *.h | wc -l

locdiff:
	@A=`hg diff | grep -v '+++' | grep ^+ |wc -l` ; \
	B=`hg diff | grep -v -- '---' | grep ^- |wc -l` ; \
	echo $$((A-B))


clean:
	rm -f ${OBJ} ${BIN}
