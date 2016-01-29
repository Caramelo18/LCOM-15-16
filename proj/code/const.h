#ifndef __CONST_H
#define __CONST_H

#include <minix/drivers.h>
#include <minix/syslib.h>
#include <minix/com.h>
#include <minix/sysutil.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>
#include "math.h"

#define OUT_BUF 0x60 /** */
#define IN_BUF 0x64 /** */
#define STAT_REG 0x64 /** */
#define IBF 0x02 /** */
#define WRITEB 0xD4 /** */
#define SSTRM 0xF4 /** */
#define DISTRM 0xF5 /** */
#define STATREQ 0xE9 /** */
#define ACK 0xFA /** */
#define NACK 0xFE /** */
#define ERROR 0xFC /** */
#define WAIT_KBC 20000 /** */
#define BIT(n) (0x01<<(n))

#define PAR_ERR 0x80 /** */
#define TO_ERR 0x40 /** */

#define VBE 0x10 /** */
#define SETVBEMODE 0x02 /** */
#define VBEBIOSFUNC 0x4F /** */
#define VBEMI 0x01 /** */
#define FCF 0x01 /** */
#define FCNS 0x02 /** */
#define FICVM 0x03 /** */
#define LINEAR_MODEL_BIT 14 /** */

#define RTC_ADDR_REG 0x70 /** */
#define RTC_DATA_REG 0x71 /** */
#define REGA 10 /** */
#define REGB 11 /** */
#define REGC 12 /** */


#define TIMER0_IRQ 0 /** */
#define KBD_IRQ 1 /** */
#define MOUSE_IRQ 12 /** */
#define RTC_IRQ 8 /** */


#define BIT(n) (0x01<<(n)) /** */
#define PB2BASE(x) (((x) >> 4) & 0x0F000) /** */
#define PB2OFF(x) ((x) & 0x0FFFF) /** */


static int TIMER_HOOK = 0; /** */
static int KBD_HOOK = 1; /** */
static int MOUSE_HOOK = 2; /** */
static int RTC_HOOK = 3; /** */


#define CLOCKY 90 /** */
#define CLOCKMIN 620 /** */
#define CLOCKDP 642 /** */
#define CLOCKSEC1 655 /** */
#define CLOCKSEC2 680 /** */

#define RTCHRS1 40 /** */
#define RTCHRS2 60 /** */
#define RTCMIN1 90 /** */
#define RTCMIN2 110 /** */
#define RTCSEC1 140 /** */
#define RTCSEC2 160 /** */
#define RTCDATEY 130 /** */
#define RTCA1 40 /** */
#define RTCA2 60 /** */
#define RTCYR1 80 /** */
#define RTCYR2 100 /** */
#define RTCM1 135 /** */
#define RTCM2 155 /** */
#define RTCD1 190 /** */
#define RTCD2 210 /** */
#define RTCDPHM 81 /** */
#define RTCDPMS 131 /** */
#define RTCDPY 93 /** */
#define RTCTR1 115 /** */
#define RTCTR2 169 /** */
#define RTCTRY 132 /** */
#define RTCDOTWY 168 /** */
#define RTCDOTWX 42 /** */

#define P1SCOREX 210 /** */
#define P2SCOREX 270 /** */
#define SCORESEP 238 /** */
#define SCOREY 90 /** */

#define MENSELX 280 /** */
#define MENSELY 306 /** */
#define LAX 250 /** */
#define RAX 754 /** */

#define GLX 510 /** */
#define GLY 475 /** */
#define TLX 510 /** */
#define TLY 397 /** */
#define MLX 500 /** */
#define MLY 560 /** */

#define IGGCOL 47 /** */

#define MAXSPEED 2 /** */
#define NMAXSPEED -2 /** */
#define PACC 1 /** */
#define P2ACC 2 /** */
#define PDACC 0.2 /** */
#define BGRIP 0.05 /** */
#define BSPEED 2.1 /** */
#define NBSPEED -2.1 /** */

#define LEFTL 99 /** */
#define RIGHTL 926 /** */
#define UPL 185 /** */
#define DOWNL 716 /** */
#define PUPL 137 /** */
#define PLEFTL 0 /** */
#define PRIGHTL 1024 /** */
#define PDOWNL 768 /** */

#define UPOST 359 /** */
#define DPOST 542 /** */

#define LNET 21 /** */
#define RNET 1003 /** */

#define P1STARTPOSX 301 /** */
#define P2STARTPOSX 701 /** */
#define PSTARTPOSY 420 /** */
#define BSTARTPOSX 501 /** */
#define BSTARTPOSY 430 /** */

#endif
