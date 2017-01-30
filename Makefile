CC     = gcc
AR     = ar
CFLAGS = -g -Wall

ifneq (1, $(CF_NO_OPTIMIZE))
	CFLAGS += -O2
endif

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

purge: clean
	rm -f *.a

cleanhist:
	rm -rf .git .gitignore
