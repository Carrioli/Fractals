

/***********************************************************************

;  Nome:    
     imut_calibrazione
;
;  Funzione:
     calcola i coefficienti di calibrazione
;
;  Formato:
     short imut_calibrazione(vet_roi, vet_valori_nominali, n_roi, direzione,
	                         flag_interno_scuro, soglia, passo, n_bit_sub_pixel,
							 coeff, offset)
;
;  Argomenti:    
     ROI *vet_roi                  vettore delle aree di interesse
	 double *vet_valori_nominali   vettore dei valori dei diametri nominali
     short n_roi                   numero di diametri da misurare
	 short direzione               0: orizzontale 1: verticale
	 short flag_interno_scuro      0: il diametro e' piu' chiaro dello sfondo
	                               1: il diametro e' piu' scuro dello sfondo
	 short soglia                  soglia per l'individuazione delle transizioni
	 short passo                   di sottocampionamento
	 short n_bit_sub_pixel         logaritmo del numero di tick per pixel
	 double *coeff                 fattore di scala misurato
	 double *offset                offset ottico misurato

;
;  Valore di ritorno: 
     SEAV_OK_ALL              nessun errore
	 SEAV_ERR_*               codice di errore

;
;  Descrizione:
     {\bf imut_calibrazione}() calcola il fattore di scala e l'offset ottico
	 che permettono di trasformare le misure da pixel a unita' di misura
	 fisiche; la procedura e' la seguente \\
	 1) per ogni area di interesse si misura il diametro in tick \\
	 2) tramite il metodo di Gram-Schmidt modificato si risolve il sistema
	    di n_roi (possibilmente n_roi > 2) equazioni in due incognite
		(coeff, offset) \\
		diametro_misurato[i] * coeff + offset = diametro_nominale[i] \\
     la soluzione ottenuta e' ottima nel senso dei minimi quadrati;

;
;**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <imutilnt.h>

short
imut_calibrazione(
	ROI *vet_roi,
	double *vet_valori_nominali,
	short n_roi,
	short direzione,
	short flag_interno_scuro,
	short soglia,
	short passo,
	short n_bit_sub_pixel,
	double *coeff,
	double *offset
)
{
	short i, flag_max = 1, n_valori_misurati, ret;
	double *vet_valori_misurati = NULL, **A=NULL, *B=NULL, *X=NULL;
	short *flag_ok = NULL;

/* allocazione di memoria */

	vet_valori_misurati = (double *) malloc((size_t) n_roi * sizeof(double));
	flag_ok = (short *) malloc((size_t) n_roi * sizeof(short));
	if(vet_valori_misurati == NULL || flag_ok == NULL) {
		free(vet_valori_misurati);
		free(flag_ok);
		return SEAV_ERR_MEMORY;
	}
	
/* misura dei diametri */

	for(i=0, n_valori_misurati=0; i<n_roi; i++) {
		if(imut_misura_diametro(
			vet_roi + i, direzione, flag_interno_scuro, soglia, passo,
			n_bit_sub_pixel, flag_max, 0.0, 0.0, vet_valori_misurati + i,
			NULL
		) == SEAV_OK_ALL) {
			flag_ok[i] = 1;
			n_valori_misurati ++;
		} else {
			flag_ok[i] = 0;
		}
	}
	if(n_valori_misurati < 2) {
		free(vet_valori_misurati);
		free(flag_ok);
		return SEAV_ERR_NOT_FOUND;
	}

/* inizializzazione degli array */

	A = (double **) util_array2(
		(long) n_valori_misurati, 2L, (long) sizeof(double)
	);
	X = (double *) malloc((size_t) (2L * sizeof(double)));
	B = (double *) malloc((size_t) ((long) n_valori_misurati * sizeof(double)));
	if(A == NULL || X == NULL || B == NULL) {
		free(vet_valori_misurati);
		free(flag_ok);
		util_free2((char **) A);
		free(X);
		free(B);
		return SEAV_ERR_MEMORY;
	}
	for(i=0, n_valori_misurati=0; i<n_roi; i++) {
		if(flag_ok[i]) {
			A[n_valori_misurati][0] = vet_valori_misurati[i];
			A[n_valori_misurati][1] = 1.0;
			B[n_valori_misurati++] = vet_valori_nominali[i];
		}
	}

/* minimi quadrati: metodo di Gram-Schmidt modificato */

	if((ret = util_gram_schmidt(A, n_valori_misurati, 2, B, X)) == SEAV_OK_ALL) {
		*coeff = X[0];
		*offset = X[1];
	}

/* uscita */

	free(vet_valori_misurati);
	free(flag_ok);
	util_free2((char **) A);
	free(X);
	free(B);

	return ret;
}

