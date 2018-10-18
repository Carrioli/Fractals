

/***********************************************************************

;  Nome:    
     imut_conn_filter
;
;  Funzione:
     estrae le componenti connesse
;
;  Formato:
     short imut_conn_filter(roi_i, roi_o, roi_s, vet, max_comp, x_size_min,
	                        y_size_min, x_size_max, y_size_max, area_min,
							area_max, dist_min, flag_conn, level)
;
;  Argomenti:    
     ROI *roi_i      regione di ingresso
     ROI *roi_o      regione di uscita
     ROI *roi_s      regione ausiliaria
	 CONN_COMP *vet  vettore delle componenti estratte
	 short max_comp  massimo numero di componenti estraibile (lunghezza di vet)
	 short x_size_min   larghezza minima delle componenti
	 short y_size_min   altezza minima delle componenti
	 short x_size_max   larghezza massima delle componenti
	 short y_size_max   altezza massima delle componenti
	 long area_min      area minima della componente
	 long area_max      area massima della componente
	 short dist_min     distanza minima dal bordo della ROI
	 short flag_conn    0:connettivita' 4, 1:connettivita' 8
	 short level        livello delle componenti estratte

;
;  Valore di ritorno: 
     n             numero di componenti estratte
	 SEAV_ERR_*               codice di errore

;
;  Descrizione:
     {\bf imut_conn_filter}() estrae le componenti connesse che, trovandosi
	 all'interno della regione di interesse di ingresso, soddisfano i vincoli
	 sulle dimensioni, sull'area e sulla distanza dal bordo; se dist_min vale
	 0 anche le componenti che toccano il bordo della ROI sono considerate
	 valide; i parametri caratteristici delle componenti estratte sono
	 inseriti nel vettore vet;
	 sono considerati appartenenti al background soltanto i punti a 0
	 nella ROI di ingresso;
	 al termine della routine nella ROI di uscita le componente estratte
	 sono poste al livello assegnato;
	 le ROI di ingresso e uscita possono coincidere, ma
	 la ROI ausiliaria non deve avere punti in comune con la regione di uscita;
	 le tre ROI devono avere le stesse dimensioni;
	 se le componenti connesse non filtrate sono piu' di 2000 oppure se
	 l'immagine presenta una struttura topologica complessa la routine
	 restituisce NON_TROVATO; i baricentri e le finestre che contengono
	 le componenti sono calcolati rispetto alla ROI di uscita;
	 se level vale 0 le componenti d'uscita sono colorate con livelli
	 progressivi da 1 in avanti;

;
;**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <imutilnt.h>

static short label_vicini(
	short lab1, short lab2, short lab3, short lab4, short flag_conn
);
static void aggiorna_tabella(
	short lab1, short lab2, short lab3, short lab4, short lab5,
	short flag_conn, short *tabella, short ultimo
);

/**********************************************************************/

short
imut_conn_filter(
	ROI *roi_i,
	ROI *roi_o,
	ROI *roi_s,
	CONN_COMP *vet,
	short max_comp,
	short x_size_min,
	short y_size_min,
	short x_size_max,
	short y_size_max,
	long area_min,
	long area_max,
	short dist_min,
	short flag_conn,
	short level
)
{
	unsigned char *riga0=NULL, *riga1=NULL, *riga0s=NULL, *riga1s=NULL;
	unsigned char *ausr, *colore;
	short fb_i, xi_i, yi_i, xf_i, yf_i;
	short fb_o, xi_o, yi_o, xf_o, yf_o;
	short fb_s, xi_s, yi_s, xf_s, yf_s;
	short n_pixel_riga, n_pixel_colonna;
	short i, j, n_componenti, label, lab, comp, ret;
	short *tabella=NULL;
	CONN_COMP *aus_vet=NULL;
	PUNTOL *somma=NULL;
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

	if(imut_intersection(roi_s, roi_o))
		return SEAV_ERR_PARAMETER;

	if(xf_i - xi_i != xf_o - xi_o)
		return SEAV_ERR_PARAMETER;
	if(yf_i - yi_i != yf_o - yi_o)
		return SEAV_ERR_PARAMETER;
	if(xf_i - xi_i != xf_s - xi_s)
		return SEAV_ERR_PARAMETER;
	if(yf_i - yi_i != yf_s - yi_s)
		return SEAV_ERR_PARAMETER;

/* allocazione di memoria */

	riga0 = (unsigned char *) malloc(n_pixel_riga + 2);
	riga1 = (unsigned char *) malloc(n_pixel_riga + 2);
	riga0s = (unsigned char *) malloc(n_pixel_riga + 2);
	riga1s = (unsigned char *) malloc(n_pixel_riga + 2);
	if(riga0 == NULL || riga1 == NULL || riga0s == NULL || riga1s == NULL) {
		free(riga1s);
		free(riga0s);
		free(riga1);
		free(riga0);
		return SEAV_ERR_MEMORY;
	}

/* copia della regione di ingresso per spostare la ROI */

	if((ret = imut_copy(roi_i, roi_o)) != SEAV_OK_ALL) {
		free(riga0);
		free(riga1);
		return ret;
	}

/* prima passata: dall'alto a sinistra con etichettatura dei punti */

	label = 1;
	memset(riga0, 0, n_pixel_riga+2);
	memset(riga0s, 0, n_pixel_riga+2);
	riga1[0] = riga1[n_pixel_riga+1] = 0;
	riga1s[0] = riga1s[n_pixel_riga+1] = 0;
	for(i=0; i<n_pixel_colonna; i++) {
		sea2_seleziona_buffer(fb_o);
		sea2_leggi_riga(xi_o, (short) (yi_o + i), n_pixel_riga, riga1+1);
		for(j=1; j<=n_pixel_riga; j++) {
			if(riga1[j]) {
				if(lab = label_vicini(
					(short) (((short) riga0s[j-1] << 8) + (short) riga0[j-1]),
					(short) (((short) riga0s[j] << 8) + (short) riga0[j]),
					(short) (((short) riga0s[j+1] << 8) + (short) riga0[j+1]),
					(short) (((short) riga1s[j-1] << 8) + (short) riga1[j-1]),
					flag_conn
				)) {
					riga1[j] = (unsigned char) lab;
					riga1s[j] = (unsigned char) (lab >> 8);
				} else {
					riga1[j] = (unsigned char) label;
					riga1s[j] = (unsigned char) (label >> 8);
					label ++;
					if(label > 30000) {
						free(riga1s);
						free(riga0s);
						free(riga1);
						free(riga0);
						return SEAV_ERR_NOT_FOUND;
					}
				}
			} else {
				riga1s[j] = 0;
			}
		}
		sea2_scrivi_riga(xi_o, (short) (yi_o + i), n_pixel_riga, riga1+1);
		sea2_seleziona_buffer(fb_s);
		sea2_scrivi_riga(xi_s, (short) (yi_s + i), n_pixel_riga, riga1s+1);
		ausr = riga0; riga0 = riga1; riga1 = ausr;
		ausr = riga0s; riga0s = riga1s; riga1s = ausr;
	}

/* controllo se l'immagine e' vuota */

	if(label == 1) { /* non ho assegnato neanche un'etichetta */
		free(riga1s);
		free(riga0s);
		free(riga1);
		free(riga0);
		return SEAV_ERR_NOT_FOUND;
	}

/* alloco memoria e inizializzo la tabella delle equivalenze; sono state
   assegnate label-1 etichette da 1 a label-1 */

	tabella = (short *) malloc(label * sizeof(short));
	if(tabella == NULL) {
		free(riga1s);
		free(riga0s);
		free(riga1);
		free(riga0);
		return SEAV_ERR_MEMORY;
	}
	for(i=0; i<label; i++)
		tabella[i] = i;

/* seconda passata: dal basso a destra con merge delle regioni
   adiacenti */

	memset(riga1, 0, n_pixel_riga+2);
	memset(riga1s, 0, n_pixel_riga+2);
	riga0[0] = riga0[n_pixel_riga+1] = 0;
	riga0s[0] = riga0s[n_pixel_riga+1] = 0;
	for(i=n_pixel_colonna-1; i>=0; i--) {
		sea2_seleziona_buffer(fb_o);
		sea2_leggi_riga(xi_o, (short) (yi_o + i), n_pixel_riga, riga0+1);
		sea2_seleziona_buffer(fb_s);
		sea2_leggi_riga(xi_s, (short) (yi_s + i), n_pixel_riga, riga0s+1);
		for(j=n_pixel_riga; j>0; j--) {
			if(riga0[j] || riga0s[j]) {
				aggiorna_tabella(
					(short) (((short) riga1s[j+1] << 8) + (short) riga1[j+1]),
					(short) (((short) riga1s[j] << 8) + (short) riga1[j]),
					(short) (((short) riga1s[j-1] << 8) + (short) riga1[j-1]),
					(short) (((short) riga0s[j+1] << 8) + (short) riga0[j+1]),
					(short) (((short) riga0s[j] << 8) + (short) riga0[j]),
					flag_conn, tabella, (short) (label - 1)
				);
			}
		}
		ausr = riga0; riga0 = riga1; riga1 = ausr;
		ausr = riga0s; riga0s = riga1s; riga1s = ausr;
	}

/* rimappo le componenti con numeri progressivi da 0 a n_componenti-1:
   dopo il mapping la tabella contiene l'indice della componente */

	n_componenti = 0;
	for(i=1; i<label; i++) {
		if(tabella[i] == i) {
			for(j=i; j<label; j++) {
				if(tabella[j] == i)
					tabella[j] = n_componenti;
			}
			n_componenti ++;
		}
	}

/* controllo il numero di componenti e inizializzo i
   vettori ausiliari */

	if(n_componenti > 2000) {
		free(tabella);
		free(riga1s);
		free(riga0s);
		free(riga1);
		free(riga0);
		return SEAV_ERR_NOT_FOUND;
	}
	aus_vet = (CONN_COMP *) malloc((size_t) n_componenti * sizeof(CONN_COMP));
	somma = (PUNTOL *) malloc((size_t) n_componenti * sizeof(PUNTOL));
	colore = (unsigned char *) malloc(n_componenti);
	if(aus_vet == NULL || somma == NULL || colore == NULL) {
		free(somma);
		free(aus_vet);
		free(colore);
		free(tabella);
		free(riga1s);
		free(riga0s);
		free(riga1);
		free(riga0);
		return SEAV_ERR_MEMORY;
	}
	for(i=0; i<n_componenti; i++) {
		somma[i].x = 0L;
		somma[i].y = 0L;
		aus_vet[i].xi = n_pixel_riga-1;
		aus_vet[i].yi = n_pixel_colonna-1;
		aus_vet[i].xf = 0;
		aus_vet[i].yf = 0;
		aus_vet[i].area = 0L;
	}

/* calcolo i parametri delle componenti */

	for(i=n_pixel_colonna-1; i>=0; i--) {
		sea2_seleziona_buffer(fb_o);
		sea2_leggi_riga(xi_o, (short) (yi_o + i), n_pixel_riga, riga0);
		sea2_seleziona_buffer(fb_s);
		sea2_leggi_riga(xi_s, (short) (yi_s + i), n_pixel_riga, riga0s);
		for(j=n_pixel_riga-1; j>=0; j--) {
			if(riga0[j] || riga0s[j]) {
				comp = tabella[((short) riga0s[j] << 8) + (short) riga0[j]];
				somma[comp].x += (long) j;
				somma[comp].y += (long) i;
				aus_vet[comp].area ++;
				if(j < aus_vet[comp].xi)
					aus_vet[comp].xi = j;
				if(j > aus_vet[comp].xf)
					aus_vet[comp].xf = j;
				if(i < aus_vet[comp].yi)
					aus_vet[comp].yi = i;
				if(i > aus_vet[comp].yf)
					aus_vet[comp].yf = i;
			}
		}
	}

/* rifaso i parametri rispetto alla ROI di uscita */

	for(i=0; i<n_componenti; i++) {
		aus_vet[i].fb = fb_o;
		aus_vet[i].x_bari =
			(double) somma[i].x / (double) aus_vet[i].area + (double) xi_o;
		aus_vet[i].y_bari =
			(double) somma[i].y / (double) aus_vet[i].area + (double) yi_o;
		aus_vet[i].xi += xi_o;
		aus_vet[i].yi += yi_o;
		aus_vet[i].xf += xi_o;
		aus_vet[i].yf += yi_o;
	}

/* filtro le componenti trasferendo in uscita quelle che soddisfano le
   condizioni */

	label = n_componenti;
	n_componenti = 0;
	for(i=0; i<label; i++) {
		short ok = 0;
		if(aus_vet[i].area >= area_min && aus_vet[i].area <= area_max) {
			short x_size = aus_vet[i].xf - aus_vet[i].xi + 1;
			short y_size = aus_vet[i].yf - aus_vet[i].yi + 1;
			if(x_size >= x_size_min && x_size <= x_size_max &&
			   y_size >= y_size_min && y_size <= y_size_max) {
				if(aus_vet[i].xi - dist_min >= xi_o &&
				   aus_vet[i].xf + dist_min <= xf_o &&
				   aus_vet[i].yi - dist_min >= yi_o &&
				   aus_vet[i].yf + dist_min <= yf_o) {
					ok = 1;
				}
			}
		}
		if(ok) { /* trasferisco in uscita */
			vet[n_componenti] = aus_vet[i];
			if(level)
				colore[i] = (unsigned char) level;
			else
				colore[i] = (unsigned char) n_componenti;
			vet[n_componenti].livello = (short) colore[i];
			vet[n_componenti].max_dist_bordo = 0;
			n_componenti++;
			if(n_componenti >= max_comp) {
				/* metto non buone tutte le rimanenti */
				for(i++; i<label; i++)
					colore[i] = 0;
			}
		} else {
			colore[i] = 0; /* non e' buona */
		}
	}

/* coloro le componenti superstiti nella ROI di uscita */

	for(i=n_pixel_colonna-1; i>=0; i--) {
		sea2_seleziona_buffer(fb_s);
		sea2_leggi_riga(xi_s, (short) (yi_s + i), n_pixel_riga, riga0s);
		sea2_seleziona_buffer(fb_o);
		sea2_leggi_riga(xi_o, (short) (yi_o + i), n_pixel_riga, riga0);
		for(j=n_pixel_riga-1; j>=0; j--) {
			if(riga0[j] || riga0s[j]) {
				comp = tabella[((short) riga0s[j] << 8) + (short) riga0[j]];
				riga0[j] = colore[comp];
			}
		}
		sea2_scrivi_riga(xi_o, (short) (yi_o + i), n_pixel_riga, riga0);
	}

/* uscita */

	free(colore);
	free(somma);
	free(aus_vet);
	free(tabella);
	free(riga1s);
	free(riga0s);
	free(riga1);
	free(riga0);

	return n_componenti;
}

/**************************************************************************/

/* ritorna 0 se non ci sono vicini gia' etichettati oppure l'etichetta
   di valore minimo tra quelle dei vicini */

static short
label_vicini(
	short lab1,   /* alto sx */
	short lab2,   /* alto centro */
	short lab3,   /* alto dx */
	short lab4,   /* sx */
	short flag_conn
)
{
	short lab_min = 30001;

	if(flag_conn) { /* connettivita' 8 */
		if(lab1)
			lab_min = lab1;
		if(lab2)
			if(lab2 < lab_min)
				lab_min = lab2;
		if(lab3)
			if(lab3 < lab_min)
				lab_min = lab3;
		if(lab4)
			if(lab4 < lab_min)
				lab_min = lab4;
	} else {        /* connettivita' 4 */
		if(lab2)
			lab_min = lab2;
		if(lab4)
			if(lab4 < lab_min)
				lab_min = lab4;
	}
	if(lab_min == 30001)
		return 0;
	else
		return lab_min;
}

/**************************************************************************/

/* aggiorna la tabella delle equivalenze tra le etichette */

static void
aggiorna_tabella(
	short lab1,   /* basso dx */
	short lab2,   /* basso centro */
	short lab3,   /* basso sx */
	short lab4,   /* dx */
	short lab5,   /* centro */
	short flag_conn,
	short *tabella,
	short ultimo  /* ultimo indice utile della tabella */
)
{
	short aus1, aus2, i;

	if(lab2) {
		if(tabella[lab2] != tabella[lab5]) {
			if(tabella[lab2] > tabella[lab5]) {
				aus1 = tabella[lab2];
				aus2 = tabella[lab5];
			} else {
				aus1 = tabella[lab5];
				aus2 = tabella[lab2];
			}
			for(i=ultimo; i>=aus1; i--) {
				if(tabella[i] == aus1)
					tabella[i] = aus2;
			}
		}
	}
	if(lab4) {
		if(tabella[lab4] != tabella[lab5]) {
			if(tabella[lab4] > tabella[lab5]) {
				aus1 = tabella[lab4];
				aus2 = tabella[lab5];
			} else {
				aus1 = tabella[lab5];
				aus2 = tabella[lab4];
			}
			for(i=ultimo; i>=aus1; i--) {
				if(tabella[i] == aus1)
					tabella[i] = aus2;
			}
		}
	}
	if(flag_conn) { /* connettivita' 8 */
		if(lab1) {
			if(tabella[lab1] != tabella[lab5]) {
				if(tabella[lab1] > tabella[lab5]) {
					aus1 = tabella[lab1];
					aus2 = tabella[lab5];
				} else {
					aus1 = tabella[lab5];
					aus2 = tabella[lab1];
				}
				for(i=ultimo; i>=aus1; i--) {
					if(tabella[i] == aus1)
						tabella[i] = aus2;
				}
			}
		}
		if(lab3) {
			if(tabella[lab3] != tabella[lab5]) {
				if(tabella[lab3] > tabella[lab5]) {
					aus1 = tabella[lab3];
					aus2 = tabella[lab5];
				} else {
					aus1 = tabella[lab5];
					aus2 = tabella[lab3];
				}
				for(i=ultimo; i>=aus1; i--) {
					if(tabella[i] == aus1)
						tabella[i] = aus2;
				}
			}
		}
	}
}

