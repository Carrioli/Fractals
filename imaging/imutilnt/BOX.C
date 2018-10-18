

/***********************************************************************

;  Nome:    
     imut_disegna_rettangolo
;
;  Funzione:
     disegno di una rettangolo di colore assegnato
;
;  Formato:
     short imut_disegna_rettangolo(roi, colore, flag_pieno)
;
;  Argomenti:    
     ROI *roi                   puntatore alla struttura che descrive
	                            l'area di interesse
     unsigned char colore       livello del rettangolo
	 short flag_pieno           0: viene disegnato solo il bordo del
	                            rettangolo, 1: il rettangolo viene riempito

;
;  Valore di ritorno: 
     SEAV_OK_ALL              nessun errore
	 SEAV_ERR_*               codice di errore

;
;  Descrizione:
     {\bf imut_disegna_rettangolo}() disegna un rettangolo di colore
	 assegnato sul frame buffer specificato;

;
;**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <imutilnt.h>

short
imut_disegna_rettangolo(
	ROI *roi,
	unsigned char colore,
	short flag_pieno
)
{
	short i;
	short lung_riga, lung_colonna, lung_max;
	unsigned char *buffer;
	PARAM_SCHEDA param;

/* controllo dei parametri */

	sea2_init_config(&param);
	if(imut_check_roi(roi, &param) != SEAV_OK_ALL)
		return SEAV_ERR_PARAMETER;
	lung_riga = roi->xf - roi->xi + 1;
	lung_colonna = roi->yf - roi->yi + 1;
	lung_max = max(lung_riga, lung_colonna);

/* allocazione di memoria */

	buffer = (unsigned char *) malloc((size_t) lung_max);
	if(buffer == NULL)
		return SEAV_ERR_MEMORY;

/* disegno */

	memset(buffer, (int) colore, (size_t) lung_max);
	sea2_seleziona_buffer(roi->fb);
	if(flag_pieno) {
		for(i=roi->yi; i<=roi->yf; i++)
			sea2_scrivi_riga(roi->xi, i, lung_riga, buffer);
	} else {
		sea2_scrivi_riga(roi->xi, roi->yi, lung_riga, buffer);
		sea2_scrivi_riga(roi->xi, roi->yf, lung_riga, buffer);
		sea2_scrivi_colonna(roi->xi, roi->yi, lung_colonna, buffer);
		sea2_scrivi_colonna(roi->xf, roi->yi, lung_colonna, buffer);
	}

	free(buffer);
	return SEAV_OK_ALL;
}

