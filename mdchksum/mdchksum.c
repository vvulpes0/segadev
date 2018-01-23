/*****************************************************************************
 * mdchksum: read and fix checksums on Sega Genesis / Mega Drive roms
 * See the file ``COPYING'' for license information.
 *************************************************************************** */
#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

enum {
	E_SUCCESS   = 0,
	E_USAGE     = 1,
	E_NXFILE    = 2,
	E_OUTPUT    = 3,
	E_READ      = 4,
	E_NOMEM     = 5
};
enum {
	M_CALC   = 1,
	M_READ   = 2,
	M_WRITE  = 4,
	M_FIX    = 8
};

#define CHECKSUM_LOCATION    0x18e
#define CHECKSUM_SIZE        0x002
#define DATA_START           0x200
#define SIZE_FIELD_LOCATION  0x1a4
#define SIZE_FIELD_SIZE      0x004
#define ROM_HEADER_SIZE      0x200

static unsigned int calculate_checksum(unsigned char const * const restrict, size_t const);
static unsigned int find_stored_checksum(unsigned char const * const restrict, size_t const);
static void fix_checksum(unsigned char * const restrict, size_t const, unsigned int const);
static void print_help(void);
static void print_version(void);

int
main(int argc, char* argv[])
{
	/* buffer to store rom header and complete ROM */
	unsigned char rom_header[ROM_HEADER_SIZE];
	unsigned char *rom;
	/* input file name, used as output for in-place operations */
	char *fname      = NULL;
	/* option character */
	int c;
	/* an error has occurred */
	int errflag      = 0;
	/* help is requested */
	int hflag        = 0;
	/* loop counter */
	int i;
	/* in-place operation */
	int iflag        = 0;
	/* operation to perform */
	int mode         = 0;
	/* length of ROM file in bytes, as read from the header */
	int rom_size     = 0;
	/* want version info */
	int vflag        = 0;
	/* data to write in explicit mode */
	int wnum         = 0;

	/* We do not use stderr
	 * */
	fclose(stderr);

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
	if (freopen(fname, "rb", stdin) == NULL)
	{
		exit(E_NXFILE);
	}

	/* Read the ROM header into memory */
	if (fread(rom_header, 1, ROM_HEADER_SIZE, stdin) < ROM_HEADER_SIZE)
	{
		exit(E_READ);
	}
	for (i = 0; i < SIZE_FIELD_SIZE; ++i)
	{
		rom_size *= 256;
		rom_size += rom_header[SIZE_FIELD_LOCATION + i];
	}
	rom = calloc(rom_size, 1);
	if (rom == NULL)
	{
		exit(E_NOMEM);
	}
	memcpy(rom, rom_header, ROM_HEADER_SIZE);
	if (fread(rom+ROM_HEADER_SIZE, 1, rom_size - ROM_HEADER_SIZE, stdin)
	    < rom_size - ROM_HEADER_SIZE)
	{
		exit(E_READ);
	}
	/* If we are here, then `rom' contains the ROM image
	 * and we assume it is safe to proceed.
	 * We are also done reading, so we can close stdin.
	 * */
	fclose(stdin);

	/* If not inplace OR not editing the file,
	 * discard output information
	 * */
	if (!iflag || !(mode & (M_FIX | M_WRITE)))
	{
		fname = NULL;
	}
	if (freopen(fname, "wb", stdout) == NULL)
	{
		exit(E_OUTPUT);
	}

	/* Perform operations on ROM data
	 * */
	switch (mode)
	{
	case M_CALC:
		printf("0x%04x\n", calculate_checksum(rom, rom_size));
		break;
	case M_FIX:
		fix_checksum(rom, rom_size, calculate_checksum(rom, rom_size));
		break;
	case M_READ:
		printf("0x%04x\n", find_stored_checksum(rom, rom_size));
		break;
	case M_WRITE:
		fix_checksum(rom, rom_size, wnum);
		break;
	default:
		break;
	}

	if (mode & (M_WRITE | M_FIX))
	{
		fwrite(rom, 1, rom_size, stdout);
	}
	fclose(stdout);

	exit(E_SUCCESS);
}

static unsigned int
calculate_checksum(unsigned char const * const restrict buffer,
                   size_t const size)
{
	unsigned int checksum = 0;
	int i;

	for (i = DATA_START; i < size; i += 2)
	{
		checksum += buffer[i] * 256;
		checksum += buffer[i + 1];
		checksum %= 65536;
	}

	return checksum;
}

static unsigned int
find_stored_checksum(unsigned char const * const restrict buffer,
                     size_t const size)
{
	unsigned int stored_sum = 0;
	int i;

	if (size < CHECKSUM_LOCATION + CHECKSUM_SIZE)
	{
		/* checksum location is not in buffer */
		return 0;
	}

	/* Read big-endian number */
	for (i = CHECKSUM_LOCATION;
	     i < CHECKSUM_LOCATION + CHECKSUM_SIZE;
	     ++i)
	{
		stored_sum *= 256;
		stored_sum += buffer[i];
	}
	return stored_sum;
}

static void
fix_checksum(unsigned char * const restrict buffer,
             size_t size,
             unsigned int const checksum)
{
	int i;
	unsigned int cksum = checksum;

	if (size < CHECKSUM_LOCATION + CHECKSUM_SIZE)
	{
		/* checksum location is not in buffer */
		return;
	}
	/* Write big-endian number */
	for (i = CHECKSUM_SIZE - 1; i >= 0; --i)
	{
		buffer[CHECKSUM_LOCATION + i] = cksum % 256;
		cksum /= 256;
	}
}

static void
print_help(void)
{
	puts("Usage: mdchksum [-c|-f|-r|-w num] [-i] [-?V] [file]");
	puts("Read and write SEGA Genesis / Mega Drive ROM checksums.");
	puts("");
	puts("  -c     : compute checksum.");
	puts("  -f     : fix checksum.");
	puts("  -i     : operate in-place.");
	puts("  -r     : read stored checksum.");
	puts("  -w num : overwrite the checksum with num.");
	puts("  -?     : display this message.");
	puts("  -V     : display version information.");
}

static void
print_version(void)
{
	puts("mdchksum 1.2");
}
