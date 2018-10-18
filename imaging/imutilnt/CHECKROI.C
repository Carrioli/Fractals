

/***********************************************************************

;  Nome:    
     imut_check_roi
;
;  Funzione:
     verifica la correttezza della ROI
;
;  Formato:
     short imut_check_roi(roi, param)
;
;  Argomenti:    
     ROI *roi              regione da controllare
     PARAM_SCHEDA *param   parametri della scheda in uso

;
;  Valore di ritorno: 
     SEAV_OK_ALL              nessun errore
	 SEAV_ERR_*               codice di errore

;
;  Descrizione:
     {\bf imut_check_roi}() verifica la correttezza della ROI;
	 in particolare controlla che il frame buffer e gli estremi della
	 ROI siano entro i limiti propri della scheda in uso; inoltre, se
	 gli estremi della ROI sono in ordine opposto (xi > xf o yi > yf)
	 essi vengono scambiati;

;
;**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <imutilnt.h>

short
imut_check_roi(
	ROI *roi,
	PARAM_SCHEDA *param
)
{
	short aus;

	if(roi->fb < param->fb_min || roi->fb > param->fb_max)
		return SEAV_ERR_PARAMETER;
	if(roi->xi < param->x_min || roi->xi > param->x_max)
		return SEAV_ERR_PARAMETER;
	if(roi->yi < param->y_min || roi->yi > param->y_max)
		return SEAV_ERR_PARAMETER;
	if(roi->xf < param->x_min || roi->xf > param->x_max)
		return SEAV_ERR_PARAMETER;
	if(roi->yf < param->y_min || roi->yf > param->y_max)
		return SEAV_ERR_PARAMETER;
	if(roi->xi > roi->xf) {
		aus = roi->xi;
		roi->xi = roi->xf;
		roi->xf = aus;
	}
	if(roi->yi > roi->yf) {
		aus = roi->yi;
		roi->yi = roi->yf;
		roi->yf = aus;
	}
	return SEAV_OK_ALL;
}

