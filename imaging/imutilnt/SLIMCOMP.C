

/***********************************************************************

;  Nome:    
     imut_slim_conn
;
;  Funzione:
     erode selettivamente una componente connessa
;
;  Formato:
     short imut_slim_conn(roi_i, roi_o, roi_s, livello, th, n_iter_max)
;
;  Argomenti:    
     ROI *roi_i      regione di ingresso contenente la componente
     ROI *roi_o      regione dell'immagine originale a livelli di grigio
     ROI *roi_s      regione ausiliaria
	 short livello   livello che identifica la componente all'interno della
	                 regione, se livello=-1 sono considerati appartenenti alla
					 componente tutti i punti diversi da 0
	 short th        soglia per l'eliminazione dei punti
	 short n_iter_max  massimo numero di iterazioni

;
;  Valore di ritorno: 
     SEAV_OK_ALL              nessun errore
	 SEAV_ERR_*               codice di errore

;
;  Descrizione:
     {\bf imut_slim_conn}() erode selettivamente i punti di bordo
	 di una componente connessa; vengono effettuate un massimo di n_iter_max
	 iterazioni, ciascuna delle quali si compone di una copia da roi_i
	 a roi_s e di una erosione selettiva da roi_a a roi_i;
	 l'erosione elimina tutti e soli i punti di contorno che hanno
	 almeno due vicini di background in connettivita' 8 e allo stesso tempo
	 presentano un valore che dista dalla media dei vicini di background
	 meno di th;
	 sono considerati punti di background soltanto i punti con livello 0;
	 le tre ROI devono avere le stesse dimensioni e non devono avere punti
	 in comune;
	 i punti che si trovano sulla cornice esterna della roi_i sono
	 comunque eliminati; se essi sono significativi le roi devono essere
	 ingrandite prima di lanciare le routine;

;
;**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <imutilnt.h>

static unsigned char un_valore(
	unsigned char **riga_s, unsigned char **riga_o, short primo, short k,
	short livello, short th, short *fine
);

#undef abs

/**********************************************************************/

short
imut_slim_conn(
	ROI *roi_i,
	ROI *roi_o,
	ROI *roi_s,
	short livello,
	short th,
	short n_iter_max
)
{
	unsigned char **riga_s=NULL, **riga_o=NULL, *risult=NULL;
	short fb_i, xi_i, yi_i, xf_i, yf_i;
	short fb_o, xi_o, yi_o, xf_o, yf_o;
	short fb_s, xi_s, yi_s, xf_s, yf_s;
	short xi, yi, xf, yf;
	short n_pixel_riga, n_pixel_colonna, x_size, y_size, n_pixel_max;
	short i, j, n, k, fine, ret, ii;
	short nr = 3, nc = 3;
	short nr2 = nr >> 1, nc2 = nc >> 1;
	PARAM_SCHEDA param;

/* controllo dei parametri */

	sea2_init_config(&param);
	if(imut_check_roi(roi_i, &param) != SEAV_OK_ALL)
		return SEAV_ERR_PARAMETER;
	if(imut_check_roi(roi_o, &param) != SEAV_OK_ALL)
		return SEAV_ERR_PARAMETER;
	if(imut_check_roi(roi_s, &param) != SEAV_OK_ALL)
		return SEAV_ERR_PARAMETER;

	fb_i = roi_i->fb;
	xi_i = roi_i->xi; yi_i = roi_i->yi;
	xf_i = roi_i->xf; yf_i = roi_i->yf;
	fb_o = roi_o->fb;
	xi_o = roi_o->xi; yi_o = roi_o->yi;
	xf_o = roi_o->xf; yf_o = roi_o->yf;
	fb_s = roi_s->fb;
	xi_s = roi_s->xi; yi_s = roi_s->yi;
	xf_s = roi_s->xf; yf_s = roi_s->yf;
	n_pixel_riga = xf_i - xi_i + 1;
	n_pixel_colonna = yf_i - yi_i + 1;

	if(xf_i - xi_i != xf_o - xi_o)
		return SEAV_ERR_PARAMETER;
	if(yf_i - yi_i != yf_o - yi_o)
		return SEAV_ERR_PARAMETER;
	if(xf_i - xi_i != xf_s - xi_s)
		return SEAV_ERR_PARAMETER;
	if(yf_i - yi_i != yf_s - yi_s)
		return SEAV_ERR_PARAMETER;

	if(imut_intersection(roi_i, roi_o))
		return SEAV_ERR_PARAMETER;
	if(imut_intersection(roi_i, roi_s))
		return SEAV_ERR_PARAMETER;
	if(imut_intersection(roi_o, roi_s))
		return SEAV_ERR_PARAMETER;

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

	riga_s = (unsigned char **) util_alloca_matrice(
		(long) nr, (long) n_pixel_riga, 1L
	);
	riga_o = (unsigned char **) util_alloca_matrice(
		(long) nr, (long) n_pixel_riga, 1L
	);
	if(n_pixel_riga > n_pixel_colonna)
		n_pixel_max = n_pixel_riga;
	else
		n_pixel_max = n_pixel_colonna;
	risult = (unsigned char *) malloc(n_pixel_max);
	if(riga_s == NULL || risult == NULL || riga_o == NULL) {
		free(risult);
		util_disalloca_matrice((char **) riga_s, (long) nr);
		util_disalloca_matrice((char **) riga_o, (long) nr);
		return SEAV_ERR_MEMORY;
	}

/* azzeramento della cornice esterna */

	memset(risult, 0, n_pixel_max);
	sea2_seleziona_buffer(fb_i);
	sea2_scrivi_riga(xi_i, yi_i, n_pixel_riga, risult);
	sea2_scrivi_riga(xi_i, yf_i, n_pixel_riga, risult);
	sea2_scrivi_colonna(xi_i, yi_i, n_pixel_colonna, risult);
	sea2_scrivi_colonna(xf_i, yi_i, n_pixel_colonna, risult);

/* iterazioni */

	for(n=0, fine=0; n<n_iter_max && !fine; n++) {
		fine = 1;

/* copia dalla regione di ingresso a quella di scratch */

		if((ret = imut_copy(roi_i, roi_s)) != SEAV_OK_ALL) {
			free(risult);
			util_disalloca_matrice((char **) riga_s, (long) nr);
			util_disalloca_matrice((char **) riga_o, (long) nr);
			return ret;
		}

/* inizializzazione dei buffer di lettura */

		k = 0; /* indice della prima riga utile (piu' vecchia) delle matrici */
		for(ii=0; ii<nr-1; ii++) {
			sea2_seleziona_buffer(fb_s);
			sea2_leggi_riga(xi_s, (short) (yi_s + ii), n_pixel_riga, riga_s[k]);
			sea2_seleziona_buffer(fb_o);
			sea2_leggi_riga(xi_o, (short) (yi_o + ii), n_pixel_riga, riga_o[k]);
			k++;
		}

/* erosione selettiva */

		for(i=yi; i<=yf; i++, ii++) {
			sea2_seleziona_buffer(fb_s);
			sea2_leggi_riga(xi_s, (short) (yi_s + ii), n_pixel_riga, riga_s[k]);
			sea2_seleziona_buffer(fb_o);
			sea2_leggi_riga(xi_o, (short) (yi_o + ii), n_pixel_riga, riga_o[k]);
			if(++k == nr)
				k = 0;
			for(j=0; j<x_size; j++)
				risult[j] = un_valore(riga_s, riga_o, j, k, livello, th, &fine);
			sea2_seleziona_buffer(fb_i);
			sea2_scrivi_riga((short) (xi_i + xi), (short) (yi_i + i), x_size, risult);
		}
	}

/* uscita */

	free(risult);
	util_disalloca_matrice((char **) riga_s, (long) nr);
	util_disalloca_matrice((char **) riga_o, (long) nr);

	return SEAV_OK_ALL;
}

/**********************************************************************/

static unsigned char
un_valore(
	unsigned char **riga_s,
	unsigned char **riga_o,
	short primo,
	short k,
	short livello,
	short th,
	short *fine
)
{
	short k0, k1, k2, j0, j1, j2, somma_vicini=0, n_vicini=0;
	long somma, differenza;

	k0 = k;
	k1 = k0 + 1;
	if(k1 == 3)
		k1 = 0;
	k2 = k1 + 1;
	if(k2 == 3)
		k2 = 0;
	j0 = primo;
	j1 = j0 + 1;
	j2 = j1 + 1;
	if(livello >= 0) {
		if(riga_s[k1][j1] != (unsigned char) livello)
			return riga_s[k1][j1];
	} else {
		if(!riga_s[k1][j1])
			return riga_s[k1][j1];
	}
	if(!riga_s[k0][j0]) {
		somma_vicini += (short) riga_o[k0][j0];
		n_vicini ++;
	}
	if(!riga_s[k0][j1]) {
		somma_vicini += (short) riga_o[k0][j1];
		n_vicini ++;
	}
	if(!riga_s[k0][j2]) {
		somma_vicini += (short) riga_o[k0][j2];
		n_vicini ++;
	}
	if(!riga_s[k1][j0]) {
		somma_vicini += (short) riga_o[k1][j0];
		n_vicini ++;
	}
	if(!riga_s[k1][j2]) {
		somma_vicini += (short) riga_o[k1][j2];
		n_vicini ++;
	}
	if(!riga_s[k2][j0]) {
		somma_vicini += (short) riga_o[k2][j0];
		n_vicini ++;
	}
	if(!riga_s[k2][j1]) {
		somma_vicini += (short) riga_o[k2][j1];
		n_vicini ++;
	}
	if(!riga_s[k2][j2]) {
		somma_vicini += (short) riga_o[k2][j2];
		n_vicini ++;
	}
	if(n_vicini < 2)
		return riga_s[k1][j1];
	somma_vicini /= n_vicini;
	somma = (long) somma_vicini + (long) riga_o[k1][j1];
	if(somma > th) {
		differenza = (long) abs(somma_vicini - (short) riga_o[k1][j1]) << 9;
	} else {
		return riga_s[k1][j1];
	}
	if(differenza < (long) th * somma) {
		*fine = 0;
		return 0;
	} else {
		return riga_s[k1][j1];
	}
}

