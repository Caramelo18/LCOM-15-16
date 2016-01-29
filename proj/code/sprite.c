#include "sprite.h"
// from class pdf's
/** Creates a new sprite with pixmap "pic"
 * (not zero) and position (within the screen limits), and
 * draws it in memory whose address is "base";
 * Returns NULL on invalid pixmap.
 */


Sprite *create_sprite(char *pic[], char *bas, int px, int py)
{
	//allocate space for the "object"
	Sprite *sp = (Sprite *) malloc ( sizeof(Sprite));
	if( sp == NULL )
		return NULL;
	// read the sprite pixmap
	sp->map = read_xpm(pic, &(sp->width), &(sp->height));
	if( sp->map == NULL )
	{
		free(sp);
		return NULL;
	}

	sp->x = px;
	sp->y = py;
	sp->xspeed = 0;
	sp->yspeed = 0;

	sp->cenx = sp->x + sp->width/2;
	sp->ceny = sp->y + sp->height/2;

	sp->subpx = 0;
	sp->subpy = 0;

	sp->radius = sp->width/2;

	drawXPM(bas, sp->x, sp->y, sp->map, sp->width, sp->height);

	return sp;
}


void destroy_sprite(Sprite *sp)
{
	if( sp == NULL )
		return;
	free(sp->map);
	free(sp);
	sp = NULL; // hopeless: pointer is passed by value
}

void updateSpeed(Sprite *sp1, Sprite *sp2, mov m1, mov m2)
{
	if(sp1->xspeed < MAXSPEED && sp1->xspeed >= NMAXSPEED && m1.pr == 1  && m1.pl == 0)
		sp1->xspeed += PACC;
	else if(sp1->xspeed <= MAXSPEED && sp1->xspeed > NMAXSPEED && m1.pl == 1 && m1.pr == 0)
		sp1->xspeed -= PACC;
	else if(m1.pl == 1 && m1.pr == 1)
		sp1->xspeed = 0;

	if(sp1->yspeed <= MAXSPEED && sp1->yspeed > NMAXSPEED && m1.pu == 1 && m1.pd == 0)
		sp1->yspeed -= PACC;
	else if(sp1->yspeed < MAXSPEED && sp1->yspeed >= NMAXSPEED && m1.pd == 1 && m1.pu == 0)
		sp1->yspeed += PACC;
	else if(m1.pl == 1 && m1.pr == 1)
		sp1->yspeed = 0;

	if(sp2->xspeed < MAXSPEED && sp2->xspeed >= NMAXSPEED && m2.pr == 1  && m2.pl == 0)
		sp2->xspeed += P2ACC;
	else if(sp2->xspeed <= MAXSPEED && sp2->xspeed > NMAXSPEED && m2.pl == 1 && m2.pr == 0)
		sp2->xspeed -= P2ACC;
	else if(m2.pl == 1 && m2.pr == 1)
		sp2->xspeed = 0;

	if(sp2->yspeed <= MAXSPEED && sp2->yspeed > NMAXSPEED && m2.pu == 1 && m2.pd == 0)
		sp2->yspeed -= P2ACC;
	else if(sp2->yspeed < MAXSPEED && sp2->yspeed >= NMAXSPEED && m2.pd == 1 && m2.pu == 0)
		sp2->yspeed += P2ACC;
	else if(m2.pl == 1 && m2.pr == 1)
		sp2->yspeed = 0;

}

void updateFrame(Sprite *sp1, Sprite *sp2, Sprite *bal, char *base, mov m1, mov m2, int *pl1score, int *pl2score, int secs)
{
	if(m1.pr == 0 && m1.pl == 0 && sp1->xspeed != 0)
	{
		if(sp1->xspeed > 0 && sp1->xspeed >= PDACC)
			sp1->xspeed -= PDACC;
		else if (sp1->xspeed < 0 && sp1->xspeed <= PDACC)
			sp1->xspeed += PDACC;
		else if(sp1->xspeed <= PDACC || sp1->xspeed >= -PDACC)
			sp1->xspeed = 0;
	}
	if(m1.pu == 0 && m1.pd == 0 && sp1->yspeed != 0)
	{
		if(sp1->yspeed > 0 && sp1->yspeed >= PDACC)
			sp1->yspeed -= PDACC;
		else if (sp1->yspeed < 0 && sp1->yspeed <= PDACC)
			sp1->yspeed += PDACC;
		else if(sp1->yspeed <= PDACC || sp1->yspeed >= -PDACC)
			sp1->yspeed = 0;
	}
	if(m2.pr == 0 && m2.pl == 0 && sp2->xspeed != 0)
	{
		if(sp2->xspeed > 0 && sp2->xspeed >= PDACC)
			sp2->xspeed -= PDACC;
		else if (sp2->xspeed < 0 && sp2->xspeed <= PDACC)
			sp2->xspeed += PDACC;
		else if(sp2->xspeed <= PDACC || sp2->xspeed >= -PDACC)
			sp2->xspeed = 0;
	}
	if(m2.pu == 0 && m2.pd == 0 && sp2->yspeed != 0)
	{
		if(sp2->yspeed > 0 && sp2->yspeed >= PDACC)
			sp2->yspeed -= PDACC;
		else if (sp2->yspeed < 0 && sp2->yspeed <= PDACC)
			sp2->yspeed += PDACC;
		else if(sp2->yspeed <= PDACC || sp2->yspeed >= -PDACC)
			sp2->yspeed = 0;
	}

	if(checkColision(sp1, sp2) == 0)
		updatePosition(sp1, sp2, bal);
	else
	{
		handleColision(sp1, sp2);
		updatePosition(sp1, sp2, bal);
	}

	if(checkColision(sp1,bal) == 1)
		moveBall(sp1, bal, m1.ps);
	if(checkColision(sp2,bal) == 1)
		moveBall(sp2, bal, m2.ps);

	updateBall(bal, sp1, sp2);
	touchWalls(sp1,sp2,bal, pl1score, pl2score, base);

	if(bal->xspeed != 0 || bal->yspeed != 0)
		animateBall(bal, secs);

	drawXPM(base, sp1->x, sp1->y, sp1->map, sp1->width, sp1->height);
	drawXPM(base, sp2->x, sp2->y, sp2->map, sp2->width, sp2->height);
	drawXPM(base, bal->x, bal->y, bal->map, bal->width, bal->height);
}

int checkColision(Sprite *sp1, Sprite *sp2) //, Sprite *ball)
{
	int distx = abs(sp2->cenx - sp1->cenx);
	int disty = abs(sp2->ceny - sp1->ceny);

	double d = sqrt((double)(distx * distx) + (double)(disty * disty));

	if (d <= (sp1->radius + sp2->radius)) // collision
		return 1;
	else
		return 0; // no collision
}

void updatePosition(Sprite *sp1, Sprite *sp2, Sprite *bal)
{
	sp1->subpx += sp1->xspeed;
	sp1->subpy += sp1->yspeed;

	sp2->subpx += sp2->xspeed;
	sp2->subpy += sp2->yspeed;

	if(sp1->subpx >= 1)
	{
		sp1->x += sp1->subpx / 1;
		sp1->subpx -= sp1->subpx / 1;
		if(checkColision(sp1, bal) == 1)
			sp1->x -= 2;
	}
	else if(sp1->subpx <= -1)
	{
		sp1->x += sp1->subpx / 1;
		sp1->subpx -= sp1->subpx / 1;
		if(checkColision(sp1, bal) == 1)
			sp1->x += 2;
	}
	if(sp1->subpy >= 1)
	{
		sp1->y += sp1->subpy / 1;
		sp1->subpy -= sp1->subpy / 1;
		if(checkColision(sp1, bal) == 1)
			sp1->y -= 2;
	}
	else if(sp1->subpy <= -1)
	{
		sp1->y += sp1->subpy / 1;
		sp1->subpy -= sp1->subpy / 1;
		if(checkColision(sp1, bal) == 1)
			sp1->y += 2;
	}


	if(sp2->subpx >= 1)
	{
		sp2->x += sp2->subpx / 1;
		sp2->subpx -= sp2->subpx / 1;
		if(checkColision(sp2, bal) == 1)
			sp2->x -= 2;
	}
	else if(sp2->subpx <= -1)
	{
		sp2->x += sp2->subpx / 1;
		sp2->subpx -= sp2->subpx / 1;
		if(checkColision(sp2, bal) == 1)
			sp2->x += 2;
	}
	if(sp2->subpy >= 1)
	{
		sp2->y += sp2->subpy / 1;
		sp2->subpy -= sp2->subpy / 1;
		if(checkColision(sp2, bal) == 1)
			sp2->y -= 2;
	}
	else if(sp2->subpy <= -1)
	{
		sp2->y += sp2->subpy / 1;
		sp2->subpy -= sp2->subpy / 1;
		if(checkColision(sp2, bal) == 1)
			sp2->y += 2;
	}

	sp1->cenx = sp1->x + sp1->width/2;
	sp1->ceny = sp1->y + sp1->height/2;

	sp2->cenx = sp2->x + sp2->width/2;
	sp2->ceny = sp2->y + sp2->height/2;
}


void updateBall(Sprite *bal, Sprite *sp1, Sprite *sp2)
{
	if(checkColision(bal, sp1) == 0 && checkColision(bal, sp2) == 0)
	{
		if(bal->xspeed > 0 && bal->xspeed > BGRIP)
			bal->xspeed -= BGRIP;
		else if (bal->xspeed < 0 && bal->xspeed < BGRIP)
			bal->xspeed += BGRIP;
		else
			bal->xspeed = 0;

		if(bal->yspeed > 0 && bal->yspeed > BGRIP)
			bal->yspeed -= BGRIP;
		else if (bal->yspeed < 0 && bal->yspeed < BGRIP)
			bal->yspeed += BGRIP;
		else
			bal->yspeed = 0;
	}
	/*else if(checkColision(bal, sp1) == 1 && sp1->xspeed == 0 && bal->xspeed != 0)
	{
		sp1->xspeed = bal->xspeed/2;
		sp1->yspeed = bal->yspeed/2;
		bal->xspeed = 0;
		bal->yspeed = 0;
	}
	else if(checkColision(bal, sp2) == 1 && sp2->xspeed == 0 && bal->xspeed != 0)
	{
		sp2->xspeed = bal->xspeed/2;
		sp2->yspeed = bal->yspeed/2;
		bal->xspeed = 0;
		bal->yspeed = 0;
	}
	else
	{
		bal->xspeed = 0;
		bal->yspeed = 0;
	}*/


	bal->subpx += bal->xspeed;
	bal->subpy += bal->yspeed;


	if(bal->subpx >= 1)
	{
		bal->x += bal->subpx / 1;
		bal->subpx -= bal->subpx / 1;
	}
	else if(bal->subpx <= -1)
	{
		bal->x += bal->subpx / 1;
		bal->subpx -= bal->subpx / 1;
	}
	if(bal->subpy >= 1)
	{
		bal->y += bal->subpy / 1;
		bal->subpy -= bal->subpy / 1;
	}
	else if(bal->subpy <= -1)
	{
		bal->y += bal->subpy / 1;
		bal->subpy -= bal->subpy / 1;
	}

	bal->cenx = bal->x + bal->width/2;
	bal->ceny = bal->y + bal->height/2;
}

void handleColision(Sprite *sp1, Sprite *sp2)
{
	if(sp1->xspeed > 0 && sp2->xspeed == 0)
	{
		sp2->xspeed = sp1->xspeed;
		sp1->xspeed = 0;
	}
	else if (sp2->xspeed < 0 && sp1->xspeed == 0)
	{
		sp1->xspeed = sp2->xspeed;
		sp2->xspeed = 0;
	}
	else if (sp2->xspeed > 0 && sp1->xspeed == 0)
	{
		sp1->xspeed = sp2->xspeed;
		sp2->xspeed = 0;
	}
	else if (sp2->xspeed != 0 && sp1->xspeed != 0)
	{
		sp1->xspeed = -sp1->xspeed;
		sp2->xspeed = -sp2->xspeed;
	}
	else if (sp1->xspeed < 0 && sp2->xspeed == 0)
	{
		sp2->xspeed = sp1->xspeed;
		sp1->xspeed = 0;
	}
	else if((sp1->xspeed * sp2->xspeed) > 0 && (sp2->xspeed > sp1->xspeed))
		sp1->xspeed = sp2->xspeed;
	else if((sp1->xspeed * sp2->xspeed) > 0 && (sp1->xspeed > sp2->xspeed))
		sp2->xspeed = sp1->xspeed;


	if(sp1->yspeed > 0 && sp2->yspeed == 0)
	{
		sp2->yspeed = sp1->yspeed;
		sp1->yspeed = 0;
	}
	else if (sp2->yspeed < 0 && sp1->yspeed == 0)
	{
		sp1->yspeed = sp2->yspeed;
		sp2->yspeed = 0;
	}
	else if (sp2->yspeed > 0 && sp1->yspeed == 0)
	{
		sp1->yspeed = sp2->yspeed;
		sp2->yspeed = 0;
	}
	else if (sp2->yspeed != 0 && sp1->yspeed != 0)
	{
		sp1->yspeed = -sp1->yspeed;
		sp2->yspeed = -sp2->yspeed;
	}
	else if (sp1->yspeed < 0 && sp2->yspeed == 0)
	{
		sp2->yspeed = sp1->yspeed;
		sp1->yspeed = 0;
	}
	else if((sp1->yspeed * sp2->yspeed) > 0 && (sp2->yspeed > sp1->yspeed))
		sp1->yspeed = sp2->yspeed;
	else if((sp1->yspeed * sp2->yspeed) > 0 && (sp1->yspeed > sp2->yspeed))
		sp2->yspeed = sp1->yspeed;
}

void moveBall(Sprite *sp1, Sprite *bal, int shoot)
{
	if(shoot == 0)
	{
		if(sp1->xspeed > 0 && bal->xspeed <= 0)
			bal->xspeed = sp1->xspeed;
		else if(sp1->xspeed < 0 && bal->xspeed >= 0)
			bal->xspeed = sp1->xspeed;
		else if ((bal->xspeed > sp1->xspeed && sp1->xspeed >= 0) || (bal->xspeed < sp1->xspeed && sp1->xspeed <= 0))
		{
			sp1->xspeed = bal->xspeed / 2;
			bal->xspeed = 0;
		}

		if(sp1->yspeed > 0 && bal->yspeed <= 0)
			bal->yspeed = BSPEED;
		else if(sp1->yspeed < 0 && bal->yspeed >= 0)
			bal->yspeed = NBSPEED;
		else if ((bal->yspeed > sp1->yspeed && sp1->yspeed >= 0) || (bal->yspeed < sp1->yspeed && sp1->yspeed <= 0))
		{
			sp1->yspeed = bal->yspeed / 2;
			bal->yspeed = 0;
		}

	}
	else if(shoot == 1)
	{
		if(sp1->xspeed == 0 || sp1->yspeed == 0)
		{
			bal->xspeed = sp1->xspeed*3.3;
			bal->yspeed = sp1->yspeed*3.3;
		}
		else
		{
			bal->xspeed = sp1->xspeed*2;
			bal->yspeed = sp1->yspeed*2;
		}

		sp1->xspeed = sp1->xspeed / 2;
		sp1->yspeed = sp1->xspeed / 2;
	}
}

void touchWalls(Sprite *sp1, Sprite *sp2, Sprite * bal, int *pl1score, int *pl2score, char* base)
{
	if(bal->x <= LEFTL)
	{
		if(bal->ceny + bal->radius  >= DPOST || bal->y <= UPOST)
		{
			bal->xspeed = -bal->xspeed / 2;
			bal->x = LEFTL;
			bal->cenx = bal->x + bal->width/2;
		}
		else if ((bal->ceny + bal->radius  <= DPOST && bal->y >= UPOST)  && bal->x + bal->width <= LEFTL)
		{
			*pl2score += 1;
			displayScore(base, pl1score, pl2score);
			kickOff(sp1, sp2, bal);
		}

		if(bal->x <= LNET)
		{
			bal->xspeed = 0;
			bal->x = LNET;
			bal->cenx = bal->x + bal->width/2;
		}
		/*if(bal->y >= UPOST)
		{
			bal->yspeed = 0;
			bal->y = UPOST;
			bal->ceny = bal->y + bal->height/2;
		}
		else if (bal->y <= DPOST)
		{
			bal->yspeed = 0;
			bal->y = DPOST;
			bal->ceny = bal->y + bal->height/2;
		}*/


	}
	else if (bal->cenx + bal->radius >= RIGHTL)
	{
		if(bal->ceny + bal->radius >= DPOST || bal->y <= UPOST)
		{
			bal->xspeed = -bal->xspeed / 2;
			bal->x = RIGHTL - bal->width;
			bal->cenx = bal->x + bal->width/2;
		}
		else if ((bal->ceny + bal->radius <= DPOST && bal->y >= UPOST) && bal->x >= RIGHTL)
		{
			*pl1score += 1;
			displayScore(base, pl1score, pl2score);
			kickOff(sp1, sp2, bal);
		}
		if(bal->cenx + bal->radius >= RNET)
		{
			bal->xspeed = 0;
			bal->x = RNET - bal->width;
			bal->cenx = bal->x + bal->width/2;
		}
		/*if(bal->y <= UPOST)
		{
			bal->yspeed = 0;
			bal->y = UPOST;
			bal->ceny = bal->y + bal->height/2;
		}
		else if (bal->y >= DPOST)
		{
			bal->yspeed = 0;
			bal->y = DPOST;
			bal->ceny = bal->y + bal->height/2;
		}*/
	}

	if(bal->y <= UPL)
	{
		bal->yspeed = -bal->yspeed / 2;
		bal->y = UPL;
		bal->ceny = bal->y + bal->height/2;
	}
	else if(bal->ceny + bal->radius  >= DOWNL)
	{
		bal->yspeed = -bal->yspeed / 2;
		bal->y = DOWNL - bal->height;
		bal->ceny = bal->y + bal->height/2;
	}

	if(sp1->x <= PLEFTL)
	{
		sp1->xspeed = -sp1->xspeed / 2;
		sp1->x = PLEFTL;
		sp1->cenx = sp1->x + sp1->width/2;
	}
	else if (sp1->cenx + sp1->radius >= PRIGHTL)
	{
		sp1->xspeed = -sp1->xspeed / 2;
		sp1->x = PRIGHTL - sp1->width;
		sp1->cenx = sp1->x + sp1->width/2;
	}
	if(sp1->y <= PUPL)
	{
		sp1->yspeed = -sp1->yspeed / 2;
		sp1->y = PUPL;
		sp1->ceny = sp1->y + sp1->height/2;
	}
	else if(sp1->ceny + sp1->radius  >= PDOWNL)
	{
		sp1->yspeed = -sp1->yspeed / 2;
		sp1->y = PDOWNL - sp1->height;
		sp1->ceny = sp1->y + sp1->height/2;
	}

	if(sp2->x <= PLEFTL)
	{
		sp2->xspeed = 0;
		sp2->x = PLEFTL;
		sp2->cenx = sp2->x + sp2->width/2;
	}
	else if (sp2->cenx + sp2->radius >= PRIGHTL)
	{
		sp2->xspeed = 0;
		sp2->x = PRIGHTL - sp2->width;
		sp2->cenx = sp2->x + sp2->width/2;
	}
	if(sp2->y <= PUPL)
	{
		sp2->yspeed = 0;
		sp2->y = PUPL;
		sp2->ceny = sp2->y + sp2->height/2;
	}
	else if(sp2->ceny + sp2->radius  >= PDOWNL)
	{
		sp2->yspeed = 0;
		sp2->y = PDOWNL - sp2->height;
		sp2->ceny = sp2->y + sp2->height/2;
	}
}

void kickOff(Sprite *sp1, Sprite *sp2, Sprite * bal)
{
	sp1->x = P1STARTPOSX;
	sp1->cenx = sp1->x + sp1->width/2;
	sp1->y = PSTARTPOSY;
	sp1->ceny = sp1->y + sp1->height/2;
	sp1->xspeed = 0;
	sp1->yspeed = 0;

	sp2->x = P2STARTPOSX;
	sp2->cenx = sp2->x + sp2->width/2;
	sp2->y = PSTARTPOSY;
	sp2->ceny = sp2->y + sp2->height/2;
	sp2->xspeed = 0;
	sp2->yspeed = 0;

	bal->x = BSTARTPOSX;
	bal->cenx = bal->x + bal->width/2;
	bal->y = BSTARTPOSY;
	bal->ceny = bal->y + bal->height/2;
	bal->xspeed = 0;
	bal->yspeed = 0;
}

void animateBall(Sprite *bal, int secs)
{
	if(secs % 10 == 0)
	{
		int s = secs / 10;
		if(s % 2 == 0)
			bal->map = read_xpm(ball, &(bal->width), &(bal->height));
		else
			bal->map = read_xpm(ball2, &(bal->width), &(bal->height));
	}
}
