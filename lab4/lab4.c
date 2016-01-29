#include "test4.h"

static int proc_args(int argc, char *argv[]);
static void print_usage(char *argv[]);
static unsigned short parse_ushrt(char *str, int base);
static short parse_shrt(char *str, int base);


int main(int argc, char **argv)
{
	sef_startup();

	if ( argc == 1 ) {
		print_usage(argv);
		return 0;
	} else {
		proc_args(argc, argv);
	}
	printf("Back to main\n");
	return 0;
}

static void print_usage(char *argv[]) {
	printf(" Usage: one of the following:\n"
			"\t service run %s -args \"packet <count>\" \n"
			"\t service run %s -args \"async <idle_time>\"  \n"
			"\t service run %s -args \"config\" \n"
			"\t service run %s -args \"gesture <length> <tolerance>\"\n",
			argv[0], argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char *argv[]) {

	unsigned short cnt, n1, length, tolerance;
	char *str;
	long num;

	/* check the function to test: if the first characters match, accept it */
	if (strncmp(argv[1], "packet", strlen("packet")) == 0)
	{
		if( argc != 3 ) {
			printf("mouse: wrong no of arguments for test of test_packet \n");
			return 1;
		}
		if( (cnt = parse_ushrt(argv[2], 10)) == USHRT_MAX )
			return 1;
		return test_packet(cnt);

	}
	else if (strncmp(argv[1], "async", strlen("async")) == 0)
	{
		if( argc != 3 ) {
			printf("mouse: wrong no of arguments for test of test_async \n");
			return 1;
		}
		if((n1 = parse_ushrt(argv[2], 10)) == USHRT_MAX )
			return 1;
		return test_async(n1);
	}
	else if (strncmp(argv[1], "config", strlen("config")) == 0) {
		if (argc != 2) {
			printf("mouse: wrong no of arguments for test of test_config \n");
			return 1;
		}
		return test_config();
	}
	else if (strncmp(argv[1], "gesture", strlen("gesture")) == 0) {
		if (argc != 4) {
			printf("mouse: wrong no of arguments for test of test_gesture \n");
			return 1;
		}
		if ((length = parse_shrt(argv[2], 10)) == SHRT_MAX)
			return 1;
		else if ((tolerance = parse_ushrt(argv[3],10)) == USHRT_MAX)
			return 1;

		return test_gesture(length, tolerance);
	}
	else
	{
		printf("mouse: non valid function \"%s\" to test\n", argv[1]);
		return 1;
	}
}


static unsigned short parse_ushrt(char *str, int base)
{
	char *endptr;
	unsigned short val;

	val = strtoul(str, &endptr, base);

	if ((errno == ERANGE && val == USHRT_MAX )
			|| (errno != 0 && val == 0)) {
		perror("strtol");
		return USHRT_MAX;
	}

	if (endptr == str) {
		printf("mouse: parse_ushrt: no digits were found in %s \n", str);
		return USHRT_MAX;
	}

	/* Successful conversion */
	return val;
}

static short parse_shrt(char *str, int base)
{
	char *endptr;
	short val;

	val = strtoul(str, &endptr, base);

	if ((errno == ERANGE && val == SHRT_MAX )
			|| (errno != 0 && val == 0)) {
		perror("strtol");
		return SHRT_MAX;
	}

	if (endptr == str) {
		printf("mouse: parse_ushrt: no digits were found in %s \n", str);
		return SHRT_MAX;
	}

	/* Successful conversion */
	return val;
}
