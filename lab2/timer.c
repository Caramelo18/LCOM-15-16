#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include "i8254.h"

static int TIMER_HOOK = 0;
static int c = 0;

int timer_set_square(unsigned long timer, unsigned long freq)
{
	if(timer < 0 || timer > 2 || freq == 0)
		return 1;

	unsigned long sta;
	unsigned char wave;
	if (timer == 2)
	{
		sys_outb(TIMER_CTRL, (TIMER_RB_CMD | TIMER_RB_COUNT_ | BIT(3)));
		sys_inb(TIMER_2, &sta);
	}

	else if (timer == 1)
	{
		sys_outb(TIMER_CTRL, (TIMER_RB_CMD | TIMER_RB_COUNT_ | BIT(2)));
		sys_inb(TIMER_1, &sta);
	}

	else if (timer == 0)
	{
		sys_outb(TIMER_CTRL, (TIMER_RB_CMD | TIMER_RB_COUNT_ | BIT(1)));
		sys_inb(TIMER_0, &sta);
	}

	wave = (unsigned char)sta;  // the code above reads the previous configurations

	wave = (wave & 0x0F);  // only keeps the 4 least significant bits
	wave = (wave | TIMER_LSB_MSB); // selects the LSB followed by MSB type of access

	if(timer == 1)
	{
		wave = (wave | TIMER_SEL1);
	}
	else if (timer == 2)
	{
		wave = (wave | TIMER_SEL2);
	}      // selects the counter to be controlled (the timer 0 is selected by default)

	sys_outb(TIMER_CTRL, wave); // writes the control word in the controller

	unsigned long div = TIMER_FREQ/freq;

	if(timer == 0)
	{
		sys_outb(TIMER_0, div);
		sys_outb(TIMER_0, (div >> 8));
	}
	else if (timer == 1)
	{
		sys_outb(TIMER_1, div);
		sys_outb(TIMER_1, (div >> 8));
	}
	else if(timer == 2)
	{
		sys_outb(TIMER_2, div);
		sys_outb(TIMER_2, (div >> 8));
	}                         // loads the initial value into the timer
	else
	{
		printf("Error \n");
		return 1;
	}

	return 0;
}

int timer_subscribe_int(void)
{
	int th = TIMER_HOOK;

	if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &TIMER_HOOK) == 0)
		if(sys_irqenable(&TIMER_HOOK) == 0)
		{
			return BIT(th);
		}
	return -1;
}

int timer_unsubscribe_int()
{
	if (sys_irqdisable(&TIMER_HOOK) == 0)
		if (sys_irqrmpolicy(&TIMER_HOOK) == 0)
			return 0;
	return 1;
}

void timer_int_handler()
{
	c++;
}


int timer_get_conf(unsigned long timer, unsigned char *st)
{
	unsigned long stx;
	if (timer == 2)
	{
		sys_outb(TIMER_CTRL, 0xE8);
		sys_inb(TIMER_2, &stx);
		*st = (unsigned char)stx;
	}

	else if (timer == 1)
	{
		sys_outb(TIMER_CTRL, 0xE4);
		sys_inb(TIMER_1, &stx);
		*st = (unsigned char)stx;
	}

	else if (timer == 0)
	{
		sys_outb(TIMER_CTRL, 0xE2);
		sys_inb(TIMER_0, &stx);
		*st = (unsigned char)stx;
	}

	else
	{
		printf("Error\n");
		return 1;
	}


	return 0;
}

int timer_display_conf(unsigned char conf)
{
	// printf("conf: %x\n",conf); //prints the word received from the control

	//BCD
	if ((conf & TIMER_BCD) == TIMER_BCD)
		printf("BCD: on\n");
	else
		printf("BCD: off\n");

	//Output
	if ((conf & TIMER_OUTPUT) == TIMER_OUTPUT)
		printf("Output: on\n");
	else
		printf("Output: off\n");

	//Null Count
	if ((conf & TIMER_NULLCOUNT) == TIMER_NULLCOUNT)
		printf("Null Count: on\n");
	else
		printf("Null Count: off\n");

	//Type of access
	if ((conf & TIMER_TYPEOFACCES) == TIMER_LSB)
		printf("Type of access: LSB\n");

	else if ((conf & TIMER_TYPEOFACCES) == TIMER_MSB)
		printf("Type of access: MSB\n");

	else if ((conf & TIMER_TYPEOFACCES) == TIMER_LSB_MSB)
		printf("Type of access: LSB followed by MSB\n");

	//Programed mode

	if ((conf & TIMER_PROGRAMEDMODE) == 0x02)
		printf("Programed mode: 1\n");

	else if (((conf & TIMER_PROGRAMEDMODE) == TIMER_RATE_GEN) || ((conf & 0x0E) == 0x0C))
		printf("Programed mode: 2\n");

	else if (((conf & TIMER_PROGRAMEDMODE) == TIMER_SQR_WAVE) || ((conf & 0x0E) == 0x0E))
		printf("Programed mode: 3\n");

	else if ((conf & TIMER_PROGRAMEDMODE) == 0x08)
		printf("Programed mode: 4\n");

	else if ((conf & TIMER_PROGRAMEDMODE) == 0x0A)
		printf("Programed mode: 5\n");

	else
		printf("Programed mode: 0\n");

	return 0;
}

int timer_test_square(unsigned long freq)
{
	if (timer_set_square(0, freq) == 0)
		return 0;
	return 1;
}

int timer_test_int(unsigned long time)
{

	int ipc_status;
	message msg;
	int r;


	int irq_set = timer_subscribe_int();
	int count = 0;

	printf("irq_set: %x\n", irq_set);
	while(c < time) /* You may want to use a different condition */
	{
		count++;
		//printf (" %d", count);
		/* Get a request message. */
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0 )
		{
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status))
		{ /* received notification */
			switch (_ENDPOINT_P(msg.m_source))
			{
			case HARDWARE: /* hardware interrupt notification */
				if ((msg.NOTIFY_ARG & irq_set)&& count == 60)
				{ /* subscribed interrupt */
					printf("LALA \n");
					count = 0;
					timer_int_handler();
				}
				break;
			default:
				break; /* no other notifications expected: do nothing */
			}
		}
		else
		{ /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}
	}
	timer_unsubscribe_int();
	return 1;
}

int timer_test_config(unsigned long timer)
{
	char conf;
	if (timer_get_conf(timer, &conf) == 0)
	{
		timer_display_conf(conf);
		return 0;
	}
	return 1;
}
