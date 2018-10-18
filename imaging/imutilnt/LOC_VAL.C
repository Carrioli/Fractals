


/***********************************************************************

;  Nome:    
     imut_local_value
;
;  Funzione:
     calcola un valore su una roi
;
;  Formato:
     short imut_local_value(roi, tipo_valore, valore)
;
;  Argomenti:    
     ROI *roi              regione di interesse
	 short tipo_valore     valore da calcolare: 0=v. medio, 1=v. max, 2=v. min
	 DWORD *valore         valore calcolato

;
;  Valore di ritorno: 
     SEAV_OK_ALL              nessun errore
	 SEAV_ERR_*               codice di errore

;
;  Descrizione:
     {\bf imut_local_value}() calcola un valore su una regione di interesse.
;
;**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <imutilnt.h>

short
imut_local_value(
	ROI *roi,
	short tipo_valore,
	long *valore
)
{
	PARAM_SCHEDA param;
	unsigned char *riga;
	short n_pixel_riga, n_pixel_colonna, i, j;
	long val = 0L;

/* controllo dei parametri */

	sea2_init_config(&param);
	if(imut_check_roi(roi, &param) != SEAV_OK_ALL)
		return SEAV_ERR_PARAMETER;

/* allocazione di memoria */

	n_pixel_riga = (short) (roi->xf - roi->xi + 1);
	n_pixel_colonna = (short) (roi->yf - roi->yi + 1);
	if(n_pixel_riga <= 0 || n_pixel_colonna <= 0)
		return SEAV_ERR_PARAMETER;
	riga = (unsigned char *) malloc(n_pixel_riga);
	if(riga == NULL)
		return SEAV_ERR_MEMORY;

/* calcolo del valore */

	if(tipo_valore == 2)
		val = 1000;
	sea2_seleziona_buffer(roi->fb);
	for(i=roi->yi; i<=roi->yf; i++) {
		sea2_leggi_riga(roi->xi, i, n_pixel_riga, riga);
		switch(tipo_valore) {
			case 0: // valor medio
				for(j=0; j<n_pixel_riga; j++)
					val += (long) riga[j];
				break;
			case 1: // valor max
				for(j=0; j<n_pixel_riga; j++)
					if((long) riga[j] > val)
						val = (long) riga[j];
				break;
			case 2: // valor min
				for(j=0; j<n_pixel_riga; j++)
					if((long) riga[j] < val)
						val = (long) riga[j];
				break;
		}
	}
	if(tipo_valore == 0)
		val /= (n_pixel_riga * n_pixel_colonna);

/* uscita */

	free(riga);
	*valore = val;
	return SEAV_OK_ALL;
}

