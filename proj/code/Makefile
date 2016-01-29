# Makefile for the project (based on that of hello).

COMPILER_TYPE= gnu

CC=gcc

PROG=	proj
SRCS=	proj.c keyboard.c timer.c video.c read_xpm.c sprite.c mouse.c rtc.c twoComplement.S

CCFLAGS= -Wall -03

DPADD+=	${LIBDRIVER} ${LIBSYS}
LDADD+= -llm -ldriver -lsys 

LDFLAGS+= -L .

MAN=

.include <bsd.prog.mk>
.include <bsd.gcc.mk>
