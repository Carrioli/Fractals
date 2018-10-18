


/***********************************************************************

;  Nome:    
     util_normalizza
;
;  Funzione:
     calcola il massimo di un vettore
;
;  Formato:
     void util_normalizza(vet, n_punti)
;
;  Argomenti:    
     long *vet_in           vettore da normalizzare
     long *vet_out          vettore normalizzato
     short n_punti          lunghezza dei vettori
	 long max               valore massimo di vet_in
	 long new_max           valore che assumera' il massimo dopo la
	                        normalizzazione
	 short flag_inversione  se diverso da 0 il risultato viene
	                        ribaltato rispetto a new_max
;
;  Descrizione:
     {\bf util_normalizza}() normalizza il vettore "vet_in"
	 portando il valore "max" al valore "new_max"; se "flag_inversione"
	 e' diverso da 0, il vettore di uscita risulta invertito,
	 con 0 laddove "vet_in" assume il valore "max" e
	 "new_max" dove "vet_in" vale 0;
;
;**********************************************************************/

#include <util_nt.h>

void
util_normalizza(
	long *vet_in,         /* vettore di ingresso */
	long *vet_out,        /* vettore di uscita */
	short n_punti,        /* lunghezza dei due vettori */
	long max,             /* valore massimo */
	long new_max,         /* valore che assumera' il massimo */
	short flag_inversione /* se diverso da 0 il risultato viene
	                         ribaltato rispetto a new_max */
)
{
	long *aus_p = vet_out;
	long *aus_p1 = vet_in;
	short i;

	if(!flag_inversione) {
		for(i=n_punti-7; i>0; i-=8) {
			*aus_p++ = (*aus_p1++ * new_max) / max;
			*aus_p++ = (*aus_p1++ * new_max) / max;
			*aus_p++ = (*aus_p1++ * new_max) / max;
			*aus_p++ = (*aus_p1++ * new_max) / max;
			*aus_p++ = (*aus_p1++ * new_max) / max;
			*aus_p++ = (*aus_p1++ * new_max) / max;
			*aus_p++ = (*aus_p1++ * new_max) / max;
			*aus_p++ = (*aus_p1++ * new_max) / max;
		}
		for(; i>-7; i--) {
			*aus_p++ = (*aus_p1++ * new_max) / max;
		}
	} else {
		for(i=n_punti-7; i>0; i-=8) {
			*aus_p++ = new_max - (*aus_p1++ * new_max) / max;
			*aus_p++ = new_max - (*aus_p1++ * new_max) / max;
			*aus_p++ = new_max - (*aus_p1++ * new_max) / max;
			*aus_p++ = new_max - (*aus_p1++ * new_max) / max;
			*aus_p++ = new_max - (*aus_p1++ * new_max) / max;
			*aus_p++ = new_max - (*aus_p1++ * new_max) / max;
			*aus_p++ = new_max - (*aus_p1++ * new_max) / max;
			*aus_p++ = new_max - (*aus_p1++ * new_max) / max;
		}
		for(; i>-7; i--) {
			*aus_p++ = new_max - (*aus_p1++ * new_max) / max;
		}
	}

	return;
}

