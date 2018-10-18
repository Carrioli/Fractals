


/***********************************************************************

;  Nome:    
     util_smooth_riga
;
;  Funzione:
     filtraggio passa basso monodimensionale
;
;  Formato:
     short util_smooth_riga(vettore, risultato, dim_vettore, dim_kernel)
;
;  Argomenti:    
     unsigned char *vettore  vettore contenente i valori da convolvere
	 long *risultato         vettore contenente il risultato
     short dim_vettore       lunghezza di "vettore" e "risultato"
     short dim_kernel        lunghezza della maschera di smooth

;
;  Valore di ritorno: 
     SEAV_OK_ALL       nessun errore
     SEAV_ERR*         codice di errore

;
;  Descrizione:
     {\bf util_smooth_riga}() effettua un filtraggio passa basso
	 monodimensionale
	 effettuando la convoluzione del vettore originale con un kernel piatto
	 di dimensione assegnata; il risultato viene normalizzato con il valore
	 "dim_kernel"; la complessita' computazionale e' lineare con il numero
	 di valori da elaborare ("dim_vettore");
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
#include <util_nt.h>

short
util_smooth_riga(
	unsigned char *vettore,
	long *risultato,
	short dim_vettore,    /* lunghezza di vettore */
	short dim_kernel      /* lunghezza della maschera di smooth */
)
{
	unsigned short *aus_vet;
	unsigned short *ps1, *ps2, aus;
	short i;
	unsigned char *p1, *p2;
	long *pl;

/* controllo dei parametri */

	if(dim_kernel < 2)
		return SEAV_ERR_PARAMETER;

/* allocazione di memoria */

	aus_vet = (unsigned short *) malloc(
		(size_t) dim_vettore * sizeof(unsigned short)
	);
	if(aus_vet == NULL)
		return SEAV_ERR_MEMORY;

/* voglio la maschera dispari */

	if(!(dim_kernel % 2))
		dim_kernel ++;

/* si crea il primo valore da inserire in aus_vet */

	ps1 = ps2 = aus_vet;
	p1 = p2 = vettore;
	for(i=dim_kernel, aus=0; i>0; i--)
		aus = (unsigned short) (*p2++ + aus);
	*ps2++ = aus;

/* valori successivi: si calcolano a partire dal precedente sottraendo
   un punto e sommandone un'altro */

	for(i=dim_vettore-dim_kernel; i>0; i--)
		*ps2++ = *ps1++ - (short) *p1++ + (short) *p2++;

/* normalizzazione con trasferimento in risultato */

	ps1 = aus_vet;
	pl = risultato;

/* il primo valore utile e' assegnato ai primi pixel */

	aus = *ps1 / dim_kernel;
	for(i=dim_kernel>>1; i>0; i--)
		*pl++ = (long) aus;

/* assegnamento dei valori utili */

	for(i=dim_vettore-dim_kernel+1; i>0; i--)
		*pl++ = (long) (*ps1++ / dim_kernel);

/* l'ultimo valore utile e' assegnato agli ultimi pixel */

	aus = *--ps1 / dim_kernel;
	for(i=dim_kernel>>1; i>0; i--)
		*pl++ = (long) aus;

	free(aus_vet);
	return SEAV_OK_ALL;
}

