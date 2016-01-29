#include <minix/drivers.h>
#include <sys/video.h>
#include <sys/mman.h>

#include <assert.h>

#include "vt_info.h"

#include "video_txt.h"

/* Private global variables */

static char *video_mem;		/* Address to which VRAM is mapped */

static unsigned scr_width;	/* Width of screen in columns */
static unsigned scr_lines;	/* Height of screen in lines */

void vt_fill(char ch, char attr)
{
	  char *vptr;         /* Pointer to video RAM */
	  vptr = video_mem;   /* Point to first byte of video RAM */
	  int i = 0;
	  int lim = scr_width * scr_lines;
	  for (i; i < lim; i++)
	  {
	  *vptr = ch;        /* First byte, is for the character to display */
	  vptr++;             /* Point to second byte of video RAM */
	  *vptr = attr;  /* ... which is for the attributes of the first character */
	  vptr++;
	  }
}

void vt_blank()
{

  char *vptr = video_mem;
  int i = 0;
  int lim = scr_width * scr_lines;
  for(i; i < lim; i++)
  {
	  *vptr = 0x50;
	  vptr++;
	  *vptr = 0x77;
	  vptr++;
  }
}

int vt_print_char(char ch, char attr, int r, int c)
{
  
  char *vptr = video_mem;
  vptr = vptr + (scr_width * r * 2);
  vptr = vptr + c * 2;
  *vptr = ch;
  vptr++;
  *vptr = attr;
  vptr++;

  return 0;
}

int vt_print_string(char *str, char attr, int r, int c) {

	char *vptr = video_mem;
	vptr = vptr + (scr_width * r * 2);
	vptr = vptr + c * 2;

		  while(*str != 0)
		  {
			  *vptr = *str;
			  vptr++;
			  str++;
			  *vptr = attr;
			  vptr++;
		  }

}

int alg(int num, int exp)
{
	int n = num;
	int i = 0;
	for (i; i < exp; i++)
	{
		n = n / 10;
	}
	n = n % 10;
	return n;
}

int vt_print_int(int num, char attr, int r, int c)
{
	char *vptr = video_mem;

	int numero = num;
	int contador = 1;


	while(numero > 10)
	{
		contador++;
		numero = numero / 10;
	}
	int cont = 0;
	double ch =  num / pow(10.0, (double)contador - 1);
	int n = 48;

	for (contador; contador > 0; contador--)
	{
		int ch = alg(num, contador - 1);
		vt_print_char((char)n + ch, attr, r, c + cont);
		cont++;
	}



/*
	int cont = contador;
	int n[contador];
	int i = 0;
	for (contador; contador > 0; contador--)
	{
		n[contador - 1] = num % pow(10,i);
		i++;
	}

	int j = 0;
	for (cont; cont > 0; cont --)
	{
		*vptr = n[j];
		j++;
		vptr++;
		*vptr = attr;
		vptr++;
	}
*/
	return 0;
}

int vt_draw_frame(int width, int height, char attr, int r, int c) {

	char *vptr = video_mem;
	vptr = vptr + (scr_width * r * 2);
	vptr = vptr + c * 2;

	int i = 0;
	for(i; i < width; i++)
	{
		*vptr = 0xCD;
		vptr++;
		*vptr = attr;
		vptr++;
	}

	vptr = vptr + 2 * (scr_width - width);

	int j = 0;
	for (j; j < (height - 2);j++)
	{
		*vptr = 0xBA;
		vptr++;
		*vptr = attr;
		vptr = vptr + 2 * (width - 1);
		*vptr = 0xBA;
		vptr++;
		*vptr = attr;
		vptr = vptr + 2 * (scr_width - width);
	}

	int k = 0;
	for(k; k < width ; k++)
	{
		*vptr = 0xCD;
		vptr++;
		*vptr = attr;
		vptr++;
	}

	return 0;
}

/*
 * THIS FUNCTION IS FINALIZED, do NOT touch it
 */

char *vt_init(vt_info_t *vi_p) {

  int r;
  struct mem_range mr;

  /* Allow memory mapping */

  mr.mr_base = (phys_bytes)(vi_p->vram_base);
  mr.mr_limit = mr.mr_base + vi_p->vram_size;

  if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
	  panic("video_txt: sys_privctl (ADD_MEM) failed: %d\n", r);

  /* Map memory */

  video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vi_p->vram_size);

  if(video_mem == MAP_FAILED)
	  panic("video_txt couldn't map video memory");

  /* Save text mode resolution */

  scr_lines = vi_p->scr_lines;
  scr_width = vi_p->scr_width;

  return video_mem;
}
