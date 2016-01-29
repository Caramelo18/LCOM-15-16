#include "test5.h"
#include "vbe.h"
#include "lmlib.h"
#include "read_xpm.h"

static int TIMER_HOOK = 2;
static int KEYBOARD_HOOK = 1;
static int c = 0;



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

int keyboard_subscribe()
{
	int hb = KEYBOARD_HOOK;

	if(sys_irqsetpolicy(KBD1 ,IRQ_REENABLE|IRQ_EXCLUSIVE, &KEYBOARD_HOOK) == 0)
		if(sys_irqenable(&KEYBOARD_HOOK) == 0)
			return BIT(hb);

	return -1;
}

int keyboard_unsubscribe()
{
	if(sys_irqdisable(&KEYBOARD_HOOK) == 0)
		if(sys_irqrmpolicy(&KEYBOARD_HOOK) == 0)
			return 0;
	return -1;
}


void waitForTimer(unsigned short delay, int irq_set)
{
	int count = 0;
	int ipc_status;
	message msg;
	int r;

	while((count/60) < delay)
	{
		// Get a request message.
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0 )
		{
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status))
		{ // received notification
			switch (_ENDPOINT_P(msg.m_source))
			{
			case HARDWARE: // hardware interrupt notification
				if ((msg.NOTIFY_ARG & irq_set))
				{ // subscribed interrupt
					count++;
					timer_int_handler();
				}
				break;
			default:
				break; // no other notifications expected: do nothing
			}
		}
		else
		{ // received a standard message, not a notification
			// no standard messages expected: do nothing
		}
	}
}

void waitForEsc(int kbc_set)
{
	unsigned long stat, data;
	while(data != 0x81)
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
						sys_inb(OUT_BUF, &data);
					else printf ("fail \n");
					tickdelay(micros_to_ticks(WAIT_KBC));
				}
				break;
			}
		}
	}

}

int pressEsc(int kbc_set)
{
	unsigned long stat, data;


	sys_inb(STAT_REG, &stat);
	if ((stat &(PAR_ERR | TO_ERR)) == 0 )
	{
		sys_inb(OUT_BUF, &data);
		//printf("%x\n", data);
		if (data == 0x81)
		{
			vg_exit();
			printf("Esc released\n");
			if (keyboard_unsubscribe() == -1)
				return -1;
			return 0;
		}
	}
	else printf ("fail \n");
	tickdelay(micros_to_ticks(1));

	return 1;

}


void *test_init(unsigned short mode, unsigned short delay)
{
	printf("test_init(%x, %d)\n", mode, delay);

	int irq_set = timer_subscribe_int();
	if (irq_set == -1)
		return NULL;

	int count = 0;

	char* add = (char *)vg_init(mode);
	if(add == NULL)
		return NULL;

	waitForTimer(delay, irq_set);

	if(vg_exit() == 1)
	{
		printf("failure exiting video mode \n");
		return NULL;
	}

	if(timer_unsubscribe_int() == 1)
		return NULL;

	vbe_print_info(mode);

	return add;
}


void colorPixel(unsigned short x, unsigned y, unsigned long color, char* add)
{
	unsigned hres = getHRes();
	unsigned bpp = getBitsPerPixel()/8;

	add += (x + y*hres)*bpp;

	*add = color;
}


int test_square(unsigned short x, unsigned short y, unsigned short size, unsigned long color)
{
	printf("test_square(%d, %d, %d, %x)\n", x, y, size, color);

	int kbc_set = keyboard_subscribe();
	if(kbc_set == -1)
		return -1;

	char* add = (char*)vg_init(0x105);

	unsigned hres = getHRes();
	unsigned vres = getVRes();
	unsigned bpp = getBitsPerPixel()/8;

	if (x == USHRT_MAX || y == USHRT_MAX || x+size > hres || y+size > vres)
	{
		vg_exit();
		printf("Invalid coordinates\n");
		return -1;
	}


	add = add + (hres * bpp * y) + (x * bpp);
	char* first = add;

	int i;
	for(i = 0; i < size; i++) // draw first line
	{
		*add = color;
		add += bpp;
		if(pressEsc(kbc_set) == -1)
			return -1;
	}
	*add = color;

	add = first + (hres * bpp * size); // address for the last line
	for(i = 0; i < size; i++)
	{
		*add = color;
		add += bpp;
		if(pressEsc(kbc_set) == -1)
			return -1;
	}
	add = first + hres*bpp;
	for (i = 0; i < size; i++)
	{
		*add = color;
		add += hres*bpp;
		if(pressEsc(kbc_set) == -1)
			return -1;
	}
	add= first + hres*bpp + size*bpp;
	for (i = 0; i < size; i++)
	{
		*add = color;
		add += hres*bpp;
		if(pressEsc(kbc_set) == -1)
			return -1;
	}


	waitForEsc(kbc_set);
	vg_exit();

	if (keyboard_unsubscribe() == -1)
		return -1;


	return 0;

}


int test_line(unsigned short xi, unsigned short yi, 
		unsigned short xf, unsigned short yf, unsigned long color)
{
	printf("test_line(%d, %d, %d, %d, %x)\n", xi, yi, xf, yf, color);

	int kbc_set = keyboard_subscribe();
	if(kbc_set == -1)
		return -1;

	char* add = (char*)vg_init(0x105);

	unsigned hres = getHRes();
	unsigned vres = getVRes();

	if (xi > hres || yi > vres || xf > hres || yf > vres)
	{
		vg_exit();
		printf("Invalid coordinates\n");
		return -1;
	}
	// Bresenham's line algorithm

	int dx = abs(xf-xi), sx = xi<xf ? 1 : -1;
	int dy = abs(yf-yi), sy = yi<yf ? 1 : -1;
	int err = (dx>dy ? dx : -dy)/2, e2;

	for(;;) //
	{
		if(pressEsc(kbc_set) == -1)
			return -1;
		colorPixel(xi,yi, color, add);
		if (xi==xf && yi==yf) break;
		e2 = err;
		if (e2 >-dx) { err -= dy; xi += sx; }
		if (e2 < dy) { err += dx; yi += sy; }
	}
	//end of Bresenham's line algorithm

	waitForEsc(kbc_set);
	vg_exit();

	if (keyboard_unsubscribe() == -1)
		return -1;

	return 0;
}

int test_xpm(unsigned short xi, unsigned short yi, char *xpm[])
{
	printf("test_xpm(%d, %d, %s)\n", xi, yi, xpm[0]);

	int kbc_set = keyboard_subscribe();
	if(kbc_set == -1)
		return -1;

	char* add = (char*)vg_init(0x105);
	unsigned hres = getHRes();
	unsigned vres = getVRes();
	unsigned bpp = getBitsPerPixel()/8;



	add = add + (hres * bpp * yi) + (xi * bpp);

	int width;
	int heigth;


	char* a;
	if((a = read_xpm(xpm, &width, &heigth)) == NULL)
		return -1;

	if (xi == USHRT_MAX || yi == USHRT_MAX || xi + width > hres || yi + heigth > vres)
	{
		vg_exit();
		printf("Invalid coordinates\n");
		return -1;
	}


	unsigned int i = 0;
	for(i; i < heigth; i++)
	{
		if(pressEsc(kbc_set) == -1)
			return -1;
		unsigned int j = 0;
		for(j; j < width; j++)
		{
			*add = *a;
			add++;
			a++;
		}
		add = add + (hres*bpp) - width;
	}

	waitForEsc(kbc_set);
	vg_exit();

	if (keyboard_unsubscribe() == -1)
		return -1;

	return 0;
}	

void eraseXPM(char* add, unsigned short x, unsigned short y, int width, int heigth)
{
	unsigned hres = getHRes();
	unsigned vres = getVRes();
	unsigned bpp = getBitsPerPixel()/8;

	add = add + (hres * bpp * y) + (x * bpp);

	unsigned int i = 0;
	for(i; i < heigth; i++)
	{
		unsigned int j = 0;
		for(j; j < width; j++)
		{
			*add = 0x00;
			add++;
		}
		add = add + (hres*bpp) - width;
	}
}

void drawXPM(char* add, unsigned short x, unsigned short y, char* a, int width, int heigth)
{
	unsigned hres = getHRes();
	unsigned vres = getVRes();
	unsigned bpp = getBitsPerPixel()/8;

	add = add + (hres * bpp * y) + (x * bpp);

	unsigned int i = 0;
	for(i; i < heigth; i++)
	{
		unsigned int j = 0;
		for(j; j < width; j++)
		{
			*add = *a;
			add++;
			a++;
		}
		add = add + (hres*bpp) - width;
	}
}

int test_move(unsigned short xi, unsigned short yi, char *xpm[], 
		unsigned short hor, short delta, unsigned short time)
{
	printf("test_move(%d, %d, %s, %d, %d, %d)\n", xi, yi, xpm[0], hor, delta, time);


	int kbd_set = keyboard_subscribe();
	if(kbd_set == -1)
		return -1;

	int timer_set = timer_subscribe_int();
	if(timer_set == -1)
		return -1;

	char* add = (char*)vg_init(0x105);

	unsigned hres = getHRes();
	unsigned vres = getVRes();
	unsigned bpp = getBitsPerPixel()/8;

	int counter = 1;
	unsigned long stat, data;
	unsigned short x = xi;
	unsigned short y = yi;
	float acc = 0;
	float d;
	int frames = 60 * time;
	d = (float)delta/frames;


	char* map;
	int width;
	int heigth;
	if((map = read_xpm(xpm, &width, &heigth)) == NULL)
		return -1;

	if (hor == 0)
	{
		if (yi + delta + heigth > vres || yi + delta <= 0)
		{
			vg_exit();
			printf("Invalid movement \n");
			if (keyboard_unsubscribe() == -1)
				return -1;
			if(timer_unsubscribe_int() == -1)
				return -1;
			return 0;
		}
	}
	else
	{
		if(xi + delta + width > hres || xi + delta  <= 0)
		{
			vg_exit();
			printf("Invalid movement \n");
			if (keyboard_unsubscribe() == -1)
				return -1;
			if(timer_unsubscribe_int() == -1)
				return -1;
			return 0;
		}
	}

	if (time == 0)
	{
		drawXPM(add, x, y, map, width, heigth);
		waitForEsc(kbd_set);
		vg_exit();
		printf("Esc released\n");
		if (keyboard_unsubscribe() == -1)
			return -1;
		if(timer_unsubscribe_int() == -1)
			return -1;
		return 0;
	}

	while((counter/60) < time)
	{
		// assuming it returns OK
		// loop while 8042 output buffer is empty
		int ipc_status;
		message msg;
		int r;

		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0 )
		{
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status))
		{ // received notification
			switch (_ENDPOINT_P(msg.m_source))
			{
			case HARDWARE: // hardware interrupt notification
				if ((msg.NOTIFY_ARG & kbd_set))
				{ // subscribed interrupt
					sys_inb(STAT_REG, &stat);
					if ( (stat &(PAR_ERR | TO_ERR)) == 0 )
					{
						sys_inb(OUT_BUF, &data);
						if(data == 0x81)
						{
							vg_exit();
							printf("Esc released\n");
							if (keyboard_unsubscribe() == -1)
								return -1;
							if(timer_unsubscribe_int() == -1)
								return -1;
							return 0;
						}

					}
					else printf ("fail \n");
					tickdelay(micros_to_ticks(WAIT_KBC));
				}
				if ((msg.NOTIFY_ARG & timer_set))
				{
					acc += d;
					if (acc >= 1 || acc <= -1)
					{
						eraseXPM(add, x, y, width, heigth);
						if(hor == 0)
							y += acc;
						else
							x += acc;
						acc = 0;
						drawXPM(add, x, y, map, width, heigth);

					}
					counter++;
				}
				break;
			}
		}
	}

	waitForEsc(kbd_set);
	vg_exit();
	printf("Esc released\n");
	if (keyboard_unsubscribe() == -1)
		return -1;
	if(timer_unsubscribe_int() == -1)
		return -1;
	return 0;

}					

int test_controller()
{
	printf("test_controller()\n");

	vbeInfoBlock inf;
	if(vbe_controller_info(&inf) == 0)
	{
		printf("success\n");

		return 0;
	}

	return -1;


}					

