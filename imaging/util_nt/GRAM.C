


/***********************************************************************

;  Nome:    
     util_gram_schmidt
;
;  Funzione:
     soluzione di un sistema di m equazioni e n incognite (m>=n)
;
;  Formato:
	 short util_gram_schmidt(A, n_equ, n_var, B, X)
;
;  Argomenti:
	double **A     matrice dei coefficienti (allocata con array2)
	short n_equ    numero di righe di A (equazioni)
	short n_var    numero di colonne di A (incognite)
	double *B      vettore dei termini noti di lunghezza n_equ
	double *X      vettore delle soluzioni calcolate di lunghezza n_var
;
;  Valore di ritorno:
     SEAV_OK_ALL       nessun errore
     SEAV_ERR*         codice di errore
;
;  Descrizione:
    {\bf util_gram_schmidt}() risolve il sistema lineare AX = B ottenendo
	una soluzione ottima nel senso dei minimi quadrati;
;
;**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <util_nt.h>

short
util_gram_schmidt(
	double **A,
	short n_equ,
	short n_var,
	double *B,
	double *X
)
{
	short i, j, k;
	double **R=NULL, **Q=NULL, *Y=NULL, *D=NULL, parz;

/* controllo che le equazioni siano in numero sufficiente */

	if(n_equ < n_var)
		return SEAV_ERR_PARAMETER;

/* inizializzazione dei vettori e della matrici per il metodo di Gram-Schmidt
   modificato */

	R = (double **) util_array2(
		(long) n_var, (long) n_var, (long) sizeof(double)
	);
	Q = (double **) util_array2(
		(long) n_equ, (long) n_var, (long) sizeof(double)
	);
	Y = (double *) malloc((size_t) ((long) n_var * sizeof(double)));
	D = (double *) malloc((size_t) ((long) n_var * sizeof(double)));
	if(R == NULL || Q == NULL || Y == NULL || D == NULL) {
		util_free2((char **) R);
		util_free2((char **) Q);
		free(Y);
		free(D);
		return SEAV_ERR_MEMORY;
	}

/* calcolo di Gram-Schmidt */

/* copio la matrice A in Q */

	for(i=0; i<n_equ; i++)
		for(j=0; j<n_var; j++)
			Q[i][j] = A[i][j];

/* fattorizzazione */

	for(k=0; k<n_var; k++) {
		for(D[k]=0.0, Y[k]=0.0, i=0; i<n_equ; i++) {
			D[k] += Q[i][k] * Q[i][k];
			Y[k] += Q[i][k] * B[i];
		}
		Y[k] /= D[k];
		R[k][k] = 1.0;
		for(j=k+1; j<n_var; j++) {
			for(R[k][j]=0.0, i=0; i<n_equ; i++) {
				R[k][j] += Q[i][k] * Q[i][j];
			}
			R[k][j] /= D[k];
			for(i=0; i<n_equ; i++) {
				Q[i][j] -= Q[i][k] * R[k][j];
			}
		}
		for(i=0; i<n_equ; i++) {
			B[i] -= Q[i][k] * Y[k];
		}
	}

/* soluzione del sistema triangolare RX = Y */

	for(i=n_var-1; i>=0; i--) {
		for(parz=0.0, j=n_var-1; j>i; j--)
			parz += R[i][j] * X[j];
		X[i] = (Y[i] - parz) / R[i][i];
	}

/* uscita */

	util_free2((char **) R);
	util_free2((char **) Q);
	free(Y);
	free(D);

	return SEAV_OK_ALL;
}

