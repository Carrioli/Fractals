

/***********************************************************************

;  Nome:    
     imut_sobel_rel
;
;  Funzione:
     estrae i contorni su regione di interesse
;
;  Formato:
     short imut_sobel_rel(roi_i, roi_o, tipo, n_bit_shift, dim)
;
;  Argomenti:    
     ROI *roi_i     regione da trasformare
     ROI *roi_o     regione trasformata
	 short tipo     0:orizzontale, 1:orizzontale assoluto,
	                2:verticale, 3:verticale assoluto,
					4:modulo in metrica SUMM 5:modulo in metrica MAX
	 short n_bit_shift  fattore di amplificazione del risultato
	 short soglia   valore medio dei livelli sotto al quale il gradiente
	                viene posto a 0
	 short dim      dimensione del kernel usato, da 3 a 101, se non e'
	                dispari viene aumentata di uno

;
;  Valore di ritorno: 
     SEAV_OK_ALL              nessun errore
	 SEAV_ERR_*               codice di errore

;
;  Descrizione:
     {\bf imut_sobel_rel}() estrae i contorni sulla regione di interesse
	 di ingresso ponendo il risultato in una seconda regione di interesse
	 che puo' coincidere con quella di origine;
	 i valori di xf e yf della regione di arrivo sono ignorati;
	 i punti che si trovano nella cornice esterna della roi di ingresso,
	 per i quali la convoluzione non puo' essere calcolata, sono messi a 0
	 in uscita;
	 la formula base di calcolo e': gradiente = (differenza * 256) / somma,
	 dove la differenza, nel caso di dim=3, e' l'usuale differenza di Sobel
	 ottenuta dalla convoluzione con il nucleo 3x3, mentre la somma e' data
	 dalla convoluzione con lo stesso nucleo in cui i segni sono tutti positivi;
	 se dim>3 il nucleo contiene sempre e solo 6 valori diversi da 0
	 posti ai quattro angoli e in centro alle righe/colonne estreme; 
	 tali valori sono gli stessi del Sobel standard;
	 per i punti in cui il valor medio (somma / 8) e' inferiore
	 alla soglia data il gradiente viene posto a 0; il valore della
	 soglia deve essere strettamente maggiore di 0.
	 la normalizzazione del risultato della avviene shiftando a
	 sinistra il valore calcolato di n_bit_shift (o a destra
	 nel caso di numero di bit negativo);
	 se dopo la normalizzazione il valore non rientra in 8 bit, il suo valore
	 viene posto al massimo o al minimo consentito;
	 n_bit_shift deve essere compreso tra -8 e 8;
	 al risultato dei gradienti monodimensionali viene sommato 128;
;
;**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <imutilnt.h>

static unsigned char un_valore(
	unsigned char *r0, unsigned char *r1, unsigned char *r2, short primo,
	short tipo, short n_bit_shift, short tipo_shift, short soglia,
	short nc_av1, short nc_av2
);

#undef abs

/**********************************************************************/

short
imut_sobel_rel(
	ROI *roi_i,
	ROI *roi_o,
	short tipo,
	short n_bit_shift,
	short soglia,
	short dim
)
{
	unsigned char **riga=NULL, *risult=NULL, *r0, *r1, *r2;
	short ii, i, j, k, kk, n_pixel_riga, n_pixel_colonna;
	short fb_i, xi_i, yi_i, xf_i, yf_i;
	short fb_o, xi_o, yi_o;
	short xi, yi, xf, yf, x_size, y_size, n_pixel_max, tipo_shift;
	short nr, nc, nr2, nc2, nr_av, nr_in, nc_av1, nc_av2;
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
	if(n_bit_shift < -8 || n_bit_shift > 8)
		return SEAV_ERR_PARAMETER;
	if(tipo < 0 || tipo > 5)
		return SEAV_ERR_PARAMETER;
	if(soglia <= 0 || soglia > 255)
		return SEAV_ERR_PARAMETER;
	if(dim < 3 || dim > 101)
		return SEAV_ERR_PARAMETER;

/* calcolo della zona utile della convoluzione relativamente all'origine
   della roi di ingresso */

	if(!(dim & 1))
		dim ++;
	nr = nc = dim;
	nr2 = nr >> 1;
	nc2 = nc >> 1;
	x_size = (xf_i - xi_i + 1) - (nc - 1);
	y_size = (yf_i - yi_i + 1) - (nr - 1);
	if(x_size <= 0 || y_size <= 0)
		return SEAV_ERR_PARAMETER;
	xi = nc2;
	yi = nr2;
	xf = xi + x_size - 1;
	yf = yi + y_size - 1;
	nr_av = nr2;
	nr_in = nr - nr2;
	nc_av1 = nc2;
	nc_av2 = nc - 1;

/* allocazione di memoria */

	riga = (unsigned char **) util_alloca_matrice(
		(long) nr, (long) n_pixel_riga, 1L
	);
	if(n_pixel_riga > n_pixel_colonna)
		n_pixel_max = n_pixel_riga;
	else
		n_pixel_max = n_pixel_colonna;
	risult = (unsigned char *) malloc(n_pixel_max);
	if(riga == NULL || risult == NULL) {
		free(risult);
		if(riga)
			util_disalloca_matrice((char **) riga, (long) nr);
		return SEAV_ERR_MEMORY;
	}

/* calcolo del numero di bit di shift totale */

	if(n_bit_shift >= 0) {
		tipo_shift = 0; /* a sx */
	} else {
		tipo_shift = 1; /* a dx */
		n_bit_shift = -n_bit_shift;
	}
	soglia <<= 3;

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
		r0 = riga[k];
		if(k <= nr_av)
			kk = k + nr_av;
		else
			kk = k - nr_in;
		r1 = riga[kk];
		if(kk <= nr_av)
			kk += nr_av;
		else
			kk -= nr_in;
		r2 = riga[kk];
		for(j=x_size-1; j>=0; j--)
			risult[j] = un_valore(
				r0, r1, r2, j, tipo, n_bit_shift, tipo_shift, soglia,
				nc_av1, nc_av2
			);
		sea2_seleziona_buffer(fb_o);
		sea2_scrivi_riga((short) (xi_o + xi), (short) (yi_o + i), x_size, risult);
	}

/* azzeramento della cornice esterna */

	if(tipo == 0 || tipo == 2) /* orizzontale e verticale con segno */
		memset(risult, 128, n_pixel_max);
	else
		memset(risult, 0, n_pixel_max);
	sea2_seleziona_buffer(fb_o);
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

static unsigned char
un_valore(
	unsigned char *r0,
	unsigned char *r1,
	unsigned char *r2,
	short primo,
	short tipo,
	short n_bit_shift,
	short tipo_shift,
	short soglia,
	short nc_av1,
	short nc_av2
)
{
	short somma, somma1, primo1=primo+nc_av1, primo2=primo+nc_av2;
	short v_medio, v_medio1, aus;

	switch(tipo) {
		case 0: /* orizzontale */
			aus = (short) r0[primo];
			somma = -aus; v_medio = aus;
			aus = (short) r0[primo2];
			somma += aus; v_medio += aus;
			aus = ((short) r1[primo] << 1);
			somma -= aus; v_medio += aus;
			aus = ((short) r1[primo2] << 1);
			somma += aus; v_medio += aus;
			aus = (short) r2[primo];
			somma -= aus; v_medio += aus;
			aus = (short) r2[primo2];
			somma += aus; v_medio += aus;

			if(v_medio >= soglia) {
				if(tipo_shift) /* dx */
					somma = (short) (
						(((long) somma << 8) / v_medio) >> n_bit_shift
					) + 128;
				else
					somma = (short) (
						(((long) somma << 8) / v_medio) << n_bit_shift
					) + 128;
				if(somma < 0)
					somma = 0;
				else if(somma > 255)
					somma = 255;
			} else {
				somma = 128;
			}
			break;
		case 1: /* orizzontale assoluto */
			aus = (short) r0[primo];
			somma = -aus; v_medio = aus;
			aus = (short) r0[primo2];
			somma += aus; v_medio += aus;
			aus = ((short) r1[primo] << 1);
			somma -= aus; v_medio += aus;
			aus = ((short) r1[primo2] << 1);
			somma += aus; v_medio += aus;
			aus = (short) r2[primo];
			somma -= aus; v_medio += aus;
			aus = (short) r2[primo2];
			somma += aus; v_medio += aus;

			if(v_medio >= soglia) {
				somma = abs(somma);
				if(tipo_shift) /* dx */
					somma = (short) (
						(((long) somma << 8) / v_medio) >> n_bit_shift
					);
				else
					somma = (short) (
						(((long) somma << 8) / v_medio) << n_bit_shift
					);
				if(somma > 255)
					somma = 255;
			} else {
				somma = 0;
			}
			break;
		case 2: /* verticale */
			somma = -((short) r0[primo]);
			somma -= ((short) r0[primo1] << 1);
			somma -= (short) r0[primo2];
			v_medio = -somma << 1;
			somma += (short) r2[primo];
			somma += ((short) r2[primo1] << 1);
			somma += (short) r2[primo2];
			v_medio += somma;

			if(v_medio >= soglia) {
				if(tipo_shift) /* dx */
					somma = (short) (
						(((long) somma << 8) / v_medio) >> n_bit_shift
					) + 128;
				else
					somma = (short) (
						(((long) somma << 8) / v_medio) << n_bit_shift
					) + 128;
				if(somma < 0)
					somma = 0;
				else if(somma > 255)
					somma = 255;
			} else {
				somma = 128;
			}
			break;
		case 3: /* verticale assoluto */
			somma = -((short) r0[primo]);
			somma -= ((short) r0[primo1] << 1);
			somma -= (short) r0[primo2];
			v_medio = -somma << 1;
			somma += (short) r2[primo];
			somma += ((short) r2[primo1] << 1);
			somma += (short) r2[primo2];
			v_medio += somma;

			if(v_medio >= soglia) {
				somma = abs(somma);
				if(tipo_shift) /* dx */
					somma = (short) (
						(((long) somma << 8) / v_medio) >> n_bit_shift
					);
				else
					somma = (short) (
						(((long) somma << 8) / v_medio) << n_bit_shift
					);
				if(somma > 255)
					somma = 255;
			} else {
				somma = 0;
			}
			break;
		case 4: /* modulo in metrica SUMM */
			aus = (short) r0[primo];
			somma = somma1 = -aus; v_medio1 = aus;
			somma -= ((short) r0[primo1] << 1);
			aus = (short) r0[primo2];
			somma1 += aus; v_medio1 += aus;
			somma -= (short) r0[primo2];
			v_medio = -somma << 1;
			aus = ((short) r1[primo] << 1);
			somma1 -= aus; v_medio1 += aus;
			aus = ((short) r1[primo2] << 1);
			somma1 += aus; v_medio1 += aus;
			aus = (short) r2[primo];
			somma1 -= aus; v_medio1 += aus;
			somma += (short) r2[primo];
			somma += ((short) r2[primo1] << 1);
			aus = (short) r2[primo2];
			somma1 += aus; v_medio1 += aus;
			somma += (short) r2[primo2];
			v_medio += somma;

			if(v_medio >= soglia) {
				somma = abs(somma);
				somma = (short) (((long) somma << 8) / v_medio);
			} else {
				somma = 0;
			}
			if(v_medio1 >= soglia) {
				somma1 = abs(somma1);
				somma1 = (short) (((long) somma1 << 8) / v_medio1);
			} else {
				somma1 = 0;
			}
			somma += somma1;
			if(tipo_shift) /* dx */
				somma >>= n_bit_shift;
			else
				somma <<= n_bit_shift;
			if(somma > 255)
				somma = 255;
			break;
		case 5: /* modulo in metrica MAX */
			aus = (short) r0[primo];
			somma = somma1 = -aus; v_medio1 = aus;
			somma -= ((short) r0[primo1] << 1);
			aus = (short) r0[primo2];
			somma1 += aus; v_medio1 += aus;
			somma -= (short) r0[primo2];
			v_medio = -somma << 1;
			aus = ((short) r1[primo] << 1);
			somma1 -= aus; v_medio1 += aus;
			aus = ((short) r1[primo2] << 1);
			somma1 += aus; v_medio1 += aus;
			aus = (short) r2[primo];
			somma1 -= aus; v_medio1 += aus;
			somma += (short) r2[primo];
			somma += ((short) r2[primo1] << 1);
			aus = (short) r2[primo2];
			somma1 += aus; v_medio1 += aus;
			somma += (short) r2[primo2];
			v_medio += somma;

			if(v_medio >= soglia) {
				somma = abs(somma);
				somma = (short) (((long) somma << 8) / v_medio);
			} else {
				somma = 0;
			}
			if(v_medio1 >= soglia) {
				somma1 = abs(somma1);
				somma1 = (short) (((long) somma1 << 8) / v_medio1);
			} else {
				somma1 = 0;
			}
			somma = max(somma, somma1);
			if(tipo_shift) /* dx */
				somma >>= n_bit_shift;
			else
				somma <<= n_bit_shift;
			if(somma > 255)
				somma = 255;
			break;
	}

	return (unsigned char) somma;
}


