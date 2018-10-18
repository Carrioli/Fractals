
// versione 7.30: N_FB_MAX_COL passa da 12 a 36; N_FB_MAX passa da 48 a 144

#ifndef __LIBORION__
#define __LIBORION__

#include <mil.h>
#include <sea2_nt.h>

/* definizioni generali */

#define NOME_SCHEDA SCHEDA_ORION
#define MEMORIA_LAVORO MEMORIA_RAM
#define CHAN_MIN 0
#define CHAN_MAX 7
#define CHAN_MIN_COL 0
#define CHAN_MAX_COL 1
#define FB_MIN 0
#define FB_MAX 0
#define FB_MIN_COL 0
#define FB_MAX_COL 0
#define FB_DMA_MIN 0 /* scheda e memoria coincidono */
#define FB_DMA_MAX 0
#define N_BOARD_MAX 4 /* massimo numero di schede */
#define N_FB_MAX 144 /* massimo numero di frame buffer allocabili */
#define N_FB_MAX_COL 36 /* massimo numero di frame buffer allocabili */

#define X_MIN 0
#define Y_MIN 0
#define X_MAX_AM              639
#define X_MAX_EU              767
#define LUNG_RIGA_AM          640
#define LUNG_RIGA_EU          768
#define Y_MAX_AM              479
#define Y_MAX_EU              575
#define LUNG_COLONNA_AM       480
#define LUNG_COLONNA_EU       576
#define LUNG_MAX_AM           640 /* max tra LUNG_RIGA_AM e LUNG_COLONNA_AM */
#define LUNG_MAX_EU           768 /* max tra LUNG_RIGA_EU e LUNG_COLONNA_EU */
#define LUNG_MAX              768 /* max tra LUNG_MAX_AM e LUNG_MAX_EU */
#define FATTORE_DIST_AM       1.0 /* non misurato */
#define FATTORE_DIST_EU       1.0 /* non misurato */

/* primitive */
#define PRIM_PUNTO      1
#define PRIM_FINESTRA   2
#define PRIM_LINEA      3
#define PRIM_CROCE      4
#define PRIM_QUADRATINO 5
#define PRIM_POLIGONO   6

/* lunghezza in byte delle primitive */
#define L_PRIM_PUNTO      11
#define L_PRIM_FINESTRA   15
#define L_PRIM_LINEA      15
#define L_PRIM_CROCE      11
#define L_PRIM_QUADRATINO 11
#define L_PRIM_POLIGONO(N_PUNTI)  (9 + ((N_PUNTI) << 2))

#endif

