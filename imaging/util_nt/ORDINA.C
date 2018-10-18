


/***********************************************************************

;  Nome:    
     util_ordinamento
;
;  Funzione:
     ordinamento di un vettore
;
;  Formato:
     void util_ordinamento(vettore, dim_vettore, tipo, flag_ordine)
;
;  Argomenti:    
     void *vettore           vettore contenente i valori da ordinare
     short dim_vettore       lunghezza di "vettore"
     short tipo              0: vet e' composto da unsigned char, 1: da short,
	                         2: da int, 3: da long, 4: da float, 5: da double
     short flag_ordine       0: ordine crescente, 1: ordine decrescente

;
;  Descrizione:
     {\bf util_ordinamento}() effettua l'ordinamento di un vettore
	 tramite la tecnica degli interscambi;
;
;  Note:
     nel caso di ordine decrescente dapprima si ordina il vettore in ordine
	 crescente poi si effettua uno scrambling dei valori;
;
;**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util_nt.h>

void
util_ordinamento(
	void *vettore,
	short dim_vettore,
	short tipo,
	short flag_ordine
)
{
	register short i;
	short fine = 0, n_giri = 1;

	switch(tipo) {
		case 0: {
			unsigned char *aus0 = (unsigned char *) vettore;
			unsigned char *aus1 = aus0 + 1, aus;

			while(!fine) {
				fine = 1;
				for(i=dim_vettore-n_giri; i>0; i--) {
					if((aus = *aus0) > *aus1) {
						*aus0 = *aus1;
						*aus1 = aus;
						fine = 0;
					}
					aus0 ++; aus1 ++;
				}
				n_giri ++; aus0 --; aus1 --;
				if(!fine) {
					fine = 1;
					for(i=dim_vettore-n_giri; i>0; i--) {
						aus0 --; aus1 --;
						if((aus = *aus0) > *aus1) {
							*aus0 = *aus1;
							*aus1 = aus;
							fine = 0;
						}
					}
				}
				n_giri ++; aus0 ++; aus1 ++;
			}
			if(flag_ordine) { /* scrambling del vettore */
				aus0 = (unsigned char *) vettore;
				aus1 = aus0 + dim_vettore - 1;
				for(i=dim_vettore>>1; i>0; i--) {
					aus = *aus0;
					*aus0++ = *aus1;
					*aus1-- = aus;
				}
			}
		}
		break;

		case 1: {
			short *aus0 = (short *) vettore;
			short *aus1 = aus0 + 1, aus;

			while(!fine) {
				fine = 1;
				for(i=dim_vettore-n_giri; i>0; i--) {
					if((aus = *aus0) > *aus1) {
						*aus0 = *aus1;
						*aus1 = aus;
						fine = 0;
					}
					aus0 ++; aus1 ++;
				}
				n_giri ++; aus0 --; aus1 --;
				if(!fine) {
					fine = 1;
					for(i=dim_vettore-n_giri; i>0; i--) {
						aus0 --; aus1 --;
						if((aus = *aus0) > *aus1) {
							*aus0 = *aus1;
							*aus1 = aus;
							fine = 0;
						}
					}
				}
				n_giri ++; aus0 ++; aus1 ++;
			}
			if(flag_ordine) { /* scrambling del vettore */
				aus0 = (short *) vettore;
				aus1 = aus0 + dim_vettore - 1;
				for(i=dim_vettore>>1; i>0; i--) {
					aus = *aus0;
					*aus0++ = *aus1;
					*aus1-- = aus;
				}
			}
		}
		break;

		case 2: {
			int *aus0 = (int *) vettore;
			int *aus1 = aus0 + 1, aus;

			while(!fine) {
				fine = 1;
				for(i=dim_vettore-n_giri; i>0; i--) {
					if((aus = *aus0) > *aus1) {
						*aus0 = *aus1;
						*aus1 = aus;
						fine = 0;
					}
					aus0 ++; aus1 ++;
				}
				n_giri ++; aus0 --; aus1 --;
				if(!fine) {
					fine = 1;
					for(i=dim_vettore-n_giri; i>0; i--) {
						aus0 --; aus1 --;
						if((aus = *aus0) > *aus1) {
							*aus0 = *aus1;
							*aus1 = aus;
							fine = 0;
						}
					}
				}
				n_giri ++; aus0 ++; aus1 ++;
			}
			if(flag_ordine) { /* scrambling del vettore */
				aus0 = (int *) vettore;
				aus1 = aus0 + dim_vettore - 1;
				for(i=dim_vettore>>1; i>0; i--) {
					aus = *aus0;
					*aus0++ = *aus1;
					*aus1-- = aus;
				}
			}
		}
		break;

		case 3: {
			long *aus0 = (long *) vettore;
			long *aus1 = aus0 + 1, aus;

			while(!fine) {
				fine = 1;
				for(i=dim_vettore-n_giri; i>0; i--) {
					if((aus = *aus0) > *aus1) {
						*aus0 = *aus1;
						*aus1 = aus;
						fine = 0;
					}
					aus0 ++; aus1 ++;
				}
				n_giri ++; aus0 --; aus1 --;
				if(!fine) {
					fine = 1;
					for(i=dim_vettore-n_giri; i>0; i--) {
						aus0 --; aus1 --;
						if((aus = *aus0) > *aus1) {
							*aus0 = *aus1;
							*aus1 = aus;
							fine = 0;
						}
					}
				}
				n_giri ++; aus0 ++; aus1 ++;
			}
			if(flag_ordine) { /* scrambling del vettore */
				aus0 = (long *) vettore;
				aus1 = aus0 + dim_vettore - 1;
				for(i=dim_vettore>>1; i>0; i--) {
					aus = *aus0;
					*aus0++ = *aus1;
					*aus1-- = aus;
				}
			}
		}
		break;

		case 4: {
			float *aus0 = (float *) vettore;
			float *aus1 = aus0 + 1, aus;

			while(!fine) {
				fine = 1;
				for(i=dim_vettore-n_giri; i>0; i--) {
					if((aus = *aus0) > *aus1) {
						*aus0 = *aus1;
						*aus1 = aus;
						fine = 0;
					}
					aus0 ++; aus1 ++;
				}
				n_giri ++; aus0 --; aus1 --;
				if(!fine) {
					fine = 1;
					for(i=dim_vettore-n_giri; i>0; i--) {
						aus0 --; aus1 --;
						if((aus = *aus0) > *aus1) {
							*aus0 = *aus1;
							*aus1 = aus;
							fine = 0;
						}
					}
				}
				n_giri ++; aus0 ++; aus1 ++;
			}
			if(flag_ordine) { /* scrambling del vettore */
				aus0 = (float *) vettore;
				aus1 = aus0 + dim_vettore - 1;
				for(i=dim_vettore>>1; i>0; i--) {
					aus = *aus0;
					*aus0++ = *aus1;
					*aus1-- = aus;
				}
			}
		}
		break;

		case 5: {
			double *aus0 = (double *) vettore;
			double *aus1 = aus0 + 1, aus;

			while(!fine) {
				fine = 1;
				for(i=dim_vettore-n_giri; i>0; i--) {
					if((aus = *aus0) > *aus1) {
						*aus0 = *aus1;
						*aus1 = aus;
						fine = 0;
					}
					aus0 ++; aus1 ++;
				}
				n_giri ++; aus0 --; aus1 --;
				if(!fine) {
					fine = 1;
					for(i=dim_vettore-n_giri; i>0; i--) {
						aus0 --; aus1 --;
						if((aus = *aus0) > *aus1) {
							*aus0 = *aus1;
							*aus1 = aus;
							fine = 0;
						}
					}
				}
				n_giri ++; aus0 ++; aus1 ++;
			}
			if(flag_ordine) { /* scrambling del vettore */
				aus0 = (double *) vettore;
				aus1 = aus0 + dim_vettore - 1;
				for(i=dim_vettore>>1; i>0; i--) {
					aus = *aus0;
					*aus0++ = *aus1;
					*aus1-- = aus;
				}
			}
		}
		break;

	}

	return;
}

