#ifndef __MOUSE_H
#define __MOUSE_H

#include "read_xpm.h"
#include "const.h"
#include "video.h"
#include "keyboard.h"
#include "sprite.h"

/** \brief Funcao que vai subscrever as interrupcoes do rato
  * \return Retorna o hook do rato se subscrever as interrupcoes, -1 se nao subscrever as interrupcoes
  */
int mouse_subscribe();

/** \brief Funcao que vai desativar as interrupcoes do rato
  * \return Retorna 0  se desativar as interrupcoes, -1 se nao desativar as interrupcoes
  */
int mouse_unsubscribe();

/** \brief Funcao que envia um comando e o respectivo argumento para o rato
  *\param command comando a enviar para o rato
  *\param argument argumento a enviar para o rato
  *\return retorna 0 caso o envio do comando e do argumento sejam bem sucedidos, -1 caso nao seja bem sucedido
  */
int mouse_write_command (unsigned long command, unsigned long argument);

/** \brief Funcao que altera a movimentacao do jogador mediante o movimento do rato. Esta funcao e usada quando se joga com o teclado e rato
  * \param m Alteracao da movimentacao do jogador mediante o movimento do rato
  * \param packet Array com os packets do rato, que se vao ler
  * \param byt Informacao de qual byte o rato estamos a ler do rato
  */
void readMouse(mov* m, unsigned long* packet, int* byt);

/** \brief Funcao que altera a movimentacao do cursor do rato, no menu
  * \param byt Informacao de qual byte o rato estamos a ler do rato
  * \param mousePacket Array com os packets do rato, que se vao ler
  * \param mx Posicao atual do cursor em x
  * \param my Posicao atual do cursor em y
  * \param lb Informacao acerca de estar ou nao pressionado o botao esquerdo do rato
  */
void mouseCursor(unsigned int* byt, unsigned long* mousePacket, int* mx, int* my, int* lb);

unsigned long twoComplement(unsigned long byte);

#endif
