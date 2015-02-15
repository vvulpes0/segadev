/*****************************************************************************
 * mdchksum: read and fix checksums on Sega Genesis / Mega Drive roms
 * See the function ``print_license'' below for license information.
 *************************************************************************** */
#define _POSIX_SOURCE
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
	int ch = 0;
	int fflag = 0;
	int hflag = 0;
	int lflag = 0;
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
			int stored;
			int checksum;
			FILE *stream = fopen(argv[0], fflag ? "rb+" : "rb");
			if (stream == NULL)
			{
				err(errno, "Could not open file %s", argv[0]);
				return EXIT_FAILURE;
			}
			stored = find_stored_checksum(stream);
			checksum = find_checksum(stream);
			
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

			fclose(stream);
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
	PERR("mdchksum\n");
	PERR("Copyright (c) 2013, Dakotah Lambert\n");
	PERR("All rights reserved.\n\n");
	PERR("Redistribution and use in source and binary forms, with or without\n");
	PERR("modification, are permitted provided that the following conditions\n");
	PERR("are met:\n\n");
	PERR("1: Redistributions of source code must retain the above copyright\n");
	PERR("   notice, this list of conditions and the following disclaimer.\n\n");
	PERR("2: Redistributions in binary form must reproduce the above copyright\n");
	PERR("   notice, this list of conditions and the following disclaimer in\n");
	PERR("   the documentation and/or other materials provided with the\n");
	PERR("   distribution.\n\n");
	PERR("3: Neither the names of copyright holders nor the names of their\n");
	PERR("   contributors may be used to endors or promote products derived\n");
	PERR("   from this software without specific prior written permission.\n\n");
	PERR("THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS\n");
	PERR("\"AS IS\" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT\n");
	PERR("LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS\n");
	PERR("FOR A PARTICULAR PURPOSE ARE DISCLAIMED, IN NO EVENT SHALL THE\n");
	PERR("COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,\n");
	PERR("INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,\n");
	PERR("BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;\n");
	PERR("LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER\n");
	PERR("CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT\n");
	PERR("LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN\n");
	PERR("ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE\n");
	PERR("POSSIBILITY OF SUCH DAMAGE.\n\n");
}
