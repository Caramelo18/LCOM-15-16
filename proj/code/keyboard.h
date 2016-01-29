#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include "const.h"

 /**
  * \brief Estrutura que mediante o input do teclado vai definir o movimento do jogador */
typedef struct
{
	int pu;
	int pd;
	int pl;
	int pr;
	int ps;
} mov;

/** \brief Funcao que vai subscrever as interrupcoes do teclado
  * \return Retorna o hook do teclado se subscrever as interrupcoes, -1 se nao subscrever as interrupcoes
  */
int keyboard_subscribe();

/** \brief Funcao que vai desactivar as interrupcoes do teclado
  * \return Retorna 0  se desactivar as interrupcoes, -1 se nao desativar as interrupcoes
  */
int keyboard_unsubscribe();

/** \brief Funcao que mediante o input do teclado vai definir o m1 e m2 que sao as movimentacoes mediante as setas e as movimentacoes mediante o wasd e que sao destinadas aos jogadores 1 e 2, respetivamente. Esta funcao e usada para quando o jogo e jogado apenas com o teclado
  * \param m1 Representa a movimentacao destinada ao jogador 1 e e alterada pelo pressionar das setas
  * \param m2 Representa a movimentacao destinada ao jogador 2 e e alterada pelo pressionar das teclas wasd
  */
void readKeyboard(mov* m1, mov* m2);

/** \brief Funcao que mediante o input do teclado vai definir o m1 que representa a movimentacao do jogador 1 mediante as setas. Esta funcao e usada para quando o jogo e jogado com o teclado e o rato
  * \param m1 Representa a movimentacao destinada ao jogador 1 e e alterada pelo pressionar das setas
  */
void readKeyboardM(mov* m1);

/** \brief Funcao que vai alterar a caixa de selecao mediante a opcao escolhida no menu
  * \param y Posicao da opcao escolhida segundo o teclado
  * \param ent Informacao sobre o pressionar ou nao da tecla de enter
  * \param rl Posicao da opcao escolhida segundo o rato
  */
void scrollMenu(int* y, int* ent, int* rl);

#endif
