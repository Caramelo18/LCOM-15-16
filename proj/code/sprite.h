#ifndef __SPRITE_H
#define __SPRITE_H

#include "read_xpm.h"
#include "const.h"
#include "video.h"
#include "keyboard.h"

/**
  * \brief Struct que guarda a informacao do "objecto" sprite
  */
typedef struct {
int x, y; // current position
int width, height; // dimensions
float xspeed, yspeed; // current speed
float subpx, subpy;
int cenx, ceny; //center of the sprite
int radius;
char *map; // the pixmap
} Sprite;

/** \brief Funcao que inicializar o "objecto", lendo o xpm correspondente a sprite
  *\param pic nome do xpm a associar a sprite
  *\bas endereco de memoria onde ira desenhar a sprite inicialmente
  *\px posicao inicial no eixo x da sprite
  *\py posicao inicial no eixo y da sprite
  *\return Retorna o apontador para a sprite criada, NULL caso ocorra um erro na criacao da sprite
  */
Sprite *create_sprite(char *pic[], char *bas, int px, int py);

/** \brief Funcao responsavel por destruir a sprite, libertando a memoria alocada para a sprite
   *\param sp apontador para a sprite a destruir
   */
void destroy_sprite(Sprite *sp);

/** \brief Funcao responsavel por actualizar as velocidades das sprites
   *\param sp1 apontador para a sprite a actualizar com a struct m1
   *\param sp2 apontador para a sprite a actualizar com a struct m2
   *\param m1 struct com informacao sobre o deslocamento da sprite sp1
   *\param m2 struct com informacao sobre o deslocamento da sprite sp2
   */
void updateSpeed(Sprite *sp1, Sprite *sp2, mov m1, mov m2);

/** \brief Funcao responsavel por actualizar o frame a mostrar no ecra
   *\param sp1 apontador para a sprite a actualizar com a struct m1
   *\param sp2 apontador para a sprite a actualizar com a struct m2
   *\param m1 struct com informacao sobre o deslocamento da sprite sp1
   *\param m2 struct com informacao sobre o deslocamento da sprite sp2
   *\param pl1score golos marcados pelo jogador 1
   *\param pl2score golos marcados pelo jogador 2
   *\param secs numero de interrupcoes do timer (parametro utilizado para a animacao da bola)
   */
void updateFrame(Sprite *sp1, Sprite *sp2, Sprite *bal, char *base, mov m1, mov m2, int *pl1score, int *pl2score, int secs);

/** \brief Funcao que verifica se existe colisao entre duas sprites
   *\param sp1 apontador para a sprite 1
   *\param sp2 apontador para a sprite 2
   *\return retorna 0 se nao houver colisao, 1 se houver colisao
   */
int checkColision(Sprite *sp1, Sprite *sp2);

/** \brief Funcao que altera a posicao das duas sprites consoante as suas velocidades
   *\param sp1 apontador para a sprite 1
   *\param sp2 apontador para a sprite 2
   */
void updatePosition(Sprite *sp1, Sprite *sp2, Sprite *bal);

/** \brief Funcao que actualiza a posicao e a velocidade da bola
   *\param sp1 apontador para a sprite 1
   *\param sp2 apontador para a sprite 2
   *\param bal apontador para a sprite da bola
   */
void updateBall(Sprite *bal, Sprite *sp1, Sprite *sp2);

/** \brief Funcao que efectua o tratamento das colisoes entre as sprites dos dois jogadores
   *\param sp1 apontador para a sprite 1
   *\param sp2 apontador para a sprite 2
   */
void handleColision(Sprite *sp1, Sprite *sp2);

/** \brief Funcao que movimenta a bola
   *\param sp1 apontador para a sprite que esta em contacto com a bola
   *\param bal apontador para a sprite da bola
   *\param shoot indica se o jogador que esta em contacto com a bola esta a rematar ou nao
   */
void moveBall(Sprite *sp1, Sprite *bal, int shoot);

/** \brief Funcao que mantem os jogadores a bola dentro dos limites de jogo e actualiza o numero de golos marcados pelos jogadores
   *\param sp1 apontador para a sprite 1
   *\param sp2 apontador para a sprite 2
   *\param bal apontador para a sprite da bola
   *\param pl1score apontador para o numero de golos do jogador 1
   *\param pl2score apontador para o numero de golos do jogadore 2
   *\param base apontador para a posicao onde se ira escrever no ecra o novo score
   */
void touchWalls(Sprite *sp1, Sprite *sp2, Sprite * bal, int *pl1score, int *pl2score, char* base);

/** \brief Funcao que reinicia as posicoes das sprites dos jogadores e da bola, pondo as velocidades das sprites a 0
   *\param sp1 apontador para a sprite 1
   *\param sp2 apontador para a sprite 2
   *\param bal apontador para a sprite da bola
   */
void kickOff(Sprite *sp1, Sprite *sp2, Sprite * bal);


/** \brief Funcao que anima a bola
   *\param sp1 apontador para a sprite da bola
   *\param secs numero de interrupcoes do timer
   */
void animateBall(Sprite *bal, int secs);

#endif
