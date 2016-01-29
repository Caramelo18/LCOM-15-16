#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>

#include "vbe.h"
#include "test5.h"

/* Constants for VBE 0x105 mode */

/* The physical address may vary from VM to VM.
 * At one time it was 0xD0000000
 *  #define VRAM_PHYS_ADDR    0xD0000000 
 * Currently on lab B107 is 0xF0000000
 * Better run my version of lab5 as follows:
 *     service run `pwd`/lab5 -args "mode 0x105"
 */
#define VRAM_PHYS_ADDR	0xF0000000
#define H_RES             1024
#define V_RES		  768
#define BITS_PER_PIXEL	 8


/* Private global variables */

static char *video_mem;		/* Process address to which VRAM is mapped */
static unsigned h_res;		/* Horizontal screen resolution in pixels */
static unsigned v_res;		/* Vertical screen resolution in pixels */
static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */

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

	if( sys_int86(&reg) != OK )
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

void vbe_print_info(unsigned short mode)
{
	vbe_mode_info_t vid;
	if(vbe_get_mode_info(mode, &vid) == -1)
		return;
	vg_exit(); // clears the screen
	printf("\tHorizontal resolution: %d\n"
			"\tVertical resolution: %d \n"
			"\tBits per pixel: %d \n"
			"\tAdress: %x\n", h_res, v_res, bits_per_pixel, vid.PhysBasePtr);

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
