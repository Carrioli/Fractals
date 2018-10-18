

/*
	imut_misura_diametro()
	imut_misura_diametro1()
	imut_misura_diametro_estremo()
	imut_cerca_prima_transizione()
	imut_cerca_ultima_transizione()
	imut_calcola_sub_pixel()
	imut_individua_primo_diametro()
	imut_individua_diametro_seguente()
	imut_estrai_bordi()
	imut_individua_primi_bordi()
	imut_individua_bordi_seguenti()
*/

/**************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <imutilnt.h>

#define LUNG_MAX     768 /* per i vettori */

static void scrivi_at(short x, short y);

/* vettori di uso comune */
static unsigned char vet_pixel[LUNG_MAX];
static short vet_diametri[LUNG_MAX], aus_vet_diametri[LUNG_MAX];
static long coo1[LUNG_MAX], coo2[LUNG_MAX]; /* per la retta interpolante */


/***********************************************************************

;  Nome:    
     imut_misura_diametro
;
;  Funzione:
     misura un diametro medio
;
;  Formato:
     short imut_misura_diametro(roi, direzione, flag_interno_scuro, soglia,
	                            passo, n_bit_sub_pixel, flag_max, cx, cy,
								diametro, retta)
;
;  Argomenti:    
     ROI *roi                      area di interesse
	 short direzione               0: orizzontale 1: verticale
	 short flag_interno_scuro      0: il diametro e' piu' chiaro dello sfondo
	                               1: il diametro e' piu' scuro dello sfondo
	 short soglia                  soglia per l'individuazione delle transizioni
	 short passo                   di sottocampionamento
	 short n_bit_sub_pixel         logaritmo del numero di tick per pixel
	 short flag_max,               se 1 si misura il diam. max., se 0 il min.
	 double cx                     risoluzione orizzontale in unita fisiche per
	                               tick
	 double cy                     risoluzione verticale in unita fisiche per
	                               tick
	 double *diametro              valore misurato in tick
	 RETTA *retta                  retta che interpola i punti di centro
	                               dei diametri individuati

;
;  Valore di ritorno: 
     SEAV_OK_ALL              nessun errore
	 SEAV_ERR_*               codice di errore

;
;  Descrizione:
     {\bf imut_misura_diametro}() calcola il diametro nell'area di
	 interesse come media di un quarto dei valori: da .5 a .75 del vettore
	 ordinato dei diametri misurati sulle righe/colonne; se "flag_max" vale 1
	 il vettore si ordina crescente, altrimenti decrescente; inoltre,
	 se retta e' diversa da NULL, calcola anche la retta che interpola i
	 punti di centro dei diametri individuati; tale retta e' calcolata
	 direttamente in unita' di misura fisiche; se la retta non viene
	 calcolata i tre coefficienti (a, b, c) sono messi a 0.0;

;
;**********************************************************************/

short
imut_misura_diametro(
	ROI *roi,
	short direzione,           /*  0: orizzontale, 1: verticale */
	short flag_interno_scuro,  /*  0: il diametro e' piu' chiaro dello sfondo
	                               1: il diametro e' piu' scuro dello sfondo */
	short soglia,          /* soglia per l'individuazione delle transizioni */
	short passo,           /* di sottocampionamento */
	short n_bit_sub_pixel, /* logaritmo del numero di tick per pixel */
	short flag_max,        /* se 1 si misura il diam. max. altrimenti il min. */
	double cx,             /* risoluzione orizzontale in unita' / tick */
	double cy,             /* risoluzione verticale in unita' / tick */
	double *diametro,      /* valore misurato in tick */
	RETTA *retta           /* retta interpolante i punti di mezzo dei
	                          diametri */
)
{
	short fb = roi->fb, xi = roi->xi, yi = roi->yi, xf = roi->xf, yf = roi->yf;
	short i, j, primo, ultimo, flag_ordine, incr;
	short coord, coord_ini, coord_fin, indice_i, indice_f, n_diametri = 0;
	short flag_inizio = 1; /* dall'alto o da sinistra */
	short diametro_min, diametro_max;
	long somma;
	

/* seleziono la coordinata finale in dipendenza dalla direzione */

	if(direzione) {  /* verticale */
		if(flag_inizio) {  /* dall'alto/sinistra */
			coord_ini = xi;
			coord_fin = xf - (xf - xi) % passo;
			incr = passo;
			if(coord_ini >= coord_fin)
				return SEAV_ERR_PARAMETER;
		} else {         /* dal basso/destra */
			coord_ini = xf;
			coord_fin = xi + (xf - xi) % passo;
			incr = -passo;
			if(coord_ini <= coord_fin)
				return SEAV_ERR_PARAMETER;
		}
	} else {         /* orizzontale */
		if(flag_inizio) {  /* dall'alto/sinistra */
			coord_ini = yi;
			coord_fin = yf - (yf - yi) % passo;
			incr = passo;
			if(coord_ini >= coord_fin)
				return SEAV_ERR_PARAMETER;
		} else {         /* dal basso/destra */
			coord_ini = yf;
			coord_fin = yi + (yf - yi) % passo;
			incr = -passo;
			if(coord_ini <= coord_fin)
				return SEAV_ERR_PARAMETER;
		}
	}

/* cerco di individuare il primo diametro */

	sea2_seleziona_buffer(fb);
	if((coord = imut_individua_primo_diametro(
		roi, direzione, flag_interno_scuro, soglia, passo, n_bit_sub_pixel,
		vet_diametri, coo1, &indice_i, &indice_f, flag_inizio
	)) < 0)
		return coord;
	coo2[0] = (long) (coord << n_bit_sub_pixel);
	n_diametri = 1;

/* a partire dalla posizione corrente misuro tutti i diametri che posso e
   li memorizzo; la lettura dalla scheda e' ottimizzata: ci si posiziona
   nel punto precedentemente trovato */

	if(coord != coord_fin) {
		for(coord+=incr; coord!=coord_fin; coord+=incr) {
			if(imut_individua_diametro_seguente(
				coord, roi, direzione, flag_interno_scuro, soglia,
				n_bit_sub_pixel, vet_diametri+n_diametri, coo1+n_diametri,
				&indice_i, &indice_f
			) == SEAV_OK_ALL) {
				coo2[n_diametri++] = (long) (coord << n_bit_sub_pixel);
			}
		}
	}

/* ordino il vettore dei diametri in ordine crescente se flag_max=1, altrimenti
   in ordine decrescente */

	if(flag_max == 1)
		flag_ordine = 0;
	else
		flag_ordine = 1;

/* eventualmente copio il vettore dei diametri in aus_vet_diametri */

	if(retta != NULL)
		memcpy(
			aus_vet_diametri, vet_diametri, (size_t) n_diametri * sizeof(short)
		);
	util_ordinamento(vet_diametri, n_diametri, 1, flag_ordine);

/* faccio la media su un quarto dei valori: da .5 a .75 del vettore ordinato */

	primo = (short) (n_diametri >> 1);
	ultimo = (short) (primo + (n_diametri >> 2));
	if(flag_max == 1) {
		diametro_min = vet_diametri[primo];
		diametro_max = vet_diametri[ultimo];
	} else {
		diametro_min = vet_diametri[ultimo];
		diametro_max = vet_diametri[primo];
	}
	for(i=primo, somma=0L; i<=ultimo; i++)
		somma += (long) vet_diametri[i];

	*diametro = (double) somma / (double) (ultimo-primo+1);

/* eventualmente calcolo la retta interpolante: uso solo i valori dei
   centri che appartengono a diametri compresi tra diametro_min e
   diametro_max */

	if(retta != NULL) {
		for(j=0, i=0; i<n_diametri; i++) {
			if(aus_vet_diametri[i] <= diametro_max &&
		   	aus_vet_diametri[i] >= diametro_min) {
				if(i != j) {
					coo1[j] = coo1[i];
					coo2[j] = coo2[i];
				}
				j++;
			}
		}
		if(direzione) { /* verticale */
			if(imut_retta_interpolante(coo2, coo1, j, 3, cx, cy, retta) != SEAV_OK_ALL)
				retta->a = retta->b = retta->c = 0.0;
		} else { /* orizzontale */
			if(imut_retta_interpolante(coo1, coo2, j, 3, cx, cy, retta) != SEAV_OK_ALL)
				retta->a = retta->b = retta->c = 0.0;
		}
	}

	return SEAV_OK_ALL;
}


/***********************************************************************

;  Nome:    
     imut_misura_diametro1
;
;  Funzione:
     misura un diametro medio
;
;  Formato:
     short imut_misura_diametro1(roi, direzione, flag_interno_scuro, soglia,
	                            passo, n_bit_sub_pixel, flag_max, cx, cy,
								diametro, retta, coord_centro)
;
;  Argomenti:    
     ROI *roi                      area di interesse
	 short direzione               0: orizzontale 1: verticale
	 short flag_interno_scuro      0: il diametro e' piu' chiaro dello sfondo
	                               1: il diametro e' piu' scuro dello sfondo
	 short soglia                  soglia per l'individuazione delle transizioni
	 short passo                   di sottocampionamento
	 short n_bit_sub_pixel         logaritmo del numero di tick per pixel
	 short flag_max,               se 1 si misura il diam. max., se 0 il min.
	 double cx                     risoluzione orizzontale in unita fisiche per
	                               tick
	 double cy                     risoluzione verticale in unita fisiche per
	                               tick
	 double *diametro              valore misurato in tick
	 RETTA *retta                  retta che interpola i punti di centro
	                               dei diametri individuati
	 double *coord_centro          coordinata del centro del diametro in tick

;
;  Valore di ritorno: 
     SEAV_OK_ALL              nessun errore
	 SEAV_ERR_*               codice di errore

;
;  Descrizione:
     {\bf imut_misura_diametro1}() effettua lo stesso lavoro di
     {\bf imut_misura_diametro}() ed inoltre restituisce il centro del diametro
	 individuato;

;
;**********************************************************************/

short
imut_misura_diametro1(
	ROI *roi,
	short direzione,           /*  0: orizzontale, 1: verticale */
	short flag_interno_scuro,  /*  0: il diametro e' piu' chiaro dello sfondo
	                               1: il diametro e' piu' scuro dello sfondo */
	short soglia,          /* soglia per l'individuazione delle transizioni */
	short passo,           /* di sottocampionamento */
	short n_bit_sub_pixel, /* logaritmo del numero di tick per pixel */
	short flag_max,        /* se 1 si misura il diam. max. altrimenti il min. */
	double cx,             /* risoluzione orizzontale in unita' / tick */
	double cy,             /* risoluzione verticale in unita' / tick */
	double *diametro,      /* valore misurato in tick */
	RETTA *retta,          /* retta interpolante i punti di mezzo dei
	                          diametri */
	double *coord_centro   /* centro del diametro in tick */
)
{
	short fb = roi->fb, xi = roi->xi, yi = roi->yi, xf = roi->xf, yf = roi->yf;
	short i, j, primo, ultimo, flag_ordine, incr;
	short coord, coord_ini, coord_fin, indice_i, indice_f, n_diametri = 0;
	short flag_inizio = 1; /* dall'alto o da sinistra */
	short diametro_min, diametro_max;
	long somma;
	

/* seleziono la coordinata finale in dipendenza dalla direzione */

	if(direzione) {  /* verticale */
		if(flag_inizio) {  /* dall'alto/sinistra */
			coord_ini = xi;
			coord_fin = xf - (xf - xi) % passo;
			incr = passo;
			if(coord_ini >= coord_fin)
				return SEAV_ERR_PARAMETER;
		} else {         /* dal basso/destra */
			coord_ini = xf;
			coord_fin = xi + (xf - xi) % passo;
			incr = -passo;
			if(coord_ini <= coord_fin)
				return SEAV_ERR_PARAMETER;
		}
	} else {         /* orizzontale */
		if(flag_inizio) {  /* dall'alto/sinistra */
			coord_ini = yi;
			coord_fin = yf - (yf - yi) % passo;
			incr = passo;
			if(coord_ini >= coord_fin)
				return SEAV_ERR_PARAMETER;
		} else {         /* dal basso/destra */
			coord_ini = yf;
			coord_fin = yi + (yf - yi) % passo;
			incr = -passo;
			if(coord_ini <= coord_fin)
				return SEAV_ERR_PARAMETER;
		}
	}

/* cerco di individuare il primo diametro */

	sea2_seleziona_buffer(fb);
	if((coord = imut_individua_primo_diametro(
		roi, direzione, flag_interno_scuro, soglia, passo, n_bit_sub_pixel,
		vet_diametri, coo1, &indice_i, &indice_f, flag_inizio
	)) < 0)
		return coord;
	coo2[0] = (long) (coord << n_bit_sub_pixel);
	n_diametri = 1;

/* a partire dalla posizione corrente misuro tutti i diametri che posso e
   li memorizzo; la lettura dalla scheda e' ottimizzata: ci si posiziona
   nel punto precedentemente trovato */

	if(coord != coord_fin) {
		for(coord+=incr; coord!=coord_fin; coord+=incr) {
			if(imut_individua_diametro_seguente(
				coord, roi, direzione, flag_interno_scuro, soglia,
				n_bit_sub_pixel, vet_diametri+n_diametri, coo1+n_diametri,
				&indice_i, &indice_f
			) == SEAV_OK_ALL) {
				coo2[n_diametri++] = (long) (coord << n_bit_sub_pixel);
			}
		}
	}

/* ordino il vettore dei diametri in ordine crescente se flag_max=1, altrimenti
   in ordine decrescente */

	if(flag_max == 1)
		flag_ordine = 0;
	else
		flag_ordine = 1;

/* copio il vettore dei diametri in aus_vet_diametri */

	memcpy(
		aus_vet_diametri, vet_diametri, (size_t) n_diametri * sizeof(short)
	);
	util_ordinamento(vet_diametri, n_diametri, 1, flag_ordine);

/* faccio la media su un quarto dei valori: da .5 a .75 del vettore ordinato
   per calcolare il diametro */

	primo = (short) (n_diametri >> 1);
	ultimo = (short) (primo + (n_diametri >> 2));
	if(flag_max == 1) {
		diametro_min = vet_diametri[primo];
		diametro_max = vet_diametri[ultimo];
	} else {
		diametro_min = vet_diametri[ultimo];
		diametro_max = vet_diametri[primo];
	}
	for(i=primo, somma=0L; i<=ultimo; i++)
		somma += (long) vet_diametri[i];

	*diametro = (double) somma / (double) (ultimo-primo+1);

/* calcolo il centro del del diametro: uso solo i valori dei
   centri che appartengono a diametri compresi tra diametro_min e
   diametro_max */

	for(j=0, i=0, somma=0L; i<n_diametri; i++) {
		if(aus_vet_diametri[i] <= diametro_max &&
	   	aus_vet_diametri[i] >= diametro_min) {
			somma += coo1[i];
			j++;
		}
	}
	if(j)
		*coord_centro = (double) somma / (double) j;
	else
		return SEAV_ERR_NOT_FOUND;

/* eventualmente calcolo la retta interpolante: uso solo i valori dei
   centri che appartengono a diametri compresi tra diametro_min e
   diametro_max */

	if(retta != NULL) {
		for(j=0, i=0; i<n_diametri; i++) {
			if(aus_vet_diametri[i] <= diametro_max &&
		   	aus_vet_diametri[i] >= diametro_min) {
				if(i != j) {
					coo1[j] = coo1[i];
					coo2[j] = coo2[i];
				}
				j++;
			}
		}
		if(direzione) { /* verticale */
			if(imut_retta_interpolante(coo2, coo1, j, 3, cx, cy, retta) != SEAV_OK_ALL)
				retta->a = retta->b = retta->c = 0.0;
		} else { /* orizzontale */
			if(imut_retta_interpolante(coo1, coo2, j, 3, cx, cy, retta) != SEAV_OK_ALL)
				retta->a = retta->b = retta->c = 0.0;
		}
	}

	return SEAV_OK_ALL;
}


/***********************************************************************

;  Nome:    
     imut_misura_diametro_estremo
;
;  Funzione:
     misura un diametro estremo
;
;  Formato:
     short imut_misura_diametro_estremo(roi, direzione, flag_interno_scuro, soglia,
	                            passo, n_bit_sub_pixel, flag_max, cx, cy,
								diametro, retta, coord_centro, coord_diam)
;
;  Argomenti:    
     ROI *roi                      area di interesse
	 short direzione               0: orizzontale 1: verticale
	 short flag_interno_scuro      0: il diametro e' piu' chiaro dello sfondo
	                               1: il diametro e' piu' scuro dello sfondo
	 short soglia                  soglia per l'individuazione delle transizioni
	 short passo                   di sottocampionamento
	 short n_bit_sub_pixel         logaritmo del numero di tick per pixel
	 short flag_max,               se 1 si misura il diam. max., se 0 il min.
	 double cx                     risoluzione orizzontale in unita fisiche per
	                               tick
	 double cy                     risoluzione verticale in unita fisiche per
	                               tick
	 double *diametro              valore misurato in tick
	 RETTA *retta                  retta che interpola i punti di centro
	                               dei diametri individuati
	 double *coord_centro          coordinata del centro del diametro in tick
	 double *coord_diam            coordinata del diametro in tick

;
;  Valore di ritorno: 
     SEAV_OK_ALL              nessun errore
	 SEAV_ERR_*               codice di errore

;
;  Descrizione:
     {\bf imut_misura_diametro_estremo}() calcola il diametro minimo o il massimo
     all'interno dell'area di interesse; inoltre calcola la posizione del
     diametro estremo e del suo centro;

;
;**********************************************************************/

short
imut_misura_diametro_estremo(
	ROI *roi,
	short direzione,           /*  0: orizzontale, 1: verticale */
	short flag_interno_scuro,  /*  0: il diametro e' piu' chiaro dello sfondo
	                               1: il diametro e' piu' scuro dello sfondo */
	short soglia,          /* soglia per l'individuazione delle transizioni */
	short passo,           /* di sottocampionamento */
	short n_bit_sub_pixel, /* logaritmo del numero di tick per pixel */
	short flag_max,        /* se 1 si misura il diam. max. altrimenti il min. */
	double cx,             /* risoluzione orizzontale in unita' / tick */
	double cy,             /* risoluzione verticale in unita' / tick */
	double *diametro,      /* valore misurato in tick */
	RETTA *retta,          /* retta interpolante i punti di mezzo dei
	                          diametri */
	double *coord_centro,  /* centro del diametro in tick */
	double *coord_diam     /* posizione del diametro in tick */
)
{
	short fb = roi->fb, xi = roi->xi, yi = roi->yi, xf = roi->xf, yf = roi->yf;
	short i, incr;
	short coord, coord_ini, coord_fin, indice_i, indice_f, n_diametri = 0;
	short flag_inizio = 0; /* dal basso o da destra */
	short diametro_min, diametro_max;
	long pos_centro_min, pos_diam_min, pos_centro_max, pos_diam_max;
	

/* seleziono la coordinata finale in dipendenza dalla direzione */

	if(direzione) {  /* verticale */
		if(flag_inizio) {  /* dall'alto/sinistra */
			coord_ini = xi;
			coord_fin = xf - (xf - xi) % passo;
			incr = passo;
			if(coord_ini >= coord_fin)
				return SEAV_ERR_PARAMETER;
		} else {         /* dal basso/destra */
			coord_ini = xf;
			coord_fin = xi + (xf - xi) % passo;
			incr = -passo;
			if(coord_ini <= coord_fin)
				return SEAV_ERR_PARAMETER;
		}
	} else {         /* orizzontale */
		if(flag_inizio) {  /* dall'alto/sinistra */
			coord_ini = yi;
			coord_fin = yf - (yf - yi) % passo;
			incr = passo;
			if(coord_ini >= coord_fin)
				return SEAV_ERR_PARAMETER;
		} else {         /* dal basso/destra */
			coord_ini = yf;
			coord_fin = yi + (yf - yi) % passo;
			incr = -passo;
			if(coord_ini <= coord_fin)
				return SEAV_ERR_PARAMETER;
		}
	}

/* cerco di individuare il primo diametro */

	sea2_seleziona_buffer(fb);
	if((coord = imut_individua_primo_diametro(
		roi, direzione, flag_interno_scuro, soglia, passo, n_bit_sub_pixel,
		vet_diametri, coo1, &indice_i, &indice_f, flag_inizio
	)) < 0)
		return coord;
	coo2[0] = (long) (coord << n_bit_sub_pixel);
	n_diametri = 1;

/* a partire dalla posizione corrente misuro tutti i diametri che posso e
   li memorizzo; la lettura dalla scheda e' ottimizzata: ci si posiziona
   nel punto precedentemente trovato */

	if(coord != coord_fin) {
		for(coord+=incr; coord!=coord_fin; coord+=incr) {
			if(imut_individua_diametro_seguente(
				coord, roi, direzione, flag_interno_scuro, soglia,
				n_bit_sub_pixel, vet_diametri+n_diametri, coo1+n_diametri,
				&indice_i, &indice_f
			) == SEAV_OK_ALL) {
				coo2[n_diametri++] = (long) (coord << n_bit_sub_pixel);
			}
		}
	}

/* cerco il diametro estremo */

	diametro_min = diametro_max = vet_diametri[0];
	pos_centro_min = pos_centro_max = coo1[0];
	pos_diam_min = pos_diam_max = coo2[0];
	for(i=1; i<n_diametri; i++) {
		if(vet_diametri[i] < diametro_min) {
			diametro_min = vet_diametri[i];
			pos_centro_min = coo1[i];
			pos_diam_min = coo2[i];
		} else if(vet_diametri[i] > diametro_max) {
			diametro_max = vet_diametri[i];
			pos_centro_max = coo1[i];
			pos_diam_max= coo2[i];
		}
	}

	if(flag_max == 1) {
		*coord_centro = (double) pos_centro_max;
		*coord_diam = (double) pos_diam_max;
		*diametro = (double) diametro_max;
	} else {
		*coord_centro = (double) pos_centro_min;
		*coord_diam = (double) pos_diam_min;
		*diametro = (double) diametro_min;
	}

/* eventualmente calcolo la retta interpolante: uso tutti i diametri */

	if(retta != NULL) {
		if(direzione) { /* verticale */
			if(imut_retta_interpolante(
				coo2, coo1, n_diametri, 3, cx, cy, retta
			) != SEAV_OK_ALL)
				retta->a = retta->b = retta->c = 0.0;
		} else { /* orizzontale */
			if(imut_retta_interpolante(
				coo1, coo2, n_diametri, 3, cx, cy, retta
			) != SEAV_OK_ALL)
				retta->a = retta->b = retta->c = 0.0;
		}
	}

	return SEAV_OK_ALL;
}

/****************************************************************************/

/* cerca la prima transizione nel vettore vettore;
   se la trova ne ritorna l'indice, altrimenti ritorna il codice di errore */

short
imut_cerca_prima_transizione(
	unsigned char *vettore,
	short primo,
	short ultimo,
	short th,
	short segno  /* 1: da scuro a chiaro
				   -1: da chiaro a scuro
				    0: entrambi */
)
{
	register short i;
	short soglia_trans = (short) (th >> 2);
	short aus1, aus2, somma;

	if(segno == -1) {
		while(soglia_trans > 10) {
			for(i=primo; i<ultimo; i++) {
				aus1 = (short) vettore[i];
				aus2 = (short) vettore[i+PASSO_TRANS];
				somma = aus1 + aus2;
				if(somma > th) {
					if((aus1 - aus2) << 7 > soglia_trans * somma)
						return i + PASSO_TRANS;
				}
			}
			soglia_trans -= 10;
		}
	} else if(segno == 1) {
		while(soglia_trans > 10) {
			for(i=primo; i<ultimo; i++) {
				aus2 = (short) vettore[i];
				aus1 = (short) vettore[i+PASSO_TRANS];
				somma = aus1 + aus2;
				if(somma > th) {
					if((aus1 - aus2) << 7 > soglia_trans * somma)
						return i + PASSO_TRANS;
				}
			}
			soglia_trans -= 10;
		}
	} else {
		while(soglia_trans > 10) {
			for(i=primo; i<ultimo; i++) {
				aus2 = (short) vettore[i];
				aus1 = (short) vettore[i+PASSO_TRANS];
				somma = aus1 + aus2;
				if(somma > th) {
					if((aus1 - aus2) << 7 > soglia_trans * somma)
						return i + PASSO_TRANS;
					else if((aus2 - aus1) << 7 > soglia_trans * somma)
						return i + PASSO_TRANS;
				}
			}
			soglia_trans -= 10;
		}
	}
	return SEAV_ERR_NOT_FOUND;
}

/****************************************************************************/

/* cerca l'ultima transizione nel vettore vettore;
   se la trova ne ritorna l'indice, altrimenti ritorna il codice di errore */

short
imut_cerca_ultima_transizione(
	unsigned char *vettore,
	short primo,
	short ultimo,
	short th,
	short segno  /* 1: da scuro a chiaro
				   -1: da chiaro a scuro
				    0: entrambi */
)
{
	register short i;
	short soglia_trans = (short) (th >> 2);
	short aus1, aus2, somma;

	if(segno == 1) {
		while(soglia_trans > 10) {
			for(i=ultimo; i>primo; i--) {
				aus1 = (short) vettore[i+PASSO_TRANS];
				aus2 = (short) vettore[i];
				somma = aus1 + aus2;
				if(somma > th) {
					if((aus1 - aus2) << 7 > soglia_trans * somma)
						return i;
				}
			}
			soglia_trans -= 10;
		}
	} else if(segno == -1) {
		while(soglia_trans > 10) {
			for(i=ultimo; i>primo; i--) {
				aus2 = (short) vettore[i+PASSO_TRANS];
				aus1 = (short) vettore[i];
				somma = aus1 + aus2;
				if(somma > th) {
					if((aus1 - aus2) << 7 > soglia_trans * somma)
						return i;
				}
			}
			soglia_trans -= 10;
		}
	} else {
		while(soglia_trans > 10) {
			for(i=ultimo; i>primo; i--) {
				aus2 = (short) vettore[i+PASSO_TRANS];
				aus1 = (short) vettore[i];
				somma = aus1 + aus2;
				if(somma > th) {
					if((aus1 - aus2) << 7 > soglia_trans * somma)
						return i;
					else if((aus2 - aus1) << 7 > soglia_trans * somma)
						return i;
				}
			}
			soglia_trans -= 10;
		}
	}
	return SEAV_ERR_NOT_FOUND;

}

/****************************************************************************/

/* calcola la correzione (in pixel) da apportare alla transizione indice;
   ritorna SEAV_OK_ALL oppure il codice di errore */

short
imut_calcola_sub_pixel(
	unsigned char *vettore,
	short n_pixel,      /* in vettore */
	short indice,       /* della transizione */
	short *correzione,  /* in 2^N_BIT_PER_SUB_PIX-esimi di pixel */
	short n_bit_sub_pixel
)
{
	short media_s = 0, media_d = 0, i, soglia, trovato;
	unsigned char *aus_s = vettore + indice - PASSO_TRANS;
	unsigned char *aus_d = vettore + indice + PASSO_TRANS;

/* controllo se ci sono pixel sufficienti per fare il calcolo */

	if(indice < N_PIXEL_MEDIA + PASSO_TRANS)
		return SEAV_ERR_PARAMETER;
	if(indice >= n_pixel - N_PIXEL_MEDIA - PASSO_TRANS)
		return SEAV_ERR_PARAMETER;

/* mi sposto a sinistra e a destra di PASSO_TRANS e, indietro e in avanti
   rispettivamente calcolo il valor medio e la soglia */

	for(i=N_PIXEL_MEDIA; i>0; i--) {
		media_s += (short) *aus_s--;
		media_d += (short) *aus_d++;
	}
	media_s >>= N_BIT_SHIFT;
	media_d >>= N_BIT_SHIFT;
	soglia = (short) ((media_s + media_d) >> 1);

/* cerco la coppia di pixel a cavallo della soglia */

	if(media_s < media_d) {  /* transizione da scuro a chiaro */
		for(
			i=(short) (indice-PASSO_TRANS), trovato=0;
			i<indice+PASSO_TRANS && !trovato;
			i++
		) {
			if((short) vettore[i] < soglia && (short) vettore[i+1] >= soglia) {
				*correzione = (short) (
					((i - indice) << n_bit_sub_pixel) +
					((soglia - vettore[i]) << n_bit_sub_pixel) /
					(vettore[i+1] - vettore[i])
				);
				trovato = 1;
			}
		}
	} else {                 /* transizione da chiaro a scuro */
		for(
			i=(short) (indice-PASSO_TRANS), trovato=0;
			i<indice+PASSO_TRANS && !trovato;
			i++
		) {
			if((short) vettore[i] >= soglia && (short) vettore[i+1] < soglia) {
				*correzione = (short) (
					((i - indice) << n_bit_sub_pixel) +
					((soglia - vettore[i]) << n_bit_sub_pixel) /
					(vettore[i+1] - vettore[i])
				);
				trovato = 1;
			}
		}
	}
	if(!trovato)
		return SEAV_ERR_NOT_FOUND;

	return SEAV_OK_ALL;
}

/****************************************************************************/

/* ritorna l'ordinata alla quale cominciare le misure oppure il codice di errore */

short
imut_individua_primo_diametro(
	ROI *roi,
	short direzione,           /*  0: orizzontale, 1: verticale */
	short flag_interno_scuro,  /*  0: il diametro e' piu' chiaro dello sfondo
	                               1: il diametro e' piu' scuro dello sfondo */
	short soglia,          /* soglia per l'individuazione delle transizioni */
	short passo,           /* di sottocampionamento */
	short n_bit_sub_pixel, /* logaritmo del numero di tick per pixel */
	short *diametro,       /* valore misurato in tick */
	long *centro,          /* centro del diametro in tick */
	short *ind_i,          /* servono per il diametro successivo */
	short *ind_f,
	short flag_inizio      /* 0: dal basso/destra, 1: dall'alto/sinistra */
)
{
	short xi = roi->xi, xf = roi->xf, yi = roi->yi, yf = roi->yf, trovato;
	short coord, c_ini, primo, ultimo, n_pixel, c_min, c_max;
	short indice_i, indice_f, correzione, coord_ini, coord_fin, incr;
	short segno_prima_trans, segno_seconda_trans;
	short n_tentativi_fatti, n_tentativi_da_fare;

	if(flag_interno_scuro) {
		segno_prima_trans = -1;
		segno_seconda_trans = 1;
	} else {
		segno_prima_trans = 1;
		segno_seconda_trans = -1;
	}

	if(direzione) {  /* verticale */
		c_ini = yi;
		c_min = yi;
		c_max = yf - N_PIXEL_TRANS + 1;
		n_pixel = (short) (yf - yi + 1);
		if(flag_inizio) {  /* dall'alto/sinistra */
			coord_ini = xi;
			coord_fin = xf - (xf - xi) % passo;
			incr = passo;
			if(coord_ini >= coord_fin)
				return SEAV_ERR_PARAMETER;
		} else {         /* dal basso/destra */
			coord_ini = xf;
			coord_fin = xi + (xf - xi) % passo;
			incr = -passo;
			if(coord_ini <= coord_fin)
				return SEAV_ERR_PARAMETER;
		}
		n_tentativi_da_fare = ((xf - xi) / passo) >> 3;
		/* un ottavo delle colonne da usare nella finestra */
	} else {         /* orizzontale */
		c_ini = xi;
		c_min = xi;
		c_max = xf - N_PIXEL_TRANS + 1;
		n_pixel = (short) (xf - xi + 1);
		if(flag_inizio) {  /* dall'alto/sinistra */
			coord_ini = yi;
			coord_fin = yf - (yf - yi) % passo;
			incr = passo;
			if(coord_ini >= coord_fin)
				return SEAV_ERR_PARAMETER;
		} else {         /* dal basso/destra */
			coord_ini = yf;
			coord_fin = yi + (yf - yi) % passo;
			incr = -passo;
			if(coord_ini <= coord_fin)
				return SEAV_ERR_PARAMETER;
		}
		n_tentativi_da_fare = ((yf - yi) / passo) >> 3;
		/* un ottavo delle righe da usare nella finestra */
	}

	for(coord=coord_ini, trovato=0, n_tentativi_fatti=0;
		!trovato && coord!=coord_fin && n_tentativi_fatti<=n_tentativi_da_fare;
		coord+=incr, n_tentativi_fatti++) {
		if(direzione)
			sea2_leggi_colonna(coord, c_ini, n_pixel, vet_pixel);
		else
			sea2_leggi_riga(c_ini, coord, n_pixel, vet_pixel);
		primo = 0;
		ultimo = (short) ((n_pixel >> 1) - PASSO_TRANS);
		if((indice_i = imut_cerca_prima_transizione(
			vet_pixel, primo, ultimo, soglia, segno_prima_trans
		)) >= 0) {
			indice_i += (c_ini /*+ PASSO_TRANS*/);

/* se posso calcolo il sub-pixel */

			if(imut_calcola_sub_pixel(
				vet_pixel, n_pixel, (short) (indice_i - c_ini), &correzione,
				n_bit_sub_pixel
			) == SEAV_OK_ALL)
				indice_i = (short) ((indice_i << n_bit_sub_pixel) + correzione);
			else
				indice_i <<= n_bit_sub_pixel;

			primo = (short) (n_pixel >> 1);
			ultimo = (short) (n_pixel - 1 - PASSO_TRANS);
			if((indice_f = imut_cerca_ultima_transizione(
				vet_pixel, primo, ultimo, soglia, segno_seconda_trans
			)) >= 0) {
				indice_f += c_ini;

/* se posso calcolo il sub-pixel */

				if(imut_calcola_sub_pixel(
					vet_pixel, n_pixel, (short) (indice_f - c_ini), &correzione,
					n_bit_sub_pixel
				) == SEAV_OK_ALL)
					indice_f = (short) (
						(indice_f << n_bit_sub_pixel) + correzione
					);
				else
					indice_f <<= n_bit_sub_pixel;
				*diametro = (short) (indice_f - indice_i + 1);
				*centro = (long) (indice_f + indice_i) >> 1;
				*ind_i = (short) (
					(indice_i >> n_bit_sub_pixel) - N_PIXEL_TRANS2
				);
				if(*ind_i < c_min)
					*ind_i = c_min;
				else if(*ind_i > c_max)
					*ind_i = c_max;
				*ind_f = (short) (
					(indice_f >> n_bit_sub_pixel) - N_PIXEL_TRANS2
				);
				if(*ind_f < c_min)
					*ind_f = c_min;
				else if(*ind_f > c_max)
					*ind_f = c_max;
				trovato = 1;
#if 0
if(direzione) {
	scrivi_at(coord, indice_i >> n_bit_sub_pixel);
	scrivi_at(coord, indice_f >> n_bit_sub_pixel);
} else {
	scrivi_at(indice_i >> n_bit_sub_pixel, coord);
	scrivi_at(indice_f >> n_bit_sub_pixel, coord);
}
#endif
			}
		}
	}
	if(!trovato)
		return SEAV_ERR_NOT_FOUND;

	return coord;
}

/****************************************************************************/

/* ritorna SEAV_OK_ALL oppure il codice di errore */

short
imut_individua_diametro_seguente(
	short coord,               /*  coordinata a cui leggere */
	ROI *roi,                  /*  limite di ricerca */
	short direzione,           /*  0: orizzontale, 1: verticale */
	short flag_interno_scuro,  /*  0: il diametro e' piu' chiaro dello sfondo
	                               1: il diametro e' piu' scuro dello sfondo */
	short soglia,          /* soglia per l'individuazione delle transizioni */
	short n_bit_sub_pixel, /* logaritmo del numero di tick per pixel */
	short *diametro,       /* valore misurato in tick */
	long *centro,          /* centro del diametro in tick */
	short *ind_i,          /* servono per il diametro successivo */
	short *ind_f
)
{
	short c_ini = *ind_i, c_fin = *ind_f, primo, ultimo, indice_i, indice_f;
	short n_pixel = N_PIXEL_TRANS, n_pixel2 = N_PIXEL_TRANS2;
	short correzione, segno_prima_trans, segno_seconda_trans;
	short c_min, c_max;

	if(flag_interno_scuro) {
		segno_prima_trans = -1;
		segno_seconda_trans = 1;
	} else {
		segno_prima_trans = 1;
		segno_seconda_trans = -1;
	}

/* leggo una riga/colonna a sinistra/alto centrata sulla posizione precedente
   e calcolo la prima transizione */

	if(direzione) {
		sea2_leggi_colonna(coord, c_ini, n_pixel, vet_pixel);
		c_min = roi->yi;
		c_max = roi->yf - n_pixel + 1;
	} else {
		sea2_leggi_riga(c_ini, coord, n_pixel, vet_pixel);
		c_min = roi->xi;
		c_max = roi->xf - n_pixel + 1;
	}
	primo = 0;
	ultimo = (short) (n_pixel - PASSO_TRANS - 1);
	if((indice_i = imut_cerca_prima_transizione(
		vet_pixel, primo, ultimo, soglia, segno_prima_trans
	)) >= 0) {
		indice_i += (short) (c_ini /*+ PASSO_TRANS*/);

/* se posso calcolo il sub-pixel */

		if(imut_calcola_sub_pixel(
			vet_pixel, n_pixel, (short) (indice_i - c_ini), &correzione, n_bit_sub_pixel
		) == SEAV_OK_ALL)
			indice_i = (short) ((indice_i << n_bit_sub_pixel) + correzione);
		else
			indice_i <<= n_bit_sub_pixel;  
		*ind_i = (short) ((indice_i >> n_bit_sub_pixel) - n_pixel2);
		if(*ind_i < c_min)
			*ind_i = c_min;
		else if(*ind_i > c_max)
			*ind_i = c_max;

/* leggo una riga/colonna a destra/basso centrata sulla posizione precedente
   e calcolo la prima transizione */

		if(direzione)
			sea2_leggi_colonna(coord, c_fin, n_pixel, vet_pixel);
		else
			sea2_leggi_riga(c_fin, coord, n_pixel, vet_pixel);
		primo = 0;
		ultimo = (short) (n_pixel - PASSO_TRANS - 1);
		if((indice_f = imut_cerca_ultima_transizione(
			vet_pixel, primo, ultimo, soglia, segno_seconda_trans
		)) >= 0) {
			indice_f += (short) c_fin;

/* se posso calcolo il sub-pixel */

			if(imut_calcola_sub_pixel(
				vet_pixel, n_pixel, (short) (indice_f - c_fin), &correzione,
				n_bit_sub_pixel
			) == SEAV_OK_ALL)
				indice_f = (short) (
					(indice_f << n_bit_sub_pixel) + correzione
				);
			else
				indice_f <<= n_bit_sub_pixel;
			*ind_f = (short) ((indice_f >> n_bit_sub_pixel) - n_pixel2);
			if(*ind_f < c_min)
				*ind_f = c_min;
			else if(*ind_f > c_max)
				*ind_f = c_max;

			*diametro = (short) (indice_f - indice_i + 1);
			*centro = (long) (indice_f + indice_i) >> 1;
#if 0
if(direzione) {
	scrivi_at(coord, indice_i >> n_bit_sub_pixel);
	scrivi_at(coord, indice_f >> n_bit_sub_pixel);
} else {
	scrivi_at(indice_i >> n_bit_sub_pixel, coord);
	scrivi_at(indice_f >> n_bit_sub_pixel, coord);
}
#endif
			return SEAV_OK_ALL;
		}
	}
	return SEAV_ERR_NOT_FOUND;
}

/****************************************************************************/

#if 0
static void
scrivi_at(
	short x,
	short y
)
{
#if 0
	short i, j, xp, yp;
//	sea2_seleziona_memoria_lavoro(0);
	for(i=0; i<=0; i++) {
		for(j=-1; j<=1; j++) {
			xp = x + j;
			yp = y + i;
			if(xp >= 0 && xp <= 767 && yp >= 0 && yp <= 511)
				sea2_scrivi_pixel(xp, yp, 255);
		}
	}
//	sea2_seleziona_memoria_lavoro(1);
#else
	sea2_scrivi_pixel(x, y, 255);
#endif
}
#endif


/***********************************************************************

;  Nome:    
     imut_estrai_bordi
;
;  Funzione:
     rileva i bordi di un oggetto
;
;  Formato:
     short imut_estrai_bordi(roi, direzione, flag_interno_scuro, soglia,
	                         passo, n_bit_sub_pixel, bordi1, n_bordi1,
							 bordi2, n_bordi2, flag_inizio)
;
;  Argomenti:    
     ROI *roi                     area di interesse
	 short direzione              0: orizzontale 1: verticale
	 short flag_interno_scuro     0: l'oggetto e' piu' chiaro dello sfondo
	                              1: l'oggetto e' piu' scuro dello sfondo
	 short soglia                 soglia per l'individuazione delle transizioni
	 short passo                  di sottocampionamento
	 short n_bit_sub_pixel        logaritmo del numero di tick per pixel
	 long *bordi1                 vettore del primo bordo (alto o sx) in tick
	 short n_bordi1               numero di elementi trovati
	 long *bordi2                 vettore del secondo bordo (basso o dx) in tick
	 short n_bordi2               numero di elementi trovati
	 short flag_inizio            direzione di ricerca 0: dal basso/destra
	                                                   1: dall'alto/sinistra

;
;  Valore di ritorno: 
     SEAV_OK_ALL              nessun errore
	 SEAV_ERR_*               codice di errore

;
;  Descrizione:
     {\bf imut_estrai_bordi}() individua i due vettori del primo e del
	 secondo bordo dell'oggetto usando la stessa tecnica di
	 {\bf imut_misura_diametro}(); i valori dei bordi sono restituiti
	 in tick e sono contigui; infatti la ricerca di un bordo si arresta
	 se anche un solo bordo non viene trovato prima di arrivare alla fine
	 della zona di interesse. In questa versione i bordi di sx sono
	 tanti quanti quelli di dx e appartenenti alle stesse coordinate;

;
;**********************************************************************/

short
imut_estrai_bordi(
	ROI *roi,
	short direzione,           /*  0: orizzontale, 1: verticale */
	short flag_interno_scuro,  /*  0: l'oggetto e' piu' chiaro dello sfondo
	                               1: l'oggetto e' piu' scuro dello sfondo */
	short soglia,          /* soglia per l'individuazione delle transizioni */
	short passo,           /* di sottocampionamento */
	short n_bit_sub_pixel, /* logaritmo del numero di tick per pixel */
	long *bordi1,          /* vettore del primo bordo (alto o sx) in tick */
	short *n_bordi1,       /* numero di elementi trovati */
	long *bordi2,          /* vettore del secondo bordo (basso o dx) in tick */
	short *n_bordi2,       /* numero di elementi trovati */
	short flag_inizio      /* 0: dal basso/destra, 1: dall'alto/sinistra */
)
{
	short coord, coord_ini, coord_fin, indice_i, indice_f, incr;
	short fb = roi->fb, xi = roi->xi, yi = roi->yi, xf = roi->xf, yf = roi->yf;

/* seleziono la coordinata finale in dipendenza dalla direzione */

	if(direzione) {  /* verticale */
		if(flag_inizio) {  /* dall'alto/sinistra */
			coord_ini = xi;
			coord_fin = xf - (xf - xi) % passo;
			incr = passo;
			if(coord_ini >= coord_fin)
				return SEAV_ERR_PARAMETER;
		} else {         /* dal basso/destra */
			coord_ini = xf;
			coord_fin = xi + (xf - xi) % passo;
			incr = -passo;
			if(coord_ini <= coord_fin)
				return SEAV_ERR_PARAMETER;
		}
	} else {         /* orizzontale */
		if(flag_inizio) {  /* dall'alto/sinistra */
			coord_ini = yi;
			coord_fin = yf - (yf - yi) % passo;
			incr = passo;
			if(coord_ini >= coord_fin)
				return SEAV_ERR_PARAMETER;
		} else {         /* dal basso/destra */
			coord_ini = yf;
			coord_fin = yi + (yf - yi) % passo;
			incr = -passo;
			if(coord_ini <= coord_fin)
				return SEAV_ERR_PARAMETER;
		}
	}

/* cerco di individuare il primo bordo sia da una parte che dall'altra */

	sea2_seleziona_buffer(fb);
	if((coord = imut_individua_primi_bordi(
		roi, direzione, flag_interno_scuro, soglia, passo, n_bit_sub_pixel,
		bordi1, bordi2, &indice_i, &indice_f, flag_inizio
	)) < 0)
		return coord;
	*n_bordi1 = *n_bordi2 = 1;

/* a partire dalla posizione corrente misuro tutti i bordi che posso e
   li memorizzo; la lettura dalla scheda e' ottimizzata: ci si posiziona
   nel punto precedentemente trovato */

	if(coord != coord_fin) {
		for(coord+=incr; coord!=coord_fin; coord+=incr) {
			if(imut_individua_bordi_seguenti(
				coord, roi, direzione, flag_interno_scuro, soglia,
				n_bit_sub_pixel, bordi1+*n_bordi1, bordi2+*n_bordi2,
				&indice_i, &indice_f
			) == SEAV_ERR_NOT_FOUND) {
				return SEAV_OK_ALL;
			} else {
				*n_bordi1 += 1;
				*n_bordi2 += 1;
			}
		}
	}

	return SEAV_OK_ALL;
}

/****************************************************************************/

/* ritorna l'ordinata alla quale cominciare le misure oppure il codice di errore */

short
imut_individua_primi_bordi(
	ROI *roi,
	short direzione,           /*  0: orizzontale, 1: verticale */
	short flag_interno_scuro,  /*  0: l'oggetto e' piu' chiaro dello sfondo
	                               1: l'oggetto e' piu' scuro dello sfondo */
	short soglia,          /* soglia per l'individuazione delle transizioni */
	short passo,           /* di sottocampionamento */
	short n_bit_sub_pixel, /* logaritmo del numero di tick per pixel */
	long *bordi1,          /* vettore del primo bordo (alto o sx) in tick */
	long *bordi2,          /* vettore del secondo bordo (basso o dx) in tick */
	short *ind_i,          /* servono per il diametro successivo */
	short *ind_f,
	short flag_inizio      /* 0: dal basso/destra, 1: dall'alto/sinistra */
)
{
	short xi = roi->xi, xf = roi->xf, yi = roi->yi, yf = roi->yf, trovato;
	short coord, c_ini, primo, ultimo, n_pixel, c_min, c_max;
	short indice_i, indice_f, correzione, coord_ini, coord_fin, incr;
	short segno_prima_trans, segno_seconda_trans;
	short n_tentativi_fatti, n_tentativi_da_fare;

	if(flag_interno_scuro) {
		segno_prima_trans = -1;
		segno_seconda_trans = 1;
	} else {
		segno_prima_trans = 1;
		segno_seconda_trans = -1;
	}

	if(direzione) {  /* il diametro e' verticale */
		c_ini = yi;
		c_min = yi;
		c_max = yf - N_PIXEL_TRANS + 1;
		n_pixel = (short) (yf - yi + 1);
		if(flag_inizio) {  /* dall'alto/sinistra */
			coord_ini = xi;
			coord_fin = xf - (xf - xi) % passo;
			incr = passo;
			if(coord_ini >= coord_fin)
				return SEAV_ERR_PARAMETER;
		} else {         /* dal basso/destra */
			coord_ini = xf;
			coord_fin = xi + (xf - xi) % passo;
			incr = -passo;
			if(coord_ini <= coord_fin)
				return SEAV_ERR_PARAMETER;
		}
		n_tentativi_da_fare = ((xf - xi) / passo) >> 3;
		/* un ottavo delle colonne da usare nella finestra */
	} else {         /* orizzontale */
		c_ini = xi;
		c_min = xi;
		c_max = xf - N_PIXEL_TRANS + 1;
		n_pixel = (short) (xf - xi + 1);
		if(flag_inizio) {  /* dall'alto/sinistra */
			coord_ini = yi;
			coord_fin = yf - (yf - yi) % passo;
			incr = passo;
			if(coord_ini >= coord_fin)
				return SEAV_ERR_PARAMETER;
		} else {         /* dal basso/destra */
			coord_ini = yf;
			coord_fin = yi + (yf - yi) % passo;
			incr = -passo;
			if(coord_ini <= coord_fin)
				return SEAV_ERR_PARAMETER;
		}
		n_tentativi_da_fare = ((yf - yi) / passo) >> 3;
		/* un ottavo delle righe da usare nella finestra */
	}

	for(coord=coord_ini, trovato=0, n_tentativi_fatti=0;
		!trovato && coord!=coord_fin && n_tentativi_fatti<=n_tentativi_da_fare;
		coord+=incr, n_tentativi_fatti++) {
		if(direzione)
			sea2_leggi_colonna(coord, c_ini, n_pixel, vet_pixel);
		else
			sea2_leggi_riga(c_ini, coord, n_pixel, vet_pixel);
		primo = 0;
		ultimo = (short) ((n_pixel >> 1) - PASSO_TRANS);
		if((indice_i = imut_cerca_prima_transizione(
			vet_pixel, primo, ultimo, soglia, segno_prima_trans
		)) >= 0) {
			indice_i += (c_ini /*+ PASSO_TRANS*/);

/* se posso calcolo il sub-pixel */

			if(imut_calcola_sub_pixel(
				vet_pixel, n_pixel, (short) (indice_i - c_ini), &correzione,
				n_bit_sub_pixel
			) == SEAV_OK_ALL)
				indice_i = (short) ((indice_i << n_bit_sub_pixel) + correzione);
			else
				indice_i <<= n_bit_sub_pixel;

			primo = (short) (n_pixel >> 1);
			ultimo = (short) (n_pixel - 1 - PASSO_TRANS);
			if((indice_f = imut_cerca_ultima_transizione(
				vet_pixel, primo, ultimo, soglia, segno_seconda_trans
			)) >= 0) {
				indice_f += c_ini;

/* se posso calcolo il sub-pixel */

				if(imut_calcola_sub_pixel(
					vet_pixel, n_pixel, (short) (indice_f - c_ini), &correzione,
					n_bit_sub_pixel
				) == SEAV_OK_ALL)
					indice_f = (short) (
						(indice_f << n_bit_sub_pixel) + correzione
					);
				else
					indice_f <<= n_bit_sub_pixel;
				*bordi1 = (long) indice_i;
				*bordi2 = (long) indice_f;
				*ind_i = (short) (
					(indice_i >> n_bit_sub_pixel) - N_PIXEL_TRANS2
				);
				if(*ind_i < c_min)
					*ind_i = c_min;
				else if(*ind_i > c_max)
					*ind_i = c_max;
				*ind_f = (short) (
					(indice_f >> n_bit_sub_pixel) - N_PIXEL_TRANS2
				);
				if(*ind_f < c_min)
					*ind_f = c_min;
				else if(*ind_f > c_max)
					*ind_f = c_max;
				trovato = 1;

#if 0
if(direzione) {
	scrivi_at(coord, indice_i >> n_bit_sub_pixel);
	scrivi_at(coord, indice_f >> n_bit_sub_pixel);
} else {
	scrivi_at(indice_i >> n_bit_sub_pixel, coord);
	scrivi_at(indice_f >> n_bit_sub_pixel, coord);
}
#endif
			}
		}
	}
	if(!trovato)
		return SEAV_ERR_NOT_FOUND;

	return coord;
}

/****************************************************************************/

/* ritorna SEAV_OK_ALL oppure il codice di errore */

short
imut_individua_bordi_seguenti(
	short coord,               /*  coordinata a cui leggere */
	ROI *roi,                  /*  limite di ricerca */
	short direzione,           /*  0: orizzontale, 1: verticale */
	short flag_interno_scuro,  /*  0: il diametro e' piu' chiaro dello sfondo
	                               1: il diametro e' piu' scuro dello sfondo */
	short soglia,          /* soglia per l'individuazione delle transizioni */
	short n_bit_sub_pixel, /* logaritmo del numero di tick per pixel */
	long *bordi1,          /* valore misurato in tick */
	long *bordi2,          /* valore misurato in tick */
	short *ind_i,          /* servono per il diametro successivo */
	short *ind_f
)
{
	short c_ini = *ind_i, c_fin = *ind_f, primo, ultimo, indice_i, indice_f;
	short n_pixel = N_PIXEL_TRANS, n_pixel2 = N_PIXEL_TRANS2;
	short correzione, segno_prima_trans, segno_seconda_trans;
	short c_min, c_max;

	if(flag_interno_scuro) {
		segno_prima_trans = -1;
		segno_seconda_trans = 1;
	} else {
		segno_prima_trans = 1;
		segno_seconda_trans = -1;
	}

/* leggo una riga/colonna a sinistra/alto centrata sulla posizione precedente
   e calcolo la prima transizione */

	if(direzione) {
		sea2_leggi_colonna(coord, c_ini, n_pixel, vet_pixel);
		c_min = roi->yi;
		c_max = roi->yf - n_pixel + 1;
	} else {
		sea2_leggi_riga(c_ini, coord, n_pixel, vet_pixel);
		c_min = roi->xi;
		c_max = roi->xf - n_pixel + 1;
	}
	primo = 0;
	ultimo = (short) (n_pixel - PASSO_TRANS - 1);
	if((indice_i = imut_cerca_prima_transizione(
		vet_pixel, primo, ultimo, soglia, segno_prima_trans
	)) >= 0) {
		indice_i += (short) (c_ini /*+ PASSO_TRANS*/);

/* se posso calcolo il sub-pixel */

		if(imut_calcola_sub_pixel(
			vet_pixel, n_pixel, (short) (indice_i - c_ini), &correzione, n_bit_sub_pixel
		) == SEAV_OK_ALL)
			indice_i = (short) ((indice_i << n_bit_sub_pixel) + correzione);
		else
			indice_i <<= n_bit_sub_pixel;  
		*ind_i = (short) ((indice_i >> n_bit_sub_pixel) - n_pixel2);
		if(*ind_i < c_min)
			*ind_i = c_min;
		else if(*ind_i > c_max)
			*ind_i = c_max;

/* leggo una riga/colonna a destra/basso centrata sulla posizione precedente
   e calcolo la prima transizione */

		if(direzione)
			sea2_leggi_colonna(coord, c_fin, n_pixel, vet_pixel);
		else
			sea2_leggi_riga(c_fin, coord, n_pixel, vet_pixel);
		primo = 0;
		ultimo = (short) (n_pixel - PASSO_TRANS - 1);
		if((indice_f = imut_cerca_ultima_transizione(
			vet_pixel, primo, ultimo, soglia, segno_seconda_trans
		)) >= 0) {
			indice_f += (short) c_fin;

/* se posso calcolo il sub-pixel */

			if(imut_calcola_sub_pixel(
				vet_pixel, n_pixel, (short) (indice_f - c_fin), &correzione,
				n_bit_sub_pixel
			) == SEAV_OK_ALL)
				indice_f = (short) (
					(indice_f << n_bit_sub_pixel) + correzione
				);
			else
				indice_f <<= n_bit_sub_pixel;
			*ind_f = (short) ((indice_f >> n_bit_sub_pixel) - n_pixel2);
			if(*ind_f < c_min)
				*ind_f = c_min;
			else if(*ind_f > c_max)
				*ind_f = c_max;

			*bordi1 = (long) indice_i;
			*bordi2 = (long) indice_f;

#if 0
if(direzione) {
	scrivi_at(coord, indice_i >> n_bit_sub_pixel);
	scrivi_at(coord, indice_f >> n_bit_sub_pixel);
} else {
	scrivi_at(indice_i >> n_bit_sub_pixel, coord);
	scrivi_at(indice_f >> n_bit_sub_pixel, coord);
}
#endif
			return SEAV_OK_ALL;
		}
	}
	return SEAV_ERR_NOT_FOUND;
}


