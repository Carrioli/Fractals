



/***********************************************************************

;  Nome:    
     util_cross_corr
;
;  Funzione:
     calcola la funzione di cross-correlazione tra due vettori
;
;  Formato:
     short util_cross_corr(vet1, vet2, cross_corr, lung1, lung2, max, tipo)
;
;  Argomenti:    
     long *vet1            vettore d'ingresso
     long *vet2            vettore d'ingresso
     long *cross_corr      vettore contenente la correlazione calcolata
	                       dalla routine
     short lung1           lunghezza di "vet1"
     short lung2           lunghezza di "vet2"
     long max              valore che assume il massimo della correlazione dopo
                           la normalizzazione
     short tipo            tipo di formula usata
	                       0: somma dei valori assoluti delle differenze
	                       1: somma dei prodotti

;
;  Valore di ritorno: 
     $n$             numero di punti utili di cross_corr
	 MEMORY_ERROR          mancata allocazione di memoria
	 PARAM_ERROR           parametri fuori range

;
;  Descrizione:
     {\bf util_cross_corr}() calcola la funzione di cross-correlazione tra due
	 vettori ponendo il risultato in un terzo vettore; la cross-correlazione
	 e' calcolata considerando i vettori non circolari; pertanto il numero
	 di punti utili di "cross_corr" dipende dai valori di "lung1" e "lung2";
	 il parametro "tipo" seleziona la formula per il calcolo della cross-
	 correlazione; l'algoritmo ha complessita' quadratica con il numero
	 di dati;
;
;  Note:
     il valore del parametro max deve essere inferiore a 1000 per evitare
	 overflow durante il calcolo;
	 la normalizzazione viene effettuata in modo da avere la massima
	 correlazione in corrispondenza del valore minimo di cross_corr;
;
;**********************************************************************/

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <util_nt.h>

short
util_cross_corr(
	long *vet1,
	long *vet2,
	long *cross_corr,
	short lung1,
	short lung2,
	long max,
	short tipo
)
{
	long massimo, *cross_corr1;
	long *ausp1, *ausp2, *auspc;
	short i, j;
	short lung = (short) (abs(lung1 - lung2) + 1); /* numero di punti utili della corr. */
	short lung_ker = min(lung1, lung2); /* cardinalita' del loop piu' interno */

/* controllo il valore di max: non deve essere troppo grande per evitare
   l'overflow nei conti della normalizzazione */

	if(max > 1000L)
		return SEAV_ERR_PARAMETER;

/* poiche' vet2 deve essere piu' lungo di vet1 eventualmente scambio il
   loro ruolo */

	if(lung2 < lung1) {
		ausp1 = vet1;
		vet1 = vet2;
		vet2 = ausp1;
	}

/* alloco memoria */

	cross_corr1 = (long *) calloc((size_t) lung, (size_t) sizeof(long));
	if(cross_corr1 == NULL)
		return SEAV_ERR_MEMORY;

/* calcolo la cross-correlazione */

	switch(tipo) {
		case 0: /* sum(abs(f(i) - g(i+t))) */
			for(i=0, auspc=cross_corr1; i<lung; i++, auspc++) {
				ausp1 = vet1;
				ausp2 = vet2 + i;
				for(j=lung_ker-7; j>0; j-=8) {
					*auspc += labs(*ausp1++ - *ausp2++);
					*auspc += labs(*ausp1++ - *ausp2++);
					*auspc += labs(*ausp1++ - *ausp2++);
					*auspc += labs(*ausp1++ - *ausp2++);
					*auspc += labs(*ausp1++ - *ausp2++);
					*auspc += labs(*ausp1++ - *ausp2++);
					*auspc += labs(*ausp1++ - *ausp2++);
					*auspc += labs(*ausp1++ - *ausp2++);
				}
				for(; j>-7; j--) {
					*auspc += labs(*ausp1++ - *ausp2++);
				}
			}
			break;
		case 1: /* sum((f(i) * g(i+t)) */
			for(i=0, auspc=cross_corr1; i<lung; i++, auspc++) {
				ausp1 = vet1;
				ausp2 = vet2 + i;
				for(j=lung_ker-7; j>0; j-=8) {
					*auspc += *ausp1++ * *ausp2++;
					*auspc += *ausp1++ * *ausp2++;
					*auspc += *ausp1++ * *ausp2++;
					*auspc += *ausp1++ * *ausp2++;
					*auspc += *ausp1++ * *ausp2++;
					*auspc += *ausp1++ * *ausp2++;
					*auspc += *ausp1++ * *ausp2++;
					*auspc += *ausp1++ * *ausp2++;
				}
				for(; j>-7; j--) {
					*auspc += *ausp1++ * *ausp2++;
				}
			}
			break;
		default:
			free(cross_corr1);
			return SEAV_ERR_PARAMETER;
			break;
	}

/* calcolo del massimo della cross-correlazione */

	massimo = util_calcola_massimo(cross_corr1, lung, 3);

/* normalizzazione della cross-correlazione */

	util_normalizza(cross_corr1, cross_corr, lung, massimo, max, tipo);

	free(cross_corr1);
	return lung;
}


