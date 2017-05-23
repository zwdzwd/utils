CC     = gcc
AR     = ar
CFLAGS = -g -Wall

ifeq (1, $(CF_OPTIMIZE))
	CFLAGS += -O2
	CFLAGS := $(filter-out -g, $(CFLAGS))
endif

LIBUTILS_OBJS = \
	encode.o \
	stats.o \
	wzvcf.o \
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
