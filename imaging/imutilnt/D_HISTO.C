

/***********************************************************************

;  Nome:    
     imut_draw_histo
;
;  Funzione:
     disegna l'istogramma su regione di interesse
;
;  Formato:
     short imut_draw_histo(roi, histo, level)
;
;  Argomenti:    
     ROI *roi     regione di interesse in cui disegnare
	 long *histo  vettore dell'istogramma
	 unsigned char level  livello con cui disegnare

;
;  Valore di ritorno: 
     SEAV_OK_ALL              nessun errore
	 SEAV_ERR_*               codice di errore

;
;  Descrizione:
     {\bf imut_draw_histo}() disegna l'istogramma sulla regione di interesse;
	 l'istogramma viene scalato in modo da avere il massimo coincidente con
	 l'altezza della regione di interesse;
;
;**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <imutilnt.h>

short
imut_draw_histo(
	ROI *roi,
	long *histo,
	unsigned char level
)
{
	short fb, xi, yi, xf, yf, n_pixel_riga, n_pixel_colonna;
	short i, n_pixel_max, xih, yih, xfh, yfh;
	unsigned char *vett=NULL;
	short *x_vet=NULL, *y_vet=NULL;
	long massimo;
	double fattx, fatty;
	PARAM_SCHEDA param;

/* controllo dei parametri */

	sea2_init_config(&param);
	if(imut_check_roi(roi, &param) != SEAV_OK_ALL)
		return SEAV_ERR_PARAMETER;
	fb = roi->fb;
	xi = roi->xi; yi = roi->yi;
	xf = roi->xf; yf = roi->yf;
	n_pixel_riga = xf - xi + 1;
	n_pixel_colonna = yf - yi + 1;

/* allocazione di memoria */

	if(n_pixel_riga > n_pixel_colonna)
		n_pixel_max = n_pixel_riga;
	else
		n_pixel_max = n_pixel_colonna;
	vett = (unsigned char *) malloc(n_pixel_max);
	x_vet = (short *) malloc(n_pixel_max * sizeof(short));
	y_vet = (short *) malloc(n_pixel_max * sizeof(short));
	if(vett == NULL || x_vet == NULL || y_vet == NULL) {
		free(y_vet);
		free(x_vet);
		free(vett);
		return SEAV_ERR_MEMORY;
	}

/* calcolo del massimo dell'istogramma */

	massimo = util_calcola_massimo(histo, 256, 3);

/* disegno della cornice */

	imut_disegna_rettangolo(roi, level, 0);

/* disegno dell'istogramma */

	memset(vett, level, n_pixel_max);
	fattx = (double) n_pixel_riga / 256.0;
	fatty = (double) n_pixel_colonna / (double) (massimo+1L);
	for(i=1; i<256; i++) {
		xih = xi + (short) ((double) (i-1) * fattx);
		yih = yf - (short) ((double) histo[i-1] * fatty);
		xfh = xi + (short) ((double) i * fattx);
		yfh = yf - (short) ((double) histo[i] * fatty);
		imut_scrivi_segmento(xih, yih, xfh, yfh, fb, vett, x_vet, y_vet);
	}

/* uscita */

	free(y_vet);
	free(x_vet);
	free(vett);
	return SEAV_OK_ALL;
}

