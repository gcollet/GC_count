#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>

/*
 * Main
 */
int main (int argc, char ** argv)
{
	/* Check if a file is given */
	if (argc < 2){
		fprintf(stderr, "Usage: %s <file>\n", argv[0]);
		exit(0);
	}

	/* Open the file */
	char * fname = argv[1];
	int fd = open (fname, O_RDONLY);
	if (fd == -1) {
		fprintf (stderr,"Cannot open file %s\n", fname);
		exit(1);
	}
	
	/* Get statistics about the file (its length in fact) */
	struct stat sb;
	if (fstat (fd, &sb) == -1) {
		fprintf (stderr,"Cannot fstat file %s\n", fname);
		exit(1);
	}
	
	/* Check the file status */
	if (!S_ISREG (sb.st_mode)) {
		fprintf (stderr, "%s is not a file\n", fname);
		exit(1);
	}
	
	/* Mmap the file in memory */
	char * buf = mmap (0, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
	if (buf == MAP_FAILED) {
		fprintf (stderr,"Cannot map file %s\n", fname);
		exit(1); 
	}
	
	/* Close the file */
	if (close (fd) == -1) {
		fprintf (stderr,"Cannot close file %s\n", fname);
		exit(1); 
	}

	long nb_gc = 0; /* Count GC positions */
	long nb_at = 0; /* Count AT positions */
	long pos = 0;   /* Position in the buffer */
	
	/* Loop on all char in buf to count AT and GC separately
	 * if a '>' is found, then go to the end of line.
	 * Ignore all other char */
	while (pos < sb.st_size) {
		/* If a '>' is found -> go to the end of line */
		if (buf[pos] == '>'){
			while (buf[pos] != '\n' && pos < sb.st_size)
				pos++;
		} else if (buf[pos] == 'T' || buf[pos] == 'A'){
			nb_at++;
		} else if (buf[pos] == 'G' || buf[pos] == 'C'){
			nb_gc++;
		}
		pos++;
	}
	
	double gc_rate = (double) nb_gc / (double) (nb_gc + nb_at);
	
	if (munmap (buf, sb.st_size) == -1) {
		fprintf (stderr,"Cannot unmap file %s\n", fname);
		exit(1); 
	}
	
	printf("G-C %1.2lf\n%ld G-C\n%ld A-T\n", gc_rate, nb_gc, nb_at);
	
	return 0;
}