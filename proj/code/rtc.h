#ifndef __RTC_H
#define __RTC_H

#include "const.h"

/**
  * \brief Estrutura que guarda a informacao da data e hora atual
  */
typedef struct
{
	unsigned long year, month, day, hours, minutes, seconds, dotw;
} Date;

/** \brief Funcao que vai subscrever as interrupcoes do rtc
  * \return Retorna hook do rtc se subscrever as interrupcoes, -1 se nao subscrever as interrupcoes
  */
int rtc_subscribe();

/** \brief Funcao que vai desactivar as interrupcoes do rtc
  * \return Retorna 0  se desativar as interrupcoes, -1 se nao desativar as interrupcoes
  */
int rtc_unsubscribe();

/** \brief Funcao que vai activar as interrupcoes por actualizacao do rtc
  */
void enable();

/** \brief Funcao que vai desactivar as interrupcoes por actualizacao do rtc
  */
void disable();

/** \brief Funcao que recolher a informacao do RTC e guarda-la na struct que e passada como argumento
  * \param d1 Struct onde os dados recolhidos pelo RTC sao armazenados
  */
void getDate(Date* d1);

#endif
