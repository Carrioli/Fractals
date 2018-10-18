#ifndef __IM_UTIL_H__
#define __IM_UTIL_H__

//
//	versione attuale:    7.64  1 marzo 2002
//	versione precedente: 7.62 28 febbraio 2002
//

#include "util_nt.h"
#include "sea2_nt.h"
#include <tchar.h>

/************************************************************************/

/* definizioni varie */

#define HDR_MAGIC 025252    /* per le immagini con header */
#define DIM_MAX_BUF 60000L  /* per pic e rpi */
#define PASSO_TRANS       3 /* passo per il calcolo delle transizioni */
#define N_PIXEL_TRANS    20 /* pixel da leggere per il calcolo del diametro */
#define N_PIXEL_TRANS2   10 /* meta' di N_PIXEL_TRANS */
#define N_PIXEL_MEDIA     4 /* per calcolare la soglia per il sub-pixel */
#define N_BIT_SHIFT       2 /* logaritmo di N_PIXEL_MEDIA */
#define WEIGHT_P     1000.0 /* peso dei punti di foreground per il calcolo
                               della matrice a risoluzione ridotta
							   (cfr. comp_mis.c) */
#define OP_EROSION        0 /* per imut_morf */
#define OP_DILATION       1 /* per imut_morf */

#define CONNECTIVITY_4    0 /* per imut_conn_filter etc. */
#define CONNECTIVITY_8    1 /* per imut_conn_filter etc. */

/* costanti per le routine DIAC */
#define DIAC_PIX32      4
#define DIAC_PIX24      3
#define DIAC_PIX16      2
#define DIAC_PIX8       1
#define DIACHEADERSIZE 10
#define NSOGLIE   15   /* n. di livelli ammessi, pertanto la dimensione max
                          del blocco e' 2**15 */
#define RANGE_VALORI 512 /* numero di valori possibili dopo la normalizzazione
                            e dimen. del vettore della statistica per Huffman */
#define DIAC_BUF_SIZE 30000L   /* dimensione dei buffer per l'I/O su disco (max 64k)*/
#define N_CODICI  22   /* n. di codici a disposizione per i valori */
#define el(N)   (1 << (N))
#define LUNG_CODICI (RANGE_VALORI * N_CODICI)
#define DIM_DIM_L 1000 /* dimesione del vettore dim_letture */

/* costanti per la codifica-decodifica del colore DIAC */
#define SC 25       /* soglie per la scelta della lunghezza del codice */
#define SB 50       /* di colore: corto, lunghino, lungo, lunghissimo */
#define SA 80

#define N_PA_CORTO 2    /* N_PA??? + 1 = numero di passi di quantizzazione */
#define N_PA_LUNGHINO 6 /* delle due componenti di colore */
#define N_PA_LUNGO 14
#define N_PA_LUNGHISSIMO 254

#define N_PA_CORTO_2 1    /* N_PA??? / 2: per la decodifica */
#define N_PA_LUNGHINO_2 3 /* delle due componenti di colore */
#define N_PA_LUNGO_2 7
#define N_PA_LUNGHISSIMO_2 127

#define N_VALORI_LUNGHISSIMI 4 /* che comunque usano il lunghissimo */
#define N_VALORI_LUNGHI     16 /* che comunque usano il lungo */
#define N_VALORI_LUNGHINI   64 /* che comunque usano il lunghino */

/* costanti numeriche */
#ifndef PI
	#define PI    3.1415926535898
#endif
#ifndef PI_PI
	#define PI_PI 6.2831853071796
#endif
#ifndef P_I
	#define P_I   1.5707963267949
#endif

/************************************************************************/

/* strutture */

/* coordinate dei pixel */

#ifndef __PUNTO__
typedef struct {     /* cartesiane intere short */
	short x;
	short y;
} PUNTO;
#define __PUNTO__
#endif

#ifndef __PUNTOL__
typedef struct {     /* cartesiane intere long */
	long x;
	long y;
} PUNTOL;
#define __PUNTOL__
#endif

#ifndef __PUNTOD__
typedef struct {     /* cartesiane double */
	double x;
	double y;
} PUNTOD;
#define __PUNTOD__
#endif

#ifndef __POLARE__
typedef struct {     /* polari double */
	double r;
	double a;
} POLARE;
#define __POLARE__
#endif

/* retta */

#ifndef __RETTA__
typedef struct {
	double a;
	double b;
	double c;
} RETTA;
#define __RETTA__
#endif

/* area di interesse */

#ifndef __ROI__
typedef struct {
	short fb;
	short xi;
	short yi;
	short xf;
	short yf;
} ROI;
#define __ROI__
#endif

/* componente connessa */

#ifndef __CONN_COMP__
typedef struct {
	short fb;
	short xi;
	short yi;
	short xf;
	short yf;
	long area;
	double x_bari;
	double y_bari;
	short livello; /* con cui e' disegnata sul fb */
	short max_dist_bordo; /* massimo valore ottenuto con la trasformata distanza */
} CONN_COMP;
#define __CONN_COMP__
#endif

/* momenti */

#ifndef __MOMENTS__
typedef struct {
	double m00;
	double m10;
	double m01;
	double m20;
	double m02;
	double m11;
	double m30;
	double m03;
	double m21;
	double m12;
} MOMENTS;
#define __MOMENTS__
#endif

/* invarianti */

#ifndef __INVARIANTS__
typedef struct {
	double phi0;
	double phi1;
	double phi2;
	double phi3;
	double phi4;
	double phi5;
	double phi6;
	double phi7;
} INVARIANTS;
#define __INVARIANTS__
#endif

/* header SEA */

typedef struct {
	long hdr;             /* magic number to indicate pvs file */
	long fmt;             /* format descriptor of this file */
	long xsize;           /* number pels/line */
	long ysize;           /* number lines */
	long zsize;           /* number bits/pel */
	long nchan;           /* number multi-spectral channels in file */
	long uchan;           /* number multi-spectral channels in use */
	long crdate[2];       /* creation date */
	long uid;             /* owner/creater's UID */
	long modified;        /* flag--header has been changed */
	char title[40];       /* 40 character title for identif. */
	char descrip[128];    /* 128 chars (2 lines) of picture descr. */
	long extdesc;         /* flag for extended description */
	long _filler_[32];    /* extra for header expansion */
	long extra[100];      /* extra for "format specific" data */
} PVS_HDR;

/* header DIAC */
 
typedef struct compressione {
	char              DIACheader[DIACHEADERSIZE];
	short             block_len;    /* block size */
	short             hor_blocks;   /* number of horizontal blocks */
	short             ver_blocks;   /* number of vertical blocks */
	short             xleft;        /* left corner of compressed image */
	short             ydown;
	unsigned short    width;        /* image size: x,y,z */
	unsigned short    height;
	short             pixdepth;
	short             cmpfactor;    /* quality of compression */
	unsigned long     cpdatasize;   /* size of compressed data */
} CMPIMAGE;

/* header aggiuntivo per il video DIAC */

typedef struct video {
	unsigned short    n_frames;     /* numero di frames nella sequenza */
	unsigned short    periodo;      /* passo di sottocampionamento temporale */
	unsigned short    sub_samp;     /* passo di sottocampionamento spaziale */
	unsigned short    xini;         /* origine nell'immagine sorgente */
	unsigned short    yini;
	short             l_burst;      /* numero di frames che costituiscono un
	                                   pacchetto codificato per differenza
									   di frames adiacenti; il primo e'
									   codificato autonomamente */
} VIDEOIMAGE;

/************************************************************************/

/* prototipi */

short imut_profile(
	ROI *roi, short dir, long max, long *profilo, short *n_punti_utili
);
short imut_leggi_segmento(
	short xi, short yi, short xf, short yf, short fb,
	unsigned char *vett, short *x_vet, short *y_vet
);
short imut_scrivi_segmento(
	short xi, short yi, short xf, short yf, short fb,
	unsigned char *vett, short *x_vet, short *y_vet
);
short imut_profile_gen(
	unsigned char **finestra, short xl, short yl, double angolo, short step_col,
	short step_row, long *profilo, short *n_punti_utili
);
short imut_disegna_segmenti(
	ROI *roi, double angolo, double periodo, unsigned char colore
);
short imut_tras(ROI *roi_i, ROI *roi_o, unsigned char *lut);
short imut_stretch_lut(
	ROI *roi_i, ROI *roi_o, short thl, short thh, short ll, short lh
);
void imut_flip_sea_header(PVS_HDR *header_in, PVS_HDR *header_out);
short imut_disegna_rettangolo(ROI *roi, unsigned char colore, short flag_pieno);
short imut_retta_interpolante(
	void *asc, void *ord, short n_punti, short tipo, double cx, double cy,
	RETTA *retta
);
short imut_calibrazione(
	ROI *vet_roi, double *vet_valori_nominali, short n_roi, short direzione,
	short flag_interno_scuro, short soglia, short passo, short n_bit_sub_pixel,
	double *coeff, double *offset
);
short imut_misura_diametro(
	ROI *roi, short direzione, short flag_interno_scuro, short soglia,
	short passo, short n_bit_sub_pixel, short flag_max, double cx, double cy,
	double *diametro, RETTA *retta
);
short imut_misura_diametro1(
	ROI *roi, short direzione, short flag_interno_scuro, short soglia,
	short passo, short n_bit_sub_pixel, short flag_max, double cx, double cy,
	double *diametro, RETTA *retta, double *coord_centro
);
short imut_misura_diametro_estremo(
	ROI *roi, short direzione, short flag_interno_scuro, short soglia,
	short passo, short n_bit_sub_pixel, short flag_max, double cx, double cy,             /* risoluzione verticale in unita' / tick */
	double *diametro, RETTA *retta, double *coord_centro, double *coord_diam
);
short imut_cerca_prima_transizione(
	unsigned char *vettore, short primo, short ultimo, short th, short segno
);
short imut_cerca_ultima_transizione(
	unsigned char *vettore, short primo, short ultimo, short th, short segno
);
short imut_calcola_sub_pixel(
	unsigned char *vet_pixel, short n_pixel, short indice, short *correzione,
	short n_bit_sub_pixel
);
short imut_individua_primo_diametro(
	ROI *roi, short direzione, short flag_interno_scuro, short soglia,
	short passo, short n_bit_sub_pixel, short *diametro, long *centro,
	short *ind_i, short *ind_f, short flag_inizio
);
short imut_individua_diametro_seguente(
	short coord, ROI *roi, short direzione, short flag_interno_scuro,
	short soglia, short n_bit_sub_pixel, short *diametro, long *centro,
	short *ind_i, short *ind_f
);
short imut_estrai_bordi(
	ROI *roi, short direzione, short flag_interno_scuro, short soglia,
	short passo, short n_bit_sub_pixel, long *bordi1, short *n_bordi1,
	long *bordi2, short *n_bordi2, short flag_inizio
);
short imut_individua_primi_bordi(
	ROI *roi, short direzione, short flag_interno_scuro, short soglia,
	short passo, short n_bit_sub_pixel, long *bordi1, long *bordi2,
	short *ind_i, short *ind_f, short flag_inizio
);
short imut_individua_bordi_seguenti(
	short coord, ROI *roi, short direzione, short flag_interno_scuro,
	short soglia, short n_bit_sub_pixel, long *bordi1, long *bordi2,
	short *ind_i, short *ind_f
);
short imut_conv(
	ROI *roi_i, ROI *roi_o, short *kernel, short nr, short nc, short norm,
	short flag_shift, short flag_abs
);
short imut_median_filter(ROI *roi_i, ROI *roi_o, short nr, short nc);
short imut_sobel(ROI *roi_i, ROI *roi_o, short tipo, short n_bit_shift);
short imut_sobel_rel(
	ROI *roi_i, ROI *roi_o, short tipo, short n_bit_shift, short soglia,
	short dim
);
short imut_morf(ROI *roi_i, ROI *roi_o, short nr, short nc, short ope);
short imut_histo(ROI *roi, long *histo);
short imut_draw_histo(ROI *roi, long *histo, unsigned char level);
short imut_copy(ROI *roi_i, ROI *roi_o);
short imut_conn_filter(
	ROI *roi_i, ROI *roi_o, ROI *roi_s, CONN_COMP *vet, short max_comp,
	short x_size_min, short y_size_min, short x_size_max, short y_size_max,
	long area_min, long area_max, short dist_min, short flag_conn, short level
);
short imut_conn_measure(
	ROI *roi, short livello, MOMENTS *momenti, INVARIANTS *invarianti,
	long **matrice, short nr, short nc
);
short imut_punctual_operation(ROI *roi1, ROI *roi2, ROI *roi_o, short op);
short imut_roi_constant_operation(
	ROI *roi_i, ROI *roi_o, short op, unsigned char value
);
short imut_slim_conn(
	ROI *roi_i, ROI *roi_o, ROI *roi_s, short livello, short th,
	short n_iter_max
);
short imut_check_roi(ROI *roi, PARAM_SCHEDA *param);
short imut_intersection(ROI *roi1, ROI *roi2);
short imut_local_max_min(
	ROI *roi_i, ROI *roi_o, short nr, short nc, short flag_ext, short liv0,
	short liv1
);
short imut_max_dist_border(ROI *roi, short flag_conn);
short imut_local_value(ROI *roi, short tipo_valore, long *valore);
short imut_init_DIAC_memory(short fd, short flag_col, short len_block);
short imut_init_DIAC_header(ROI *roi, CMPIMAGE *header);
unsigned long imut_compr_buf_DIAC(
	short fb, char *buf_out, unsigned long max_buf_out_len
);
void imut_clean_DIAC_comp(void);
CMPIMAGE *imut_get_DIAC_header(short fb);
void imut_cmpDIACblock(
	short fb, unsigned char *ptcdata, short ii, short jj, short *dim_blo_com
);
short imut_rpi(ROI *roi, wchar_t *file_name);
short imut_rpi_char(ROI *roi, char *file_name);
short imut_rpi_col(ROI *roi, wchar_t *file_name);
short imut_rpi_DIAC(
	ROI *roi, wchar_t *file_name, short fd, short flag_col,
	double *comp_ratio, double *bit_per_pixel, unsigned long *n_byte_file
);
short imut_rpi_lossless(
	ROI *roi, wchar_t *file_name, short flag_col, double *comp_ratio,
	double *bit_per_pixel, unsigned long *n_byte_file
);
short imut_pic_bmp(wchar_t *file_name, short fb, short xi, short yi); // 7.64-6
short imut_pic_col_bmp(wchar_t *file_name, short fb, short xi, short yi); // 7.64-6
short imut_pic(wchar_t *file_name, short fb, short xi, short yi);
short imut_pic_char(char *file_name, short fb, short xi, short yi);
short imut_pic_col(wchar_t *file_name, short fb, short xi, short yi);
short imut_pic_DIAC(wchar_t *file_name, short fb, short offset_x, short offset_y);
short imut_pic_lossless(
	wchar_t *file_name, short fb, short offset_x, short offset_y
);

#endif /* __IM_UTIL_H__ */

