/*****************************************************************************
 * bin2hilo: split BIN formatted roms
 * See the function ``print_license'' below for license information.
 ****************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <err.h>
#include <errno.h>

#define PERR(str) fprintf(stderr, str)

void bin2hilo(FILE*, FILE*, FILE*);
void print_help();
void print_license();

int
main(int argc, char* argv[])
{
	int ch = 0, hflag = 0, lflag = 0;
	while ( (ch = getopt(argc, argv, "h?l")) != -1 )
	{
		switch (ch)
		{
		case 'l':
			lflag = 1;
			break;
		default:
			hflag = 1;
			break;
		}
	}
	argc -= optind;
	argv += optind;

	if (lflag)
	{
		print_license();
	}

	if (hflag || !lflag && argc != 3)
	{
		print_help();
	}
	else
	{
		if (argc == 3)
		{
			FILE *istream, *high_stream, *low_stream;
			istream =	fopen(argv[0], "rb");
			if (istream == NULL)
			{
				err(errno, "Could not open file %s", argv[0]);
				return EXIT_FAILURE;
			}

			high_stream = fopen(argv[1], "wb");
			if (high_stream == NULL)
			{
				err(errno, "Could not open file %s", argv[1]);
				fclose(istream);
				return EXIT_FAILURE;
			}

			low_stream = fopen(argv[2], "wb");
			if (low_stream == NULL)
			{
				err(errno, "Could not open file %s", argv[2]);
				fclose(istream);
				fclose(high_stream);
				return EXIT_FAILURE;
			}

			bin2hilo(istream, high_stream, low_stream);
			fclose(istream);
			fclose(high_stream);
			fclose(low_stream);
		}
	}
	return EXIT_SUCCESS;
}


void
bin2hilo(FILE *istream, FILE *high_stream, FILE* low_stream)
{
	char next_high, next_low;
	
	fseek(istream, 0, SEEK_SET);
	fseek(high_stream, 0, SEEK_SET);
	fseek(low_stream, 0, SEEK_SET);
	next_high = fgetc(istream);
	next_low = fgetc(istream);
	while (!feof(istream) && !ferror(istream)
	       && !ferror(high_stream) && !ferror(low_stream))
	{
		fputc(next_high, high_stream);
		fputc(next_low, low_stream);
		next_high = fgetc(istream);
		next_low = fgetc(istream);
	}
}


void
print_help()
{
	PERR("bin2hilo Copyright (c) 2014, Dakotah Lambert.\n");
	PERR("All rights reserved.\n\n");
	PERR("usage: bin2hilo [-h?] bin_file high_file low_file\n\n");
	PERR("  -h, -?    : show this message\n");
	PERR("  bin_file  : input file, must be BIN / RAW formatted\n");
	PERR("  high_file : file for high-order bytes\n");
	PERR("  low_file  : file for low-order bytes\n\n");
}

void
print_license()
{
	PERR("\n bin2hilo\n\
 Copyright (c) 2014, Dakotah Lambert\n\
 All rights reserved.\n\
\n\
 Redistribution and use in source and binary forms, with or without\n\
 modification, are permitted provided that the following conditions\n\
 are met:\n\
\n\
 1: Redistributions of source code must retain the above copyright\n\
    notice, this list of conditions and the following disclaimer.\n\
\n\
 2: Redistributions in binary form must reproduce the above copyright\n\
    notice, this list of conditions and the following disclaimer in\n\
    the documentation and/or other materials provided with the\n\
    distribution.\n\
\n\
 3: Neither the names of copyright holders nor the names of their\n\
    contributors may be used to endors or promote products derived\n\
    from this software without specific prior written permission.\n\
\n\
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS\n\
  \"AS IS\" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT\n\
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS\n\
  FOR A PARTICULAR PURPOSE ARE DISCLAIMED, IN NO EVENT SHALL THE\n\
  COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,\n\
  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,\n\
  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;\n\
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER\n\
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT\n\
  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN\n\
  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE\n\
  POSSIBILITY OF SUCH DAMAGE.\n\n");
}
