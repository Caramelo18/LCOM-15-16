#ifndef __TIMER_H
#define __TIMER_H


#include "const.h"
#include "video.h"
#include "pixmap.h"
#include "read_xpm.h"

/** \brief Funcao que vai subscrever as interrupcoes do timer
  * \return Retorna hook do timer  se activar as interrupcoes, -1 se nao desativar as interrupcoes
  */
int timer_subscribe();

/** \brief Funcao que vai desactivar as interrupcoes do timer
  * \return Retorna 0  se desativar as interrupcoes, -1 se nao desativar as interrupcoes
  */
int timer_unsubscribe();

/** \brief Funcao que imprime no ecra o tempo pela primeira vez
  * \param min numero de minutos a mostrar no ecra
  */
void startTime(char* add, int min);

/** \brief Funcao que actualiza no ecra o tempo de jogo restante
  * \param min numero de minutos a mostrar no ecra
  * \param sec numero de segundos a mostrar no ecra
  */
void updateTime(char* add, int* min, int* sec);

#endif
