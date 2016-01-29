#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>

#include "vbe.h"
#include "lmlib.h"
#include "test5.h"


#define PB2BASE(x) (((x) >> 4) & 0x0F000)
#define PB2OFF(x) ((x) & 0x0FFFF)


int vbe_get_mode_info(unsigned short mode, vbe_mode_info_t *vmi_p)
{
	mmap_t inf;
	struct reg86u reg;

	if(lm_init() == NULL)
	{
		vg_exit();
		printf("fail initializing memory \n");
		return -1;
	}

	if(lm_alloc(sizeof(vbe_mode_info_t), &inf) == NULL)
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

int vbe_controller_info(vbeInfoBlock *inf)
{
	mmap_t map;
	struct reg86u reg;

	if(lm_alloc(sizeof(vbeInfoBlock), &map) == NULL)
	{
		printf("fail allocating memory \n");
		return -1;
	}

	reg.u.b.intno = VBE;
	reg.u.b.ah = VBEBIOSFUNC;
	reg.u.b.al = VBECONTINF;
	reg.u.w.es = PB2BASE(map.phys);
	reg.u.w.di = PB2OFF(map.phys);

	if( sys_int86(&reg) != OK )
	{
		printf("\tvbe_get_mode_info: sys_int86() failed \n");
		return -1;
	}

	unsigned long ret = reg.u.w.ax;

	if(ret == FCF || ret == FCNS || ret == FICVM)
	{
		lm_free(&map);
		return -1;
	}
	else
	{
		*inf = *((vbeInfoBlock*)map.virtual);
		lm_free(&map);
		return 0;
	}
}
