// Open a file and create a new stream for it.
FILE  fopen (const char *__restrict __filename,
		    const char *__restrict __modes)

// Open a file, replacing an existing stream with it.
FILE* freopen (const char *__restrict __filename,
		      const char *__restrict __modes,
		      FILE *__restrict __stream)

// Write formatted output to STREAM.
int fprintf (FILE *__restrict __stream,
		    const char *__restrict __format, ...);

// Write formatted output to stdout.
int printf (const char *__restrict __format, ...);

// Write formatted output to S.
int sprintf (char *__restrict __s,
		    const char *__restrict __format, ...) __THROWNL;

// Read formatted input from STREAM.
int fscanf (FILE *__restrict __stream,
		   const char *__restrict __format, ...) __wur;

// Read formatted input from stdin.
int scanf (const char *__restrict __format, ...) __wur;

// Read a character from STREAM.
int fgetc (FILE *__stream);
int getc (FILE *__stream);

// Read a character from stdin.
int getchar (void);

// Write a character to STREAM.
fputc (int __c, FILE *__stream);
putc (int __c, FILE *__stream);

// Write a character to stdout.
int putchar (int __c);

// Get a newline-terminated string of finite length from STREAM.
char *fgets (char *__restrict __s, int __n, FILE *__restrict __stream)
     __wur __fortified_attr_access (__write_only__, 1, 2);

/* Read up to (and including) a DELIMITER from STREAM into *LINEPTR
   (and null-terminate it). *LINEPTR is a pointer returned from malloc (or
   NULL), pointing to *N characters of space.  It is realloc'd as
   necessary.  Returns the number of characters read (not including the
   null terminator), or -1 on error or EOF. */

__ssize_t getdelim (char **__restrict __lineptr,
                           size_t *__restrict __n, int __delimiter,
                           FILE *__restrict __stream) __wur;

// Like `getdelim', but reads up to a newline.
__ssize_t getline (char **__restrict __lineptr,
                          size_t *__restrict __n,
                          FILE *__restrict __stream) __wur;

// Write a string to STREAM.
int fputs (const char *__restrict __s, FILE *__restrict __stream);

// Write a string, followed by a newline, to stdout.
int puts (const char *__s);

//  Write a string, followed by a newline, to stdout.
int puts (const char *__s);

// Push a character back onto the input buffer of STREAM.
int ungetc (int __c, FILE *__stream);

// Read chunks of generic data from STREAM.
size_t fread (void *__restrict __ptr, size_t __size,
		     size_t __n, FILE *__restrict __stream) __wur;

// Write chunks of generic data to STREAM.
size_t fwrite (const void *__restrict __ptr, size_t __size,
		      size_t __n, FILE *__restrict __s);

// Seek to a certain position on STREAM.
int fseek (FILE *__stream, long int __off, int __whence);

// Return the current position of STREAM.
long int ftell (FILE *__stream) __wur;

// Rewind to the beginning of STREAM.
void rewind (FILE *__stream);

// Rename file OLD to NEW.
int rename (const char *__old, const char *__new);

// Remove file FILENAME
int remove (const char *__filename);