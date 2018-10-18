
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <imutilnt.h>


/***********************************************************************

;  Nome:    
     imut_conv
;
;  Funzione:
     effettua una convoluzione su regione di interesse
;
;  Formato:
     short imut_conv(roi_i, roi_o, kernel, nr, nc, norm, flag_shift, flag_abs)
;
;  Argomenti:    
     ROI *roi_i     regione da trasformare
     ROI *roi_o     regione trasformata
	 short *kernel  matrice del nucleo della convoluzione
	 short nr       numero di righe di kernel
	 short nc       numero di colonne di kernel
	 short norm     valore di normalizzazione del risultato
	 short flag_shift  se 1 norm e' da intendersi come numero di bit di shift
	 short flag_abs    se 1 si calcola il valore assoluto
	                   prima di normalizzare il risultato

;
;  Valore di ritorno: 
     SEAV_OK_ALL              nessun errore
	 SEAV_ERR_*               codice di errore

;
;  Descrizione:
     {\bf imut_conv}() effettua una convoluzione sulla regione di interesse
	 di ingresso ponendo il risultato in una seconda regione di interesse
	 che puo' coincidere con quella di origine;
	 i valori di xf e yf della regione di arrivo sono ignorati;
	 si considera origine del kernel il punto centrale;
	 i punti che si trovano nella cornice esterna della roi di ingresso,
	 per i quali la convoluzione non puo' essere calcolata, sono trasferiti
	 inalterati in uscita;
;
;**********************************************************************/

static long un_valore(
	unsigned char **riga, short primo, short k, short *kernel,
	short nr, short nc
);
static short massimo_stretto(
	unsigned char centrale, unsigned char **riga, short primo, short k,
	short nr, short nc, short indice_centrale
);
static short massimo_largo(
	unsigned char centrale, unsigned char **riga, short primo, short k,
	short nr, short nc, short indice_centrale
);
static short minimo_stretto(
	unsigned char centrale, unsigned char **riga, short primo, short k,
	short nr, short nc, short indice_centrale
);
static short minimo_largo(
	unsigned char centrale, unsigned char **riga, short primo, short k,
	short nr, short nc, short indice_centrale
);

#undef labs

/**********************************************************************/

short
imut_conv(
	ROI *roi_i,
	ROI *roi_o,
	short *kernel,
	short nr,
	short nc,
	short norm,
	short flag_shift,
	short flag_abs
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

/* calcolo della zona utile della convoluzione relativamente all'origine
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
	if(riga == NULL || risult == NULL) {
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

/* convoluzione */

	for(i=yi; i<=yf; i++, ii++) {
		sea2_seleziona_buffer(fb_i);
		sea2_leggi_riga(xi_i, (short) (yi_i + ii), n_pixel_riga, riga[k]);
		if(++k == nr)
			k = 0;
		if(flag_abs) {
			if(flag_shift) {
				for(j=0; j<x_size; j++)
					risult[j] = (unsigned char) (
						labs(un_valore(riga, j, k, kernel, nr, nc)) >> norm
					);
			} else {
				for(j=0; j<x_size; j++)
					risult[j] = (unsigned char) (
						labs(un_valore(riga, j, k, kernel, nr, nc)) / norm
					);
			}
		} else {
			if(flag_shift) {
#if 0
				long ausl;
				for(j=0; j<x_size; j++) {
					ausl = un_valore(riga, j, k, kernel, nr, nc);
					if(ausl < 0)
						ausl = 0;
					ausl >>= norm;
					if(ausl > 255)
						ausl = 255;
					risult[j] = (unsigned char) ausl;
				}
#else
				for(j=0; j<x_size; j++) {
					risult[j] = (unsigned char) (
						un_valore(riga, j, k, kernel, nr, nc) >> norm
					);
				}
#endif
			} else {
				for(j=0; j<x_size; j++)
					risult[j] = (unsigned char) (
						un_valore(riga, j, k, kernel, nr, nc) / norm
					);
			}
		}
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

	free(risult);
	util_disalloca_matrice((char **) riga, (long) nr);

	return SEAV_OK_ALL;
}

/**********************************************************************/

static long
un_valore(
	unsigned char **riga,
	short primo,
	short k,
	short *kernel,
	short nr,
	short nc
)
{
	short i, j, kk, n;
	long somma = 0L;
	unsigned char *r0;

	for(i=0, kk=k, n=0; i<nr; i++) {
		r0 = riga[kk];
		for(j=0; j<nc; j++, n++) {
			if(kernel[n])
				somma += (long) kernel[n] * (long) r0[primo + j];
		}
		if(++kk == nr)
			kk = 0;
	}
	return somma;
}


/***********************************************************************

;  Nome:    
     imut_local_max_min
;
;  Funzione:
     estrae gli estremi locali da una regione di interesse
;
;  Formato:
     short imut_local_max_min(roi_i, roi_o, nr, nc, flag_ext, liv0, liv1)
;
;  Argomenti:    
     ROI *roi_i     regione di ingresso
     ROI *roi_o     regione di uscita
	 short nr       numero di righe dell'intorno locale
	 short nc       numero di colonne dell'intorno locale
	 short flag_ext 0: si estraggono i punti che sono > dell'intorno
                    1: si estraggono i punti che sono >= dell'intorno
                    2: si estraggono i punti che sono < dell'intorno
                    3: si estraggono i punti che sono <= dell'intorno
     short liv0     valore da dare ai punti che non risultano estremi locali
     short liv1     valore da dare ai punti che risultano estremi locali
                    
;
;  Valore di ritorno: 
     OK_ALL        nessun errore
	 SEAV_ERR_PARAMETER   parametri non corretti
	 MEMORY_ERROR  mancata allocazione di memoria

;
;  Descrizione:
     {\bf imut_local_max_min}() individua i punti di una roi che risultano
     essere estremi locali secondo il criterio dato da flag_ext;
     il risultato, (liv0 per i punti non estremi e liv1 per i punti estremi),
     e' posto in una seconda regione di interesse
	 che puo' coincidere con quella di origine;
	 i valori di xf e yf della regione di arrivo sono ignorati;
	 si considera origine dell'intorno locale il punto centrale;
	 i punti che si trovano nella cornice esterna della roi di ingresso,
	 per i quali l'intorno non esiste, sono considerati non estremi e pertanto
	 viene loro assegnato il valore liv0;
;
;**********************************************************************/


short
imut_local_max_min(
	ROI *roi_i,
	ROI *roi_o,
	short nr,
	short nc,
	short flag_ext,
	short liv0,
	short liv1
)
{
	unsigned char **riga=NULL, *risult=NULL;
	short fb_i, xi_i, yi_i, xf_i, yf_i;
	short fb_o, xi_o, yi_o;
	short ii, i, j, k, kc, n_pixel_riga, n_pixel_colonna, n_pixel_max;
	short nr2 = nr >> 1, nc2 = nc >> 1;
	short xi, yi, xf, yf, x_size, y_size, indice_centrale;
	PARAM_SCHEDA param;

/* controllo dei parametri */

	if(flag_ext < 0 || flag_ext > 3)
		return SEAV_ERR_PARAMETER;
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
	if(n_pixel_riga > n_pixel_colonna)
		n_pixel_max = n_pixel_riga;
	else
		n_pixel_max = n_pixel_colonna;

/* calcolo della zona utile del calcolo relativamente all'origine
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
	risult = (unsigned char *) malloc(n_pixel_max);
	if(riga == NULL || risult == NULL) {
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

/* calcolo */

	kc = nr2; /* indice della riga centrale della matrice riga */
	indice_centrale = nr2 * nc + nc2;
	for(i=yi; i<=yf; i++, ii++) {
		unsigned char *r0 = riga[kc];
		sea2_seleziona_buffer(fb_i);
		sea2_leggi_riga(xi_i, (short) (yi_i + ii), n_pixel_riga, riga[k]);
		if(++k == nr)
			k = 0;
		switch(flag_ext) {
			case 0: /* > */
				for(j=0; j<x_size; j++) {
					risult[j] = (unsigned char) (
						massimo_stretto(
							r0[j+nc2], riga, j, k, nr, nc, indice_centrale
						) ? liv1 : liv0
					);
				}
				break;
			case 1: /* >= */
				for(j=0; j<x_size; j++) {
					risult[j] = (unsigned char) (
						massimo_largo(
							r0[j+nc2], riga, j, k, nr, nc, indice_centrale
						) ? liv1 : liv0
					);
				}
				break;
			case 2: /* < */
				for(j=0; j<x_size; j++) {
					risult[j] = (unsigned char) (
						minimo_stretto(
							r0[j+nc2], riga, j, k, nr, nc, indice_centrale
						) ? liv1 : liv0
					);
				}
				break;
			case 3: /* <= */
				for(j=0; j<x_size; j++) {
					risult[j] = (unsigned char) (
						minimo_largo(
							r0[j+nc2], riga, j, k, nr, nc, indice_centrale
						) ? liv1 : liv0
					);
				}
				break;
		}
		if(++kc == nr)
			kc = 0;
		sea2_seleziona_buffer(fb_o);
		sea2_scrivi_riga((short) (xi_o + xi), (short) (yi_o + i), x_size, risult);
	}

/* azzeramento della cornice esterna */

	sea2_seleziona_buffer(fb_o);
	memset(risult, liv0, n_pixel_max);
	for(i=0; i<yi; i++)
		sea2_scrivi_riga(xi_o, (short) (yi_o + i), n_pixel_riga, risult);
	for(i=yf+1; i<n_pixel_colonna; i++)
		sea2_scrivi_riga(xi_o, (short) (yi_o + i), n_pixel_riga, risult);
	for(j=0; j<xi; j++)
		sea2_scrivi_colonna((short) (xi_o + j), yi_o, n_pixel_colonna, risult);
	for(j=xf+1; j<n_pixel_riga; j++)
		sea2_scrivi_colonna((short) (xi_o + j), yi_o, n_pixel_colonna, risult);

/* uscita */

	free(risult);
	util_disalloca_matrice((char **) riga, (long) nr);

	return SEAV_OK_ALL;
}

/**********************************************************************/

/* ritorna 1 se il punto e' un massimo stretto, 0 se non lo e' */

static short
massimo_stretto(
	unsigned char centrale,
	unsigned char **riga,
	short primo,
	short k,
	short nr,
	short nc,
	short indice_centrale
)
{
	short i, j, n, kk;
	unsigned char *r0;

	for(i=0, kk=k, n=0; i<nr; i++) {
		r0 = riga[kk];
		for(j=0; j<nc; j++, n++) {
			if(n != indice_centrale) {
				if(centrale <= r0[primo + j])
					return 0;
			}
		}
		if(++kk == nr)
			kk = 0;
	}
	return 1;
}

/**********************************************************************/

/* ritorna 1 se il punto e' un massimo largo, 0 se non lo e' */

static short
massimo_largo(
	unsigned char centrale,
	unsigned char **riga,
	short primo,
	short k,
	short nr,
	short nc,
	short indice_centrale
)
{
	short i, j, n, kk;
	unsigned char *r0;

	for(i=0, kk=k, n=0; i<nr; i++) {
		r0 = riga[kk];
		for(j=0; j<nc; j++, n++) {
			if(n != indice_centrale) {
				if(centrale < r0[primo + j])
					return 0;
			}
		}
		if(++kk == nr)
			kk = 0;
	}
	return 1;
}

/**********************************************************************/

/* ritorna 1 se il punto e' un minimo stretto, 0 se non lo e' */

static short
minimo_stretto(
	unsigned char centrale,
	unsigned char **riga,
	short primo,
	short k,
	short nr,
	short nc,
	short indice_centrale
)
{
	short i, j, n, kk;
	unsigned char *r0;

	for(i=0, kk=k, n=0; i<nr; i++) {
		r0 = riga[kk];
		for(j=0; j<nc; j++, n++) {
			if(n != indice_centrale) {
				if(centrale >= r0[primo + j])
					return 0;
			}
		}
		if(++kk == nr)
			kk = 0;
	}
	return 1;
}

/**********************************************************************/

/* ritorna 1 se il punto e' un minimo largo, 0 se non lo e' */

static short
minimo_largo(
	unsigned char centrale,
	unsigned char **riga,
	short primo,
	short k,
	short nr,
	short nc,
	short indice_centrale
)
{
	short i, j, n, kk;
	unsigned char *r0;

	for(i=0, kk=k, n=0; i<nr; i++) {
		r0 = riga[kk];
		for(j=0; j<nc; j++, n++) {
			if(n != indice_centrale) {
				if(centrale > r0[primo + j])
					return 0;
			}
		}
		if(++kk == nr)
			kk = 0;
	}
	return 1;
}

