/******************************************************************************
*
* entplot - a tool for generating entropy plots using gnuplot.
*
* bmccourt, March 2015
*
* Based heavily on the stackoverflow discussion at:
* http://stackoverflow.com/questions/990477/how-to-calculate-the-entropy-of-a-file 
*
* This tool simply spits out gnuplot data and plt files. Gnuplot is used
* to actually create the image.
*
* Build instructions - gcc entplot.c -o entplot -lm -Wall
*
* usage: entplot <target file> <output.png>  ; this generates entplot.plt
*        gnuplot entplot.plt                 ; this generates <output.png>
*
* Feel free to use this code however you like. 
*
******************************************************************************/

#include <math.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char U08;

U08 *FileMap(char *filename, uint32_t *size)
{
  FILE *in;
  U08 *buffer;

  if ((in = fopen(filename, "rb")) == NULL)
  {
    return NULL;
  }

  fseek(in, 0, SEEK_END);
  *size = ftell(in);
  fseek(in, 0, SEEK_SET);

  if ((buffer = malloc(*size)) == NULL)
  {
    fclose(in);
    fprintf(stderr, "Error allocating memory!\n");
    return NULL;
  }

  fread(buffer, *size, 1, in);
  fclose(in);

  return buffer;
}

int createPlt(uint32_t size, char *filename, char *pngname)
{
  FILE *fh;
  double d = 0.0;
  uint32_t xtics;

  if ((fh = fopen("entplot.plt", "w")) == NULL)
  {
    fprintf(stderr, "Error creating entplot.plt!\n");
    return -1;
  }

  while((pow(2.0,d) < size/10.0)) 
  {
    xtics = (uint32_t) pow(2.0,d++);
  }

  fprintf(fh, "set key outside font \"Verdana,14\"\n");
  fprintf(fh, "set terminal pngcairo enhanced font \"Verdana,14\"\n");
  fprintf(fh, "set size ratio 0.5\n");
  fprintf(fh, "set style fill solid noborder\n");
  fprintf(fh, "set format x \"%%x\"\n");
  fprintf(fh, "set xtics nomirror %d,%d,0x%x\n", 0, xtics , size);
  fprintf(fh, "set xtics rotate font \"Verdata,12\"\n");
  fprintf(fh, "set mxtics 2\n");
  fprintf(fh, "set yrange [-0.05:1.05]\n");
  fprintf(fh, "set xrange [%d:%d]\n", 0, size);
  fprintf(fh, "set xlabel \"File offset (Hex)\" font \"Verdana,14\"\n");
  fprintf(fh, "set ylabel \"Entropy\" font \"Verdana,14\"\n");
  fprintf(fh, "set title \"%s\" font \"Verdana,14\"\n", filename);
  fprintf(fh, "set grid\n");
  fprintf(fh, "set terminal png size 2048,1536 crop\n");
  fprintf(fh, "set output \"%s\"\n", pngname);
  fprintf(fh, "plot \"entplot.dat\" using 1:2 title \"Entropy\" with line\n");

  fclose(fh);

  return 0;
}

int calcEntropy(U08 *FileBuf, uint32_t size)
{
  int i, j, count;
  U08 byte;
  size_t blocks = size / 256;
  int byte_counts[256];
  float entropy = 0;

  FILE *dataOut;

  if ((dataOut = fopen("entplot.dat", "w")) == NULL)
  {
    fprintf(stderr, "Error creating entplot.dat!\n");
    return -1;
  }

  for (i=0; i<blocks; i++)
  {
    memset(byte_counts, 0, sizeof(byte_counts));
    entropy = 0;

    for (j=0; j<256; j++)
    {
      byte = FileBuf[(i*256) + j];
      byte_counts[byte]++;
    }

    for (j=0; j<256; j++)
    {
      float p;
      count = byte_counts[j];

      if (count == 0)
        continue;

      p = 1.0 * count / 256;
      entropy -= (p * log2(p)) / 8;

    }

    fprintf(dataOut, "0x%06x %f\n", i * 256, entropy);
  }

  fclose(dataOut);

  return 0;
}

void Usage()
{
  printf("\nUsage: entplot <input file> <output png name>\n");
}

int main(int argc, char *argv[])
{
  U08 *buf;
  uint32_t size;
  
  if (argc != 3)
  {
    Usage();
    return -1;
  }

  if ((buf = FileMap(argv[1], &size)) == NULL)
    return -1;

  if (size < 512)
  {
    fprintf(stderr,"Error: file is too small\n");
    free(buf);
    return -1;
  }

  if (calcEntropy(buf, size) == -1)
  {
    free(buf);
    return -1;
  }

  if (createPlt(size, argv[1], argv[2]) == -1)
  {
    free(buf);
    return -1;
  }

  printf("Data files created - now run \"gnuplot entplot.plt\" to generate png\n"); 

  free(buf);
  return 0;
}
