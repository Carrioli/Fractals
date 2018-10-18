

/***********************************************************************

;  Nome:    
     imut_copy
;
;  Funzione:
     copia una regione di interesse
;
;  Formato:
     short imut_copy(roi_i, roi_o)
;
;  Argomenti:    
     ROI *roi_i     regione di ingresso
     ROI *roi_o     regione di uscita

;
;  Valore di ritorno: 
     SEAV_OK_ALL              nessun errore
	 SEAV_ERR_*               codice di errore

;
;  Descrizione:
     {\bf imut_copy}() opia la regione di interesse
	 di ingresso in quella di uscita; se le due regioni sono
	 parzialmente sovrapponentisi la regione di ingresso viene
	 sovrascritta da quella di uscita;
	 i valori di xf e yf della regione di arrivo sono ignorati;
;
;**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <imutilnt.h>

short
imut_copy(
	ROI *roi_i,
	ROI *roi_o
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

/* copia */

	if(yi_i > yi_o) { /* la copia parte dall'alto */
		for(i=0; i<n_pixel_colonna; i++) {
			sea2_seleziona_buffer(fb_i);
			sea2_leggi_riga(xi_i, (short) (yi_i + i), n_pixel_riga, riga);
			sea2_seleziona_buffer(fb_o);
			sea2_scrivi_riga(xi_o, (short) (yi_o + i), n_pixel_riga, riga);
		}
	} else { /* la copia parte dal basso */
		for(i=n_pixel_colonna-1; i>=0; i--) {
			sea2_seleziona_buffer(fb_i);
			sea2_leggi_riga(xi_i, (short) (yi_i + i), n_pixel_riga, riga);
			sea2_seleziona_buffer(fb_o);
			sea2_scrivi_riga(xi_o, (short) (yi_o + i), n_pixel_riga, riga);
		}
	}

/* uscita */

	free(riga);
	return SEAV_OK_ALL;
}

