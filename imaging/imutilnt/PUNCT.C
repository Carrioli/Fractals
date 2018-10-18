

/***********************************************************************

;  Nome:    
     imut_punctual_operation
;
;  Funzione:
     effettua operazioni puntuali tra ROI
;
;  Formato:
     short imut_punctual_operation(roi1, roi2, roi_o, op)
;
;  Argomenti:    
     ROI *roi1      regione di ingresso
     ROI *roi2      regione di ingresso
     ROI *roi_o     regione di uscita
	 short op       operazione da compiere

;
;  Valore di ritorno: 
     SEAV_OK_ALL              nessun errore
	 SEAV_ERR_*               codice di errore

;
;  Descrizione:
     {\bf imut_punctual_operation}() effettua operazioni puntuali tra
	 due regioni di interesse ponendo il risultato nella regione di uscita;
	 le ROI di uscita puo' avere punti in comune con una sola delle ROI
	 di ingresso;
	 le tre ROI devono avere le stesse dimensioni;
	 l'operazione effettuata dipende dal valore di op:
	 0: massimo
	 1: minimo
	 2: somma modulo 256
	 3: somma con massimo consentito pari a 255
	 4: differenza modulo 256 (roi1 - roi2)
	 5: differenza con minimo consentito pari a 0 (roi1 - roi2)
	 6: valore assoluto della differenza
	 7: AND
	 8: OR
	 9: EXOR

;
;**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <imutilnt.h>

short
imut_punctual_operation(
	ROI *roi1,
	ROI *roi2,
	ROI *roi_o,
	short op
)
{
	unsigned char *riga0=NULL, *riga1=NULL;
	short fb1, xi1, yi1, xf1, yf1;
	short fb2, xi2, yi2, xf2, yf2;
	short fb_o, xi_o, yi_o, xf_o, yf_o;
	short i, j, primo, ultimo, incr, aus;
	short n_pixel_riga, n_pixel_colonna;
	PARAM_SCHEDA param;

/* controllo dei parametri */

	sea2_init_config(&param);
	if(imut_check_roi(roi1, &param) != SEAV_OK_ALL)
		return SEAV_ERR_PARAMETER;
	if(imut_check_roi(roi2, &param) != SEAV_OK_ALL)
		return SEAV_ERR_PARAMETER;
	if(imut_check_roi(roi_o, &param) != SEAV_OK_ALL)
		return SEAV_ERR_PARAMETER;

	fb1 = roi1->fb;
	xi1 = roi1->xi; yi1 = roi1->yi;
	xf1 = roi1->xf; yf1 = roi1->yf;
	fb2 = roi2->fb;
	xi2 = roi2->xi; yi2 = roi2->yi;
	xf2 = roi2->xf; yf2 = roi2->yf;
	fb_o = roi_o->fb;
	xi_o = roi_o->xi; yi_o = roi_o->yi;
	xf_o = roi_o->xf; yf_o = roi_o->yf;
	n_pixel_riga = xf1 - xi1 + 1;
	n_pixel_colonna = yf1 - yi1 + 1;

	if(xf1 - xi1 != xf_o - xi_o)
		return SEAV_ERR_PARAMETER;
	if(yf1 - yi1 != yf_o - yi_o)
		return SEAV_ERR_PARAMETER;
	if(xf1 - xi1 != xf2 - xi2)
		return SEAV_ERR_PARAMETER;
	if(yf1 - yi1 != yf2 - yi2)
		return SEAV_ERR_PARAMETER;

	if(imut_intersection(roi1, roi_o) && imut_intersection(roi2, roi_o))
		return SEAV_ERR_PARAMETER;

/* decido il verso di scansione */

	if(imut_intersection(roi1, roi_o)) {
		if(yi1 > yi_o) { /* la scansione parte dall'alto */
			primo = 0;
			ultimo = n_pixel_colonna;
			incr = 1;
		} else {         /* la scansione parte dal basso */ 
			primo = n_pixel_colonna - 1;
			ultimo = -1;
			incr = -1;
		}
	} else {
		if(yi2 > yi_o) { /* la scansione parte dall'alto */
			primo = 0;
			ultimo = n_pixel_colonna;
			incr = 1;
		} else {         /* la scansione parte dal basso */ 
			primo = n_pixel_colonna - 1;
			ultimo = -1;
			incr = -1;
		}
	}

/* allocazione di memoria */

	riga0 = (unsigned char *) malloc(n_pixel_riga);
	riga1 = (unsigned char *) malloc(n_pixel_riga);
	if(riga0 == NULL || riga1 == NULL) {
		free(riga1);
		free(riga0);
		return SEAV_ERR_MEMORY;
	}

/* scansione */

	for(i=primo; i!=ultimo; i+=incr) {
		sea2_seleziona_buffer(fb1);
		sea2_leggi_riga(xi1, (short) (yi1 + i), n_pixel_riga, riga0);
		sea2_seleziona_buffer(fb2);
		sea2_leggi_riga(xi2, (short) (yi2 + i), n_pixel_riga, riga1);
		switch(op) {
			case 0: /* MAX */
				for(j=n_pixel_riga-1; j>=0; j--) {
					if(riga0[j] < riga1[j])
						riga0[j] = riga1[j];
				}
				break;
			case 1: /* MIN */
				for(j=n_pixel_riga-1; j>=0; j--) {
					if(riga0[j] > riga1[j])
						riga0[j] = riga1[j];
				}
				break;
			case 2: /* SOMMA modulo 256 */
				for(j=n_pixel_riga-1; j>=0; j--) {
					riga0[j] = (unsigned char) (
						(short) riga0[j] + (short) riga1[j]
					);
				}
				break;
			case 3: /* SOMMA SATURATA */
				for(j=n_pixel_riga-1; j>=0; j--) {
					if((aus = (short) riga0[j] + (short) riga1[j]) > 255)
						riga0[j] = (unsigned char) 255;
					else
						riga0[j] = (unsigned char) aus;
				}
				break;
			case 4: /* DIFFERENZA modulo 256 */
				for(j=n_pixel_riga-1; j>=0; j--) {
					riga0[j] = (unsigned char) (
						(short) riga0[j] - (short) riga1[j]
					);
				}
				break;
			case 5: /* DIFFERENZA SATURATA */
				for(j=n_pixel_riga-1; j>=0; j--) {
					if((aus = (short) riga0[j] - (short) riga1[j]) < 0)
						riga0[j] = (unsigned char) 0;
					else
						riga0[j] = (unsigned char) aus;
				}
				break;
			case 6: /* VALORE ASSOLUTO DELLA DIFFERENZA */
				for(j=n_pixel_riga-1; j>=0; j--) {
					if((aus = (short) riga0[j] - (short) riga1[j]) < 0)
						riga0[j] = (unsigned char) -aus;
					else
						riga0[j] = (unsigned char) aus;
				}
				break;
			case 7: /* AND */
				for(j=n_pixel_riga-1; j>=0; j--) {
					riga0[j] = (unsigned char) (
						(short) riga0[j] & (short) riga1[j]
					);
				}
				break;
			case 8: /* OR */
				for(j=n_pixel_riga-1; j>=0; j--) {
					riga0[j] = (unsigned char) (
						(short) riga0[j] | (short) riga1[j]
					);
				}
				break;
			case 9: /* EXOR */
				for(j=n_pixel_riga-1; j>=0; j--) {
					riga0[j] = (unsigned char) (
						(short) riga0[j] ^ (short) riga1[j]
					);
				}
				break;
		}
		sea2_seleziona_buffer(fb_o);
		sea2_scrivi_riga(xi_o, (short) (yi_o + i), n_pixel_riga, riga0);
	}

/* uscita */

	free(riga1);
	free(riga0);

	return SEAV_OK_ALL;
}

