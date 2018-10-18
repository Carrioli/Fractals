



/***********************************************************************

;  Nome:    
     util_sub_sampling
;
;  Funzione:
     sottocampionamento di un vettore
;
;  Formato:
     short util_sub_sampling(vet1, vet2, dim_vettore, step, tipo)
;
;  Argomenti:    
     void *vet1              vettore di ingresso
     void *vet2              vettore di uscita
     short dim_vettore       lunghezza di "vet1"
     short step              passo di sottocampionamento
     short tipo              0: vet1 e' composto da char, 1: da short,
	                         2: da int, 3: da long

;
;  Valore di ritorno: 
     $n$               numero di punti utili di "vet2"
     SEAV_ERR*         codice di errore

;
;  Descrizione:
     {\bf util_sub_sampling}() effettua un sottocampionamento di un vettore
	 intero con passo assegnato;
;
;**********************************************************************/

#include <util_nt.h>

short
util_sub_sampling(
	void *vet1,           /* vettore di ingresso */
	void *vet2,           /* vettore di uscita */
	short dim_vettore,    /* lunghezza di vet1 */
	short step,           /* passo di sottocampionamento */
	short tipo
)
{
	register short i;
	short n_punti = dim_vettore / step;

/* controllo dei parametri */

	if(step < 1)
		return SEAV_ERR_PARAMETER;

/* sottocampionamento */

	switch(tipo) {
		case 0: {
			char *p1 = (char *) vet1;
			char *p2 = (char *) vet2;
			for(i=n_punti-7; i>0; i-=8) {
				*p2++ = *p1; p1 += step;
				*p2++ = *p1; p1 += step;
				*p2++ = *p1; p1 += step;
				*p2++ = *p1; p1 += step;
				*p2++ = *p1; p1 += step;
				*p2++ = *p1; p1 += step;
				*p2++ = *p1; p1 += step;
				*p2++ = *p1; p1 += step;
			}
			for(; i>-7; i--) {
				*p2++ = *p1; p1 += step;
			}
		}
		break;
		case 1: {
			short *p1 = (short *) vet1;
			short *p2 = (short *) vet2;
			for(i=n_punti-7; i>0; i-=8) {
				*p2++ = *p1; p1 += step;
				*p2++ = *p1; p1 += step;
				*p2++ = *p1; p1 += step;
				*p2++ = *p1; p1 += step;
				*p2++ = *p1; p1 += step;
				*p2++ = *p1; p1 += step;
				*p2++ = *p1; p1 += step;
				*p2++ = *p1; p1 += step;
			}
			for(; i>-7; i--) {
				*p2++ = *p1; p1 += step;
			}
		}
		break;
		case 2: {
			int *p1 = (int *) vet1;
			int *p2 = (int *) vet2;
			for(i=n_punti-7; i>0; i-=8) {
				*p2++ = *p1; p1 += step;
				*p2++ = *p1; p1 += step;
				*p2++ = *p1; p1 += step;
				*p2++ = *p1; p1 += step;
				*p2++ = *p1; p1 += step;
				*p2++ = *p1; p1 += step;
				*p2++ = *p1; p1 += step;
				*p2++ = *p1; p1 += step;
			}
			for(; i>-7; i--) {
				*p2++ = *p1; p1 += step;
			}
		}
		break;
		case 3: {
			long *p1 = (long *) vet1;
			long *p2 = (long *) vet2;
			for(i=n_punti-7; i>0; i-=8) {
				*p2++ = *p1; p1 += step;
				*p2++ = *p1; p1 += step;
				*p2++ = *p1; p1 += step;
				*p2++ = *p1; p1 += step;
				*p2++ = *p1; p1 += step;
				*p2++ = *p1; p1 += step;
				*p2++ = *p1; p1 += step;
				*p2++ = *p1; p1 += step;
			}
			for(; i>-7; i--) {
				*p2++ = *p1; p1 += step;
			}
		}
		break;
		default:
			return SEAV_ERR_PARAMETER;
			break;
	}

	return n_punti;
}

