/* The MIT License

   Copyright (c) 2012-2013 Andreas Bremges <abremges@cebitec.uni-bielefeld.de>

   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files (the
   "Software"), to deal in the Software without restriction, including
   without limitation the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to
   the following conditions:

   The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
   BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
   ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/

#include <stdio.h> // printf
#include <unistd.h> // getopt
#include <zlib.h> // gzip

#include "kseq.h"
KSEQ_INIT(gzFile, gzread)

void stk_printseq(const kseq_t *s) {
	fputc(s->qual.l? '@' : '>', stdout);
	fputs(s->name.s, stdout);
	if (s->comment.l) {
			fputc(' ', stdout);
			fputs(s->comment.s, stdout);
	}
	fputc('\n', stdout);
	fputs(s->seq.s, stdout);
	fputc('\n', stdout);
	if (s->qual.l) {
			fputs("+\n", stdout);
			fputs(s->qual.s, stdout);
			fputc('\n', stdout);
	}
}

void processFile(const char *file, const int n, const int N, const int p) {
    gzFile fp = strcmp(file, "-") ? gzopen(file, "r") : gzdopen(fileno(stdin), "r");
    kseq_t *r = kseq_init(fp);
    static long n_seqs = 0;
    while (kseq_read(r) >= 0) {
        ++n_seqs;
        if (n_seqs % N == n) {
            stk_printseq(r);
            if (p && kseq_read(r) >= 0) {
                stk_printseq(r);
            }
        }
    }
    kseq_destroy(r);
    gzclose(fp);
}

static int usage() {
    fprintf(stderr, "\nUsage: seqSplitByNode [options] [file ...]\n\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "       -n INT  Node ID\n");
    fprintf(stderr, "       -N INT  Total number of nodes\n");
    fprintf(stderr, "       -p      Treat reads as pairs\n\n");
    return 1;
}

int main(int argc, char *argv[]) {
	char *file = 0;
	int c, n = 0, N = 1, p = 0;
	while((c = getopt(argc, argv, "n:N:p")) != -1) {
		switch (c) {
			case 'n':
				n = atoi(optarg);
				if (n < 0) n = 0;
				break;
            case 'N':
                N = atoi(optarg);
                if (N < 1) N = 1;
                break;
            case 'p':
                p = 1;
                break;
			default:
				return usage();
		}
	}

    if (n <= N) {
        n %= N;
    } else return usage();

    if (optind < argc) {
        while (optind < argc) {
            file = argv[optind++];
            processFile(file, n, N, p);
        }
    } else return usage();

	return 0;
}
