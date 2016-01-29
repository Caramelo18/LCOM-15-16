#include "test5.h"
#include "pixmap.h"
#include <minix/drivers.h>
#include <minix/syslib.h>


static int proc_args(int argc, char *argv[]);
static void print_usage(char *argv[]);
static unsigned short parse_ushrt(char *str, int base);
static unsigned long parse_ulong(char *str, int base);
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

	return 0;
}

static void print_usage(char *argv[])
{
	printf(" Usage: one of the following:\n"
			"\t service run %s -args \"init <mode> <delay>\" \n"
			"\t service run %s -args \"square <x> <y> <size> <color>\"  \n"
			"\t service run %s -args \"line <xi> <yi> <xf> <yf> <color>\" \n"
			"\t service run %s -args \"xpm <xi> <yi> <xpm>\"\n"
			"\t service run %s -args \"move <xi> <yi> <xpm> <hor> <delta> <time>\"\n"
			"\t service run %s -args \"controller\"\n",
			argv[0], argv[0], argv[0], argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char *argv[])
{

	unsigned short mode, delay, x, y, size, xi, yi, xf, yf, hor, time;
	short delta;
	unsigned long color;
	char *str;
	long num;

	/* check the function to test: if the first characters match, accept it */
	if (strncmp(argv[1], "init", strlen("init")) == 0)
	{
		if( argc != 4 ) {
			printf("video: wrong no of arguments for test of test_init \n");
			return 1;
		}
		if ((mode = parse_ushrt(argv[2], 16)) == USHRT_MAX )
			return 1;
		else if ((delay = parse_ushrt(argv[3],10)) == USHRT_MAX)
			return 1;
		test_init(mode, delay);
		return 0;

	}
	else if (strncmp(argv[1], "square", strlen("square")) == 0)
	{
		if( argc != 6 ) {
			printf("video: wrong no of arguments for test of test_square \n");
			return 1;
		}
		if ((x = parse_ushrt(argv[2], 10)) == USHRT_MAX )
		{
			printf("%d \n %d\n", x,USHRT_MAX);
			return 1;
		}
		else if ((y = parse_ushrt(argv[3],10)) == USHRT_MAX)
			return 1;
		else if ((size = parse_ushrt(argv[4],10)) == USHRT_MAX)
			return 1;
		else if ((color = parse_ulong(argv[5],16)) == ULONG_MAX)
			return 1;
		return test_square(x, y, size, color);
	}

	else if (strncmp(argv[1], "line", strlen("line")) == 0)
	{
		if (argc != 7) {
			printf("video: wrong no of arguments for test of test_line \n");
			return 1;
		}
		if ((xi = parse_ushrt(argv[2], 10)) == USHRT_MAX )
			return 1;
		else if ((yi = parse_ushrt(argv[3],10)) == USHRT_MAX)
			return 1;
		else if ((xf = parse_ushrt(argv[4],10)) == USHRT_MAX)
			return 1;
		else if ((yf = parse_ushrt(argv[5],10)) == USHRT_MAX)
			return 1;
		else if ((color = parse_ulong(argv[6],16)) == ULONG_MAX)
			return 1;
		return test_line(xi, yi, xf, yf, color);
	}
	else if (strncmp(argv[1], "xpm", strlen("xpm")) == 0)
	{
		if (argc != 5) {
			printf("video: wrong no of arguments for test of test_xpm \n");
			return 1;
		}

		if ((xi = parse_ushrt(argv[2], 10)) == USHRT_MAX)
			return 1;
		else if ((yi = parse_ushrt(argv[3],10)) == USHRT_MAX)
			return 1;
		else if((strncmp(argv[4], "pic1", strlen("pic1")) == 0))
			return test_xpm(xi, yi, pic1);
		else if((strncmp(argv[4], "pic2", strlen("pic2")) == 0))
			return test_xpm(xi, yi, pic2);
		else if((strncmp(argv[4], "pic3", strlen("pic3")) == 0))
			return test_xpm(xi, yi, pic3);
		else if((strncmp(argv[4], "cross", strlen("cross")) == 0))
			return test_xpm(xi, yi, cross);
		else if((strncmp(argv[4], "penguin", strlen("penguin")) == 0))
			return test_xpm(xi, yi, penguin);
		else
		{
			printf("video: invalid xpm mode, please choose one of the following xpm modes: pic1, pic2, pic3, cross or penguin \n");
			return 1;
		}
	}
	else if (strncmp(argv[1], "move", strlen("move")) == 0)
	{
		if (argc != 8) {
			printf("video: wrong no of arguments for test of test_move \n");
			return 1;
		}
		if ((xi = parse_ushrt(argv[2], 10)) == USHRT_MAX )
			return 1;
		else if ((yi = parse_ushrt(argv[3],10)) == USHRT_MAX)
			return 1;
		else if ((hor = parse_ushrt(argv[5],10)) == USHRT_MAX)
			return 1;
		else if ((delta = parse_shrt(argv[6],10)) == SHRT_MAX)
			return 1;
		else if ((time = parse_ulong(argv[7],10)) == ULONG_MAX)
			return 1;
		else if((strncmp(argv[4], "pic1", strlen("pic1")) == 0))
			return test_move(xi, yi, pic1, hor, delta, time);
		else if((strncmp(argv[4], "pic2", strlen("pic2")) == 0))
			return test_move(xi, yi, pic2, hor, delta, time);
		else if((strncmp(argv[4], "pic3", strlen("pic3")) == 0))
			return test_move(xi, yi, pic3, hor, delta, time);
		else if((strncmp(argv[4], "cross", strlen("cross")) == 0))
			return test_move(xi, yi, cross, hor, delta, time);
		else if((strncmp(argv[4], "penguin", strlen("penguin")) == 0))
			return test_move(xi, yi, penguin, hor, delta, time);
		else
		{
			printf("video: invalid xpm mode, please choose one of the following xpm modes: pic1, pic2, pic3, cross or penguin \n");
			return 1;
		}

	}
	else if (strncmp(argv[1], "controller", strlen("controller")) == 0)
	{
		if (argc != 2) {
			printf("video: wrong no of arguments for test of test_controller \n");
			return 1;
		}

		return test_controller();
	}
	else
	{
		printf("video: non valid function \"%s\" to test\n", argv[1]);
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


static unsigned long parse_ulong(char *str, int base)
{
	char *endptr;
	unsigned long val;

	val = strtoul(str, &endptr, base);

	if ((errno == ERANGE && val == ULONG_MAX )
			|| (errno != 0 && val == 0)) {
		perror("strtol");
		return ULONG_MAX;
	}

	if (endptr == str) {
		printf("video_txt: parse_ulong: no digits were found in %s \n", str);
		return ULONG_MAX;
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
