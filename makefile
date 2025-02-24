CC=gcc
CFLAGS=-g 
TARGET:test.exe
LIBS : -lpthread -L ./CommandParser -lcli
OBJS=gluethread/glthread.o	\
		 graph.o 			\
		 topologies.o 		\
		 net.o \
		 CommandParser/libcli.a\
		 comm.o 
		 
test.exe:testapp.o ${OBJS} CommandParser/libcli.a
	${CC}	${CFLAGS}	testapp.o ${OBJS} ${LIBS} -o test.exe

testapp.o:testapp.c 
	${CC}	${CFLAGS}	-c testapp.c -o testapp.o 

gluethread/glthread.o:gluethread/glthread.c 
	${CC}	${CFLAGS}	-c -I gluethread gluethread/glthread.c -o gluethread/glthread.o 
graph.o:graph.c 
	${CC}	${CFLAGS}	-c -I . graph.c -o graph.o 
topologies.o:topologies.c 
	${CC}	${CFLAGS}	-c -I . topologies.c -o topologies.o 
net.o:net.c
	${CC} ${CFLAGS} -c -I . net.c -o net.o
comm.o:comm.c
	${CC} ${CFLAGS} -c comm.c -o comm.o
CommandParser/libcli.a:
	(cd CommandParser; make)

clean:
	rm *.o 
	rm gluethread/glthread.o 
	rm *exe
	(cd CommandParser; make clean)
all:
	make
	(cd CommandParser; make)
