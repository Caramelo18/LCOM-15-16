#include <minix/drivers.h>


#include "test3.h"

static int proc_args(int argc, char *argv[]);
static void print_usage(char *argv[]);
static unsigned short parse_ushrt(char *str, int base);
unsigned short* parse_ushrta(unsigned short num, unsigned short n);

int main(int argc, char **argv)
{
    sef_startup();

    /* Enable IO-sensitive operations for ourselves */
    sys_enable_iop(SELF);

	if ( argc == 1 ) {
		print_usage(argv);
		return 0;
	} else {
		proc_args(argc, argv);
	}

	return 0;
}

static void print_usage(char *argv[]) {
	printf("Usage: one of the following:\n"
			"\t service run %s -args \"scan <asm> (0 to C, 1 to assembly)\" \n"
			"\t service run %s -args \"leds <size> <array> \n"
			"\t service run %s -args \"timedscan <time> \n",
			argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char *argv[]) {

	unsigned short ass, n1, n2;
	unsigned short *e1;
	char *str;
	long num;

	/* check the function to test: if the first characters match, accept it */
	if (strncmp(argv[1], "scan", strlen("scan")) == 0)
	{
		if( argc != 3 ) {
			printf("keyboard: wrong no of arguments for test of kbd_test_scan \n");
			return 1;
		}
		if( (ass = parse_ushrt(argv[2], 10)) == USHRT_MAX )
			return 1;
		else if (ass == 0 || ass == 1)
			return kbd_test_scan(ass);
		else
		{
			printf("invalid argument \n");
			return 1;
		}
	}
	else if (strncmp(argv[1], "leds", strlen("leds")) == 0)
	{
		if( argc != 4 ) {
			printf("keyboard: wrong no of arguments for test of kbd_test_leds \n");
			return 1;
		}
		if((n1 = parse_ushrt(argv[2], 10)) == USHRT_MAX )
			return 1;
		unsigned short x = parse_ushrt(argv[3], 10);
		unsigned short *e1 = parse_ushrta(n1, x);
		return kbd_test_leds(n1, e1);
	}
	else if (strncmp(argv[1], "timedscan", strlen("timedscan")) == 0) {
		if( argc != 3) {
			printf("keyboard: wrong no of arguments for test of kbd_test_timed_scan \n");
			return 1;
		}
		if( (n2 = parse_ushrt(argv[2], 10)) == USHRT_MAX )
			return 1;
		return kbd_test_timed_scan(n2);
	}
	else
	{
		printf("keyboard: non valid function \"%s\" to test\n", argv[1]);
		return 1;
	}
}

static unsigned short parse_ushrt(char *str, int base) {
	char *endptr;
	unsigned short val;

	val = strtoul(str, &endptr, base);

	if ((errno == ERANGE && val == USHRT_MAX )
			|| (errno != 0 && val == 0)) {
		perror("strtol");
		return USHRT_MAX;
	}

	if (endptr == str) {
		printf("video_txt: parse_ulong: no digits were found in %s \n", str);
		return USHRT_MAX;
	}

	/* Successful conversion */
	return val;
}

unsigned short* parse_ushrta(unsigned short num, unsigned short ita)
{
	unsigned short *arr = malloc(sizeof(unsigned short) * num);
	unsigned int i = 1;
	for (i; i <= num; i++)
	{
		arr[num - i] = ita % 10;
		ita = ita / 10;
	}

	return arr;
}
