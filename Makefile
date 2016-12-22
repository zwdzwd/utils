CC     = gcc
AR     = ar
CFLAGS = -g -Wall -O2

LIBUTILS_OBJS = \
	encode.o \
	stats.o \
	wzhmm.o

.c.o :
	$(CC) -c $(CFLAGS) $< -o $@

libutils.a: $(LIBUTILS_OBJS)
	@-rm -f $@
	$(AR) -csr $@ $^

clean:
	rm -f *.o

purge:
	rm -f *.a

cleanhist:
	rm -rf .git .gitignore
