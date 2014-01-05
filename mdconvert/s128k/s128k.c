/*****************************************************************************
 * s128k: split a rom to Sega's ideal format:
 * The rom is split into 128 kB odd and even pieces
 * +---------+---------+
 * | 0: Even | 1: Odd  |
 * +---------+---------+
 * | 2: Even | 3: Odd  |
 * +---------+---------+
 * |        ...        |
 * +---------+---------+
 * See the function ``print_license'' below for license information.
 ****************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <err.h>
#include <errno.h>

#define kB * 1024
#define PERR(str) fprintf(stderr, str)

void s128k(char*);
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
		case 'h':
		case '?':
			hflag = 1;
			break;
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

	if (hflag || (argc != 1 && !lflag))
	{
		print_help();
	}
	else
	{
		if (argc == 1)
		{
			s128k(argv[0]);
		}
	}
	return EXIT_SUCCESS;
}


void
s128k(char* filename)
{
	ssize_t cpos = 0, length = strlen(filename) + 4;
	char next_even, next_odd;
	char name_buffer[length];
	FILE *ifile = NULL, *odd_file = NULL, *even_file = NULL;
	
	ifile = fopen(filename, "rb");
	if (ifile == NULL)
	{
		err(errno, "Could not open file %s", filename);
		exit(EXIT_FAILURE);
	}
	
	next_odd = fgetc(ifile);
	next_even = fgetc(ifile);
	while (!feof(ifile) && !ferror(ifile)
	       && (odd_file == NULL || !ferror(odd_file))
	       && (even_file == NULL || !ferror(even_file)))
	{
		if (cpos % (128 kB) == 0)
		{
			if (odd_file != NULL) fclose(odd_file);
			if (even_file != NULL) fclose(even_file);
			snprintf(name_buffer, length, "%s.%d",
			         filename, 2 * cpos / (128 kB));
			even_file = fopen(name_buffer, "wb");
			if (even_file == NULL)
			{
				fclose(ifile);
				exit(EXIT_FAILURE);
			}
			
			snprintf(name_buffer, length, "%s.%d",
			         filename, 2 * cpos / (128 kB) + 1);
			odd_file = fopen(name_buffer, "wb");
			if (odd_file == NULL)
			{
				fclose(ifile);
				fclose(even_file);
				exit(EXIT_FAILURE);
			}
		}
		
		fputc(next_odd, odd_file);
		next_odd = fgetc(ifile);
		fputc(next_even, even_file);
		next_even = fgetc(ifile);
		++cpos;
	}
}


void
print_help()
{
	PERR("s128k Copyright (c) 2014, Dakotah Lambert.\n");
	PERR("All rights reserved.\n\n");
	PERR("usage: s128k [-h?l] infile\n\n");
	PERR("  -h, -? : show this message\n");
	PERR("  -l     : display license information\n");
	PERR("  infile : name of file to split\n\n");
}

void
print_license()
{
	PERR("\n s128k\n\
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
