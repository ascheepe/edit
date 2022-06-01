CFLAGS=	-O2 -pipe -Wall -Wextra -Werror -pedantic
CFLAGS+= -D_DEFAULT_SOURCE
SRCS= align.c center.c
OBJS= align.o center.o
PRGS= align center

all: $(OBJS)
	$(CC) $(CFLAGS) -o align align.o
	$(CC) $(CFLAGS) -o center center.o

clean:
	rm -f $(OBJS) $(PRGS)

