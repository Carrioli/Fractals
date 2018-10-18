

/*
	util_array2()                      pubblica
	util_free2()                       pubblica
	util_array2_dos()                  pubblica
	util_free2_dos()                   pubblica
	util_alloca_matrice()              pubblica
	util_disalloca_matrice()           pubblica
	util_alloca_array_di_matrici()     pubblica
	util_disalloca_array_di_matrici()  pubblica
*/

#include <stdio.h>
#include <stdlib.h>
#include <util_nt.h>


/***********************************************************************

;  Nome:    
     util_array2
;
;  Funzione:
     alloca memoria per una matrice
;
;  Formato:
     char **util_array2(n_righe, n_colonne, n_bytes_per_pix)
;
;  Argomenti:    
     long n_righe           numero di righe della matrice
     long n_colonne         numero di colonne della matrice
     long n_byte_per_pix    numero di byte per ogni elemento della matrice

;
;  Valore di ritorno: 
     pointer               indirizzo del vettore di puntatori
	 NULL                  mancata allocazione

;
;  Descrizione:
     {\bf util_array2}() alloca memoria per una matrice di elementi
	 di dimesione n_byte_per_pix; sono effettuate due allocazioni:
	 la prima per un vettore di "n_righe" puntatori; la seconda per
	 i dati che, pertanto, risultano fisicamente contigui;
	 il vettore di puntatori viene poi inizializzato con gli indirizzi
	 del primo dato di ogni riga;
;
;**********************************************************************/

char **
util_array2(
	long n_righe,
	long n_colonne,
	long n_bytes_per_pix
)
{
	char *data, **ptr;
	long byte_totali; /* numero totale di bytes da allocare */
	long byte_per_riga; /* numero di bytes per riga */
	long i;

/* allocazione del vettore di puntatori */

	ptr = (char **) malloc((size_t) n_righe * sizeof(char *));
	if(ptr == NULL)
		return NULL;

/* allocazione dei dati contigui */

	byte_per_riga = n_colonne * n_bytes_per_pix;
	byte_totali = n_righe * byte_per_riga;
	data = (char *) malloc((size_t) byte_totali);
	if(data == NULL) {
		free(ptr);
		return NULL;
	}

/* assegnazione dei puntatori */

	for(i=0; i<n_righe; i++)
		ptr[i] = data + i * byte_per_riga;

	return ptr;
}


/***********************************************************************

;  Nome:    
     util_free2
;
;  Funzione:
     libera la memoria allocata con util_array2
;
;  Formato:
     void util_free2(pointer)
;
;  Argomenti:    
     char **pointer       puntatore ritornato da util_array2()
;
;  Descrizione:
     {\bf util_free2}() libera la memoria allocata da 
     {\bf util_array2}();
;
;**********************************************************************/

void
util_free2(
	char **ptr
)
{
	if(ptr) {
		free(ptr[0]);
		free(ptr);
	}
	return;
}


/***********************************************************************

;  Nome:    
     util_array2_dos
;
;  Funzione:
     alloca memoria per una matrice
;
;  Formato:
     char **util_array2_dos(n_righe, n_colonne, n_bytes_per_pix, n_blocchi)
;
;  Argomenti:    
     long n_righe           numero di righe della matrice
     long n_colonne         numero di colonne della matrice
     long n_byte_per_pix    numero di byte per ogni elemento della matrice
	 short *n_blocchi       numero di blocchi di memoria contigua

;
;  Valore di ritorno: 
     pointer               indirizzo del vettore di puntatori
	 NULL                  mancata allocazione

;
;  Descrizione:
     {\bf util_array2_dos}() alloca memoria per una matrice di elementi
	 di dimesione n_byte_per_pix; a differenza
     di array2 la memoria per i dati non e' necessariamente contigua ma
     suddivisa in blocchi che non superano SIZE_ARRAY2; SIZE_ARRAY2
	 e' minore di 64k, limite per il modello large del Microsoft-C in DOS;
	 la routine restituisce nella variabile n_blocchi il numero di blocchi
	 di memoria contigua allocati;
;
;  Note:
	 la dimensione di una singola riga (n_colonne x n_byte_per_pix)
	 deve essere comunque inferiore a SIZE_ARRAY2; inoltre n_righe
	 deve essere una potenza di 2;
;
;**********************************************************************/

char **
util_array2_dos(
	long n_righe,
	long n_colonne,
	long n_bytes_per_pix,
	short *n_blocchi /* numero di blocchi di memoria contigua */
)
{
	char **ptr, *data;
	long rbytes; /* numero di bytes per riga */
	long abytes; /* numero di bytes totali */
	long n_byte_blocco;
	short i, j, n_righe_blocco;

/* calcolo di n_blocchi */

	rbytes = n_colonne * n_bytes_per_pix;
	n_byte_blocco = abytes = n_righe * rbytes;
	while(n_byte_blocco > SIZE_ARRAY2)
		n_byte_blocco /= 2;
	*n_blocchi = (short) (abytes / n_byte_blocco);
	n_righe_blocco = (short) n_righe / *n_blocchi;

/* allocazione del vettore di puntatori */

	ptr = (char **) malloc((size_t) sizeof(char *) * (size_t) n_righe);
	if(ptr == NULL)
		return NULL;

/* allocazione dei blocchi */

	for(i=0; i<*n_blocchi; i++) {

		short offset = i * n_righe_blocco;

/* allocazione dei dati contigui */

		data = (char *) malloc((size_t) n_byte_blocco);
		if(data == NULL) {
			for(j=0; j<i; j++) {
				free(ptr[j * n_righe_blocco]);
			}
			free(ptr);
			return NULL;
		}

/* assegnazione dei puntatori */

		for(j=0; j<n_righe_blocco; j++)
			*(ptr + offset + j) = data + j * rbytes;
	}

	return ptr;
}


/***********************************************************************

;  Nome:    
     util_free2_dos
;
;  Funzione:
     libera la memoria allocata con util_array2_dos
;
;  Formato:
     void util_free2_dos(pointer, n_righe, n_blocchi)
;
;  Argomenti:    
     char **pointer       puntatore ritornato da util_array2_dos()
     long n_righe         numero di righe della matrice
	 short n_blocchi      numero di blocchi di memoria contigua ritornato da
	                      util_array2_dos()
;
;  Descrizione:
     {\bf util_free2_dos}() libera la memoria allocata da 
     {\bf util_array2_dos}();
;
;**********************************************************************/

void
util_free2_dos(
	char **ptr,
	long n_righe,
	short n_blocchi
)
{
	short i, n_righe_blocco = (short) n_righe / n_blocchi;

	if(ptr) {
		for(i=0; i<n_blocchi; i++)
			free(ptr[i * n_righe_blocco]);

		free(ptr);
	}

	return;
}


/***********************************************************************

;  Nome:    
     util_alloca_matrice
;
;  Funzione:
     alloca memoria per una matrice
;
;  Formato:
     char **util_alloca_matrice(n_righe, n_colonne, n_bytes_per_pix)
;
;  Argomenti:    
     long n_righe           numero di righe della matrice
     long n_colonne         numero di colonne della matrice
     long n_byte_per_pix    numero di byte per ogni elemento della matrice

;
;  Valore di ritorno: 
     pointer               indirizzo del vettore di puntatori
	 NULL                  mancata allocazione

;
;  Descrizione:
     {\bf util_alloca_matrice}() alloca memoria per una matrice di elementi
	 di dimesione n_byte_per_pix; a differenza
     di array2 e di array2_dos vine fatta una allocazione per ogni riga; il
	 numero di righe non deve essere necessariamente potenza di 2;
;
;  Note:
	 la dimensione di una singola riga (n_colonne x n_byte_per_pix)
	 deve essere comunque inferiore a 64k;
;
;**********************************************************************/

char **
util_alloca_matrice(
	long n_righe,
	long n_colonne,
	long n_bytes_per_pix
)
{
	char **ptr;
	short i, j;

	long rbytes = n_colonne * n_bytes_per_pix; /* numero di bytes per riga */

/* allocazione del vettore di puntatori */

	ptr = (char **) malloc((size_t) sizeof(char *) * (size_t) n_righe);
	if(ptr == NULL)
		return NULL;

/* allocazione delle righe */

	for(i=0; i<n_righe; i++) {
		ptr[i] = (char *) malloc((size_t) rbytes);
		if(ptr[i] == NULL) {
			for(j=0; j<i; j++)
				free(ptr[j]);
			free(ptr);
			return NULL;
		}
	}

	return ptr;
}


/***********************************************************************

;  Nome:    
     util_disalloca_matrice
;
;  Funzione:
     libera la memoria allocata con util_alloca_matrice
;
;  Formato:
     void util_disalloca_matrice(pointer, n_righe)
;
;  Argomenti:    
     char **pointer       puntatore ritornato da util_alloca_matrice()
     long n_righe         numero di righe della matrice
;
;  Descrizione:
     {\bf util_disalloca_matrice}() libera la memoria allocata da 
     {\bf util_alloca_matrice}();
;
;**********************************************************************/

void
util_disalloca_matrice(
	char **ptr,
	long n_righe
)
{
	short i;

	if(ptr) {
		for(i=0; i<n_righe; i++)
			free(ptr[i]);
		free(ptr);
	}

	return;
}


/***********************************************************************

;  Nome:    
     util_alloca_array_di_matrici
;
;  Funzione:
     alloca memoria per un vettore di matrici
;
;  Formato:
     char ***util_alloca_array_di_matrici(n_matrici, n_righe, n_colonne,
	                                      n_bytes_per_pix)
;
;  Argomenti:    
     long n_matrici         numero di matrici da allocare
     long n_righe           numero di righe della matrice
     long n_colonne         numero di colonne della matrice
     long n_byte_per_pix    numero di byte per ogni elemento della matrice

;
;  Valore di ritorno: 
     pointer               indirizzo del vettore di puntatori
	 NULL                  mancata allocazione

;
;  Descrizione:
     {\bf util_alloca_array_di_matrici}() alloca memoria per un vettore di
	 matrice di elementi di dimesione n_byte_per_pix;
;
;  Note:
	 la dimensione di una singola riga (n_colonne x n_byte_per_pix)
	 deve essere inferiore a 64k;
;
;**********************************************************************/

char ***
util_alloca_array_di_matrici(
	long n_matrici,
	long n_righe,
	long n_colonne,
	long n_bytes_per_pix
)
{
	char ***ptr;
	short i, j;

/* allocazione del vettore di matrici */

	ptr = (char ***) malloc((size_t) sizeof(char **) * (size_t) n_matrici);
	if(ptr == NULL)
		return NULL;

/* allocazione delle matrici */

	for(i=0; i<n_matrici; i++) {
		ptr[i] = util_alloca_matrice(
			n_righe, n_colonne, n_bytes_per_pix
		);
		if(ptr[i] == NULL) {
			for(j=0; j<i; j++)
				util_disalloca_matrice((char **) ptr[j], n_righe);
			free(ptr);
			return NULL;
		}
	}

	return ptr;
}


/***********************************************************************

;  Nome:    
     util_disalloca_array_di_matrici
;
;  Funzione:
     libera la memoria allocata con util_alloca_array_di_matrici
;
;  Formato:
     void util_disalloca_array_di_matrici(pointer, n_righe)
;
;  Argomenti:    
     char ***pointer      puntatore ritornato da util_alloca_array_di_matrici()
     long n_matrici       numero di matrici
     long n_righe         numero di righe della matrice
;
;  Descrizione:
     {\bf util_disalloca_array_di_matrici}() libera la memoria allocata da 
     {\bf util_alloca_array_di_matrici}();
;
;**********************************************************************/

void
util_disalloca_array_di_matrici(
	char ***ptr,
	long n_matrici,
	long n_righe
)
{
	short i;

	if(ptr) {
		for(i=0; i<n_matrici; i++)
			util_disalloca_matrice(ptr[i], n_righe);
		free(ptr);
	}

	return;
}

