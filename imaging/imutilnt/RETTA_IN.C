

/***********************************************************************

;  Nome:    
     imut_retta_interpolante
;
;  Funzione:
     calcola i coefficienti della retta interpolante
;
;  Formato:
     short imut_retta_interpolante(asc, ord, n_punti, tipo, cx, cy, retta)
;
;  Argomenti:    
     void *asc        vettore delle ascisse dei punti
     void *ord        vettore delle ordinate dei punti
	 short n_punti    lunghezza dei vettori
     short tipo       0: i vettori sono composti da unsigned char, 1: da short,
	                  2: da int, 3: da long, 4: da float, 5: da double
	 double cx        fattore di conversione da pixel ad unita' fisica per le
	                  ascisse
	 double cy        fattore di conversione da pixel ad unita' fisica per le
	                  ordinate
	 RETTA *retta     puntatore alla struttura che conterra' il risultato

;
;  Valore di ritorno: 
     SEAV_OK_ALL              nessun errore
	 SEAV_ERR_*               codice di errore

;
;  Descrizione:
     {\bf imut_retta_interpolante}() calcola i coefficienti a, b, c della
	 retta che interpola l'insieme di punti dati nel senso dei minimi quadrati;
	 i coefficienti sono calcolati nelle unita' fisiche che si ottengono
	 moltiplicando la ascisse per cx e le ordinate per cy; non e' prevista
	 una trasformazione di coordinate non lineare;
	 le somme dei valori e dei loro quadrati sono fatte con dati long
	 per i tipi 0 e 1, con dati double con i tipi 2, 3, 4, 5; nei primi
	 due casi bisogna stare attenti a che non si abbiano overflow;

;
;**********************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <imutilnt.h>

short
imut_retta_interpolante(
	void *asc,
	void *ord,
	short n_punti,
	short tipo,
	double cx,
	double cy,
	RETTA *retta
)
{
	short flag_diversi = 0;

/* controllo di n_punti */

	if(n_punti < 2)
		return SEAV_ERR_PARAMETER;

/* calcolo dei parametri della retta per ogni tipo di dato */

	switch(tipo) {
		case 0: { /* unsigned char */
			short j;
			long sx=0L, sx2=0L, sy=0L, sy2=0L, sxy=0L, ausx, ausy;
			unsigned char *px = (unsigned char *) asc;
			unsigned char *py = (unsigned char *) ord;

			for(j=n_punti-2; j>0 && !flag_diversi; j--) {
				if(px[j] != px[j+1])
					flag_diversi = 1;
				else if(py[j] != py[j+1])
					flag_diversi = 1;
			}
			if(!flag_diversi)
				return SEAV_ERR_NOT_FOUND;
			for(j=n_punti; j>0; j--) {
				ausx = (long) *px++; ausy = (long) *py++;
				sx += ausx; sy += ausy;
				sx2 += ausx * ausx; sy2 += ausy * ausy; sxy += ausx * ausy;
			}
			retta->a = ((double) sx * (double) sy2 -
			            (double) sy * (double) sxy) / cx;
			retta->b = ((double) sx2 * (double) sy -
			            (double) sx * (double) sxy) / cy;
			retta->c = (double) sxy * (double) sxy -
			           (double) sx2 * (double) sy2;
		}
		break;
		case 1: { /* short */
			short j;
			long sx=0L, sx2=0L, sy=0L, sy2=0L, sxy=0L, ausx, ausy;
			short *px = (short *) asc, *py = (short *) ord;

			for(j=n_punti-2; j>0 && !flag_diversi; j--) {
				if(px[j] != px[j+1])
					flag_diversi = 1;
				else if(py[j] != py[j+1])
					flag_diversi = 1;
			}
			if(!flag_diversi)
				return SEAV_ERR_NOT_FOUND;
			for(j=n_punti; j>0; j--) {
				ausx = (long) *px++; ausy = (long) *py++;
				sx += ausx; sy += ausy;
				sx2 += ausx * ausx; sy2 += ausy * ausy; sxy += ausx * ausy;
			}
			retta->a = ((double) sx * (double) sy2 -
			            (double) sy * (double) sxy) / cx;
			retta->b = ((double) sx2 * (double) sy -
			            (double) sx * (double) sxy) / cy;
			retta->c = (double) sxy * (double) sxy -
			           (double) sx2 * (double) sy2;
		}
		break;
		case 2: { /* int */
			short j;
			double sx=0.0, sx2=0.0, sy=0.0, sy2=0.0, sxy=0.0, ausx, ausy;
			int *px = (int *) asc, *py = (int *) ord;

			for(j=n_punti-2; j>0 && !flag_diversi; j--) {
				if(px[j] != px[j+1])
					flag_diversi = 1;
				else if(py[j] != py[j+1])
					flag_diversi = 1;
			}
			if(!flag_diversi)
				return SEAV_ERR_NOT_FOUND;
			for(j=n_punti; j>0; j--) {
				ausx = (double) *px++; ausy = (double) *py++;
				sx += ausx; sy += ausy;
				sx2 += ausx * ausx; sy2 += ausy * ausy; sxy += ausx * ausy;
			}
			retta->a = (sx * sy2 - sy * sxy) / cx;
			retta->b = (sx2 * sy - sx * sxy) / cy;
			retta->c = sxy * sxy - sx2 * sy2;
		}
		break;
		case 3: { /* long */
			short j;
			double sx=0.0, sx2=0.0, sy=0.0, sy2=0.0, sxy=0.0, ausx, ausy;
			long *px = (long *) asc, *py = (long *) ord;

			for(j=n_punti-2; j>0 && !flag_diversi; j--) {
				if(px[j] != px[j+1])
					flag_diversi = 1;
				else if(py[j] != py[j+1])
					flag_diversi = 1;
			}
			if(!flag_diversi)
				return SEAV_ERR_NOT_FOUND;
			for(j=n_punti; j>0; j--) {
				ausx = (double) *px++; ausy = (double) *py++;
				sx += ausx; sy += ausy;
				sx2 += ausx * ausx; sy2 += ausy * ausy; sxy += ausx * ausy;
			}
			retta->a = (sx * sy2 - sy * sxy) / cx;
			retta->b = (sx2 * sy - sx * sxy) / cy;
			retta->c = sxy * sxy - sx2 * sy2;
		}
		break;
		case 4: { /* float */
			short j;
			double sx=0.0, sx2=0.0, sy=0.0, sy2=0.0, sxy=0.0, ausx, ausy;
			float *px = (float *) asc, *py = (float *) ord;

			for(j=n_punti-2; j>0 && !flag_diversi; j--) {
				if(px[j] != px[j+1])
					flag_diversi = 1;
				else if(py[j] != py[j+1])
					flag_diversi = 1;
			}
			if(!flag_diversi)
				return SEAV_ERR_NOT_FOUND;
			for(j=n_punti; j>0; j--) {
				ausx = (double) *px++; ausy = (double) *py++;
				sx += ausx; sy += ausy;
				sx2 += ausx * ausx; sy2 += ausy * ausy; sxy += ausx * ausy;
			}
			retta->a = (sx * sy2 - sy * sxy) / cx;
			retta->b = (sx2 * sy - sx * sxy) / cy;
			retta->c = sxy * sxy - sx2 * sy2;
		}
		break;
		case 5: { /* double */
			short j;
			double sx=0.0, sx2=0.0, sy=0.0, sy2=0.0, sxy=0.0, ausx, ausy;
			double *px = (double *) asc, *py = (double *) ord;

			for(j=n_punti-2; j>0 && !flag_diversi; j--) {
				if(px[j] != px[j+1])
					flag_diversi = 1;
				else if(py[j] != py[j+1])
					flag_diversi = 1;
			}
			if(!flag_diversi)
				return SEAV_ERR_NOT_FOUND;
			for(j=n_punti; j>0; j--) {
				ausx = *px++; ausy = *py++;
				sx += ausx; sy += ausy;
				sx2 += ausx * ausx; sy2 += ausy * ausy; sxy += ausx * ausy;
			}
			retta->a = (sx * sy2 - sy * sxy) / cx;
			retta->b = (sx2 * sy - sx * sxy) / cy;
			retta->c = sxy * sxy - sx2 * sy2;
		}
		break;
	}

	return SEAV_OK_ALL;
}

