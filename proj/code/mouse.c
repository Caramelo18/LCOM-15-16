#include "mouse.h"

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

void readMouse(mov* m, unsigned long* packet, int* byt)
{
	unsigned long data;

	sys_inb(OUT_BUF, &data);
	if((data & BIT(3)) != 0)
	{
		*byt = 0;
		packet[*byt] = data;
	}
	else
	{
		*byt += 1;
		packet[*byt] = data;
	}
	if(*byt == 2)
	{

		unsigned short lb = 0;
		if (packet[0] & BIT(4))
		{
			packet[1] = twoComplement(packet[1]);
		}
		if (packet[0] & BIT(5))
		{
			packet[2] = twoComplement(packet[2]);
		}
		if (packet[0] & BIT(0))
			lb = 1;

		if((packet[1] > 1) && ((packet[0] & BIT(4)) != 0))
		{
			m->pl = 1;
			m->pr = 0;
		}
		else if ((packet[1] > 1) && ((packet[0] & BIT(4)) == 0))
		{
			m->pr = 1;
			m->pl = 0;
		}
		else
		{
			m->pl = 0;
			m->pr = 0;
		}
		if ((packet[2] > 1) && ((packet[0] & BIT(5)) != 0))
		{
			m->pu = 0;
			m->pd = 1;
		}
		else if ((packet[2] > 1) && ((packet[0] & BIT(5)) == 0))
		{
			m->pd = 0;
			m->pu = 1;
		}
		else
		{
			m->pd = 0;
			m->pu = 0;
		}

		if (lb == 1)
			m->ps = 1;
		else if (lb == 0)
			m->ps = 0;


		//printf("x: %d   y: %d\n", packet[1], packet[2]);
		//printf("p1: up: %d   down: %d   left: %d   right: %d   shoot: %d \n", m->pu, m->pd, m->pl, m->pr, m->ps);


	}

	//tickdelay(micros_to_ticks(WAIT_KBC));
}

void mouseCursor(unsigned int* byt, unsigned long* mousePacket, int* mx, int* my, int* lb)
{
	unsigned long data;

	sys_inb(OUT_BUF, &data);
	if((data & BIT(3)) != 0)
	{
		*byt = 0;
		mousePacket[*byt] = data;
	}
	else
	{
		*byt += 1;
		mousePacket[*byt] = data;
	}
	if(*byt == 2)
	{
		if (mousePacket[0] & BIT(4))
		{
			mousePacket[1] = twoComplement(mousePacket[1]);
		}
		if (mousePacket[0] & BIT(5))
		{
			mousePacket[2] = twoComplement(mousePacket[2]);
		}
		if (mousePacket[0] & BIT(0))
			*lb = 1;
		else
			*lb = 0;

		if (mousePacket[0] & BIT(4))
		{
			*mx -= mousePacket[1];
			if (*mx < 0)
				*mx = 0;
		}
		else
		{
			*mx += mousePacket[1];
			if ((*mx + 32) > 1024 )
				*mx = 1024 - 32;
		}
		if (mousePacket[0] & BIT(5))
		{
			*my += mousePacket[2];
			if ((*my + 32) > 768)
				*my = 768 - 32;
		}
		else
		{
			*my -= mousePacket[2];
			if (*my < 0)
				*my = 0;
		}
	}
}
