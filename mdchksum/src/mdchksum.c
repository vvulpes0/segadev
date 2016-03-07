/*****************************************************************************
 * mdchksum: read and fix checksums on Sega Genesis / Mega Drive roms
 * See the file ``COPYING'' for license information.
 *************************************************************************** */
#include <stdlib.h>
#include <stdio.h>

#define CHECKSUM_LOCATION 0x18E
#define DATA_START 0x200

static unsigned int calculate_checksum(FILE*);
static unsigned int find_stored_checksum(FILE*);
static void fix_checksum(FILE*, unsigned int);
static void print_help(void);

int
main(int argc, char* argv[])
{
	unsigned int stored;
	unsigned int checksum;
	FILE *stream;
	int fflag = 0;
	int hflag = 0;
	int i = 0;
	int j = 0;

	for (i = 1; i < argc; ++i)
	{
		if (argv[i][0] == '-')
		{
			j = 1;
			while (argv[i][j] != '\0')
			{
				hflag |= argv[i][j] == 'h';
				hflag |= argv[i][j] == '?';
				fflag |= argv[i][j] == 'f';
				++j;
			}
		}
	}
	if ((argv[argc - 1][0] == '-') || (argc < 2) || hflag)
	{
		print_help();
		return hflag ? EXIT_SUCCESS : EXIT_FAILURE;
	}
	stream = fopen(argv[argc - 1], fflag ? "rb+" : "rb");
	if (stream == NULL)
	{
		perror(argv[argc - 1]);
		return EXIT_FAILURE;
	}
	stored = find_stored_checksum(stream);
	checksum = calculate_checksum(stream);
	if (stored == checksum)
	{
		printf("Checksum OK: 0x%04X.\n", checksum );
	}
	else
	{
		printf("Bad checksum: expected 0x%04X, found 0x%04X.\n",
		       checksum, stored);
		if (fflag)
		{
			fix_checksum(stream, checksum);
			puts("Checksum fixed.");
		}
	}
	fclose(stream);
	return EXIT_SUCCESS;
}

static unsigned int
calculate_checksum(FILE *stream)
{
	unsigned int checksum = 0;
	unsigned int to_add = 0;

	fseek(stream, DATA_START, SEEK_SET);
	while (!(feof(stream)) && !(ferror(stream)))
	{
		checksum += to_add;
		checksum %= 65536;
		to_add = (unsigned int)fgetc(stream) * 256;
		to_add += (unsigned int)fgetc(stream); 
	}
	return checksum;
}

static unsigned int
find_stored_checksum(FILE* stream)
{
	unsigned int stored_sum = 0;

	fseek(stream, CHECKSUM_LOCATION, SEEK_SET);
	stored_sum = (unsigned int)fgetc(stream) * 256;
	stored_sum += (unsigned int)fgetc(stream);
	return stored_sum;
}

static void
fix_checksum(FILE* stream, unsigned int checksum)
{
	fseek(stream, CHECKSUM_LOCATION, SEEK_SET);
	fputc((int)checksum / 256, stream);
	fputc((int)checksum % 256, stream);
	return;
}

static void
print_help(void)
{
	puts("Usage: mdchksum [-h|-?] [-f] [FILE]");
	puts("Display or correct standard SEGA checksum in FILE.");
	puts("");
	puts("  -h, -? : Display this message");
	puts("  -f     : Overwrite checksum in the source file");
}
