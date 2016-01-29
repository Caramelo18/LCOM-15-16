#include "test6.h"

int main()
{
	sef_startup();

	rtc_test_conf();

	printf("\n\n");

	rtc_test_date();

	rtc_test_int();
}
