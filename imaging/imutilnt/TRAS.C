


/***********************************************************************

;  Nome:    
     imut_tras
;
;  Funzione:
     trasforma una regione di interesse secondo una tabella
;
;  Formato:
     short imut_tras(roi_i, roi_o, lut)
;
;  Argomenti:    
     ROI *roi_i     regione da trasformare
     ROI *roi_o     regione trasformata
     unsigned char *lut  look-up-table di trasformazione dei pixel

;
;  Valore di ritorno: 
     SEAV_OK_ALL              nessun errore
	 SEAV_ERR_*               codice di errore

;
;  Descrizione:
     {\bf imut_tras}() trasforma puntualmente una regione di interesse
	 ponendo il risultato in una seconda regione di interesse che puo'
	 coincidere con quella di origine; i valori di xf e yf della regione
	 di arrivo sono ignorati; la trasformazione e'
	 realizzata attraverso il mapping per il vettore "lut";
;
;**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <imutilnt.h>

static void trasforma(unsigned char *vet, short n_pixel, unsigned char *lut);

/**********************************************************************/

short
imut_tras(
	ROI *roi_i,
	ROI *roi_o,
	unsigned char *lut
)
{
	unsigned char *riga;
	short fb_i, xi_i, yi_i, xf_i, yf_i;
	short fb_o, xi_o, yi_o;
	short i, n_pixel_riga, n_pixel_colonna;
	PARAM_SCHEDA param;

/* controllo dei parametri */

	sea2_init_config(&param);
	if(imut_check_roi(roi_i, &param) != SEAV_OK_ALL)
		return SEAV_ERR_PARAMETER;
	if(imut_check_roi(roi_o, &param) != SEAV_OK_ALL)
		return SEAV_ERR_PARAMETER;
	fb_i = roi_i->fb;
	xi_i = roi_i->xi; yi_i = roi_i->yi;
	xf_i = roi_i->xf; yf_i = roi_i->yf;
	fb_o = roi_o->fb;
	xi_o = roi_o->xi; yi_o = roi_o->yi;
	n_pixel_riga = xf_i - xi_i + 1;
	n_pixel_colonna = yf_i - yi_i + 1;

/* allocazione di memoria */

	riga = (unsigned char *) malloc(n_pixel_riga);
	if(riga == NULL)
		return SEAV_ERR_MEMORY;

/* trasformazione una riga per volta */

	if(yi_i > yi_o) { /* la copia parte dall'alto */
		for(i=0; i<n_pixel_colonna; i++) {
			sea2_seleziona_buffer(fb_i);
			sea2_leggi_riga(xi_i, (short) (yi_i + i), n_pixel_riga, riga);
			trasforma(riga, n_pixel_riga, lut);
			sea2_seleziona_buffer(fb_o);
			sea2_scrivi_riga(xi_o, (short) (yi_o + i), n_pixel_riga, riga);
		}
	} else { /* la copia parte dal basso */
		for(i=n_pixel_colonna-1; i>=0; i--) {
			sea2_seleziona_buffer(fb_i);
			sea2_leggi_riga(xi_i, (short) (yi_i + i), n_pixel_riga, riga);
			trasforma(riga, n_pixel_riga, lut);
			sea2_seleziona_buffer(fb_o);
			sea2_scrivi_riga(xi_o, (short) (yi_o + i), n_pixel_riga, riga);
		}
	}
	free(riga);

	return SEAV_OK_ALL;
}

/*************************************************************************/

static void
trasforma(
	unsigned char *vet,
	short n_pixel,
	unsigned char *lut
)
{
	short j;
	unsigned char *aus1 = vet;

	for(j=n_pixel-7; j>0; j-=8) {
		*aus1 = lut[*aus1]; aus1 ++;
		*aus1 = lut[*aus1]; aus1 ++;
		*aus1 = lut[*aus1]; aus1 ++;
		*aus1 = lut[*aus1]; aus1 ++;
		*aus1 = lut[*aus1]; aus1 ++;
		*aus1 = lut[*aus1]; aus1 ++;
		*aus1 = lut[*aus1]; aus1 ++;
		*aus1 = lut[*aus1]; aus1 ++;
	}
	for(; j>-7; j--) {
		*aus1 = lut[*aus1]; aus1 ++;
	}
}


