


/***********************************************************************

;  Nome:    
     util_spot_detection
;
;  Funzione:
     filtraggio passa alto non lineare
;
;  Formato:
     void util_spot_detection(vet_in, vet_out, n_punti, semi_larg)
;
;  Argomenti:    
	short *vet_in         vettore di ingresso
	short *vet_out        vettore di uscita
	short n_punti         lunghezza dei vettori
	short semi_larg       semilarghezza della maschera di convoluzione
;
;  Descrizione:
     {\bf util_spot_detection}() effettua la convoluzione del vettore di
	 ingresso con una maschera di tipo spot detection:
	 -1,0,...,0,2,0,...,0,-1; i valori negativi
     del risultato sono posti a 0; i valori iniziali e finali del vettore,
     per i quali non puo' essere calcolata la convoluzione, sono ugualmente
     posti a 0;
;
;**********************************************************************/

#include <util_nt.h>

void
util_spot_detection(
	short *vet_in,       /* vettore di ingresso */
	short *vet_out,      /* vettore di uscita */
	short n_punti,       /* lunghezza dei vettori */
	short semi_larg      /* semilarghezza della maschera di convoluzione */
)
{
	short aus, j;
	short n_punti0 = semi_larg;
	short n_punti1 = n_punti - 2 * semi_larg;
	short *ausp1 = vet_in;
	short *ausp2 = ausp1 + semi_larg;
	short *ausp3 = ausp2 + semi_larg;
	short *ausp_out = vet_out;

	for(j=n_punti0; j>0; j--)
		*ausp_out++ = (short) 0;
	for(j=n_punti1; j>0; j--)
		*ausp_out++ = (aus = ((*ausp2++)<<1)-(*ausp1++ + *ausp3++))>0 ? aus : 0;
	for(j=n_punti0; j>0; j--)
		*ausp_out++ = (short) 0;

	return;
}

