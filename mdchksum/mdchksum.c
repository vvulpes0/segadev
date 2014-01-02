/*****************************************************************************
 * mdchksum: read and fix checksums on Sega Genesis / Mega Drive roms
 * See the function ``print_license'' below for license information.
 ****************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <err.h>
#include <errno.h>

#define CHECKSUM_LOCATION 0x18E
#define DATA_START 0x200
#define PERR(str) fprintf(stderr, str)

int find_checksum(FILE*);
int find_stored_checksum(FILE*);
void fix_checksum(FILE*, int);
void print_help();
void print_license();

int
main(int argc, char* argv[])
{
	int ch = 0, fflag = 0, hflag = 0, lflag = 0;
	while ( (ch = getopt(argc, argv, "h?lf")) != -1 )
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
		case 'f':
			fflag = 1;
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
			FILE *stream = fopen(argv[0], fflag ? "rb+" : "rb");
			if (! stream)
			{
				err(errno, "Could not open file %s", argv[0]);
				return EXIT_FAILURE;
			}
			int stored = find_stored_checksum(stream);
			int checksum = find_checksum(stream);
			
			if (stored == checksum)
			{
				printf( "Checksum OK: 0x%04X.\n", checksum );
			}
			else
			{
				printf( "Bad checksum: expected 0x%04X, found 0x%04X.\n",
				        checksum, stored);
				if (fflag)
				{
					fix_checksum(stream, checksum);
					printf( "Checksum fixed.\n" );
				}
			}
		}
	}
	return EXIT_SUCCESS;
}


int
find_checksum(FILE *stream)
{
	int checksum = 0;
	int to_add = 0;
	fseek(stream, DATA_START, SEEK_SET);
	while (!( feof(stream) ) && !( ferror(stream) ))
	{
		checksum += to_add;
		checksum %= 65536;
		to_add = fgetc(stream) * 256 + fgetc(stream); 
	}
	return checksum;
}

int
find_stored_checksum(FILE* stream)
{
	fseek(stream, CHECKSUM_LOCATION, SEEK_SET);
	return (fgetc(stream) * 256 + fgetc(stream));
}

void
fix_checksum(FILE* stream, int checksum)
{
	fseek(stream, CHECKSUM_LOCATION, SEEK_SET);
	fputc(checksum / 256, stream);
	fputc(checksum % 256, stream);
	return;
}


void
print_help()
{
	PERR("mdchksum Copyright (c) 2013, Dakotah Lambert.\n");
	PERR("All rights reserved.\n\n");
	PERR("usage: mdchksum [-h?l] [-f] file\n\n");
	PERR("  -h, -? : show this message\n");
	PERR("  -f     : automatically fix the checksum\n");
	PERR("  -l     : display license information\n");
	PERR("  file   : name of file to check\n\n");
	PERR("The program assumes the file specified is a valid\n");
	PERR("Sega Genesis / Mega Drive rom in BIN / RAW format;\n");
	PERR("currently no checking is done to verify this.\n");
	PERR("Do not try to fix the checksum on arbitrary files,\n");
	PERR("as this will almost certainly cause problems.\n");
}

void
print_license()
{
	PERR("\n mdchksum\n\
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
