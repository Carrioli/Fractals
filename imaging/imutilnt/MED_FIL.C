


/***********************************************************************

;  Nome:    
     imut_median_filter
;
;  Funzione:
     filtraggio mediano su regione di interesse
;
;  Formato:
     short imut_median_filter(roi_i, roi_o, nr, nc)
;
;  Argomenti:    
     ROI *roi_i     regione da filtrare
     ROI *roi_o     regione filtrata
	 short nr       numero di righe di kernel
	 short nc       numero di colonne di kernel

;
;  Valore di ritorno: 
     SEAV_OK_ALL              nessun errore
	 SEAV_ERR_*               codice di errore

;
;  Descrizione:
     {\bf imut_median_filter}() effettua un filtraggio mediano bidimensionale
	 sulla regione di interesse di ingresso ponendo il risultato in una
	 seconda regione di interesse che puo' coincidere con quella di origine;
	 i valori di xf e yf della regione di arrivo sono ignorati;
	 si considera origine del kernel il punto centrale;
	 i punti che si trovano nella cornice esterna della roi di ingresso,
	 per i quali il filtro non puo' essere calcolato, sono trasferiti
	 inalterati in uscita;
;
;**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <imutilnt.h>

static unsigned char un_valore(
	unsigned char **riga, short primo, short nr, short nc
);
static unsigned char *vettore;

/**********************************************************************/

short
imut_median_filter(
	ROI *roi_i,
	ROI *roi_o,
	short nr,
	short nc
)
{
	unsigned char **riga=NULL, *risult=NULL;
	short fb_i, xi_i, yi_i, xf_i, yf_i;
	short fb_o, xi_o, yi_o;
	short ii, i, j, k, n_pixel_riga, n_pixel_colonna;
	short nr2 = nr >> 1, nc2 = nc >> 1;
	short xi, yi, xf, yf, x_size, y_size;
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

/* calcolo della zona utile del filtraggio relativamente all'origine
   della roi di ingresso */

	x_size = (xf_i - xi_i + 1) - (nc - 1);
	y_size = (yf_i - yi_i + 1) - (nr - 1);
	if(x_size <= 0 || y_size <= 0)
		return SEAV_ERR_PARAMETER;
	xi = nc2;
	yi = nr2;
	xf = xi + x_size - 1;
	yf = yi + y_size - 1;

/* allocazione di memoria */

	riga = (unsigned char **) util_alloca_matrice(
		(long) nr, (long) n_pixel_riga, 1L
	);
	if(n_pixel_riga > n_pixel_colonna)
		risult = (unsigned char *) malloc(n_pixel_riga);
	else
		risult = (unsigned char *) malloc(n_pixel_colonna);
	vettore = (unsigned char *) malloc(nr * nc);
	if(riga == NULL || risult == NULL || vettore == NULL) {
		free(vettore);
		free(risult);
		util_disalloca_matrice((char **) riga, (long) nr);
		return SEAV_ERR_MEMORY;
	}

/* inizializzazione di riga */

	sea2_seleziona_buffer(fb_i);
	k = 0; /* indice della prima riga utile (piu' vecchia) della matrice riga */
	for(ii=0; ii<nr-1; ii++) {
		sea2_leggi_riga(xi_i, (short) (yi_i + ii), n_pixel_riga, riga[k]);
		k++;
	}

/* filtraggio */

	for(i=yi; i<=yf; i++, ii++) {
		sea2_seleziona_buffer(fb_i);
		sea2_leggi_riga(xi_i, (short) (yi_i + ii), n_pixel_riga, riga[k]);
		if(++k == nr)
			k = 0;
		for(j=0; j<x_size; j++)
			risult[j] = un_valore(riga, j, nr, nc);
		sea2_seleziona_buffer(fb_o);
		sea2_scrivi_riga((short) (xi_o + xi), (short) (yi_o + i), x_size, risult);
	}

/* trasferimento della cornice esterna */

	for(i=0; i<yi; i++) {
		sea2_seleziona_buffer(fb_i);
		sea2_leggi_riga(xi_i, (short) (yi_i + i), n_pixel_riga, risult);
		sea2_seleziona_buffer(fb_o);
		sea2_scrivi_riga(xi_o, (short) (yi_o + i), n_pixel_riga, risult);
	}
	for(i=yf+1; i<n_pixel_colonna; i++) {
		sea2_seleziona_buffer(fb_i);
		sea2_leggi_riga(xi_i, (short) (yi_i + i), n_pixel_riga, risult);
		sea2_seleziona_buffer(fb_o);
		sea2_scrivi_riga(xi_o, (short) (yi_o + i), n_pixel_riga, risult);
	}
	for(j=0; j<xi; j++) {
		sea2_seleziona_buffer(fb_i);
		sea2_leggi_colonna((short) (xi_i + j), yi_i, n_pixel_colonna, risult);
		sea2_seleziona_buffer(fb_o);
		sea2_scrivi_colonna((short) (xi_o + j), yi_o, n_pixel_colonna, risult);
	}
	for(j=xf+1; j<n_pixel_riga; j++) {
		sea2_seleziona_buffer(fb_i);
		sea2_leggi_colonna((short) (xi_i + j), yi_i, n_pixel_colonna, risult);
		sea2_seleziona_buffer(fb_o);
		sea2_scrivi_colonna((short) (xi_o + j), yi_o, n_pixel_colonna, risult);
	}

/* uscita */

	free(vettore);
	free(risult);
	util_disalloca_matrice((char **) riga, (long) nr);

	return SEAV_OK_ALL;
}

/**********************************************************************/

static unsigned char
un_valore(
	unsigned char **riga,
	short primo,
	short nr,
	short nc
)
{
	short i, j, n;
	unsigned char *r0;

	for(i=0, n=0; i<nr; i++) {
		r0 = riga[i];
		for(j=0; j<nc; j++, n++)
			vettore[n] = r0[primo + j];
	}
	util_ordinamento(vettore, n, 0, 0);
	return vettore[n >> 1];
}

