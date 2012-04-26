GC_count
========

GC_count calculates the gc rate in a file.

It uses a mmap to open file and quickly parse all the sequences inside.

Sequences must be in fasta format.

Compile the file with gcc :

gcc -O3 -std=c99 -o gc_count gc_count.c
