


/***********************************************************************

;  Nome:    
     util_median_riga
;
;  Funzione:
     filtraggio mediano monodimensionale
;
;  Formato:
     short util_median_riga(vettore, risultato, dim_vettore, dim_kernel, tipo)
;
;  Argomenti:    
     void *vettore           vettore contenente i valori da filtrare
	 void *risultato         vettore contenente il risultato
     short dim_vettore       lunghezza di "vettore" e "risultato"
     short dim_kernel        lunghezza della maschera
     short tipo              0: vet e' composto da unsigned char, 1: da short,
	                         2: da int, 3: da long

;
;  Valore di ritorno: 
     SEAV_OK_ALL          nessun errore
	 SEAV_ERR_*           codice di errore

;
;  Descrizione:
     {\bf util_median_riga}() effettua un filtraggio mediano
	 monodimensionale su un vettore di interi; se tipo vale 0
	 (vettore di unsigned char)
	 viene usata la tecnica di aggiornamento dell'istogramma;
	 pertanto la complessita' computazionale e' lineare con il numero
	 di valori da elaborare ("dim_vettore");
	 per gli altri casi viene usato un algoritmo di ordinamento per
	 calcolare ogni valore mediano;
	 agli estremi del vettore risultato sono riportati il
	 primo e l'ultimo valore utile;
;
;  Note:
     "dim_kernel" deve essere dispari: se non lo e' il suo valore e'
	 incrementato di 1;
;
;**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util_nt.h>

/**********************************************************************/

/* prototipi delle funzioni private */

static short calcola_valore_mediano(unsigned short *isto, short soglia);
static short calcola_valore_mediano_s(
	short *vet, short dim_kernel, short *aus_vet
);
static int calcola_valore_mediano_i(
	int *vet, short dim_kernel, int *aus_vet
);
static long calcola_valore_mediano_l(
	long *vet, short dim_kernel, long *aus_vet
);

/**********************************************************************/

short
util_median_riga(
	void *vettore,
	void *risultato,
	short dim_vettore,    /* lunghezza di vettore */
	short dim_kernel,
	short tipo
)
{
	short i, dim_kernel2;

/* controllo dei parametri */

	if(dim_kernel < 2)
		return SEAV_ERR_PARAMETER;

/* voglio la maschera dispari */

	if(!(dim_kernel % 2))
		dim_kernel ++;
	dim_kernel2 = dim_kernel / 2;

	if(tipo == 0) {
		unsigned short *aus_vet;
		unsigned char *p1, *p2, *pr = risultato, aus;

/* allocazione di memoria per il vettore dell'istogramma */

		aus_vet = (unsigned short *) calloc(
			(size_t) 256, (size_t) sizeof(unsigned short)
		);
		if(aus_vet == NULL)
			return SEAV_ERR_MEMORY;

/* si inizializza l'istogramma */

		p1 = p2 = vettore;
		for(i=dim_kernel; i>0; i--)
			aus_vet[(short) *p1++] ++;

/* si calcola il primo valore e lo si assegna ai primi punti del risultato */

		aus = (unsigned char) calcola_valore_mediano(aus_vet, dim_kernel2);
		for(i=dim_kernel2; i>=0; i--)
			*pr++ = aus;

/* valori successivi: si calcolano a partire dal precedente sottraendo
   un punto e sommandone un'altro all'interno dell'istogramma */

		for(i=dim_vettore-dim_kernel; i>0; i--) {
			aus_vet[(short) *p1++] ++;
			aus_vet[(short) *p2++] --;
			aus = (unsigned char) calcola_valore_mediano(aus_vet, dim_kernel2);
			*pr++ = aus;
		}

/* l'ultimo valore utile e' assegnato agli ultimi pixel */

		for(i=dim_kernel2; i>0; i--)
			*pr++ = aus;
		free(aus_vet);

	} else {
		switch(tipo) {
			case 1: {
				short *pr = (short *) risultato;
				short *pv = (short *) vettore;
				short aus;
				short *aus_vet = (short *) malloc(
					(size_t) dim_kernel * sizeof(short)
				);
				if(aus_vet == NULL)
					return SEAV_ERR_MEMORY;

/* si calcola il primo valore e lo si assegna ai primi punti del risultato */

				aus = calcola_valore_mediano_s(pv++, dim_kernel, aus_vet);
				for(i=dim_kernel2; i>=0; i--)
					*pr++ = aus;

/* valori successivi */

				for(i=dim_vettore-dim_kernel; i>0; i--)
					*pr++ = calcola_valore_mediano_s(pv++, dim_kernel, aus_vet);

/* l'ultimo valore utile e' assegnato agli ultimi pixel */

				aus = *(pr-1);
				for(i=dim_kernel2; i>0; i--)
					*pr++ = aus;

				free(aus_vet);
			}
			break;
			case 2: {
				int *pr = (int *) risultato;
				int *pv = (int *) vettore;
				int aus;
				int *aus_vet = (int *) malloc(
					(size_t) dim_kernel * sizeof(int)
				);
				if(aus_vet == NULL)
					return SEAV_ERR_MEMORY;

/* si calcola il primo valore e lo si assegna ai primi punti del risultato */

				aus = calcola_valore_mediano_i(pv++, dim_kernel, aus_vet);
				for(i=dim_kernel2; i>=0; i--)
					*pr++ = aus;

/* valori successivi */

				for(i=dim_vettore-dim_kernel; i>0; i--)
					*pr++ = calcola_valore_mediano_i(pv++, dim_kernel, aus_vet);

/* l'ultimo valore utile e' assegnato agli ultimi pixel */

				aus = *(pr-1);
				for(i=dim_kernel2; i>0; i--)
					*pr++ = aus;

				free(aus_vet);
			}
			break;
			case 3: {
				long *pr = (long *) risultato;
				long *pv = (long *) vettore;
				long aus;
				long *aus_vet = (long *) malloc(
					(size_t) dim_kernel * sizeof(long)
				);
				if(aus_vet == NULL)
					return SEAV_ERR_MEMORY;

/* si calcola il primo valore e lo si assegna ai primi punti del risultato */

				aus = calcola_valore_mediano_l(pv++, dim_kernel, aus_vet);
				for(i=dim_kernel2; i>=0; i--)
					*pr++ = aus;

/* valori successivi */

				for(i=dim_vettore-dim_kernel; i>0; i--)
					*pr++ = calcola_valore_mediano_l(pv++, dim_kernel, aus_vet);

/* l'ultimo valore utile e' assegnato agli ultimi pixel */

				aus = *(pr-1);
				for(i=dim_kernel2; i>0; i--)
					*pr++ = aus;

				free(aus_vet);
			}
			break;
		}
	}
	return SEAV_OK_ALL;
}

/**********************************************************************/

static short
calcola_valore_mediano(
	unsigned short *isto,
	short soglia
)
{
	register unsigned short *p_isto = isto;
	short acc = 0;

	while(acc <= soglia) {
		acc = (short) (*p_isto++ + acc);
	}

	return (short) (p_isto - isto - 1);
}

/**********************************************************************/

static short
calcola_valore_mediano_s(
	short *vet,
	short dim_kernel,
	short *aus_vet
)
{
	register short i, aus;
	short *aus0 = aus_vet;
	short *aus1 = aus0 + 1;
	short fine;

/* copio il vettore nel buffer di lavoro */

	memcpy(aus_vet, vet, (size_t) dim_kernel * sizeof(short));

/* ordinamento di aus_vet */

	fine = 0;
	while(!fine) {
		fine = 1;
		for(i=dim_kernel-1; i>0; i--) {
			if((aus = *aus0) > *aus1) {
				*aus0 = *aus1;
				*aus1 = aus;
				fine = 0;
			}
			aus0 ++;
			aus1 ++;
		}
		if(!fine) {
			fine = 1;
			for(i=dim_kernel-1; i>0; i--) {
				aus0 --;
				aus1 --;
				if((aus = *aus0) > *aus1) {
					*aus0 = *aus1;
					*aus1 = aus;
					fine = 0;
				}
			}
		}
	}

	return aus_vet[dim_kernel >> 1];
}

/**********************************************************************/

static int
calcola_valore_mediano_i(
	int *vet,
	short dim_kernel,
	int *aus_vet
)
{
	register short i;
	int aus;
	int *aus0 = aus_vet;
	int *aus1 = aus0 + 1;
	short fine;

/* copio il vettore nel buffer di lavoro */

	memcpy(aus_vet, vet, (size_t) dim_kernel * sizeof(int));

/* ordinamento di aus_vet */

	fine = 0;
	while(!fine) {
		fine = 1;
		for(i=dim_kernel-1; i>0; i--) {
			if((aus = *aus0) > *aus1) {
				*aus0 = *aus1;
				*aus1 = aus;
				fine = 0;
			}
			aus0 ++;
			aus1 ++;
		}
		if(!fine) {
			fine = 1;
			for(i=dim_kernel-1; i>0; i--) {
				aus0 --;
				aus1 --;
				if((aus = *aus0) > *aus1) {
					*aus0 = *aus1;
					*aus1 = aus;
					fine = 0;
				}
			}
		}
	}

	return aus_vet[dim_kernel >> 1];
}

/**********************************************************************/

static long
calcola_valore_mediano_l(
	long *vet,
	short dim_kernel,
	long *aus_vet
)
{
	register short i;
	long aus;
	long *aus0 = aus_vet;
	long *aus1 = aus0 + 1;
	short fine;

/* copio il vettore nel buffer di lavoro */

	memcpy(aus_vet, vet, (size_t) dim_kernel * sizeof(long));

/* ordinamento di aus_vet */

	fine = 0;
	while(!fine) {
		fine = 1;
		for(i=dim_kernel-1; i>0; i--) {
			if((aus = *aus0) > *aus1) {
				*aus0 = *aus1;
				*aus1 = aus;
				fine = 0;
			}
			aus0 ++;
			aus1 ++;
		}
		if(!fine) {
			fine = 1;
			for(i=dim_kernel-1; i>0; i--) {
				aus0 --;
				aus1 --;
				if((aus = *aus0) > *aus1) {
					*aus0 = *aus1;
					*aus1 = aus;
					fine = 0;
				}
			}
		}
	}

	return aus_vet[dim_kernel >> 1];
}

