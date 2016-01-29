#include "keyboard.h"
#include "timer.h"
#include "const.h"
#include "pixmap.h"
#include "sprite.h"
#include "mouse.h"
#include "Campo.h"
#include "rtc.h"
#include "goal-limit.h"
#include "minute-limit.h"
#include "mode.h"
#include "menu-selection.h"
#include "mtxt.h"



typedef enum {PLAY, TIMEL, GOALL, MODE, EXITG} MenuOptions;

/**
 * \brief Altera o estado atual do menu mediante a posicao y, com o teclado
 * \param current O estado que se vai definir
 * \param y A posicao nos eixos do y
 */
void updateOption(MenuOptions* current, int y);

/**
 * \brief Altera o estado atual do menu mediante as posicos mx e my, com o rato
 * \param current Oestado que se vai definir
 * \param mx Posicao em x do rato
 * \param my Posicao em y do rato
 * \param mensely Posicao inicial do y da caixa de selecao
 * \param lb Informacao acerca de estar ou nao pressionado o botao esquerdo do rato
 * \param rl Informacao da mudanca do tempo/golos/modo no menu
 */
void updateMouseSel(MenuOptions* current, int mx, int my, int* mensely, int lb, int* rl);

/**
 * \brief Altera o tempo na opcao do menu
 * \param rl Informacao da mudanca da opcao
 * \param time Tempo que vai ser alterado
 */
void changeTime(int rl, int* time);

/**
 * \brief Altera o numero de golos na opcao do menu
 * \param rl Informacao da mudanca da opcao
 * \param goals Numero de golos que vao ser alterados
 */
void changeGoals(int rl, int* goals);

/**
 * \brief Altera o modo na opcao do menu
 * \param rl Informacao da mudanca da opcao
 * \param mode Modo que vai ser alterado
 */
void changeMode(int rl, int* mode);

/**
 * \brief Funcao que inicia um dos dois jogos mediante o modo escolhido
 * \param mode Modo escolhido
 * \param kbc_set Retorno do hook do teclado
 * \param timer_set Retorno do hook do tempo
 * \param mouse_set Retorno do hook do rato
 * \param add Apontador para a primeira posicao da memoria grafica
 * \param time Tempo de jogo definido
 * \param goals Numero de golos definido
 */
void modeSelector(int mode, int kbc_set, int timer_set, int mouse_set, char* add, int time, int goals);

/**
 * \brief Funcao que inicia o jogo apenas com o teclado
 * \param kbc_set Retorno do hook do teclado
 * \param timer_set Retorno do hook do tempo
 * \param add Apontador para a primeira posicao da memoria grafica
 * \param time Tempo de jogo definido
 * \param goals Numero de golos definido
 */
void game(int kbc_set, int timer_set, char* add, int time, int goals)
{
	int n = 0;
	int limit = time * 60;
	int min = time - 1;
	int sec = 0;
	int ssec = 60;
	int pl1score = 0;
	int pl2score = 0;

	unsigned int fsize = getHRes() * getVRes() * getBitsPerPixel()/8;
	char* frame = malloc(fsize);

	int bkgw, bkgh;
	char* bkg = read_xpm(Campo, &bkgw, &bkgh);

	startTime(frame, time);
	displayScore(frame, &pl1score, &pl2score);
	Sprite* pl1 = create_sprite(p1, frame, P1STARTPOSX, PSTARTPOSY);
	Sprite* pl2 = create_sprite(p2, frame, P2STARTPOSX, PSTARTPOSY);
	Sprite* bal = create_sprite(ball, frame, BSTARTPOSX, BSTARTPOSY);
	memcpy(add, frame, fsize);

	mov m1, m2;
	m1.pu = 0;
	m1.pd = 0;
	m1.pl = 0;
	m1.pr = 0;
	m1.ps = 0;

	m2.pu = 0;
	m2.pd = 0;
	m2.pl = 0;
	m2.pr = 0;
	m2.ps = 0;

	while((sec <= limit) && (((pl1score < goals) && (pl2score < goals)) || (goals == 6)))
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
		{ // received notification
			switch (_ENDPOINT_P(msg.m_source))
			{
			case HARDWARE: // hardware interrupt notification
				if ((msg.NOTIFY_ARG & kbc_set))
					// subscribed interrupt
				{
					readKeyboard(&m1, &m2);
					updateSpeed(pl1, pl2, m1, m2);
				}
				if (msg.NOTIFY_ARG & timer_set)
				{
					n++;
					drawXPM(frame, 0, 0,bkg, bkgw,bkgh);
					updateFrame(pl1, pl2, bal, frame, m1, m2, &pl1score, &pl2score, n);
					if(n == 60)
					{
						n = 0;
						updateTime(frame, &min, &ssec);
						sec++;
					}
					memcpy(add, frame, fsize);
				}
				break;
			}
		}
	}

	destroy_sprite(pl1);
	destroy_sprite(pl2);
	destroy_sprite(bal);
	memset(bkg, 0, fsize);
	memset(frame, 0, fsize); // sets the memory used to store the frames all black
	free(bkg);
	free(frame);

}

/**
 * \brief Funcao que inicia o jogo com o teclado e o rato
 * \param kbc_set Retorno do hook do teclado
 * \param timer_set Retorno do hook do tempo
 * \param mouse_set Retorno do hook do rato
 * \param add Apontador para a primeira posicao da memoria grafica
 * \param time Tempo de jogo definido
 * \param goals Numero de golos definido
 */
void gameM(int kbc_set, int timer_set, int mouse_set, char* add, int time, int goals)
{
	int n = 0;
	int limit = time * 60;
	int sec = 0;
	int min = time - 1;
	int ssec = 60;
	int pl1score = 0;
	int pl2score = 0;
	int byt = 0;
	int counter = 0;


	unsigned int fsize = getHRes() * getVRes() * getBitsPerPixel()/8;
	char* frame = malloc(fsize);

	int bkgw, bkgh;
	char* bkg = read_xpm(Campo, &bkgw, &bkgh);


	startTime(frame, time);
	displayScore(frame, &pl1score, &pl2score);
	Sprite* pl1 = create_sprite(p1, frame, P1STARTPOSX, PSTARTPOSY);
	Sprite* pl2 = create_sprite(p2, frame, P2STARTPOSX, PSTARTPOSY);
	Sprite* bal = create_sprite(ball, frame, BSTARTPOSX, BSTARTPOSY);
	memcpy(add, frame, fsize);

	mov m1, m2;
	m1.pu = 0;
	m1.pd = 0;
	m1.pl = 0;
	m1.pr = 0;
	m1.ps = 0;

	m2.pu = 0;
	m2.pd = 0;
	m2.pl = 0;
	m2.pr = 0;
	m2.ps = 0;

	unsigned long packet[3];


	while((sec <= limit) && (((pl1score < goals) && (pl2score < goals)) || (goals == 6)))
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
		{ // received notification
			switch (_ENDPOINT_P(msg.m_source))
			{
			case HARDWARE: // hardware interrupt notification
				if ((msg.NOTIFY_ARG & kbc_set))
					// subscribed interrupt
				{
					readKeyboardM(&m1);
					updateSpeed(pl1, pl2, m1, m2);
				}
				if (msg.NOTIFY_ARG & mouse_set)
				{
					readMouse(&m2, packet, &byt);
					if(byt == 2)
					{
						byt = 0;
						updateSpeed(pl1, pl2, m1, m2);
						counter = 0;
					}
				}
				if (msg.NOTIFY_ARG & timer_set)
				{
					counter++;
					n++;
					drawXPM(frame, 0, 0,bkg, bkgw,bkgh);
					updateFrame(pl1, pl2, bal, frame, m1, m2, &pl1score, &pl2score, n);
					if(n == 60)
					{
						n = 0;
						updateTime(frame, &min, &ssec);
						sec++;
					}
					if(counter == 15)
					{
						m2.pr = 0;
						m2.pl = 0;
						m2.pu = 0;
						m2.pd = 0;
						m2.ps = 0;
						updateSpeed(pl1, pl2, m1, m2);
						counter = 0;
					}
					memcpy(add, frame, fsize);
				}
				break;
			}
		}
	}

	destroy_sprite(pl1);
	destroy_sprite(pl2);
	destroy_sprite(bal);
	memset(bkg, 0, fsize);
	memset(frame, 0, fsize); // sets the memory used to store the frames all black
	free(bkg);
	free(frame);


	return;
}

int main()
{
	sef_startup();

	int kbc_set = keyboard_subscribe();
	if(kbc_set == -1)
		return -1;

	int timer_set = timer_subscribe();
	if(timer_set == -1)
		return -1;

	int mouse_set = mouse_subscribe();
	if (mouse_set == -1)
		return -1;

	if (mouse_write_command(WRITEB, SSTRM) == -1)
		return -1;

	int rtc_set = rtc_subscribe();
	if(rtc_set == -1)
		return -1;

	char* add = (char *)vg_init(0x105);
	if(add == NULL)
		return -1;

	int msw, msh;
	char* mensel = read_xpm(menu_selection, &msw, &msh);


	int mensely = MENSELY;
	int ent = 0;
	int rl = 0;

	int exit = 0;
	int n = 0;
	int sec = 0;

	Date d;
	getDate(&d);
	enable();

	int mx = getHRes()/2;
	int my = getVRes()/2;
	unsigned long mousePacket[3];
	unsigned int byt = 0;
	int lb;

	unsigned int fsize = getHRes() * getVRes() * getBitsPerPixel()/8;
	char* frame = malloc(fsize);

	int bkgw, bkgh;
	//char* mtxt = read_xpm(menutext, &bkgw, &bkgh);

	int tlimw, tlimh;
	char* tlim = read_xpm(threemins, &tlimw, &tlimh);
	int time = 3;

	int glimw, glimh;
	char* glim = read_xpm(threegoals, &glimw, &glimh);
	int goals = 3;

	int mlimw, mlimh;
	char* md = read_xpm(keybrd, &mlimw, &mlimh);
	int mode = 0;

	int rw, rh;
	char* lr = read_xpm(lrow, &rw, &rh);
	char* rr = read_xpm(rrow, &rw, &rh);

	int mw, mh;
	char* mMouse = read_xpm(mouse, &mw, &mh);

	MenuOptions current = PLAY;


	while (exit != 1)
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
		{ // received notification
			switch (_ENDPOINT_P(msg.m_source))
			{
			case HARDWARE: // hardware interrupt notification
				if ((msg.NOTIFY_ARG & kbc_set))
					// subscribed interrupt
				{
					scrollMenu(&mensely, &ent, &rl);
					updateOption(&current, mensely);
					if(current == PLAY && ent == 1)
						modeSelector(mode, kbc_set, timer_set, mouse_set, add, time, goals);
					else if (current == TIMEL && rl != 0)
					{
						changeTime(rl, &time);
						if(time == 2)
							tlim = read_xpm(twomins, &tlimw, &tlimh);
						else if (time == 3)
							tlim = read_xpm(threemins, &tlimw, &tlimh);
						else if (time == 4)
							tlim = read_xpm(fourmins, &tlimw, &tlimh);
						else if (time == 5)
							tlim = read_xpm(fivemins, &tlimw, &tlimh);
						rl = 0;
					}
					else if (current == GOALL && rl != 0)
					{
						changeGoals(rl, &goals);
						if(goals == 3)
							glim = read_xpm(threegoals, &glimw, &glimh);
						else if (goals == 4)
							glim = read_xpm(fourgoals, &glimw, &glimh);
						else if (goals == 5)
							glim = read_xpm(fivegoals, &glimw, &glimh);
						else if (goals == 6)
							glim = read_xpm(no_limit, &glimw, &glimh);
						rl = 0;
					}
					else if(current == MODE && rl != 0)
					{
						changeMode(rl, &mode);
						if(mode == 0)
							md = read_xpm(keybrd, &mlimw, &mlimh);
						else if (mode == 1)
							md = read_xpm(kbdmouse, &mlimw, &mlimh);
						rl = 0;
					}
					else if (current == EXITG && ent == 1)
						exit = 1;
				}
				if (msg.NOTIFY_ARG & mouse_set)
				{
					mouseCursor(&byt, mousePacket, &mx, &my, &lb);
					if(byt == 2)
					{
						byt = 0;
						updateMouseSel(&current, mx, my, &mensely, lb, &rl);
					}
					if(current == PLAY && lb == 1)
						modeSelector(mode, kbc_set, timer_set, mouse_set, add, time, goals);
					else if (current == TIMEL && rl != 0)
					{
						changeTime(rl, &time);
						if(time == 2)
							tlim = read_xpm(twomins, &tlimw, &tlimh);
						else if (time == 3)
							tlim = read_xpm(threemins, &tlimw, &tlimh);
						else if (time == 4)
							tlim = read_xpm(fourmins, &tlimw, &tlimh);
						else if (time == 5)
							tlim = read_xpm(fivemins, &tlimw, &tlimh);
						rl = 0;
					}
					else if (current == GOALL && rl != 0)
					{
						changeGoals(rl, &goals);
						if(goals == 3)
							glim = read_xpm(threegoals, &glimw, &glimh);
						else if (goals == 4)
							glim = read_xpm(fourgoals, &glimw, &glimh);
						else if (goals == 5)
							glim = read_xpm(fivegoals, &glimw, &glimh);
						else if (goals == 6)
							glim = read_xpm(no_limit, &glimw, &glimh);
						rl = 0;
					}
					else if(current == MODE && rl != 0)
					{
						changeMode(rl, &mode);
						if(mode == 0)
							md = read_xpm(keybrd, &mlimw, &mlimh);
						else if (mode == 1)
							md = read_xpm(kbdmouse, &mlimw, &mlimh);
						rl = 0;
					}
					else if (current == EXITG && lb == 1)
						exit = 1;
				}
				if ((msg.NOTIFY_ARG & rtc_set))
				{
					getDate(&d);
				}
				if (msg.NOTIFY_ARG & timer_set)
				{
					memset(frame, 10, fsize); // fills the background with green
					//drawXPM(frame, 300, 100, mtxt, bkgw, bkgh); // menu text
					drawXPM(frame, GLX, GLY, glim, glimw, glimh); // goals limit text
					drawXPM(frame, TLX, TLY, tlim, tlimw, tlimh); // time limit text
					drawXPM(frame, MLX, MLY, md, mlimw, mlimh); // game mode text
					drawXPM(frame, MENSELX, mensely, mensel, msw, msh); // box outside the selected option
					displayTime(&d, frame); // displays the real time
					if(current == TIMEL) // arrows arround the selected box
					{
						if (time > 2)
							drawXPM(frame, LAX, mensely + 15, lr, rw, rh);
						if (time < 5)
							drawXPM(frame, RAX, mensely + 15, rr, rw, rh);
					}

					if(current == GOALL)
					{
						if (goals > 3)
							drawXPM(frame, LAX, mensely + 15, lr, rw, rh);
						if (goals < 6)
							drawXPM(frame, RAX, mensely + 15, rr, rw, rh);
					}

					if(current == MODE)
					{
						if (mode == 1)
							drawXPM(frame, LAX, mensely + 15, lr, rw, rh);
						if (mode == 0)
							drawXPM(frame, RAX, mensely + 15, rr, rw, rh);
					}
					drawXPM(frame, mx, my, mMouse, mw, mh); // mouse cursor
					n++;
					if(n == 60)
					{
						n = 0;
						sec++;
					}
					memcpy(add, frame, fsize);
				}
				break;
			}
		}
	}

	free(frame);
	free(mensel);
	//free(mtxt);
	free(tlim);
	free(glim);
	free(md);
	free(lr);
	free(rr);
	free(mMouse);
	disable();

	if(vg_exit() == 1)
	{
		printf("failure exiting video mode \n");
		return -1;
	}

	if(timer_unsubscribe() == -1)
		return -1;

	if(keyboard_unsubscribe() == -1)
		return -1;

	if (mouse_write_command(WRITEB, DISTRM) == -1)
		return -1;

	if(mouse_unsubscribe() == -1)
		return -1;

	if(rtc_unsubscribe() == -1)
		return -1;


}

void updateOption(MenuOptions* current, int y)
{
	if (y == 306)
		*current = PLAY;
	else if (y == 386)
		*current = TIMEL;
	else if (y == 466)
		*current = GOALL;
	else if (y == 546)
		*current = MODE;
	else if (y == 626)
		*current = EXITG;
}

void updateMouseSel(MenuOptions* current, int mx, int my, int* mensely, int lb, int* rl)
{
	if(mx >= 280 && mx < 744)
	{
		if(my >= 306 && my <= 379)
		{
			*current = PLAY;
			*mensely = 306;
		}
		else if(my >= 386 && my <= 459)
		{
			*current = TIMEL;
			*mensely = 386;
		}
		else if(my >= 466 && my <= 539)
		{
			*current = GOALL;
			*mensely = 466;
		}
		else if (my >= 546 && my <= 619)
		{
			*current = MODE;
			*mensely = 546;
		}
		else if (my >= 626 && my <= 699)
		{
			*current = EXITG;
			*mensely = 626;
		}
	}
	else if (mx >= 230 && mx <= 269) // left arrow
	{
		if(my >= *mensely + 15 && my <= *mensely + 37 && lb == 1)
			*rl = -1;
	}
	else if (mx >= 735 && mx <= 773) // right arrow
	{
		if(my >= *mensely + 15 && my <= *mensely + 37 && lb == 1)
			*rl = 1;
	}
}

void changeTime(int rl, int* time)
{
	if(*time > 2 && rl == -1)
		*time -= 1;
	else if (*time < 5 && rl == 1)
		*time += 1;
}

void changeGoals(int rl, int* goals)
{
	if(*goals > 3 && rl == -1)
		*goals -= 1;
	else if (*goals < 6 && rl == 1)
		*goals += 1;
}

void changeMode(int rl, int* mode)
{
	if(*mode == 1 && rl == -1)
		*mode = 0;
	else if (*mode == 0 && rl == 1)
		*mode = 1;
}

void modeSelector(int mode, int kbc_set, int timer_set, int mouse_set, char* add, int time, int goals)
{
	disable();
	if(mode == 0) // keyboard only
		game(kbc_set, timer_set, add, time, goals);
	else if(mode == 1)
		gameM(kbc_set, timer_set, mouse_set, add, time, goals);
	enable();
}
