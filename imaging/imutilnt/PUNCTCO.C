

/***********************************************************************

;  Nome:    
     imut_roi_constant_operation
;
;  Funzione:
     effettua operazioni puntuali tra una ROI e una costante
;
;  Formato:
     short imut_roi_constant_operation(roi_i, roi_o, op)
;
;  Argomenti:    
     ROI *roi_i             regione di ingresso
     ROI *roi_o             regione di uscita
	 short op               operazione da compiere
	 unsigned char value    costante

;
;  Valore di ritorno: 
     SEAV_OK_ALL              nessun errore
	 SEAV_ERR_*               codice di errore

;
;  Descrizione:
     {\bf imut_roi_constant_operation}() effettua operazioni puntuale tra
	 una regione di interesse ed una costante; il risultato e' posto
	 nella regione di uscita;
	 le ROI di uscita puo' avere punti in comune con quella di ingresso;
	 le due ROI devono avere le stesse dimensioni;
	 l'operazione effettuata dipende dal valore di op:
	 0: massimo
	 1: minimo
	 2: somma modulo 256
	 3: somma con massimo consentito pari a 255
	 4: differenza modulo 256 (roi1 - value)
	 5: differenza con minimo consentito pari a 0 (roi1 - value)
	 6: valore assoluto della differenza
	 7: AND
	 8: OR
	 9: EXOR
	10: shift a destra di "value" bit
	11: shift a sinistra di "value" bit con massimo consentito pari a 255

;
;**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <imutilnt.h>


short
imut_roi_constant_operation(
	ROI *roi_i,
	ROI *roi_o,
	short op,
	unsigned char value
)
{
	unsigned char *riga=NULL;
	short fb_i, xi_i, yi_i, xf_i, yf_i;
	short fb_o, xi_o, yi_o, xf_o, yf_o;
	short i, j, primo, ultimo, incr, aus;
	short n_pixel_riga, n_pixel_colonna;
	PARAM_SCHEDA param;

/* controllo dei parametri */

	sea2_init_config(&param);
	if(imut_check_roi(roi_i, &param) != SEAV_OK_ALL)
		return SEAV_ERR_PARAMETER;
	if(imut_check_roi(roi_o, &param) != SEAV_OK_ALL)
		return SEAV_ERR_PARAMETER;
	fb_i = roi_i->fb;
	xi_i = roi_i->xi; yi_i = roi_i->yi;
	xf_i = roi_i->xf; yf_i = roi_i->yf;
	fb_o = roi_o->fb;
	xi_o = roi_o->xi; yi_o = roi_o->yi;
	xf_o = roi_o->xf; yf_o = roi_o->yf;
	n_pixel_riga = xf_i - xi_i + 1;
	n_pixel_colonna = yf_i - yi_i + 1;

	if(xf_i - xi_i != xf_o - xi_o)
		return SEAV_ERR_PARAMETER;
	if(yf_i - yi_i != yf_o - yi_o)
		return SEAV_ERR_PARAMETER;

/* decido il verso di scansione */

	if(yi_i > yi_o) { /* la scansione parte dall'alto */
		primo = 0;
		ultimo = n_pixel_colonna;
		incr = 1;
	} else {         /* la scansione parte dal basso */ 
		primo = n_pixel_colonna - 1;
		ultimo = -1;
		incr = -1;
	}

/* allocazione di memoria */

	riga = (unsigned char *) malloc(n_pixel_riga);
	if(riga == NULL)
		return SEAV_ERR_MEMORY;

/* scansione */

	for(i=primo; i!=ultimo; i+=incr) {
		sea2_seleziona_buffer(fb_i);
		sea2_leggi_riga(xi_i, (short) (yi_i + i), n_pixel_riga, riga);
		switch(op) {
			case 0: /* MAX */
				for(j=n_pixel_riga-1; j>=0; j--) {
					if(riga[j] < value)
						riga[j] = value;
				}
				break;
			case 1: /* MIN */
				for(j=n_pixel_riga-1; j>=0; j--) {
					if(riga[j] > value)
						riga[j] = value;
				}
				break;
			case 2: /* SOMMA modulo 256 */
				for(j=n_pixel_riga-1; j>=0; j--) {
					riga[j] = (unsigned char) (
						(short) riga[j] + (short) value
					);
				}
				break;
			case 3: /* SOMMA SATURATA */
				for(j=n_pixel_riga-1; j>=0; j--) {
					if((aus = (short) riga[j] + (short) value) > 255)
						riga[j] = (unsigned char) 255;
					else
						riga[j] = (unsigned char) aus;
				}
				break;
			case 4: /* DIFFERENZA modulo 256 */
				for(j=n_pixel_riga-1; j>=0; j--) {
					riga[j] = (unsigned char) (
						(short) riga[j] - (short) value
					);
				}
				break;
			case 5: /* DIFFERENZA SATURATA */
				for(j=n_pixel_riga-1; j>=0; j--) {
					if((aus = (short) riga[j] - (short) value) < 0)
						riga[j] = (unsigned char) 0;
					else
						riga[j] = (unsigned char) aus;
				}
				break;
			case 6: /* VALORE ASSOLUTO DELLA DIFFERENZA */
				for(j=n_pixel_riga-1; j>=0; j--) {
					if((aus = (short) riga[j] - (short) value) < 0)
						riga[j] = (unsigned char) -aus;
					else
						riga[j] = (unsigned char) aus;
				}
				break;
			case 7: /* AND */
				for(j=n_pixel_riga-1; j>=0; j--) {
					riga[j] = (unsigned char) (
						(short) riga[j] & (short) value
					);
				}
				break;
			case 8: /* OR */
				for(j=n_pixel_riga-1; j>=0; j--) {
					riga[j] = (unsigned char) (
						(short) riga[j] | (short) value
					);
				}
				break;
			case 9: /* EXOR */
				for(j=n_pixel_riga-1; j>=0; j--) {
					riga[j] = (unsigned char) (
						(short) riga[j] ^ (short) value
					);
				}
				break;
			case 10: /* SHIFT a DX */
				for(j=n_pixel_riga-1; j>=0; j--) {
					riga[j] = (unsigned char) (
						(short) riga[j] >> (short) value
					);
				}
				break;
			case 11: /* SHIFT a SX SATURATO */
				for(j=n_pixel_riga-1; j>=0; j--) {
					riga[j] = (unsigned char) (
						(short) riga[j] ^ (short) value
					);
					if((aus = (short) riga[j] << (short) value) > 255)
						riga[j] = (unsigned char) 255;
					else
						riga[j] = (unsigned char) aus;
				}
				break;
		}
		sea2_seleziona_buffer(fb_o);
		sea2_scrivi_riga(xi_o, (short) (yi_o + i), n_pixel_riga, riga);
	}

/* uscita */

	free(riga);

	return SEAV_OK_ALL;
}

