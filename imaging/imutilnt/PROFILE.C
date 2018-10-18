


/***********************************************************************

;  Nome:    
     imut_profile
;
;  Funzione:
     calcola il profilo lungo una direzione cardinale
;
;  Formato:
     short imut_profile(roi, dir, max, profilo, n_punti_utili)
;
;  Argomenti:    
     ROI *roi              puntatore alla struttura che descrive l'area di
	                       interesse
     short dir             numero identificatore della direzione
	                       0: perpendicolare all'asse X
	                       1: diagonale dall'alto a sinistra al basso a destra
	                       2: perpendicolare all'asse Y
	                       3: diagonale dall'alto a destra al basso a sinistra
     long max              valore che assume il massimo del profilo dopo
                           la normalizzazione
     long *profilo         vettore contenente il profilo calcolato dalla routine
	 short *n_punti_utili  numero di punti di cui si compone il profilo

;
;  Valore di ritorno: 
     OK_ALL                nessun errore
     SEAV_OK_ALL              nessun errore
	 SEAV_ERR_*               codice di errore

;
;  Descrizione:
     {\bf imut_profile}() calcola il profilo di una area di interesse lungo una
	 delle quattro direzioni cardinali; il valore del profilo e' normalizzato
	 in modo che il valore massimo sia "max"; il profilo e' memorizzato nel
	 vettore "profilo" a partire dal primo elemento e il numero di punti che
	 lo compongono e' restituito nella variabile "n_punti_utili";
	 se "dir" = 0 "profilo[0]" e' relativo alla colonna di sinistra della ROI;
	 se "dir" = 1 "profilo[0]" e' relativo al punto in basso a sinistra
	 della ROI;
	 se "dir" = 2 "profilo[0]" e' relativo alla riga in alto della ROI;
	 se "dir" = 3 "profilo[0]" e' relativo al punto in alto a sinistra
	 della ROI;
;
;  Note:
     il valore del parametro "max" deve essere inferiore a 1000 per evitare
	 overflow durante il calcolo;

;**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <imutilnt.h>

short
imut_profile(
	ROI *roi,
	short dir,
	long max,
	long *profilo,
	short *n_punti_utili
)
{
	short i, j;
	short fb, xi, yi, xf, yf, xl, yl;
	unsigned char *buffer;
	long massimo, *profilo1;
	PARAM_SCHEDA param;

/* controllo dei parametri */

	sea2_init_config(&param);
	if(imut_check_roi(roi, &param) != SEAV_OK_ALL)
		return SEAV_ERR_PARAMETER;
	fb = roi->fb;
	xi = roi->xi; yi = roi->yi;
	xf = roi->xf; yf = roi->yf;
	xl = xf - xi + 1;
	yl = yf - yi + 1;

/* controllo il valore di max: non deve essere troppo grande per evitare
   l'overflow nei conti della normalizzazione */

	if(max > 1000L)
		return SEAV_ERR_PARAMETER;

/* calcolo del numero di punti utili del profilo */

	switch(dir) {
		case 0:
			*n_punti_utili = xl;
			break;
		case 1:
		case 3:
			*n_punti_utili = xl + yl - 1;
			break;
		case 2:
			*n_punti_utili = yl;
			break;
	}

/* selezione del buffer */

	sea2_seleziona_buffer(fb);

/* alloco memoria */

	buffer = (unsigned char *) malloc(
		(size_t) xl * (size_t) yl * sizeof(unsigned char)
	);
	if(buffer == NULL)
		return SEAV_ERR_MEMORY;
	profilo1 = (long *) malloc((size_t) (*n_punti_utili) * sizeof(long));
	if(profilo1 == NULL) {
		free(buffer);
		return SEAV_ERR_MEMORY;
	}

/* leggo la finestra dal frame buffer */

	sea2_leggi_finestra(xi, yi, xf, yf, buffer);

/* azzeramento del vettore */

	memset(profilo1, 0, (size_t) (*n_punti_utili * sizeof(long)));

/* calcolo il profilo */

	switch(dir) {
		case 0: {
			long *aus_p;
			unsigned char *aus_b = buffer;
			for(i=yl; i>0; i--) {
				aus_p = profilo1;
				for(j=xl-7; j>0; j-=8) {
					*aus_p += *aus_b++; aus_p ++;
					*aus_p += *aus_b++; aus_p ++;
					*aus_p += *aus_b++; aus_p ++;
					*aus_p += *aus_b++; aus_p ++;
					*aus_p += *aus_b++; aus_p ++;
					*aus_p += *aus_b++; aus_p ++;
					*aus_p += *aus_b++; aus_p ++;
					*aus_p += *aus_b++; aus_p ++;
				}
				for(; j>-7; j--) {
					*aus_p += *aus_b++; aus_p ++;
				}
			}
		}
		break;
		case 1: {
			long *aus_p;
			unsigned char *aus_b = buffer;
			for(i=yl; i>0; i--) {
				aus_p = profilo1 + i - 1;
				for(j=xl-7; j>0; j-=8) {
					*aus_p += *aus_b++; aus_p ++;
					*aus_p += *aus_b++; aus_p ++;
					*aus_p += *aus_b++; aus_p ++;
					*aus_p += *aus_b++; aus_p ++;
					*aus_p += *aus_b++; aus_p ++;
					*aus_p += *aus_b++; aus_p ++;
					*aus_p += *aus_b++; aus_p ++;
					*aus_p += *aus_b++; aus_p ++;
				}
				for(; j>-7; j--) {
					*aus_p += *aus_b++; aus_p ++;
				}
			}
		}
		break;
		case 2: {
			long *aus_p = profilo1;
			unsigned char *aus_b = buffer;
			for(i=yl; i>0; i--) {
				for(j=xl-7; j>0; j-=8) {
					*aus_p += *aus_b++;
					*aus_p += *aus_b++;
					*aus_p += *aus_b++;
					*aus_p += *aus_b++;
					*aus_p += *aus_b++;
					*aus_p += *aus_b++;
					*aus_p += *aus_b++;
					*aus_p += *aus_b++;
				}
				for(; j>-7; j--) {
					*aus_p += *aus_b++;
				}
				aus_p ++;
			}
		}
		break;
		case 3: {
			long *aus_p;
			unsigned char *aus_b = buffer;
			for(i=yl; i>0; i--) {
				aus_p = profilo1 + yl - i;
				for(j=xl-7; j>0; j-=8) {
					*aus_p += *aus_b++; aus_p ++;
					*aus_p += *aus_b++; aus_p ++;
					*aus_p += *aus_b++; aus_p ++;
					*aus_p += *aus_b++; aus_p ++;
					*aus_p += *aus_b++; aus_p ++;
					*aus_p += *aus_b++; aus_p ++;
					*aus_p += *aus_b++; aus_p ++;
					*aus_p += *aus_b++; aus_p ++;
				}
				for(; j>-7; j--) {
					*aus_p += *aus_b++; aus_p ++;
				}
			}
		}
		break;
	}

/* calcolo del massimo del profilo */

	massimo = util_calcola_massimo(profilo1, *n_punti_utili, 3);

/* normalizzazione del profilo */

	util_normalizza(profilo1, profilo, *n_punti_utili, massimo, max, (short) 0);

	free(buffer);
	free(profilo1);
	return SEAV_OK_ALL;
}



