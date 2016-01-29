#include "video.h"

void *vg_init(unsigned short mode)
{
	struct reg86u reg;
	struct mem_range m;
	unsigned size;
	vbe_mode_info_t vid;
	unsigned tam;

	reg.u.b.intno = VBE;
	reg.u.b.ah = VBEBIOSFUNC;
	reg.u.b.al = SETVBEMODE;
	reg.u.w.bx = (BIT(LINEAR_MODEL_BIT) | mode);

	if(sys_int86(&reg) != OK )
	{
		printf("\tvg_init(): sys_int86() failed \n");
		return NULL;
	}

	unsigned long ret = reg.u.w.ax;

	if(ret == FCF || ret == FCNS || ret == FICVM)
		return NULL;


	if(vbe_get_mode_info(mode, &vid) == -1)
	{
		vg_exit();
		return NULL;
	}

	h_res = vid.XResolution;
	v_res = vid.YResolution;
	bits_per_pixel = vid.BitsPerPixel;
	size = h_res * v_res * bits_per_pixel;
	m.mr_base = vid.PhysBasePtr;
	m.mr_limit = m.mr_base + size;
	video_mem = (char*)vid.PhysBasePtr;

	if(sys_privctl(SELF, SYS_PRIV_ADD_MEM, &m))
		panic("vg_init could not map video memory\n");


	return vm_map_phys(SELF, (void *)m.mr_base, size);
}


int vg_exit()
{
	struct reg86u reg86;

	reg86.u.b.intno = 0x10; /* BIOS video services */

	reg86.u.b.ah = 0x00;    /* Set Video Mode function */
	reg86.u.b.al = 0x03;    /* 80x25 text mode*/

	if( sys_int86(&reg86) != OK ) {
		printf("\tvg_exit(): sys_int86() failed \n");
		return 1;
	} else
		return 0;
}

unsigned getHRes()
{
	return h_res;
}

unsigned getVRes()
{
	return v_res;
}

unsigned getBitsPerPixel()
{
	printf("%d\n", bits_per_pixel);
	return bits_per_pixel;
}


int vbe_get_mode_info(unsigned short mode, vbe_mode_info_t *vmi_p)
{
	mmap_t inf;
	struct reg86u reg;

	if((void*)lm_init() == NULL)
	{
		vg_exit();
		printf("fail initializing memory \n");
		return -1;
	}

	if((void*)lm_alloc(sizeof(vbe_mode_info_t), &inf) == NULL)
	{
		vg_exit();
		printf("fail allocating memory \n");
		return -1;
	}

	reg.u.b.intno = VBE;
	reg.u.b.ah = VBEBIOSFUNC;
	reg.u.b.al = VBEMI;
	reg.u.w.cx = mode;
	reg.u.w.es = PB2BASE(inf.phys);
	reg.u.w.di = PB2OFF(inf.phys);

	if( sys_int86(&reg) != OK )
	{
		printf("\tvbe_get_mode_info: sys_int86() failed \n");
		return -1;
	}

	unsigned long ret = reg.u.w.ax;
	if(ret == FCF || ret == FCNS || ret == FICVM)
	{
		lm_free(&inf);
		return -1;
	}
	else
	{
		*vmi_p = *((vbe_mode_info_t*)inf.virtual);
		lm_free(&inf);
		return 0;
	}


}


void drawXPM(char* add, unsigned short x, unsigned short y, char* a, int width, int heigth)
{
	unsigned hres = h_res;
	unsigned vres = v_res;
	unsigned bpp = bits_per_pixel/8;

	add = add + (hres * bpp * y) + (x * bpp);

	unsigned int i = 0;
	for(i; i < heigth; i++)
	{
		unsigned int j = 0;
		for(j; j < width; j++)
		{
			if(*a == IGGCOL)
			{
				add++;
				a++;
				continue;
			}
			*add = *a;
			add++;
			a++;
		}
		add = add + (hres*bpp) - width;
	}
}

void eraseXPM(char* add, unsigned short x, unsigned short y, int width, int heigth)
{
	unsigned hres = h_res;
	unsigned vres = v_res;
	unsigned bpp = bits_per_pixel/8;

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

void displayMin(char* add, int min)
{
	int mw, mh;
	char* m;
	if (min == 0)
	{
		if((m = read_xpm(num0, &mw, &mh)) == NULL)
			return;
	}
	else if(min == 1)
	{
		if((m = read_xpm(num1, &mw, &mh)) == NULL)
			return;
	}
	else if(min == 2)
	{
		if((m = read_xpm(num2, &mw, &mh)) == NULL)
			return;
	}
	else if(min == 3)
	{
		if((m = read_xpm(num3, &mw, &mh)) == NULL)
			return;
	}
	else if(min == 4)
	{
		if((m = read_xpm(num4, &mw, &mh)) == NULL)
			return;
	}
	else if(min == 5)
	{
		if((m = read_xpm(num5, &mw, &mh)) == NULL)
			return;
	}
	eraseXPM(add, CLOCKMIN, CLOCKY, mw, mh);
	drawXPM(add, CLOCKMIN, CLOCKY, m, mw, mh);
}

void displaySec(char* add, int sec)
{
	int sw, sh;
	if (sec < 10)
	{
		char* zero;
		if((zero = read_xpm(num0, &sw, &sh)) == NULL)
			return;
		eraseXPM(add, CLOCKSEC1, CLOCKY, sw,sh);
		drawXPM(add, CLOCKSEC1, CLOCKY, zero, sw, sh);
	}
	else if(sec / 10 == 1)
	{
		char* d;
		if((d = read_xpm(num1, &sw, &sh)) == NULL)
			return;
		eraseXPM(add, CLOCKSEC1, CLOCKY, sw,sh);
		drawXPM(add, CLOCKSEC1, CLOCKY, d, sw, sh);
	}
	else if (sec / 10 == 2)
	{
		char* d;
		if((d = read_xpm(num2, &sw, &sh)) == NULL)
			return;
		eraseXPM(add, CLOCKSEC1, CLOCKY, sw,sh);
		drawXPM(add, CLOCKSEC1, CLOCKY, d, sw, sh);
	}
	else if (sec / 10 == 3)
	{
		char* d;
		if((d = read_xpm(num3, &sw, &sh)) == NULL)
			return;
		eraseXPM(add, CLOCKSEC1, CLOCKY, sw,sh);
		drawXPM(add, CLOCKSEC1, CLOCKY, d, sw, sh);
	}
	else if (sec / 10 == 4)
	{
		char* d;
		if((d = read_xpm(num4, &sw, &sh)) == NULL)
			return;
		eraseXPM(add, CLOCKSEC1, CLOCKY, sw,sh);
		drawXPM(add, CLOCKSEC1, CLOCKY, d, sw, sh);
	}
	else if (sec / 10 == 5)
	{
		char* d;
		if((d = read_xpm(num5, &sw, &sh)) == NULL)
			return;
		eraseXPM(add, CLOCKSEC1, CLOCKY, sw,sh);
		drawXPM(add, CLOCKSEC1, CLOCKY, d, sw, sh);
	}

	if(sec % 10 == 0)
	{
		char* u;
		if((u = read_xpm(num0, &sw, &sh)) == NULL)
			return;
		eraseXPM(add, CLOCKSEC2, CLOCKY, sw,sh);
		drawXPM(add, CLOCKSEC2, CLOCKY, u, sw, sh);
	}
	else if(sec % 10 == 1)
	{
		char* u;
		if((u = read_xpm(num1, &sw, &sh)) == NULL)
			return;
		eraseXPM(add, CLOCKSEC2, CLOCKY, sw,sh);
		drawXPM(add, CLOCKSEC2, CLOCKY, u, sw, sh);
	}
	else if(sec % 10 == 2)
	{
		char* u;
		if((u = read_xpm(num2, &sw, &sh)) == NULL)
			return;
		eraseXPM(add, CLOCKSEC2, CLOCKY, sw,sh);
		drawXPM(add, CLOCKSEC2, CLOCKY, u, sw, sh);
	}
	else if(sec % 10 == 3)
	{
		char* u;
		if((u = read_xpm(num3, &sw, &sh)) == NULL)
			return;
		eraseXPM(add, CLOCKSEC2, CLOCKY, sw,sh);
		drawXPM(add, CLOCKSEC2, CLOCKY, u, sw, sh);
	}
	else if(sec % 10 == 4)
	{
		char* u;
		if((u = read_xpm(num4, &sw, &sh)) == NULL)
			return;
		eraseXPM(add, CLOCKSEC2, CLOCKY, sw,sh);
		drawXPM(add, CLOCKSEC2, CLOCKY, u, sw, sh);
	}
	else if(sec % 10 == 5)
	{
		char* u;
		if((u = read_xpm(num5, &sw, &sh)) == NULL)
			return;
		eraseXPM(add, CLOCKSEC2, CLOCKY, sw,sh);
		drawXPM(add, CLOCKSEC2, CLOCKY, u, sw, sh);
	}
	else if(sec % 10 == 6)
	{
		char* u;
		if((u = read_xpm(num6, &sw, &sh)) == NULL)
			return;
		eraseXPM(add, CLOCKSEC2, CLOCKY, sw,sh);
		drawXPM(add, CLOCKSEC2, CLOCKY, u, sw, sh);
	}
	else if(sec % 10 == 7)
	{
		char* u;
		if((u = read_xpm(num7, &sw, &sh)) == NULL)
			return;
		eraseXPM(add, CLOCKSEC2, CLOCKY, sw,sh);
		drawXPM(add, CLOCKSEC2, CLOCKY, u, sw, sh);
	}
	else if(sec % 10 == 8)
	{
		char* u;
		if((u = read_xpm(num8, &sw, &sh)) == NULL)
			return;
		eraseXPM(add, CLOCKSEC2, CLOCKY, sw,sh);
		drawXPM(add, CLOCKSEC2, CLOCKY, u, sw, sh);
	}
	else if(sec % 10 == 9)
	{
		char* u;
		if((u = read_xpm(num9, &sw, &sh)) == NULL)
			return;
		eraseXPM(add, CLOCKSEC2, CLOCKY, sw,sh);
		drawXPM(add, CLOCKSEC2, CLOCKY, u, sw, sh);
	}
}

void displayScore(char* add, int *pl1score, int *pl2score)
{
	int pl1x = 140;
	int pl2x = 325;

	char* pl1;
	int pl1w, pl1h;
	if((pl1 = read_xpm(p1, &pl1w, &pl1h)) == NULL)
		return;
	eraseXPM(add, pl1x, 82, pl1w, pl1h);
	drawXPM(add, pl1x, 82, pl1, pl1w, pl1h);

	char* pl2;
	int pl2w, pl2h;
	if((pl2 = read_xpm(p2, &pl2w, &pl2h)) == NULL)
		return;
	eraseXPM(add, pl2x, 82, pl2w, pl2h);
	drawXPM(add, pl2x, 82, pl2, pl2w, pl2h);


	char* tr;
	int trw, trh;
	if((tr = read_xpm(trac, &trw, &trh)) == NULL)
		return;
	eraseXPM(add, SCORESEP, SCOREY, trw, trh);
	drawXPM(add, SCORESEP, SCOREY, tr, trw, trh);

	int p1w, p1h;
	if (*pl1score == 0)
	{
		char* zero;
		if((zero = read_xpm(num0, &p1w, &p1h)) == NULL)
			return;
		eraseXPM(add, P1SCOREX, SCOREY, p1w, p1h);
		drawXPM(add, P1SCOREX, SCOREY, zero, p1w, p1h);
	}
	else if(*pl1score == 1)
	{
		char* u;
		if((u = read_xpm(num1, &p1w, &p1h)) == NULL)
			return;
		eraseXPM(add, P1SCOREX, SCOREY, p1w, p1h);
		drawXPM(add, P1SCOREX, SCOREY, u, p1w, p1h);
	}
	else if(*pl1score == 2)
	{
		char* u;
		if((u = read_xpm(num2, &p1w, &p1h)) == NULL)
			return;
		eraseXPM(add, P1SCOREX, SCOREY, p1w, p1h);
		drawXPM(add, P1SCOREX, SCOREY, u, p1w, p1h);
	}
	else if(*pl1score == 3)
	{
		char* u;
		if((u = read_xpm(num3, &p1w, &p1h)) == NULL)
			return;
		eraseXPM(add, P1SCOREX, SCOREY, p1w, p1h);
		drawXPM(add, P1SCOREX, SCOREY, u, p1w, p1h);
	}
	else if(*pl1score == 4)
	{
		char* u;
		if((u = read_xpm(num4, &p1w, &p1h)) == NULL)
			return;
		eraseXPM(add, P1SCOREX, SCOREY, p1w, p1h);
		drawXPM(add, P1SCOREX, SCOREY, u, p1w, p1h);
	}
	else if(*pl1score == 5)
	{
		char* u;
		if((u = read_xpm(num5, &p1w, &p1h)) == NULL)
			return;
		eraseXPM(add, P1SCOREX, SCOREY, p1w, p1h);
		drawXPM(add, P1SCOREX, SCOREY, u, p1w, p1h);
	}
	else if(*pl1score == 6)
	{
		char* u;
		if((u = read_xpm(num6, &p1w, &p1h)) == NULL)
			return;
		eraseXPM(add, P1SCOREX, SCOREY, p1w, p1h);
		drawXPM(add, P1SCOREX, SCOREY, u, p1w, p1h);
	}
	else if(*pl1score == 7)
	{
		char* u;
		if((u = read_xpm(num7, &p1w, &p1h)) == NULL)
			return;
		eraseXPM(add, P1SCOREX, SCOREY, p1w, p1h);
		drawXPM(add, P1SCOREX, SCOREY, u, p1w, p1h);
	}
	else if(*pl1score == 8)
	{
		char* u;
		if((u = read_xpm(num8, &p1w, &p1h)) == NULL)
			return;
		eraseXPM(add, P1SCOREX, SCOREY, p1w, p1h);
		drawXPM(add, P1SCOREX, SCOREY, u, p1w, p1h);
	}
	else if(*pl1score == 9)
	{
		char* u;
		if((u = read_xpm(num9, &p1w, &p1h)) == NULL)
			return;
		eraseXPM(add, P1SCOREX, SCOREY, p1w, p1h);
		drawXPM(add, P1SCOREX, SCOREY, u, p1w, p1h);
	}
	else if(*pl1score > 9)
	{
		if (*pl1score % 10 == 0)
		{
			char* zero;
			if((zero = read_xpm(num0, &p1w, &p1h)) == NULL)
				return;
			eraseXPM(add, P1SCOREX, SCOREY, p1w, p1h);
			drawXPM(add, P1SCOREX, SCOREY, zero, p1w, p1h);
		}
		else if(*pl1score % 10 == 1)
		{
			char* u;
			if((u = read_xpm(num1, &p1w, &p1h)) == NULL)
				return;
			eraseXPM(add, P1SCOREX, SCOREY, p1w, p1h);
			drawXPM(add, P1SCOREX, SCOREY, u, p1w, p1h);
		}
		else if(*pl1score % 10 == 2)
		{
			char* u;
			if((u = read_xpm(num2, &p1w, &p1h)) == NULL)
				return;
			eraseXPM(add, P1SCOREX, SCOREY, p1w, p1h);
			drawXPM(add, P1SCOREX, SCOREY, u, p1w, p1h);
		}
		else if(*pl1score % 10 == 3)
		{
			char* u;
			if((u = read_xpm(num3, &p1w, &p1h)) == NULL)
				return;
			eraseXPM(add, P1SCOREX, SCOREY, p1w, p1h);
			drawXPM(add, P1SCOREX, SCOREY, u, p1w, p1h);
		}
		else if(*pl1score % 10 == 4)
		{
			char* u;
			if((u = read_xpm(num4, &p1w, &p1h)) == NULL)
				return;
			eraseXPM(add, P1SCOREX, SCOREY, p1w, p1h);
			drawXPM(add, P1SCOREX, SCOREY, u, p1w, p1h);
		}
		else if(*pl1score % 10 == 5)
		{
			char* u;
			if((u = read_xpm(num5, &p1w, &p1h)) == NULL)
				return;
			eraseXPM(add, P1SCOREX, SCOREY, p1w, p1h);
			drawXPM(add, P1SCOREX, SCOREY, u, p1w, p1h);
		}
		else if(*pl1score % 10 == 6)
		{
			char* u;
			if((u = read_xpm(num6, &p1w, &p1h)) == NULL)
				return;
			eraseXPM(add, P1SCOREX, SCOREY, p1w, p1h);
			drawXPM(add, P1SCOREX, SCOREY, u, p1w, p1h);
		}
		else if(*pl1score % 10 == 7)
		{
			char* u;
			if((u = read_xpm(num7, &p1w, &p1h)) == NULL)
				return;
			eraseXPM(add, P1SCOREX, SCOREY, p1w, p1h);
			drawXPM(add, P1SCOREX, SCOREY, u, p1w, p1h);
		}
		else if(*pl1score % 10 == 8)
		{
			char* u;
			if((u = read_xpm(num8, &p1w, &p1h)) == NULL)
				return;
			eraseXPM(add, P1SCOREX, SCOREY, p1w, p1h);
			drawXPM(add, P1SCOREX, SCOREY, u, p1w, p1h);
		}
		else if(*pl1score % 10 == 9)
		{
			char* u;
			if((u = read_xpm(num9, &p1w, &p1h)) == NULL)
				return;
			eraseXPM(add, P1SCOREX, SCOREY, p1w, p1h);
			drawXPM(add, P1SCOREX, SCOREY, u, p1w, p1h);
		}

		if(*pl1score / 10 == 1)
		{
			char* u;
			if((u = read_xpm(num1, &p1w, &p1h)) == NULL)
				return;
			eraseXPM(add, 188, SCOREY, p1w, p1h);
			drawXPM(add, 188, SCOREY, u, p1w, p1h);
		}
		else if(*pl1score / 10 == 2)
		{
			char* u;
			if((u = read_xpm(num2, &p1w, &p1h)) == NULL)
				return;
			eraseXPM(add,188, SCOREY, p1w, p1h);
			drawXPM(add, 188, SCOREY, u, p1w, p1h);
		}
		else if(*pl1score / 10 == 3)
		{
			char* u;
			if((u = read_xpm(num3, &p1w, &p1h)) == NULL)
				return;
			eraseXPM(add, 188, SCOREY, p1w, p1h);
			drawXPM(add, 188, SCOREY, u, p1w, p1h);
		}
		else if(*pl1score / 10 == 4)
		{
			char* u;
			if((u = read_xpm(num4, &p1w, &p1h)) == NULL)
				return;
			eraseXPM(add, 188, SCOREY, p1w, p1h);
			drawXPM(add, 188, SCOREY, u, p1w, p1h);
		}
		else if(*pl1score / 10 == 5)
		{
			char* u;
			if((u = read_xpm(num5, &p1w, &p1h)) == NULL)
				return;
			eraseXPM(add, 188, SCOREY, p1w, p1h);
			drawXPM(add, 188, SCOREY, u, p1w, p1h);
		}
		else if(*pl1score / 10 == 6)
		{
			char* u;
			if((u = read_xpm(num6, &p1w, &p1h)) == NULL)
				return;
			eraseXPM(add, 188, SCOREY, p1w, p1h);
			drawXPM(add, 188, SCOREY, u, p1w, p1h);
		}
		else if(*pl1score / 10 == 7)
		{
			char* u;
			if((u = read_xpm(num7, &p1w, &p1h)) == NULL)
				return;
			eraseXPM(add, 188, SCOREY, p1w, p1h);
			drawXPM(add, 188, SCOREY, u, p1w, p1h);
		}
		else if(*pl1score / 10 == 8)
		{
			char* u;
			if((u = read_xpm(num8, &p1w, &p1h)) == NULL)
				return;
			eraseXPM(add, 188, SCOREY, p1w, p1h);
			drawXPM(add, 188, SCOREY, u, p1w, p1h);
		}
		else if(*pl1score / 10 == 9)
		{
			char* u;
			if((u = read_xpm(num9, &p1w, &p1h)) == NULL)
				return;
			eraseXPM(add, 188 , SCOREY, p1w, p1h);
			drawXPM(add, 188, SCOREY, u, p1w, p1h);
		}
		//		eraseXPM(add, pl1x, 82, pl1w, pl1h);
		//		pl1x = pl1x - 62;
	}


	int p2w, p2h;
	if (*pl2score == 0)
	{
		char* zero;
		if((zero = read_xpm(num0, &p2w, &p2h)) == NULL)
			return;
		eraseXPM(add, P2SCOREX, SCOREY, p2w, p2h);
		drawXPM(add, P2SCOREX, SCOREY, zero, p2w, p2h);
	}
	else if(*pl2score == 1)
	{
		char* z;
		if((z = read_xpm(num1, &p2w, &p2h)) == NULL)
			return;
		eraseXPM(add, P2SCOREX, SCOREY, p2w, p2h);
		drawXPM(add, P2SCOREX, SCOREY, z, p2w, p2h);
	}
	else if(*pl2score == 2)
	{
		char* z;
		if((z = read_xpm(num2, &p2w, &p2h)) == NULL)
			return;
		eraseXPM(add, P2SCOREX, SCOREY, p2w, p2h);
		drawXPM(add, P2SCOREX, SCOREY, z, p2w, p2h);
	}
	else if(*pl2score == 3)
	{
		char* z;
		if((z = read_xpm(num3, &p2w, &p2h)) == NULL)
			return;
		eraseXPM(add, P2SCOREX, SCOREY, p2w, p2h);
		drawXPM(add, P2SCOREX, SCOREY, z, p2w, p2h);
	}
	else if(*pl2score == 4)
	{
		char* z;
		if((z = read_xpm(num4, &p2w, &p2h)) == NULL)
			return;
		eraseXPM(add, P2SCOREX, SCOREY, p2w, p2h);
		drawXPM(add, P2SCOREX, SCOREY, z, p2w, p2h);
	}
	else if(*pl2score == 5)
	{
		char* z;
		if((z = read_xpm(num5, &p2w, &p2h)) == NULL)
			return;
		eraseXPM(add, P2SCOREX, SCOREY, p2w, p2h);
		drawXPM(add, P2SCOREX, SCOREY, z, p2w, p2h);
	}
	else if(*pl2score == 6)
	{
		char* z;
		if((z = read_xpm(num6, &p2w, &p2h)) == NULL)
			return;
		eraseXPM(add, P2SCOREX, SCOREY, p2w, p2h);
		drawXPM(add, P2SCOREX, SCOREY, z, p2w, p2h);
	}
	else if(*pl2score == 7)
	{
		char* z;
		if((z = read_xpm(num7, &p2w, &p2h)) == NULL)
			return;
		eraseXPM(add, P2SCOREX, SCOREY, p2w, p2h);
		drawXPM(add, P2SCOREX, SCOREY, z, p2w, p2h);
	}
	else if(*pl2score == 8)
	{
		char* z;
		if((z = read_xpm(num8, &p2w, &p2h)) == NULL)
			return;
		eraseXPM(add, P2SCOREX, SCOREY, p2w, p2h);
		drawXPM(add, P2SCOREX, SCOREY, z, p2w, p2h);
	}
	else if(*pl2score == 9)
	{
		char* z;
		if((z = read_xpm(num9, &p2w, &p2h)) == NULL)
			return;
		eraseXPM(add, P2SCOREX, SCOREY, p2w, p2h);
		drawXPM(add, P2SCOREX, SCOREY, z, p2w, p2h);
	}

	else if(*pl2score > 9)
	{
		if (*pl2score % 10 == 0)
		{
			char* zero;
			if((zero = read_xpm(num0, &p2w, &p2h)) == NULL)
				return;
			eraseXPM(add, 292, SCOREY, p2w, p2h);
			drawXPM(add, 292, SCOREY, zero, p2w, p2h);
		}
		else if(*pl2score % 10 == 1)
		{
			char* u;
			if((u = read_xpm(num1, &p2w, &p2h)) == NULL)
				return;
			eraseXPM(add, 292, SCOREY, p2w, p2h);
			drawXPM(add, 292, SCOREY, u, p2w, p2h);
		}
		else if(*pl2score % 10 == 2)
		{
			char* u;
			if((u = read_xpm(num2, &p2w, &p2h)) == NULL)
				return;
			eraseXPM(add, 292, SCOREY, p2w, p2h);
			drawXPM(add, 292, SCOREY, u, p2w, p2h);
		}
		else if(*pl2score % 10 == 3)
		{
			char* u;
			if((u = read_xpm(num3, &p2w, &p2h)) == NULL)
				return;
			eraseXPM(add, 292, SCOREY, p2w, p2h);
			drawXPM(add, 292, SCOREY, u, p2w, p2h);
		}
		else if(*pl2score % 10 == 4)
		{
			char* u;
			if((u = read_xpm(num4, &p2w, &p2h)) == NULL)
				return;
			eraseXPM(add, 292, SCOREY, p2w, p2h);
			drawXPM(add, 292, SCOREY, u, p2w, p2h);
		}
		else if(*pl2score % 10 == 5)
		{
			char* u;
			if((u = read_xpm(num5, &p2w, &p2h)) == NULL)
				return;
			eraseXPM(add, 292, SCOREY, p2w, p2h);
			drawXPM(add, 292, SCOREY, u, p2w, p2h);
		}
		else if(*pl2score % 10 == 6)
		{
			char* u;
			if((u = read_xpm(num6, &p2w, &p2h)) == NULL)
				return;
			eraseXPM(add, 292, SCOREY, p2w, p2h);
			drawXPM(add, 292, SCOREY, u, p2w, p2h);
		}
		else if(*pl2score % 10 == 7)
		{
			char* u;
			if((u = read_xpm(num7,  &p2w, &p2h)) == NULL)
				return;
			eraseXPM(add, 292, SCOREY, p2w, p2h);
			drawXPM(add, 292, SCOREY, u, p2w, p2h);
		}
		else if(*pl2score % 10 == 8)
		{
			char* u;
			if((u = read_xpm(num8,  &p2w, &p2h)) == NULL)
				return;
			eraseXPM(add, 292, SCOREY, p2w, p2h);
			drawXPM(add, 292, SCOREY, u, p2w, p2h);
		}
		else if(*pl2score% 10 == 9)
		{
			char* u;
			if((u = read_xpm(num9,  &p2w, &p2h)) == NULL)
				return;
			eraseXPM(add, 292, SCOREY, p2w, p2h);
			drawXPM(add, 292, SCOREY, u, p2w, p2h);
		}

		if(*pl2score / 10 == 1)
		{
			char* u;
			if((u = read_xpm(num1,  &p2w, &p2h)) == NULL)
				return;
			eraseXPM(add, P2SCOREX, SCOREY, p2w, p2h);
			drawXPM(add, P2SCOREX, SCOREY, u, p2w, p2h);
		}
		else if(*pl2score / 10 == 2)
		{
			char* u;
			if((u = read_xpm(num2, &p2w, &p2h)) == NULL)
				return;
			eraseXPM(add, P2SCOREX, SCOREY, p2w, p2h);
			drawXPM(add, P2SCOREX, SCOREY, u, p2w, p2h);
		}
		else if(*pl2score / 10 == 3)
		{
			char* u;
			if((u = read_xpm(num3,  &p2w, &p2h)) == NULL)
				return;
			eraseXPM(add, P2SCOREX, SCOREY, p2w, p2h);
			drawXPM(add, P2SCOREX, SCOREY, u, p2w, p2h);
		}
		else if(*pl2score / 10 == 4)
		{
			char* u;
			if((u = read_xpm(num4,  &p2w, &p2h)) == NULL)
				return;
			eraseXPM(add, P2SCOREX, SCOREY, p2w, p2h);
			drawXPM(add, P2SCOREX, SCOREY, u, p2w, p2h);
		}
		else if(*pl2score / 10 == 5)
		{
			char* u;
			if((u = read_xpm(num5,  &p2w, &p2h)) == NULL)
				return;
			eraseXPM(add, P2SCOREX, SCOREY, p2w, p2h);
			drawXPM(add, P2SCOREX, SCOREY, u, p2w, p2h);
		}
		else if(*pl2score / 10 == 6)
		{
			char* u;
			if((u = read_xpm(num6,  &p2w, &p2h)) == NULL)
				return;
			eraseXPM(add, P2SCOREX, SCOREY, p2w, p2h);
			drawXPM(add, P2SCOREX, SCOREY, u, p2w, p2h);
		}
		else if(*pl2score / 10 == 7)
		{
			char* u;
			if((u = read_xpm(num7, &p2w, &p2h)) == NULL)
				return;
			eraseXPM(add, P2SCOREX, SCOREY, p2w, p2h);
			drawXPM(add, P2SCOREX, SCOREY, u, p2w, p2h);
		}
		else if(*pl2score / 10 == 8)
		{
			char* u;
			if((u = read_xpm(num8,  &p2w, &p2h)) == NULL)
				return;
			eraseXPM(add, P2SCOREX, SCOREY, p2w, p2h);
			drawXPM(add, P2SCOREX, SCOREY, u, p2w, p2h);
		}
		else if(*pl2score / 10 == 9)
		{
			char* u;
			if((u = read_xpm(num9,  &p2w, &p2h)) == NULL)
				return;
			eraseXPM(add, P2SCOREX, SCOREY, p2w, p2h);
			drawXPM(add, P2SCOREX, SCOREY, u, p2w, p2h);
		}
		//		eraseXPM(add, pl1x, 82, pl1w, pl1h);
		//		pl1x = pl1x - 62;
	}
	/*else
	{
		char* u;
		if((u = read_xpm(num0, &p2w, &p2w)) == NULL)
			return;
		eraseXPM(add, P2SCOREX, SCOREY, p2w, p2h);
		drawXPM(add, P2SCOREX, SCOREY, u, p2w, p2h);
	}*/
}

void displayTime(Date* d, char* add)
{
	char* doisp;
	int wp, hp;
	if((doisp = read_xpm(dp, &wp, &hp)) == NULL)
		return;
	drawXPM(add, RTCDPHM, RTCDPY, doisp, wp, hp);
	drawXPM(add, RTCDPMS, RTCDPY, doisp, wp, hp);

	if((doisp = read_xpm(trac, &wp, &hp)) == NULL)
		return;
	drawXPM(add, RTCTR1, RTCTRY, doisp, wp, hp);
	drawXPM(add, RTCTR2, RTCTRY, doisp, wp, hp);

	int sw, sh;
	int sec = d->seconds;
	if (sec < 10)
	{
		char* zero;
		if((zero = read_xpm(num0, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCSEC1, CLOCKY, zero, sw, sh);
	}
	else if(sec / 10 == 1)
	{
		char* d;
		if((d = read_xpm(num1, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCSEC1, CLOCKY, d, sw, sh);
	}
	else if (sec / 10 == 2)
	{
		char* d;
		if((d = read_xpm(num2, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCSEC1, CLOCKY, d, sw, sh);
	}
	else if (sec / 10 == 3)
	{
		char* d;
		if((d = read_xpm(num3, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCSEC1, CLOCKY, d, sw, sh);
	}
	else if (sec / 10 == 4)
	{
		char* d;
		if((d = read_xpm(num4, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCSEC1, CLOCKY, d, sw, sh);
	}
	else if (sec / 10 == 5)
	{
		char* d;
		if((d = read_xpm(num5, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCSEC1, CLOCKY, d, sw, sh);
	}

	if(sec % 10 == 0)
	{
		char* u;
		if((u = read_xpm(num0, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCSEC2, CLOCKY, u, sw, sh);
	}
	else if(sec % 10 == 1)
	{
		char* u;
		if((u = read_xpm(num1, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCSEC2, CLOCKY, u, sw, sh);
	}
	else if(sec % 10 == 2)
	{
		char* u;
		if((u = read_xpm(num2, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCSEC2, CLOCKY, u, sw, sh);
	}
	else if(sec % 10 == 3)
	{
		char* u;
		if((u = read_xpm(num3, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCSEC2, CLOCKY, u, sw, sh);
	}
	else if(sec % 10 == 4)
	{
		char* u;
		if((u = read_xpm(num4, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCSEC2, CLOCKY, u, sw, sh);
	}
	else if(sec % 10 == 5)
	{
		char* u;
		if((u = read_xpm(num5, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCSEC2, CLOCKY, u, sw, sh);
	}
	else if(sec % 10 == 6)
	{
		char* u;
		if((u = read_xpm(num6, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCSEC2, CLOCKY, u, sw, sh);
	}
	else if(sec % 10 == 7)
	{
		char* u;
		if((u = read_xpm(num7, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCSEC2, CLOCKY, u, sw, sh);
	}
	else if(sec % 10 == 8)
	{
		char* u;
		if((u = read_xpm(num8, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCSEC2, CLOCKY, u, sw, sh);
	}
	else if(sec % 10 == 9)
	{
		char* u;
		if((u = read_xpm(num9, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCSEC2, CLOCKY, u, sw, sh);
	}

	int min = d->minutes;
	if (min < 10)
	{
		char* zero;
		if((zero = read_xpm(num0, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCMIN1, CLOCKY, zero, sw, sh);
	}
	else if(min / 10 == 1)
	{
		char* d;
		if((d = read_xpm(num1, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCMIN1, CLOCKY, d, sw, sh);
	}
	else if (min / 10 == 2)
	{
		char* d;
		if((d = read_xpm(num2, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCMIN1, CLOCKY, d, sw, sh);
	}
	else if (min / 10 == 3)
	{
		char* d;
		if((d = read_xpm(num3, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCMIN1, CLOCKY, d, sw, sh);
	}
	else if (min / 10 == 4)
	{
		char* d;
		if((d = read_xpm(num4, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCMIN1, CLOCKY, d, sw, sh);
	}
	else if (min / 10 == 5)
	{
		char* d;
		if((d = read_xpm(num5, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCMIN1, CLOCKY, d, sw, sh);
	}

	if(min % 10 == 0)
	{
		char* u;
		if((u = read_xpm(num0, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCMIN2, CLOCKY, u, sw, sh);
	}
	else if(min % 10 == 1)
	{
		char* u;
		if((u = read_xpm(num1, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCMIN2, CLOCKY, u, sw, sh);
	}
	else if(min % 10 == 2)
	{
		char* u;
		if((u = read_xpm(num2, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCMIN2, CLOCKY, u, sw, sh);
	}
	else if(min % 10 == 3)
	{
		char* u;
		if((u = read_xpm(num3, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCMIN2, CLOCKY, u, sw, sh);
	}
	else if(min % 10 == 4)
	{
		char* u;
		if((u = read_xpm(num4, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCMIN2, CLOCKY, u, sw, sh);
	}
	else if(min % 10 == 5)
	{
		char* u;
		if((u = read_xpm(num5, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCMIN2, CLOCKY, u, sw, sh);
	}
	else if(min % 10 == 6)
	{
		char* u;
		if((u = read_xpm(num6, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCMIN2, CLOCKY, u, sw, sh);
	}
	else if(min % 10 == 7)
	{
		char* u;
		if((u = read_xpm(num7, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCMIN2, CLOCKY, u, sw, sh);
	}
	else if(min % 10 == 8)
	{
		char* u;
		if((u = read_xpm(num8, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCMIN2, CLOCKY, u, sw, sh);
	}
	else if(min % 10 == 9)
	{
		char* u;
		if((u = read_xpm(num9, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCMIN2, CLOCKY, u, sw, sh);
	}

	int hrs = d->hours;
	if (hrs < 10)
	{
		char* zero;
		if((zero = read_xpm(num0, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCHRS1, CLOCKY, zero, sw, sh);
	}
	else if(hrs / 10 == 1)
	{
		char* d;
		if((d = read_xpm(num1, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCHRS1, CLOCKY, d, sw, sh);
	}
	else if (hrs / 10 == 2)
	{
		char* d;
		if((d = read_xpm(num2, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCHRS1, CLOCKY, d, sw, sh);
	}
	else if (hrs / 10 == 3)
	{
		char* d;
		if((d = read_xpm(num3, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCHRS1, CLOCKY, d, sw, sh);
	}
	else if (hrs / 10 == 4)
	{
		char* d;
		if((d = read_xpm(num4, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCHRS1, CLOCKY, d, sw, sh);
	}
	else if (hrs / 10 == 5)
	{
		char* d;
		if((d = read_xpm(num5, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCHRS1, CLOCKY, d, sw, sh);
	}

	if(hrs % 10 == 0)
	{
		char* u;
		if((u = read_xpm(num0, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCHRS2, CLOCKY, u, sw, sh);
	}
	else if(hrs % 10 == 1)
	{
		char* u;
		if((u = read_xpm(num1, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCHRS2, CLOCKY, u, sw, sh);
	}
	else if(hrs % 10 == 2)
	{
		char* u;
		if((u = read_xpm(num2, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCHRS2, CLOCKY, u, sw, sh);
	}
	else if(hrs % 10 == 3)
	{
		char* u;
		if((u = read_xpm(num3, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCHRS2, CLOCKY, u, sw, sh);
	}
	else if(hrs % 10 == 4)
	{
		char* u;
		if((u = read_xpm(num4, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCHRS2, CLOCKY, u, sw, sh);
	}
	else if(hrs % 10 == 5)
	{
		char* u;
		if((u = read_xpm(num5, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCHRS2, CLOCKY, u, sw, sh);
	}
	else if(hrs % 10 == 6)
	{
		char* u;
		if((u = read_xpm(num6, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCHRS2, CLOCKY, u, sw, sh);
	}
	else if(hrs % 10 == 7)
	{
		char* u;
		if((u = read_xpm(num7, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCHRS2, CLOCKY, u, sw, sh);
	}
	else if(hrs % 10 == 8)
	{
		char* u;
		if((u = read_xpm(num8, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCHRS2, CLOCKY, u, sw, sh);
	}
	else if(hrs % 10 == 9)
	{
		char* u;
		if((u = read_xpm(num9, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCHRS2, CLOCKY, u, sw, sh);
	}



	char* a;
	if((a = read_xpm(num2, &sw, &sh)) == NULL)
		return;
	drawXPM(add, RTCA1, RTCDATEY, a, sw, sh);
	if((a = read_xpm(num0, &sw, &sh)) == NULL)
		return;
	drawXPM(add, RTCA2, RTCDATEY, a, sw, sh);

	int yr = d->year;
	if (yr < 10)
	{
		char* zero;
		if((zero = read_xpm(num0, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCYR1, RTCDATEY, zero, sw, sh);
	}
	else if(yr / 10 == 1)
	{
		char* d;
		if((d = read_xpm(num1, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCYR1, RTCDATEY, d, sw, sh);
	}
	else if (yr / 10 == 2)
	{
		char* d;
		if((d = read_xpm(num2, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCYR1, RTCDATEY, d, sw, sh);
	}
	else if (yr / 10 == 3)
	{
		char* d;
		if((d = read_xpm(num3, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCYR1, RTCDATEY, d, sw, sh);
	}
	else if (yr / 10 == 4)
	{
		char* d;
		if((d = read_xpm(num4, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCYR1, RTCDATEY, d, sw, sh);
	}
	else if (yr / 10 == 5)
	{
		char* d;
		if((d = read_xpm(num5, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCYR1, RTCDATEY, d, sw, sh);
	}

	if(yr % 10 == 0)
	{
		char* u;
		if((u = read_xpm(num0, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCYR2, RTCDATEY, u, sw, sh);
	}
	else if(yr % 10 == 1)
	{
		char* u;
		if((u = read_xpm(num1, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCYR2, RTCDATEY, u, sw, sh);
	}
	else if(yr % 10 == 2)
	{
		char* u;
		if((u = read_xpm(num2, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCYR2, RTCDATEY, u, sw, sh);
	}
	else if(yr % 10 == 3)
	{
		char* u;
		if((u = read_xpm(num3, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCYR2, RTCDATEY, u, sw, sh);
	}
	else if(yr % 10 == 4)
	{
		char* u;
		if((u = read_xpm(num4, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCYR2, RTCDATEY, u, sw, sh);
	}
	else if(yr % 10 == 5)
	{
		char* u;
		if((u = read_xpm(num5, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCYR2, RTCDATEY, u, sw, sh);
	}
	else if(yr % 10 == 6)
	{
		char* u;
		if((u = read_xpm(num6, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCYR2, RTCDATEY, u, sw, sh);
	}
	else if(yr % 10 == 7)
	{
		char* u;
		if((u = read_xpm(num7, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCYR2, RTCDATEY, u, sw, sh);
	}
	else if(yr % 10 == 8)
	{
		char* u;
		if((u = read_xpm(num8, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCYR2, RTCDATEY, u, sw, sh);
	}
	else if(yr % 10 == 9)
	{
		char* u;
		if((u = read_xpm(num9, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCYR2, RTCDATEY, u, sw, sh);
	}


	int mth = d->month;
	if (mth < 10)
	{
		char* zero;
		if((zero = read_xpm(num0, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCM1, RTCDATEY, zero, sw, sh);
	}
	else if(mth / 10 == 1)
	{
		char* d;
		if((d = read_xpm(num1, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCM1, RTCDATEY, d, sw, sh);
	}
	else if (mth / 10 == 2)
	{
		char* d;
		if((d = read_xpm(num2, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCM1, RTCDATEY, d, sw, sh);
	}
	else if (mth / 10 == 3)
	{
		char* d;
		if((d = read_xpm(num3, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCM1, RTCDATEY, d, sw, sh);
	}
	else if (mth / 10 == 4)
	{
		char* d;
		if((d = read_xpm(num4, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCM1, RTCDATEY, d, sw, sh);
	}
	else if (mth / 10 == 5)
	{
		char* d;
		if((d = read_xpm(num5, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCM1, RTCDATEY, d, sw, sh);
	}

	if(mth % 10 == 0)
	{
		char* u;
		if((u = read_xpm(num0, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCM2, RTCDATEY, u, sw, sh);
	}
	else if(mth % 10 == 1)
	{
		char* u;
		if((u = read_xpm(num1, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCM2, RTCDATEY, u, sw, sh);
	}
	else if(mth % 10 == 2)
	{
		char* u;
		if((u = read_xpm(num2, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCM2, RTCDATEY, u, sw, sh);
	}
	else if(mth % 10 == 3)
	{
		char* u;
		if((u = read_xpm(num3, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCM2, RTCDATEY, u, sw, sh);
	}
	else if(mth % 10 == 4)
	{
		char* u;
		if((u = read_xpm(num4, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCM2, RTCDATEY, u, sw, sh);
	}
	else if(mth % 10 == 5)
	{
		char* u;
		if((u = read_xpm(num5, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCM2, RTCDATEY, u, sw, sh);
	}
	else if(mth % 10 == 6)
	{
		char* u;
		if((u = read_xpm(num6, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCM2, RTCDATEY, u, sw, sh);
	}
	else if(mth % 10 == 7)
	{
		char* u;
		if((u = read_xpm(num7, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCM2, RTCDATEY, u, sw, sh);
	}
	else if(mth % 10 == 8)
	{
		char* u;
		if((u = read_xpm(num8, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCM2, RTCDATEY, u, sw, sh);
	}
	else if(mth % 10 == 9)
	{
		char* u;
		if((u = read_xpm(num9, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCM2, RTCDATEY, u, sw, sh);
	}


	int day = d->day;
	if (day < 10)
	{
		char* zero;
		if((zero = read_xpm(num0, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCD1, RTCDATEY, zero, sw, sh);
	}
	else if(day / 10 == 1)
	{
		char* d;
		if((d = read_xpm(num1, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCD1, RTCDATEY, d, sw, sh);
	}
	else if (day / 10 == 2)
	{
		char* d;
		if((d = read_xpm(num2, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCD1, RTCDATEY, d, sw, sh);
	}
	else if (day / 10 == 3)
	{
		char* d;
		if((d = read_xpm(num3, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCD1, RTCDATEY, d, sw, sh);
	}

	if(day % 10 == 0)
	{
		char* u;
		if((u = read_xpm(num0, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCD2, RTCDATEY, u, sw, sh);
	}
	else if(day % 10 == 1)
	{
		char* u;
		if((u = read_xpm(num1, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCD2, RTCDATEY, u, sw, sh);
	}
	else if(day % 10 == 2)
	{
		char* u;
		if((u = read_xpm(num2, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCD2, RTCDATEY, u, sw, sh);
	}
	else if(day % 10 == 3)
	{
		char* u;
		if((u = read_xpm(num3, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCD2, RTCDATEY, u, sw, sh);
	}
	else if(day % 10 == 4)
	{
		char* u;
		if((u = read_xpm(num4, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCD2, RTCDATEY, u, sw, sh);
	}
	else if(day % 10 == 5)
	{
		char* u;
		if((u = read_xpm(num5, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCD2, RTCDATEY, u, sw, sh);
	}
	else if(day % 10 == 6)
	{
		char* u;
		if((u = read_xpm(num6, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCD2, RTCDATEY, u, sw, sh);
	}
	else if(day % 10 == 7)
	{
		char* u;
		if((u = read_xpm(num7, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCD2, RTCDATEY, u, sw, sh);
	}
	else if(day % 10 == 8)
	{
		char* u;
		if((u = read_xpm(num8, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCD2, RTCDATEY, u, sw, sh);
	}
	else if(day % 10 == 9)
	{
		char* u;
		if((u = read_xpm(num9, &sw, &sh)) == NULL)
			return;
		drawXPM(add, RTCD2, RTCDATEY, u, sw, sh);
	}

	int dtw = d->dotw;
	int dayw, dayh;
	char* dotw;
	if(dtw == 0)
	{
		if((dotw = read_xpm(SUNDAY, &dayw, &dayh)) == NULL)
			return;
	}
	else if (dtw == 1)
	{
		if((dotw = read_xpm(MONDAY, &dayw, &dayh)) == NULL)
			return;
	}
	else if (dtw == 2)
	{
		if((dotw = read_xpm(TUESDAY, &dayw, &dayh)) == NULL)
			return;
	}
	else if (dtw == 3)
	{
		if((dotw = read_xpm(WEDNESDAY, &dayw, &dayh)) == NULL)
			return;
	}
	else if (dtw == 4)
	{
		if((dotw = read_xpm(THURSDAY, &dayw, &dayh)) == NULL)
			return;
	}
	else if (dtw == 5)
	{
		if((dotw = read_xpm(FRIDAY, &dayw, &dayh)) == NULL)
			return;
	}
	else if (dtw == 6)
	{
		if((dotw = read_xpm(SATURDAY, &dayw, &dayh)) == NULL)
			return;
	}
	drawXPM(add, RTCDOTWX, RTCDOTWY, dotw, dayw, dayh);
}
