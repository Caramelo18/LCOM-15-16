#include "keyboard.h"


int keyboard_subscribe()
{
	int hb = KBD_HOOK;

	if(sys_irqsetpolicy(KBD_IRQ ,IRQ_REENABLE|IRQ_EXCLUSIVE, &KBD_HOOK) == 0)
		if(sys_irqenable(&KBD_HOOK) == 0)
			return BIT(hb);

	return -1;
}


int keyboard_unsubscribe()
{
	if(sys_irqdisable(&KBD_HOOK) == 0)
		if(sys_irqrmpolicy(&KBD_HOOK) == 0)
			return 0;

	return -1;
}

void readKeyboard(mov* m1, mov* m2)
{
	unsigned long stat, data;

	sys_inb(STAT_REG, &stat);
	if ((stat &(PAR_ERR | TO_ERR)) == 0 )
		sys_inb(OUT_BUF, &data);
	else
		printf ("fail \n");
	if(data == 0xE0)
	{
		tickdelay(micros_to_ticks(WAIT_KBC));
		sys_inb(OUT_BUF, &data);
		if (data == 0x48) // player 1 up
			m1->pu = 1;
		else if (data == 0xC8)
			m1->pu = 0;
		else if (data == 0x4B) //player 1 left
			m1->pl = 1;
		else if (data == 0xCB)
			m1->pl = 0;
		else if (data == 0x4D) //player 1 right
			m1->pr  = 1;
		else if (data == 0xCD)
			m1->pr = 0;
		else if (data == 0x50) //player 1 down
			m1->pd = 1;
		else if (data == 0xD0)
			m1->pd = 0;
	}
	else
	{
		if(data == 0x39) // player 2 shoot
			m2->ps  = 1;
		else if (data == 0xB9)
			m2->ps = 0;
		else if (data == 0x11) // player 2 up
			m2->pu = 1;
		else if (data == 0x91)
			m2->pu = 0;
		else if (data == 0x1E) //player 2 left
			m2->pl = 1;
		else if (data == 0x9E)
			m2->pl = 0;
		else if (data == 0x20) //player 2 right
			m2->pr = 1;
		else if (data == 0xA0)
			m2->pr = 0;
		else if (data == 0x1F) //player 2 down
			m2->pd = 1;
		else if (data == 0x9F)
			m2->pd = 0;
		if(data == 0x34) // player 1 shoot
			m1->ps = 1;
		else if (data == 0xB4)
			m1->ps = 0;
	}
	tickdelay(micros_to_ticks(WAIT_KBC));
}


void readKeyboardM(mov* m1)
{
	unsigned long stat, data;

	sys_inb(STAT_REG, &stat);
	if ((stat &(PAR_ERR | TO_ERR)) == 0 )
		sys_inb(OUT_BUF, &data);
	else
		printf ("fail \n");
	if(data == 0xE0)
	{
		tickdelay(micros_to_ticks(WAIT_KBC));
		sys_inb(OUT_BUF, &data);
		if (data == 0x48) // player 1 up
			m1->pu = 1;
		else if (data == 0xC8)
			m1->pu = 0;
		else if (data == 0x4B) //player 1 left
			m1->pl = 1;
		else if (data == 0xCB)
			m1->pl = 0;
		else if (data == 0x4D) //player 1 right
			m1->pr  = 1;
		else if (data == 0xCD)
			m1->pr = 0;
		else if (data == 0x50) //player 1 down
			m1->pd = 1;
		else if (data == 0xD0)
			m1->pd = 0;
	}
	else
	{
		if(data == 0x34) // player 1 shoot
			m1->ps = 1;
		else if (data == 0xB4)
			m1->ps = 0;
	}
	tickdelay(micros_to_ticks(WAIT_KBC));
}

void scrollMenu(int* y, int* ent, int* rl)
{
	unsigned long stat, data;

	sys_inb(STAT_REG, &stat);
	if ((stat &(PAR_ERR | TO_ERR)) == 0 )
		sys_inb(OUT_BUF, &data);
	else
		printf ("fail \n");
	if(data == 0xE0)
	{
		tickdelay(micros_to_ticks(WAIT_KBC));
		sys_inb(OUT_BUF, &data);
		if (data == 0x48) // scroll up
		{
			if(*y > MENSELY)
				*y -= 80;
		}
		else if (data == 0x50) // scroll down
		{
			if(*y < 626)
				*y += 80;
		}
		if (data == 0x4B) // scroll left
			*rl = -1;
		else if (data == 0x4D) //scroll right
			*rl = 1;
		else
			*rl = 0;
	}
	if (data == 0x1C)
		*ent = 1;
	else
		*ent = 0;


}
