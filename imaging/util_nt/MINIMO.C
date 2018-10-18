


/***********************************************************************

;  Nome:    
     util_calcola_minimo
;
;  Funzione:
     calcola il minimo di un vettore
;
;  Formato:
     long util_calcola_minimo(vet, n_punti, tipo)
;
;  Argomenti:    
     void *vet             vettore del quale calcolare il minimo
     short n_punti         lunghezza di vet
     short tipo            0: vet e' composto da unsigned char, 1: da short,
	                       2: da int, 3: da long
;
;  Valore di ritorno: 
     minimo               minimo del vettore

;
;  Descrizione:
     {\bf util_calcola_minimo}() calcola il valore minimo di un vettore
	 di interi di formato arbitrario;
;
;**********************************************************************/

#include <util_nt.h>

long
util_calcola_minimo(
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
			unsigned char minimo = (unsigned char) 255;

			for(i=n_punti-7; i>0; i-=8) {
				if((ausv = *aus_p++) < minimo)
					minimo = ausv;
				if((ausv = *aus_p++) < minimo)
					minimo = ausv;
				if((ausv = *aus_p++) < minimo)
					minimo = ausv;
				if((ausv = *aus_p++) < minimo)
					minimo = ausv;
				if((ausv = *aus_p++) < minimo)
					minimo = ausv;
				if((ausv = *aus_p++) < minimo)
					minimo = ausv;
				if((ausv = *aus_p++) < minimo)
					minimo = ausv;
				if((ausv = *aus_p++) < minimo)
					minimo = ausv;
			}
			for(; i>-7; i--) {
				if((ausv = *aus_p++) < minimo)
					minimo = ausv;
			}
			return (long) minimo;
			break;
		}
		case 1: {
			short *aus_p = (short *) vet, ausv;
			short minimo = *aus_p;

			for(i=n_punti-7; i>0; i-=8) {
				if((ausv = *aus_p++) < minimo)
					minimo = ausv;
				if((ausv = *aus_p++) < minimo)
					minimo = ausv;
				if((ausv = *aus_p++) < minimo)
					minimo = ausv;
				if((ausv = *aus_p++) < minimo)
					minimo = ausv;
				if((ausv = *aus_p++) < minimo)
					minimo = ausv;
				if((ausv = *aus_p++) < minimo)
					minimo = ausv;
				if((ausv = *aus_p++) < minimo)
					minimo = ausv;
				if((ausv = *aus_p++) < minimo)
					minimo = ausv;
			}
			for(; i>-7; i--) {
				if((ausv = *aus_p++) < minimo)
					minimo = ausv;
			}
			return (long) minimo;
			break;
		}
		case 2: {
			int *aus_p = (int *) vet, ausv;
			int minimo = *aus_p;

			for(i=n_punti-7; i>0; i-=8) {
				if((ausv = *aus_p++) < minimo)
					minimo = ausv;
				if((ausv = *aus_p++) < minimo)
					minimo = ausv;
				if((ausv = *aus_p++) < minimo)
					minimo = ausv;
				if((ausv = *aus_p++) < minimo)
					minimo = ausv;
				if((ausv = *aus_p++) < minimo)
					minimo = ausv;
				if((ausv = *aus_p++) < minimo)
					minimo = ausv;
				if((ausv = *aus_p++) < minimo)
					minimo = ausv;
				if((ausv = *aus_p++) < minimo)
					minimo = ausv;
			}
			for(; i>-7; i--) {
				if((ausv = *aus_p++) < minimo)
					minimo = ausv;
			}
			return (long) minimo;
			break;
		}
		case 3: {
			long *aus_p = (long *) vet, ausv;
			long minimo = *aus_p;

			for(i=n_punti-7; i>0; i-=8) {
				if((ausv = *aus_p++) < minimo)
					minimo = ausv;
				if((ausv = *aus_p++) < minimo)
					minimo = ausv;
				if((ausv = *aus_p++) < minimo)
					minimo = ausv;
				if((ausv = *aus_p++) < minimo)
					minimo = ausv;
				if((ausv = *aus_p++) < minimo)
					minimo = ausv;
				if((ausv = *aus_p++) < minimo)
					minimo = ausv;
				if((ausv = *aus_p++) < minimo)
					minimo = ausv;
				if((ausv = *aus_p++) < minimo)
					minimo = ausv;
			}
			for(; i>-7; i--) {
				if((ausv = *aus_p++) < minimo)
					minimo = ausv;
			}
			return minimo;
			break;
		}
	}
	return 0;
}


