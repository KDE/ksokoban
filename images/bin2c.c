/*
 *  bin2c - compresses data files & converts the result to C source code
 *  Copyright (C) 1998  Anders Widell  <d95-awi@nada.kth.se>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
 * This command uses the zlib library to compress each file given on
 * the command line, and outputs the compressed data as C source code
 * to the file 'data.c' in the current directory
 *
 */

#include "../../config.h"

#include <stdio.h>
#include <stdlib.h>

#ifdef USE_LIBZ
#include <zlib.h>
#else
typedef unsigned char Bytef;
typedef unsigned long uLongf;
#endif

#define BUFSIZE 16384            /* Increase buffer size by this amount */

static Bytef *source=NULL;       /* Buffer containing uncompressed data */
static Bytef *dest=NULL;         /* Buffer containing compressed data */
static uLongf sourceBufSize=0;   /* Buffer size */
static uLongf destBufSize=0;     /* Buffer size */

static uLongf sourceLen;         /* Length of uncompressed data */
static uLongf destLen;           /* Length of compressed data */

static FILE *infile=NULL;        /* The input file containing binary data */
static FILE *outfile=NULL;       /* The output file 'data.c' */

static char *programName="";

/*
 * Print error message and free allocated resources
 *
 */

static int
error (msg1, msg2, msg3)
     char *msg1;
     char *msg2;
     char *msg3;
{
  fprintf (stderr, "%s: %s%s%s\n", programName, msg1, msg2, msg3);

  if (infile != NULL) fclose (infile);
  if (outfile != NULL) fclose (outfile);
  remove ("data.c");
  free (dest);
  free (source);

  return 1;
}

/*
 * Replacement for strrchr in case it isn't present in libc
 *
 */

static char *
my_strrchr (s, c)
     char *s;
     int c;
{
  char *ptr = NULL;

  while (*s) {
    if (*s == c) ptr = s;
    s++;
  }

  return ptr;
}

#ifdef USE_LIBZ
/*
 * NOTE: my_compress2 is taken directly from zlib 1.1.3
 *
 * This is for compability with early versions of zlib that
 * don't have the compress2 function.
 *
 */

/* ===========================================================================
     Compresses the source buffer into the destination buffer. The level
   parameter has the same meaning as in deflateInit.  sourceLen is the byte
   length of the source buffer. Upon entry, destLen is the total size of the
   destination buffer, which must be at least 0.1% larger than sourceLen plus
   12 bytes. Upon exit, destLen is the actual size of the compressed buffer.

     compress2 returns Z_OK if success, Z_MEM_ERROR if there was not enough
   memory, Z_BUF_ERROR if there was not enough room in the output buffer,
   Z_STREAM_ERROR if the level parameter is invalid.
*/
int my_compress2 (dest, destLen, source, sourceLen, level)
    Bytef *dest;
    uLongf *destLen;
    const Bytef *source;
    uLong sourceLen;
    int level;
{
    z_stream stream;
    int err;

    stream.next_in = (Bytef*)source;
    stream.avail_in = (uInt)sourceLen;
#ifdef MAXSEG_64K
    /* Check for source > 64K on 16-bit machine: */
    if ((uLong)stream.avail_in != sourceLen) return Z_BUF_ERROR;
#endif
    stream.next_out = dest;
    stream.avail_out = (uInt)*destLen;
    if ((uLong)stream.avail_out != *destLen) return Z_BUF_ERROR;

    stream.zalloc = (alloc_func)0;
    stream.zfree = (free_func)0;
    stream.opaque = (voidpf)0;

    err = deflateInit(&stream, level);
    if (err != Z_OK) return err;

    err = deflate(&stream, Z_FINISH);
    if (err != Z_STREAM_END) {
        deflateEnd(&stream);
        return err == Z_OK ? Z_BUF_ERROR : err;
    }
    *destLen = stream.total_out;

    err = deflateEnd(&stream);
    return err;
}
#endif

int
main (argc, argv)
     int argc;
     char **argv;
{
  int i;
  int result;
  unsigned j;
  char *ptr;

  programName = argv[0];

  outfile = fopen ("data.c", "w");
  if (outfile == NULL) {
      fprintf (stderr, "%s: can't open 'data.c' for writing\n", argv[0]);
      return 1;
  }

  /* Process each file given on command line */
  for (i=1; i<argc; i++) {
    infile = fopen (argv[i], "rb");
    if (infile == NULL) return error ("can't open '", argv[i], "' for reading");

    /* Read infile to source buffer */
    sourceLen = 0;
    while (!feof (infile)) {
      if (sourceLen + BUFSIZE > sourceBufSize) {
	sourceBufSize += BUFSIZE;
	source = realloc (source, sourceBufSize);
	if (source == NULL) return error ("memory exhausted", "", "");
      }
      sourceLen += fread (source+sourceLen, 1, BUFSIZE, infile);
      if (ferror (infile)) return error ("error reading '", argv[i], "'");
    }
    fclose (infile);

#ifdef USE_LIBZ

    /* (Re)allocate dest buffer */
    destLen = sourceBufSize + (sourceBufSize+9)/10 + 12;
    if (destBufSize < destLen) {
      destBufSize = destLen;
      dest = realloc (dest, destBufSize);
      if (dest == NULL) return error ("memory exhausted", "", "");
    }

    /* Compress dest buffer */
    destLen = destBufSize;
    result = my_compress2 (dest, &destLen, source, sourceLen, 9);
    if (result != Z_OK) return error ("error compressing '", argv[i], "'");

#else

    destLen = sourceLen;
    dest = source;

#endif

    /* Output dest buffer as C source code to outfile */
    ptr = my_strrchr (argv[i], '.');
    if (ptr != NULL) *ptr = '\0';
    fprintf (outfile, "static const unsigned char %s_data[] = {\n", argv[i]);

    for (j=0; j<destLen-1; j++) {
      switch (j%8) {
      case 0:
	fprintf (outfile, "  0x%02x, ", ((unsigned) dest[j]) & 0xffu);
	break;
      case 7:
	fprintf (outfile, "0x%02x,\n", ((unsigned) dest[j]) & 0xffu);
	break;
      default:
	fprintf (outfile, "0x%02x, ", ((unsigned) dest[j]) & 0xffu);
	break;
      }
    }

    if ((destLen-1)%8 == 0) fprintf (outfile, "  0x%02x\n};\n\n", ((unsigned) dest[destLen-1]) & 0xffu);
    else fprintf (outfile, "0x%02x\n};\n\n", ((unsigned) dest[destLen-1]) & 0xffu);
  }

  fclose (outfile);
#ifdef USE_LIBZ
  free (dest);
#endif
  free (source);

  return 0;
}
