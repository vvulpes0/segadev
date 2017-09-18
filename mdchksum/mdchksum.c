/*****************************************************************************
 * mdchksum: read and fix checksums on Sega Genesis / Mega Drive roms
 * See the file ``COPYING'' for license information.
 *************************************************************************** */
#define _POSIX_C_SOURCE 2
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

enum {
	E_SUCCESS  = 0,
	E_USAGE    = 1,
	E_NXFILE   = 2
};
enum {
	M_CALC   = 1,
	M_READ   = 2,
	M_WRITE  = 4,
	M_FIX    = 8
};

#define CHECKSUM_LOCATION 0x18E
#define DATA_START 0x200

static unsigned int calculate_checksum(FILE*);
static int cp(FILE *from, FILE *to);
static unsigned int find_stored_checksum(FILE*);
static void fix_checksum(FILE*, unsigned int);
static void print_help(void);
static void print_version(void);

int
main(int argc, char* argv[])
{
	char *fname     = NULL;
	FILE *tempfile  = tmpfile();
	int c;
	int errflag     = 0;
	int hflag       = 0;
	int iflag       = 0;
	int mode        = 0;
	int vflag       = 0;
	int wnum        = 0;

	while ((c = getopt(argc, argv, ":cfirVw:")) != -1)
	{
		switch (c)
		{
		case 'c':
			if ((mode |= M_CALC) & ~M_CALC)
			{
				errflag = 1;
			}
			break;
		case 'f':
			if ((mode |= M_FIX) & ~M_FIX)
			{
				errflag = 1;
			}
			break;
		case 'i':
			iflag = 1;
			break;
		case 'r':
			if ((mode |= M_READ) & ~M_READ)
			{
				errflag = 1;
			}
			break;
		case 'V':
			vflag = 1;
			break;
		case 'w':
			if ((mode |= M_WRITE) & ~M_WRITE)
			{
				errflag = 1;
			}
			sscanf(optarg, "%i", &wnum);
			break;
		case '?':
		default:
			hflag = 1;
			errflag = (optopt != '?');
			break;
		}
	}

	if (hflag | errflag)
	{
		print_help();
		exit(errflag ? E_USAGE : E_SUCCESS);
	}
	if (vflag)
	{
		print_version();
		exit(E_SUCCESS);
	}
	if (!mode)
	{
		mode = M_CALC;  /* Default mode is "calculate" */
	}
	if ((optind < argc) && (strncmp(argv[optind], "-", 2) != 0))
	{
		fname = argv[optind];
	}
	else
	{
		fname = NULL;
	}
	stdin = freopen(fname, "rb", stdin);
	if (stdin == NULL)
	{
		exit(E_NXFILE);
	}
	cp(stdin, tempfile);
	if (iflag && (fname != NULL) && (mode & (M_FIX | M_WRITE)))
	{
		stdin = freopen(fname, "rb+", stdin);
		if (stdin != NULL)
		{
			stdout = stdin;
		}
	}
	stdin = tempfile;

	switch (mode)
	{
	case M_CALC:
		printf("0x%04x\n", calculate_checksum(stdin));
		break;
	case M_FIX:
		fix_checksum(stdin, calculate_checksum(stdin));
		cp(stdin, stdout);
		break;
	case M_READ:
		printf("0x%04x\n", find_stored_checksum(stdin));
		break;
	case M_WRITE:
		fix_checksum(stdin, wnum);
		cp(stdin, stdout);
		break;
	default:
		break;
	}
	exit(E_SUCCESS);
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
find_stored_checksum(FILE *stream)
{
	unsigned int stored_sum = 0;

	fseek(stream, CHECKSUM_LOCATION, SEEK_SET);
	stored_sum = (unsigned int)fgetc(stream) * 256;
	stored_sum += (unsigned int)fgetc(stream);
	return stored_sum;
}

static void
fix_checksum(FILE *stream, unsigned int checksum)
{
	fseek(stream, CHECKSUM_LOCATION, SEEK_SET);
	fputc(((int)checksum / 256) % 256, stream);
	fputc((int)checksum % 256, stream);
	return;
}

static void
print_help(void)
{
	puts("Usage: mdchksum [-c|-f|-r|-w num] [-i] [-?V] [file]");
	puts("Read and write checksums in SEGA Genesis / Mega Drive ROMs.");
	puts("");
	puts("  -c     : compute checksum.");
	puts("  -f     : fix checksum.");
	puts("  -i     : operate in-place.");
	puts("  -r     : read stored checksum.");
	puts("  -w num : overwrite the checksum with num");
	puts("  -?     : display this message");
	puts("  -V     : display version information");
}

static void
print_version(void)
{
	puts("mdchksum 2.0");
}

static int
cp(FILE *from, FILE *to)
{
	char buf[4096];
	char *out = buf;
	size_t n_read;
	size_t n_written;

	rewind(from);
	rewind(to);

	while ((n_read = fread(buf, 1, sizeof(buf), from)) > 0)
	{
		out = buf;
		while (n_read > 0)
		{
			n_written = fwrite(out, 1, n_read, to);
			n_read -= n_written;
			out += n_written;
		}
	}

	return 1;
}
