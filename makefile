CC = gcc
CFLAGS = -Wall -g

all: pcode testing/test_ht testing/test_resize testing/test_update

pcode: pcode.c ht.c ht.h ht_impl.h
	$(CC) $(CFLAGS) -o pcode pcode.c ht.c

testing/test_ht: testing/test_ht.c ht.c ht.h ht_impl.h
	$(CC) $(CFLAGS) -o testing/test_ht testing/test_ht.c ht.c

testing/test_resize: testing/test_resize.c ht.c ht.h ht_impl.h
	$(CC) $(CFLAGS) -o testing/test_resize testing/test_resize.c ht.c

testing/test_update: testing/test_update.c ht.c ht.h ht_impl.h
	$(CC) $(CFLAGS) -o testing/test_update testing/test_update.c ht.c

testing: all
	./testing/test_ht
	./testing/test_resize
	./testing/test_update

clean:
	rm -f pcode testing/test_ht testing/test_resize testing/test_update

