#include "timer.h"

int timer_subscribe(void)
{
	int th = TIMER_HOOK;

	if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &TIMER_HOOK) == 0)
		if(sys_irqenable(&TIMER_HOOK) == 0)
		{
			return BIT(th);
		}
	return -1;
}

int timer_unsubscribe()
{
	if (sys_irqdisable(&TIMER_HOOK) == 0)
		if (sys_irqrmpolicy(&TIMER_HOOK) == 0)
			return 0;
	return -1;
}

void startTime(char* add, int min)
{
	char* doisp;
	int wp, hp;
	if((doisp = read_xpm(dp, &wp, &hp)) == NULL)
		return;
	drawXPM(add, CLOCKDP, CLOCKY, doisp, wp, hp);


	int sw, sh;
	char* zero;
	if((zero = read_xpm(num0, &sw, &sh)) == NULL)
		return;
	drawXPM(add, CLOCKSEC1, CLOCKY, zero, sw, sh);
	drawXPM(add, CLOCKSEC2, CLOCKY, zero, sw, sh);

	displayMin(add, min);

}

void updateTime(char* add, int* min, int* sec)
{

	*sec -=  1;
	if (*sec == -1)
	{
		*sec = 59;
		*min -= 1;
	}
	displayMin(add, *min);
	displaySec(add, *sec);
}




