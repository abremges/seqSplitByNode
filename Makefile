CC=gcc
CFLAGS=-Wall -pedantic -std=gnu99 -O3

seqSplitByNode:main.c kseq.h
	$(CC) $(CFLAGS) main.c -o $@ -lz

clean:
	rm -fr gmon.out *.o ext/*.o a.out seqSplitByNode *~ *.a *.dSYM session*
