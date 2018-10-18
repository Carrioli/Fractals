


/***********************************************************************

;  Nome:    
     imut_stretch_lut
;
;  Funzione:
     trasforma una regione di interesse secondo una tabella lineare a tratti
;
;  Formato:
     short imut_stretch_lut(roi_i, roi_o, thl, thh, ll, lh)
;
;  Argomenti:    
     ROI *roi_i    regione di interesse da trasformare
     ROI *roi_o    regione trasformata
     short thl     valore della soglia inferiore (0..255)
     short thh     valore della soglia superiore (thl..255)
     short ll      livello inferiore (0..255)
     short lh      livello superiore (0..255)

;
;  Valore di ritorno: 
     SEAV_OK_ALL              nessun errore
	 SEAV_ERR_*               codice di errore

;
;  Descrizione:
     {\bf imut_stretch_lut}() trasforma puntualmente una regione di interesse
	 attraverso il mapping per il vettore "lut", che contiene
	 una funzione costante di valore "ll" da 0 a "thl", lineare da
	 "thl" a "thh", costante di valore "lh" da "thh" a 255;
;
;  Note:
     se "thl" coincide con "thh" la trasformazione diventa una binarizzazione
	 dell'immagine;
;
;**********************************************************************/

#include <imutilnt.h>

/* prototipi privati */

static void
calcola_stretch_lut(
	unsigned char *lut_corrente, short shift, short last,
	short livello_di_sinistra, short livello_di_destra
);

/******************************************************************************/

short
imut_stretch_lut(
	ROI *roi_i,
	ROI *roi_o,
	short thl,
	short thh,
	short ll,
	short lh
)
{
	unsigned char lut[256];

/* controllo dei parametri */

	if(thl < 0 || thl > 255)
		return SEAV_ERR_PARAMETER;
	if(thh < thl || thh > 255)
		return SEAV_ERR_PARAMETER;

/* inizializzazione della lut */

	calcola_stretch_lut(lut, thl, thh, ll, lh);

/* trasformazione */

	return imut_tras(roi_i, roi_o, lut);
}

/******************************************************************************/

static void
calcola_stretch_lut(
	unsigned char *lut_corrente,
	short shift,
	short last,
	short livello_di_sinistra,
	short livello_di_destra
)
{
	short i;
	double rapporto;

	for(i=0; i<shift; i++) {
		lut_corrente[i] = (unsigned char) livello_di_sinistra;
	}
	rapporto = (double)(livello_di_destra-livello_di_sinistra+1) /
	           (double) (last-shift+1);
	for(i=shift; i<=last; i++) {
		lut_corrente[i] = (unsigned char) ((double) (i-shift) * rapporto +
		                                   (double) livello_di_sinistra);
	}
	for(i=last+1; i<256; i++) {
		lut_corrente[i] = (unsigned char) livello_di_destra;
	}
}

