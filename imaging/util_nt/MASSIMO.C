


/***********************************************************************

;  Nome:    
     util_calcola_massimo
;
;  Funzione:
     calcola il massimo di un vettore
;
;  Formato:
     long util_calcola_massimo(vet, n_punti, tipo)
;
;  Argomenti:    
     void *vet             vettore del quale calcolare il massimo
     short n_punti         lunghezza di vet
     short tipo            0: vet e' composto da unsigned char, 1: da short,
	                       2: da int, 3: da long
;
;  Valore di ritorno: 
     massimo               massimo del vettore

;
;  Descrizione:
     {\bf util_calcola_massimo}() calcola il valore massimo di un vettore
	 di interi di formato arbitrario;
;
;**********************************************************************/

#include <util_nt.h>

long
util_calcola_massimo(
	void *vet,
	short n_punti,
	short tipo
)
{
	short i;
	if(n_punti <= 0)
		return (long) SEAV_ERR_PARAMETER;

	switch(tipo) {
		case 0: {
			unsigned char *aus_p = (unsigned char *) vet, ausv;
			unsigned char massimo = (unsigned char) 0;

			for(i=n_punti-7; i>0; i-=8) {
				if((ausv = *aus_p++) > massimo)
					massimo = ausv;
				if((ausv = *aus_p++) > massimo)
					massimo = ausv;
				if((ausv = *aus_p++) > massimo)
					massimo = ausv;
				if((ausv = *aus_p++) > massimo)
					massimo = ausv;
				if((ausv = *aus_p++) > massimo)
					massimo = ausv;
				if((ausv = *aus_p++) > massimo)
					massimo = ausv;
				if((ausv = *aus_p++) > massimo)
					massimo = ausv;
				if((ausv = *aus_p++) > massimo)
					massimo = ausv;
			}
			for(; i>-7; i--) {
				if((ausv = *aus_p++) > massimo)
					massimo = ausv;
			}
			return (long) massimo;
			break;
		}
		case 1: {
			short *aus_p = (short *) vet, ausv;
			short massimo = *aus_p;

			for(i=n_punti-7; i>0; i-=8) {
				if((ausv = *aus_p++) > massimo)
					massimo = ausv;
				if((ausv = *aus_p++) > massimo)
					massimo = ausv;
				if((ausv = *aus_p++) > massimo)
					massimo = ausv;
				if((ausv = *aus_p++) > massimo)
					massimo = ausv;
				if((ausv = *aus_p++) > massimo)
					massimo = ausv;
				if((ausv = *aus_p++) > massimo)
					massimo = ausv;
				if((ausv = *aus_p++) > massimo)
					massimo = ausv;
				if((ausv = *aus_p++) > massimo)
					massimo = ausv;
			}
			for(; i>-7; i--) {
				if((ausv = *aus_p++) > massimo)
					massimo = ausv;
			}
			return (long) massimo;
			break;
		}
		case 2: {
			int *aus_p = (int *) vet, ausv;
			int massimo = *aus_p;

			for(i=n_punti-7; i>0; i-=8) {
				if((ausv = *aus_p++) > massimo)
					massimo = ausv;
				if((ausv = *aus_p++) > massimo)
					massimo = ausv;
				if((ausv = *aus_p++) > massimo)
					massimo = ausv;
				if((ausv = *aus_p++) > massimo)
					massimo = ausv;
				if((ausv = *aus_p++) > massimo)
					massimo = ausv;
				if((ausv = *aus_p++) > massimo)
					massimo = ausv;
				if((ausv = *aus_p++) > massimo)
					massimo = ausv;
				if((ausv = *aus_p++) > massimo)
					massimo = ausv;
			}
			for(; i>-7; i--) {
				if((ausv = *aus_p++) > massimo)
					massimo = ausv;
			}
			return (long) massimo;
			break;
		}
		case 3: {
			long *aus_p = (long *) vet, ausv;
			long massimo = *aus_p;

			for(i=n_punti-7; i>0; i-=8) {
				if((ausv = *aus_p++) > massimo)
					massimo = ausv;
				if((ausv = *aus_p++) > massimo)
					massimo = ausv;
				if((ausv = *aus_p++) > massimo)
					massimo = ausv;
				if((ausv = *aus_p++) > massimo)
					massimo = ausv;
				if((ausv = *aus_p++) > massimo)
					massimo = ausv;
				if((ausv = *aus_p++) > massimo)
					massimo = ausv;
				if((ausv = *aus_p++) > massimo)
					massimo = ausv;
				if((ausv = *aus_p++) > massimo)
					massimo = ausv;
			}
			for(; i>-7; i--) {
				if((ausv = *aus_p++) > massimo)
					massimo = ausv;
			}
			return massimo;
			break;
		}
	}
	return 0;
}


