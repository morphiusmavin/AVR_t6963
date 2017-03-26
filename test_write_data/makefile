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
SFLAGS =

all: test_data # test_menu

menu.o : ../menus.c 
	${CC} -c ${CFLAGS} ${SFLAGS} -o menu.o ../menus.c
	
test_write_data.o : test_write_data.c ../main.h ../sfr_helper.h
	${CC} -c ${CFLAGS} ${SFLAGS} -o test_write_data.o test_write_data.c

test_write_menu.o : test_write_menu.c
	${CC} -c ${CFLAGS} ${SFLAGS} -o test_write_menu.o test_write_menu.c

test_data : test_write_data.o
	${CC} ${CFLAGS} test_write_data.o -o test_data -lcurses
	
test_menu : test_write_menu.o menu.o
	${CC} ${CFLAGS} test_write_menu.o menu.o -o test_menu -lcurses

clean :
	rm -f *.o *~ *# core  a.out \
	test_data test_menu

