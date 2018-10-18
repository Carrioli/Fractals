

/***********************************************************************

;  Nome:    
     imut_intersection
;
;  Funzione:
     verifica l'intersezione di due ROI
;
;  Formato:
     short imut_intersection(roi1, roi2, roi_o, op)
;
;  Argomenti:    
     ROI *roi1      regione di ingresso
     ROI *roi2      regione di ingresso

;
;  Valore di ritorno: 
     0             l'intersezione tra le ROI e' vuota
	 1             le ROI hanno qualche punto in comune ma non tutti
	 2             la prima ROI e' contenuta nella seconda
	 3             la seconda ROI e' contenuta nella prima
	 4             le due ROI coincidono
	 SEAV_ERR_*               codice di errore

;
;  Descrizione:
     {\bf imut_intersection}() verifica l'intersezione tra le ROI;

;
;**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <imutilnt.h>

static short contenuto_in(short x, short y, ROI *roi);

/**********************************************************************/

short
imut_intersection(
	ROI *roi1,
	ROI *roi2
)
{
	PARAM_SCHEDA param;

/* controllo delle roi */

	sea2_init_config(&param);
	if(imut_check_roi(roi1, &param) != SEAV_OK_ALL)
		return SEAV_ERR_PARAMETER;
	if(imut_check_roi(roi2, &param) != SEAV_OK_ALL)
		return SEAV_ERR_PARAMETER;

	if(roi1->fb != roi2->fb)
		return 0;
	if(roi1->yi > roi2->yf || roi2->yi > roi1->yf)
		return 0;
	if(roi1->xi > roi2->xf || roi2->xi > roi1->xf)
		return 0;
	if(roi1->xi == roi2->xi && roi1->yi == roi2->yi &&
	   roi1->xf == roi2->xf && roi1->yf == roi2->yf)
		return 4;
	if(contenuto_in(roi1->xi, roi1->yi, roi2) &&
	   contenuto_in(roi1->xf, roi1->yf, roi2))
		return 2;
	if(contenuto_in(roi2->xi, roi2->yi, roi1) &&
	   contenuto_in(roi2->xf, roi2->yf, roi1))
		return 3;
	return 1;
}

/**********************************************************************/

/* ritorna 1 se il punto appartiene alla ROI, 0 altrimenti */

static short
contenuto_in(
	short x,
	short y,
	ROI *roi
)
{
	if(x < roi->xi)
		return 0;
	if(x > roi->xf)
		return 0;
	if(y < roi->yi)
		return 0;
	if(y > roi->yf)
		return 0;
	return 1;
}

