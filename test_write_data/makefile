# GCC/Linux compile flags
GNUCC = gcc
GNUCFLAGS = -g -std=gnu99 -Wstrict-prototypes -DNOAVR
GNULDFLAGS = -static
GNUSFLAGS = -D_SVID_SOURCE -D_XOPEN_SOURCE
GNUNOANSI = -g -std=gnu99 -Wstrict-prototypes 

#####################################################
CC = ${GNUCC}
NOANSI = ${GNUNOANSI}
CFLAGS = ${GNUCFLAGS}
LDFLAGS = ${GNULDFLAGS}
#SFLAGS = ${GNUSFLAGS}
SFLAGS = -DDEBUG

all: test_data

t6963.o : ../t6963.c
	${CC} -c ${CFLAGS} ${SFLAGS} -o t6963.o ../t6963.c

menus.o : ../menus.c 
	${CC} -c ${CFLAGS} ${SFLAGS} -o menus.o ../menus.c
	
test_write_data.o : test_write_data.c ../main.h ../sfr_helper.h
	${CC} -c  ${CFLAGS} ${SFLAGS} -o test_write_data.o test_write_data.c

test_data : menus.o test_write_data.o t6963.o
	${CC} ${CFLAGS} test_write_data.o t6963.o menus.o -o test_data -lcurses
	
clean :
	rm -f *.o *~ *# core  a.out \
	test_data

