#include "test4.h"

#define MOUSE_IRQ 12
#define TIMER0_IRQ 0
#define OUT_BUF 0x60
#define IN_BUF 0x64
#define STAT_REG 0x64
#define IBF 0x02
#define WRITEB 0xD4
#define SSTRM 0xF4
#define DISTRM 0xF5
#define STATREQ 0xE9
#define ACK 0xFA
#define NACK 0xFE
#define ERROR 0xFC
#define WAIT_KBC 20000
#define BIT(n) (0x01<<(n))

static int MOUSE_HOOK = 0;
static int TIMER_HOOK = 1;

int mouse_subscribe()
{
	int mh = MOUSE_HOOK;
	if(sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE|IRQ_EXCLUSIVE, &MOUSE_HOOK) == 0)
		if(sys_irqenable(&MOUSE_HOOK) == 0)
			return BIT(mh);

	return -1;
}

int mouse_unsubscribe()
{
	if(sys_irqdisable(&MOUSE_HOOK) == 0)
		if(sys_irqrmpolicy(&MOUSE_HOOK) == 0)
			return 0;
	return -1;
}

int timer_subscribe(void)
{
	int th = TIMER_HOOK;
	if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &TIMER_HOOK) == 0)
		if(sys_irqenable(&TIMER_HOOK) == 0)
			return BIT(th);

	return -1;
}

int timer_unsubscribe()
{
	if (sys_irqdisable(&TIMER_HOOK) == 0)
		if (sys_irqrmpolicy(&TIMER_HOOK) == 0)
			return 0;
	return -1;
}
/*
int mouse_check_response(unsigned long *d)
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
 */
int mouse_write_command (unsigned long command, unsigned long argument)
{
	int t = 0;
	unsigned long sta;

	while(t < 10)
	{
		if(sys_inb(STAT_REG, &sta) == OK)
		{
			if((sta & IBF) == 0)
			{
				if(sys_outb(IN_BUF, command) == OK)
				{
					tickdelay(micros_to_ticks(WAIT_KBC));
					if(sys_inb(STAT_REG, &sta) == OK)
					{
						if((sta & IBF) == 0)
						{
							if (sys_outb(OUT_BUF, argument) == 0)
							{
								tickdelay(micros_to_ticks(WAIT_KBC));
								if (sys_inb(OUT_BUF, &sta) == 0)
								{
									if (sta == ACK)
										return 0;
								}
							}
						}
					}
				}
			}

		}
		t++;
		tickdelay(micros_to_ticks(WAIT_KBC));
	}
	return -1;
}


int test_packet(unsigned short cnt)
{
	printf("test_packet(%d)\n", cnt);
	int mouse_set = mouse_subscribe();

	if (mouse_set == -1)
		return -1;

	if (mouse_write_command(WRITEB, SSTRM) == -1)
		return -1;

	unsigned long data;
	unsigned long packet[3];
	unsigned short counter = 1;
	unsigned int byt = 0;

	while(counter <= cnt)
	{
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
				if ((msg.NOTIFY_ARG & mouse_set))
				{ /* subscribed interrupt */
					sys_inb(OUT_BUF, &data);
					if((data & BIT(3)) != 0)
					{
						byt = 0;
						packet[byt] = data;
					}
					else
					{
						byt++;
						packet[byt] = data;
					}
					if(byt == 2)
					{
						byt = 0;
						unsigned short lb = 0, mb = 0, rb = 0, xov = 0, yov = 0;
						if (packet[0] & BIT(7))
							yov = 1;
						if (packet[0] & BIT(6))
							xov = 1;
						if (packet[0] & BIT(4))
						{
							packet[1] = packet[1] ^ 0xFF;
							packet[1] += 1;
						}
						if (packet[0] & BIT(5))
						{
							packet[2] = packet[2] ^ 0xFF;
							packet[2] += 1;
						}
						if (packet[0] & BIT(2))
							mb = 1;
						if (packet[0] & BIT(1))
							rb = 1;
						if (packet[0] & BIT(0))
							lb = 1;
						printf("Counter: %d \n", counter);
						printf("B1: 0x%x\n",packet[0]);
						printf("B2: 0x%x\n",packet[1]);
						printf("B3: 0x%x\n",packet[2]);
						printf("LB=%d  MB=%d  RB=%d  XOV=%d  YOV=%d  X=",lb, mb, rb, xov, yov);
						if (packet[0] & BIT(4))
							printf("-");
						printf("%d  Y=", packet[1]);
						if (packet[0] & BIT(5))
							printf("-");
						printf("%d \n\n", packet[2]);
						counter++;
					}
					tickdelay(micros_to_ticks(WAIT_KBC));
					break;
				}
			}
		}
	}
	if (mouse_write_command(WRITEB, DISTRM) == -1)
		return -1;
	if(mouse_unsubscribe() == -1)
		return -1;

	return 0;
}

int test_async(unsigned short idle_time)
{
	printf("test_async(%d)\n", idle_time);
	int mouse_set = mouse_subscribe();
	int timer_set = timer_subscribe();

	if ((mouse_set == -1) || (timer_set == -1))
		return -1;

	if (mouse_write_command(WRITEB, SSTRM) == -1)
		return -1;

	unsigned long data;
	unsigned long packet[3];
	unsigned short counter = 0;
	unsigned int byt = 0;
	unsigned s = 0;

	while(counter < idle_time)
	{
		s++;
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
				if ((msg.NOTIFY_ARG & mouse_set))
				{ /* subscribed interrupt */
					sys_inb(OUT_BUF, &data);
					if((data & BIT(3)) != 0)
					{
						byt = 0;
						packet[byt] = data;
					}
					else
					{
						byt++;
						packet[byt] = data;
					}
					if(byt == 2)
					{
						byt = 0;
						unsigned short lb = 0, mb = 0, rb = 0, xov = 0, yov = 0;
						if (packet[0] & BIT(7))
							yov = 1;
						if (packet[0] & BIT(6))
							xov = 1;
						if (packet[0] & BIT(4))
						{
							packet[1] = packet[1] ^ 0xFF;
							packet[1] += 1;
						}
						if (packet[0] & BIT(5))
						{
							packet[2] = packet[2] ^ 0xFF;
							packet[2] += 1;
						}
						if (packet[0] & BIT(2))
							mb = 1;
						if (packet[0] & BIT(1))
							rb = 1;
						if (packet[0] & BIT(0))
							lb = 1;
						printf("B1: 0x%x\n",packet[0]);
						printf("B2: 0x%x\n",packet[1]);
						printf("B3: 0x%x\n",packet[2]);
						printf("LB=%d  MB=%d  RB=%d  XOV=%d  YOV=%d  X=",lb, mb, rb, xov, yov);
						if (packet[0] & BIT(4))
							printf("-");
						printf("%d  Y=", packet[1]);
						if (packet[0] & BIT(5))
							printf("-");
						printf("%d \n\n", packet[2]);
						counter = 0;
						s = 0;
					}
					tickdelay(micros_to_ticks(WAIT_KBC));
					break;
				}
				else if ((msg.NOTIFY_ARG & timer_set) && s == 60)
				{
					counter++;
					s = 0;
					printf("%d \n", idle_time - counter);
				}
				break;
			}
		}
	}
	if (mouse_write_command(WRITEB, DISTRM) == -1)
		return -1;
	if (timer_unsubscribe() == -1)
		return -1;
	if(mouse_unsubscribe() == -1)
		return -1;

	return 0;
}

int test_config(void)
{
	printf("test_config()\n");
	int mouse_set = mouse_subscribe();

	if (mouse_set == -1)
		return -1;

	if (mouse_write_command(WRITEB, STATREQ) == -1)
		return -1;

	unsigned long config[3];
	unsigned long data;
	unsigned int i;
	for(i = 0; i < 3; i++)
	{
		sys_inb(OUT_BUF, &data);
		config[i] = data;
		tickdelay(micros_to_ticks(WAIT_KBC));
	}


	int mode = 0, enable = 0, scaling = 0, lb = 0, mb = 0, rb = 0;

	if(config[0] & BIT(6))
		mode = 1;
	if(config[0] & BIT(5))
		enable = 1;
	if(config[0] & BIT(4))
		scaling = 1;
	if(config[0] & BIT(2))
		lb = 1;
	if(config[0] & BIT(1))
		mb = 1;
	if(config[0] & BIT(0))
		rb = 1;
	printf("Mode: %d, Enable: %d, Scaling: %d, LB: %d, MB:%d, RB: %d\n", mode, enable, scaling, lb, mb, rb);
	printf("Resolution: %d\n", config[1]);
	printf("Sample rate: %d\n", config[2]);

	if (mouse_unsubscribe() == -1)
		return -1;
	return 0;
}

int test_gesture(short length, unsigned short tolerance)
{
	printf("test_gesture(%d, %d)\n", length, tolerance);

	int mouse_set = mouse_subscribe();

	if (mouse_set == -1)
		return -1;

	if (mouse_write_command(WRITEB, SSTRM) == -1)
		return -1;

	unsigned long data;
	unsigned long packet[3];
	unsigned short dy = 0, dx = 0;
	unsigned int byt = 0;

	while(dy < abs(length))
	{
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
				if ((msg.NOTIFY_ARG & mouse_set))
				{ /* subscribed interrupt */
					sys_inb(OUT_BUF, &data);
					if((data & BIT(3)) != 0)
					{
						byt = 0;
						packet[byt] = data;
					}
					else
					{
						byt++;
						packet[byt] = data;
					}
					if(byt == 2)
					{
						byt = 0;
						unsigned short lb = 0, mb = 0, rb = 0, xov = 0, yov = 0;
						if (packet[0] & BIT(7))
							yov = 1;
						if (packet[0] & BIT(6))
							xov = 1;
						if (packet[0] & BIT(4))
						{
							packet[1] = packet[1] ^ 0xFF;
							packet[1] += 1;
						}
						if (packet[0] & BIT(5))
						{
							packet[2] = packet[2] ^ 0xFF;
							packet[2] += 1;
						}
						if (packet[0] & BIT(2))
							mb = 1;
						if (packet[0] & BIT(1))
							rb = 1;
						if (packet[0] & BIT(0))
							lb = 1;
						printf("B1: 0x%x\n",packet[0]);
						printf("B2: 0x%x\n",packet[1]);
						printf("B3: 0x%x\n",packet[2]);
						printf("LB=%d  MB=%d  RB=%d  XOV=%d  YOV=%d  X=",lb, mb, rb, xov, yov);
						if (packet[0] & BIT(4))
							printf("-");
						printf("%d  Y=", packet[1]);
						if (packet[0] & BIT(5))
							printf("-");
						printf("%d \n", packet[2]);
						if(rb == 1 && mb == 0 && lb == 0)
						{
							dx += abs(packet[1]);
							if ((length < 0) && (packet[0] & BIT(5)) || (length > 0) && !(packet[0] & BIT(5)))
								dy += packet[2];
							else if ((length < 0) && !(packet[0] & BIT(5)) || (length > 0) && (packet[0] & BIT(5)))
								dy= 0;
							else
								dy = 0;
						}
						else
						{
							dx = 0;
							dy = 0;
						}
						if (dx > tolerance)
						{
							dy = 0;
							dx = 0;
						}
						printf("DX: %d   DY: %d\n\n", dx, dy);

					}
					tickdelay(micros_to_ticks(WAIT_KBC));
					break;
				}
			}
		}
	}
	if (mouse_write_command(WRITEB, DISTRM) == -1)
		return -1;
	if(mouse_unsubscribe() == -1)
		return -1;

	return 0;
}
