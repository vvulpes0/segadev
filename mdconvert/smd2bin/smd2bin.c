/*****************************************************************************
 * smd2bin: convert roms from SMD to BIN format
 * See the function ``print_license'' below for license information.
 ****************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <err.h>
#include <errno.h>

#define HEADER_SIZE 0x200
#define PERR(str) fprintf(stderr, str)

void smd2bin(FILE*, FILE*);
void print_help();
void print_license();

int
main(int argc, char* argv[])
{
	int ch = 0, hflag = 0, lflag = 0, oflag = 0;
	char output_filename[256];
	while ( (ch = getopt(argc, argv, "h?lo:")) != -1 )
	{
		switch (ch)
		{
		case 'h':
		case '?':
			hflag = 1;
			break;
		case 'l':
			lflag = 1;
			break;
		case 'o':
			oflag = 1;
			strncpy(output_filename, optarg, 255);
			output_filename[255] = '\0';
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

	if (hflag || (argc != 1 && !lflag) || (!oflag && !lflag))
	{
		print_help();
	}
	else
	{
		if (argc == 1 && oflag)
		{
			FILE *istream, *ostream;
			istream =	fopen(argv[0], "rb");
			if (istream == NULL)
			{
				err(errno, "Could not open file %s", argv[0]);
				return EXIT_FAILURE;
			}

			ostream = fopen(output_filename, "wb");
			if (ostream == NULL)
			{
				err(errno, "Could not open file %s", output_filename);
				fclose(istream);
				return EXIT_FAILURE;
			}

			smd2bin(istream, ostream);
			fclose(istream);
			fclose(ostream);
		}
	}
	return EXIT_SUCCESS;
}


void
smd2bin(FILE *istream, FILE *ostream)
{
	ssize_t cpos = 0;
	char next_char;
	
	fseek(istream, HEADER_SIZE, SEEK_SET);
	fseek(ostream, 0, SEEK_SET);
	next_char = fgetc(istream);
	while (!( feof(istream) ) && !( ferror(istream) )
	       && !( ferror(ostream) ))
	{
		fputc('\0', ostream);
		fputc(next_char, ostream);
		++cpos;
		if ((cpos / 8192) % 2 == 1)
		{
			cpos += 8192;
			fseek(istream, 8192, SEEK_CUR);
		}
		next_char = fgetc(istream);
	}

	cpos = 8192;
	fseek(istream, HEADER_SIZE + 8192, SEEK_SET);
	fseek(ostream, 0, SEEK_SET);
	next_char = fgetc(istream);
	while (!( feof(istream) ) && !( ferror(istream) )
	       && !( ferror(ostream) ))
	{
		fputc(next_char, ostream);
		fseek(ostream, 1, SEEK_CUR);
		++cpos;
		if ((cpos / 8192) % 2 == 0)
		{
			cpos += 8192;
			fseek(istream, 8192, SEEK_CUR);
		}
		next_char = fgetc(istream);
	}
}


void
print_help()
{
	PERR("smd2bin Copyright (c) 2013, Dakotah Lambert.\n");
	PERR("All rights reserved.\n\n");
	PERR("usage: smd2bin [-h?l] -o outfile infile\n\n");
	PERR("  -h, -? : show this message\n");
	PERR("  -l     : display license information\n");
	PERR("  -o     : mandatory; followed by name of output file\n");
	PERR("  infile : name of file to convert\n\n");
}

void
print_license()
{
	PERR("\n smd2bin\n\
 Copyright (c) 2013, Dakotah Lambert\n\
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
