#ifndef __VIDEO_H
#define __VIDEO_H


#include "const.h"
#include "pixmap.h"
#include "read_xpm.h"
#include "rtc.h"
#include "weekdays.h"

/* Private global variables */

static char *video_mem;		/* Process address to which VRAM is mapped */
static unsigned h_res;		/* Horizontal screen resolution in pixels */
static unsigned v_res;		/* Vertical screen resolution in pixels */
static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */


typedef struct {
	/*  Mandatory information for all VBE revisions */
	uint16_t ModeAttributes; 	/**< @brief mode attributes */
	uint8_t WinAAttributes; 		/**< @brief window A attributes */
	uint8_t WinBAttributes; 		/**< @brief window B attributes */
	uint16_t WinGranularity; 	/**< @brief window granularity */
	uint16_t WinSize;		/**< @brief window size */
	uint16_t WinASegment;		/**< @brief window A start segment */
	uint16_t WinBSegment;		/**< @brief window B start segment */
	phys_bytes WinFuncPtr;	/**< @brief real mode/far pointer to window function */
	uint16_t BytesPerScanLine; 	/**< @brief bytes per scan line */

	/* Mandatory information for VBE 1.2 and above */

	uint16_t XResolution;      	/**< @brief horizontal resolution in pixels/characters */
	uint16_t YResolution;      	/**< @brief vertical resolution in pixels/characters */
	uint8_t XCharSize; 		/**< @brief character cell width in pixels */
	uint8_t YCharSize; 		/**< @brief character cell height in pixels */
	uint8_t NumberOfPlanes; 		/**< @brief number of memory planes */
	uint8_t BitsPerPixel; 		/**< @brief bits per pixel */
	uint8_t NumberOfBanks;		/**< @brief number of banks */
	uint8_t MemoryModel;		/**< @brief memory model type */
	uint8_t BankSize;		/**< @brief bank size in KB */
	uint8_t NumberOfImagePages;	/**< @brief number of images */
	uint8_t Reserved1;		/**< @brief reserved for page function */

	/* Direct Color fields (required for direct/6 and YUV/7 memory models) */

	uint8_t RedMaskSize;		/* size of direct color red mask in bits */
	uint8_t RedFieldPosition;	/* bit position of lsb of red mask */
	uint8_t GreenMaskSize;		/* size of direct color green mask in bits */
	uint8_t GreenFieldPosition;	/* bit position of lsb of green mask */
	uint8_t BlueMaskSize; 		/* size of direct color blue mask in bits */
	uint8_t BlueFieldPosition;	/* bit position of lsb of blue mask */
	uint8_t RsvdMaskSize;		/* size of direct color reserved mask in bits */
	uint8_t RsvdFieldPosition;	/* bit position of lsb of reserved mask */
	uint8_t DirectColorModeInfo;	/* direct color mode attributes */

	/* Mandatory information for VBE 2.0 and above */
	phys_bytes PhysBasePtr;       /**< @brief physical address for flat memory frame buffer */
	uint8_t Reserved2[4]; 		/**< @brief Reserved - always set to 0 */
	uint8_t Reserved3[2]; 		/**< @brief Reserved - always set to 0 */

	/* Mandatory information for VBE 3.0 and above */
	uint16_t LinBytesPerScanLine;    /* bytes per scan line for linear modes */
	uint8_t BnkNumberOfImagePages; 	/* number of images for banked modes */
	uint8_t LinNumberOfImagePages; 	/* number of images for linear modes */
	uint8_t LinRedMaskSize; 	        /* size of direct color red mask (linear modes) */
	uint8_t LinRedFieldPosition; 	/* bit position of lsb of red mask (linear modes) */
	uint8_t LinGreenMaskSize; 	/* size of direct color green mask (linear modes) */
	uint8_t LinGreenFieldPosition; /* bit position of lsb of green mask (linear  modes) */
	uint8_t LinBlueMaskSize; 	/* size of direct color blue mask (linear modes) */
	uint8_t LinBlueFieldPosition; 	/* bit position of lsb of blue mask (linear modes ) */
	uint8_t LinRsvdMaskSize; 	/* size of direct color reserved mask (linear modes) */
	uint8_t LinRsvdFieldPosition;	 /* bit position of lsb of reserved mask (linear modes) */
	uint32_t MaxPixelClock; 	         /* maximum pixel clock (in Hz) for graphics mode */
	uint8_t Reserved4[190]; 		 /* remainder of ModeInfoBlock */
} __attribute__((packed)) vbe_mode_info_t;


typedef struct {
	phys_bytes phys;	/**< @brief physical address */
	void *virtual;	/**< @brief virtual address */
	unsigned long size;   /**< @brief size of memory region */
} mmap_t;

/**
 * \brief Funcao que inicia o modo de video definido
 * \param mode Modo de video escolhido
 */
void *vg_init(unsigned short mode);

/**
 * \brief Funcao que sai do modo de video
 * \return Retorna 0 se sair corretamente do modo de video, 1 no caso contrario
 */
int vg_exit();

/**
 * \brief Funcao que retorna a resolucao horizontal do ecra
 * \return Retorna a resolucao horizontal do ecra
 */
unsigned getHRes();

/**
 * \brief Funcao que retorna a resolucao vertical do ecra
 * \return Retorna a resolucao vertical do ecra
 */
unsigned getVRes();

/**
 * \brief Funcao que retorna a quantidade de bits por cada pixel
 * \return Retorna a quantidade de bits por cada pixel
 */
unsigned getBitsPerPixel();

/**
 * \brief Funcao que inicia e alloca a memoria de video
 * \param mode Modo de video
 * \param vmi_p Informacoes do modo de video a serem preenchidos
 * \return Retorna 0 se correr sem erros, -1 no caso contrario
 */
int vbe_get_mode_info(unsigned short mode, vbe_mode_info_t *vmi_p);

/**
 * \brief Funcao que desenha o xpm no ecra
 * \param add Array com as cores a definir na posicao de  memoria de video do xpm
 * \param x Posicao inicial no eixo dos x
 * \param y Posicao inicial no eixo dos y
 * \param a Endereco da primeira posicao de memoria de video
 * \param width Largura do xpm
 * \param heigth Altura do xpm
 */
void drawXPM(char* add, unsigned short x, unsigned short y, char* a, int width, int heigth);

/**
 * \brief Funcao que apaga o xpm da memoria de video
 * \param add Endereco da primeira posicao de memoria de video
 * \param x Posicao inicial no eixo dos x
 * \param y Posicao inicial no eixo dos y
 * \param width Largura do xpm
 * \param heigth Altura do xpm
 */
void eraseXPM(char* add, unsigned short x, unsigned short y, int width, int heigth);

/**
 * \brief Imprime os minutos de jogo no ecra
 * \param add Endereco da primeira posicao de memoria de video
 * \param min Minutos de jogo a imprimir
 */
void displayMin(char* add, int min);

/**
 * \brief Imprime os segundos de jogo no ecra
 * \param add Endereco da primeira posicao de memoria de video
 * \param sec Segundos de jogo a imprimir
 */
void displaySec(char* add, int sec);

/**
 * \brief Imprime o score do jogo no ecra
 * \param add Endereco da primeira posicao de memoria de video
 * \param pl1score Score do jogador 1
 * \param pl2score Score do jogador 2
 */
void displayScore(char* add, int *pl1score, int *pl2score);

/**
 * \brief Imprime a data e horas atuais no ecra
 * \param d Struct com as informacoes da data, horas e dia da semana para imprimir no ecra
 * \param add Endereco da primeira posicao de memoria de video
 */
void displayTime(Date* d, char* add);
#endif
