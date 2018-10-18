

/***********************************************************************

;  Nome:    
     imut_histo
;
;  Funzione:
     istogramma su regione di interesse
;
;  Formato:
     short imut_histo(roi, histo)
;
;  Argomenti:    
     ROI *roi     regione di interesse
	 long *histo  vettore dell'istogramma

;
;  Valore di ritorno: 
     OK_ALL        nessun errore
     SEAV_OK_ALL              nessun errore
	 SEAV_ERR_*               codice di errore

;
;  Descrizione:
     {\bf imut_histo}() calcola l'istogramma della regione di interesse;
	 il vettore histo deve essere lungo almeno 256 elementi;
;
;**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <imutilnt.h>


short
imut_histo(
	ROI *roi,
	long *histo
)
{
	unsigned char *riga;
	short fb, xi, yi, xf, yf, n_pixel_riga;
	short i, j;
	PARAM_SCHEDA param;

/* controllo dei parametri */

	sea2_init_config(&param);
	if(imut_check_roi(roi, &param) != SEAV_OK_ALL)
		return SEAV_ERR_PARAMETER;
	fb = roi->fb;
	xi = roi->xi; yi = roi->yi;
	xf = roi->xf; yf = roi->yf;
	n_pixel_riga = xf - xi + 1;

/* allocazione di memoria */

	riga = (unsigned char *) malloc(n_pixel_riga);
	if(riga == NULL)
		return SEAV_ERR_MEMORY;

/* calcolo dell'istogramma */

	memset(histo, 0, 256 * sizeof(long));
	sea2_seleziona_buffer(fb);
	for(i=yi; i<=yf; i++) {
		sea2_leggi_riga(xi, i, n_pixel_riga, riga);
		for(j=n_pixel_riga-1; j>=0; j--)
			histo[riga[j]] ++;
	}

/* uscita */

	free(riga);
	return SEAV_OK_ALL;
}

