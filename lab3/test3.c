#include "test3.h"
void _handler_asm();
extern unsigned long val;

#define OUT_BUF 0x60
#define STAT_REG 0x64
#define KBC_CMD_REG 0x64
#define KBD1 1
#define OBF 0x01
#define IBF 0x02
#define PAR_ERR 0x80
#define TO_ERR 0x40
#define ACK 0xFA
#define RESEND 0xFE
#define ERROR 0xFC
#define WAIT_KBC 20000
#define SWITCH_LEDS 0xED
#define BIT(n) (0x01<<(n))
#define TIMER0_IRQ 0

static int HOOK = 0;
static int TIMER_HOOK = 1;

int keyboard_subscribe()
{
	int hb = HOOK;

	if(sys_irqsetpolicy(KBD1 ,IRQ_REENABLE|IRQ_EXCLUSIVE, &HOOK) == 0)
		if(sys_irqenable(&HOOK) == 0)
		{
			return BIT(hb);
		}
	return -1;
}


int kbd_test_scan(unsigned short ass)
{
	if(ass == 0)
	{

		int kbc_set = keyboard_subscribe();

		if(kbc_set == -1)
			return 1;

		unsigned long stat, data;
		data == 0x00;
		while( data != 0x81 )
		{
			/* assuming it returns OK */
			/* loop while 8042 output buffer is empty */
			int ipc_status;
			message msg;
			int r;

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
					if ((msg.NOTIFY_ARG & kbc_set))
					{ /* subscribed interrupt */
						sys_inb(STAT_REG, &stat);
						if ( (stat &(PAR_ERR | TO_ERR)) == 0 )
						{
							sys_inb(OUT_BUF, &data);
							if((data & 0x80) == 0x80) // if MSB is set, it's a break code
								printf("Breakcode: 0x%x\n", data);
							else printf("Makecode: 0x%x\n", data);
						}
						else printf ("fail \n");
						tickdelay(micros_to_ticks(WAIT_KBC));
					}
					break;
				}
			}
		}
		if(sys_irqdisable(&HOOK) == 0)
			sys_irqrmpolicy(&HOOK);

		return 0;
	}

	else
	{
		int kbc_set = keyboard_subscribe();

		if(kbc_set == -1)
			return 1;

		unsigned long stat, x;
		x == 0x00;
		while( x != 0x81 )
		{
			/* assuming it returns OK */
			/* loop while 8042 output buffer is empty */
			int ipc_status;
			message msg;
			int r;

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
					if ((msg.NOTIFY_ARG & kbc_set))
					{ /* subscribed interrupt */
						handler_asm();
						x = val;
						sys_inb(STAT_REG, &stat);
						if ( (stat &(PAR_ERR | TO_ERR)) == 0 )
						{
							if((x & 0x80) == 0x80) // if MSB is set, it's a break code
								printf("Breakcode: 0x%x\n", x);
							else printf("Makecode: 0x%x\n", x);
						}
						else printf ("fail \n");
						tickdelay(micros_to_ticks(WAIT_KBC));
					}
					break;
				}
			}
		}
		if(sys_irqdisable(&HOOK) == 0)
			sys_irqrmpolicy(&HOOK);

		return 0;
	}


}

int kbd_check_response(unsigned long *d)
{
	unsigned long sta;
	unsigned int t = 0;
	while (t < 10) // prevents an infinite loop
	{
		if (sys_inb(STAT_REG, &sta) != OK)
			return 1;

		if((sta & OBF) != 0)
		{
			sys_inb(OUT_BUF,d);
			if ((sta & (PAR_ERR | TO_ERR)) == 0x00)
			{
				return 0;
			}
			else
			{
				return 1;
			}
		}
		tickdelay(micros_to_ticks(WAIT_KBC));
		t++;
	}
}

int kbd_write_command (unsigned long command)
{
	int t;
	unsigned long sta;

	while(t < 10)
	{
		if(sys_inb(STAT_REG, &sta) == OK)
		{
			if((sta & IBF) == 0)
			{
				if(sys_outb(0x60, command) == OK)
					return 0;
				else
					return 1;
			}
		}
		t++;
		tickdelay(micros_to_ticks(WAIT_KBC));
	}
	return 1;
}

int timer_subscribe_int(void)
{
	int th =TIMER_HOOK;
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


int kbd_test_leds(unsigned short n, unsigned short *leds)
{
	int irq;
	if((irq = timer_subscribe_int()) ==  -1)
		return -1;
	unsigned long temp;
	tickdelay(micros_to_ticks(400000));
	sys_inb(OUT_BUF,&temp); //clears the buffer

	keyboard_subscribe();

	int counter = 0;
	unsigned int i = 0;
	unsigned long data = 0;
	unsigned short led_config = 0x00;
	message msg;
	int r;
	int ipc_status;
	while(i < n)
	{
		counter ++;

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
				if ((msg.NOTIFY_ARG & irq) && counter == 60)
				{
					if (kbd_write_command(SWITCH_LEDS) == 0)
					{
						if(kbd_check_response(&data) == 0)
						{
							while (data != ACK)
							{
								//printf("data:%x \n", data);
								if(data == ERROR)
								{
									printf("Erro \n");
									if (kbd_write_command(SWITCH_LEDS) == 0)
										kbd_check_response(&data);
								}
								else if(data == RESEND)
								{
									printf("Resend \n");
									kbd_check_response(&data);
								}
							}
							led_config = led_config ^ BIT(leds[i]);
							data = ERROR;
							do
							{
								if(data == ERROR)
								{
									if (kbd_write_command(led_config) == 0)
										kbd_check_response(&data);
								}
								if(data == RESEND)
									kbd_check_response(&data);
							}while (data != ACK);
							i++;
							printf("Hi \n");
							counter = 0;
						}
					}
				}
			}
		}
	}
	timer_unsubscribe_int();
	if(sys_irqdisable(&HOOK) == 0)
		sys_irqrmpolicy(&HOOK);
	return 0;
}
int kbd_test_timed_scan(unsigned short n)
{
	int timer_irq, kbd_irq;
	if((timer_irq = timer_subscribe_int()) ==  -1 || (kbd_irq = keyboard_subscribe()) == -1)
		return -1;

	unsigned int counter = 1;
	unsigned int s = 0;
	unsigned long stat, data;

	while(counter <= n && data != 0x81)
	{
		s++;
		/* assuming it returns OK */
		/* loop while 8042 output buffer is empty */
		int ipc_status;
		message msg;
		int r;

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
				if ((msg.NOTIFY_ARG & kbd_irq))
				{ /* subscribed interrupt */
					sys_inb(STAT_REG, &stat);
					if ( (stat &(PAR_ERR | TO_ERR)) == 0 )
					{
						sys_inb(OUT_BUF, &data);
						if((data & 0x80) == 0x80) // if MSB is set, it's a break code
							printf("Breakcode: 0x%x\n", data);
						else printf("Makecode: 0x%x\n", data);
						counter = 1;
						s = 0;
					}
					else printf ("fail \n");
					tickdelay(micros_to_ticks(WAIT_KBC));
				}
				else if ((msg.NOTIFY_ARG & timer_irq) && s == 60)
				{
					counter++;
					s = 0;
					printf("RESET \n");
				}
				break;
			}
		}
	}

	timer_unsubscribe_int();
	if(sys_irqdisable(&HOOK) == 0)
		sys_irqrmpolicy(&HOOK);
	return 0;
}


