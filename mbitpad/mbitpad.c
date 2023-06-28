/*****************************************************************************
 * mbitpad: pad a file to an integer number of megabits
 * See the function ``print_license'' below for license information.
 ****************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <err.h>
#include <errno.h>

#define DEFAULT_CHAR ((unsigned char) 0xFF)
#define DEFAULT_SIZE 8
#define Mbit * 1024 * 1024 / 8
#define PERR(str) fprintf(stderr, str)

void pad(ssize_t, unsigned char, FILE*);
void print_help();
void print_license();

int
main(int argc, char* argv[])
{
	int ch = 0, hflag = 0, lflag = 0;
	ssize_t pad_to = DEFAULT_SIZE Mbit;
	unsigned char pad_with = DEFAULT_CHAR;
	while ( (ch = getopt(argc, argv, "h?lc:s:")) != -1 )
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
		case 'c':
			if (sscanf(optarg, "%hhi", &pad_with) != 1)
			{
				pad_with = DEFAULT_CHAR;
				fprintf(
					stderr,
					"Could not convert %s to a valid char, defaulting to %02hhX.",
					optarg, DEFAULT_CHAR);
			}
			break;
		case 's':
			if (sscanf(optarg, "%lud", &pad_to) != 1)
			{
				pad_to = DEFAULT_SIZE Mbit;
				fprintf(
					stderr,
					"Could not convert %s to a valid number, defaulting to %d Mbit.",
					optarg, DEFAULT_SIZE);
			} else {
				pad_to = pad_to Mbit;
			}
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
			FILE *ostream;
			ostream =	fopen(argv[0], "ab");
			if (ostream == NULL)
			{
				err(errno, "Could not open file %s", argv[0]);
				return EXIT_FAILURE;
			}

			pad(pad_to, pad_with, ostream);
			fclose(ostream);
		}
	}
	return EXIT_SUCCESS;
}


void
pad(ssize_t pad_to, unsigned char pad_with, FILE *ostream)
{
	ssize_t cpos = ftell(ostream);
	while (cpos < pad_to)
	{
		fputc(pad_with, ostream);
		++cpos;
	}
}


void
print_help()
{
	PERR("mbitpad Copyright (c) 2014, Dakotah Lambert.\n");
	PERR("All rights reserved.\n\n");
	PERR("usage: mbitpad [-h?l] [-c pad_char] [-s size] file\n\n");
	PERR("  -h, -? : show this message\n");
	PERR("  -l     : display license information\n");
	PERR("  -c     : ASCII value of char to pad with\n");
	PERR("  -s     : size to which the file should be padded\n");
	PERR("  file   : name of file to pad\n\n");
	PERR("Example:\n");
	PERR("mbitpad -c 0xFF -s 4 rom.bin\n");
	PERR("  pads rom.bin with character <FF> to 4 Mbit (0.5 MB)\n\n");
}

void
print_license()
{
	PERR("\n mbitpad\n\
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
