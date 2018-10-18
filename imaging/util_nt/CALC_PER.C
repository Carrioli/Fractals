



/***********************************************************************

;  Nome:    
     util_calcola_periodo
;
;  Funzione:
     calcolo del periodo di una funzione di autocorrelazione
;
;  Formato:
     long util_calcola_periodo(auto_corr, lung)

;
;  Argomenti:    
	 long *auto_corr      vettore contenente la funzione di autocorrelazione
     short lung           lunghezza di auto_corr

;
;  Valore di ritorno: 
     $n$            periodo dell'autocorrelazione
	 MEMORY_ERROR         mancata allocazione
	 PARAM_ERROR          parametri fuori range

;
;  Descrizione:
     {\bf util_calcola_periodo}() individua il periodo della funzione di
	 autocorrelazione misurando la distanza media tra due minimi adiacenti;

;
;**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <util_nt.h>

#define MAX_NUM_ESTREMI 100
#define DIST_CHECK       10  /* distanza alla quale controllare i vicini */
#define LUNG_MIN          3
#define DIST_MIN          3

long
util_calcola_periodo(
	long *auto_corr,
	short lung
)
{
	short i, i0, i1, n_estremi;
	long periodo = 0L;
	short *estremo;
	static short inizio[MAX_NUM_ESTREMI], fine[MAX_NUM_ESTREMI];
	static short lunghezza[MAX_NUM_ESTREMI];
	short lung_min = LUNG_MIN;  /* dei segmenti */
	short dist_min = DIST_MIN;  /* tra i segmenti */

/* allocazione di memoria */

	estremo = (short *) malloc((size_t) lung * sizeof(short));
	if(estremo == NULL)
		return (long) SEAV_ERR_MEMORY;

/* ricerca dei minimi locali */

	for(i=0; i<DIST_CHECK; i++)
		estremo[i] = 0;
	for(i=DIST_CHECK, i0=0, i1=2*DIST_CHECK; i1<lung; i++, i0++, i1++) {
		if(auto_corr[i] < auto_corr[i0] && auto_corr[i] < auto_corr[i1])
			estremo[i] = 255;
		else
			estremo[i] = 0;
	}
	for(; i<lung; i++)
		estremo[i] = 0;

/* segmentazione degli estremi adiacenti */

	n_estremi = util_trova_segmenti(
		estremo, lung, (short) 128, lung_min, dist_min, inizio, fine, lunghezza
	);

/* controllo di n_estremi */

	if(n_estremi < 2) {
		free(estremo);
		return (long) SEAV_ERR_NOT_FOUND;
	}

/* calcolo della media dei periodi */

	for(i=0; i<n_estremi-1; i++)
		periodo += (long) ((inizio[i+1] + fine[i+1] - inizio[i] - fine[i]) / 2);

	free(estremo);
	return periodo / (long) (n_estremi - 1);
}

