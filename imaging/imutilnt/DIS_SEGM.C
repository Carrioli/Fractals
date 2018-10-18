


/***********************************************************************

;  Nome:    
     imut_disegna_segmenti
;
;  Funzione:
     disegna un insieme di segmenti paralleli
;
;  Formato:
     short imut_disegna_segmenti(roi, angolo, periodo, colore)
;
;  Argomenti:    
     ROI *roi              puntatore alla struttura che descrive
	                       l'area di interesse
	 double angolo         direzione dei segmenti, in radianti da 0 a pigreco
	 double periodo        distanza euclidea tra i segmenti, in pixel
	 unsigned char colore  livello di grigio dei segmenti

;
;  Valore di ritorno: 
     OK_ALL                nessun errore
     SEAV_OK_ALL              nessun errore
	 SEAV_ERR_*               codice di errore

;
;  Descrizione:
     {\bf imut_disegna_segmenti}() disegna all'interno della regione di
	 interesse della memoria video un insieme di segmenti paralleli
	 equispaziati di direzioni determinata dal valore di "angolo";
;
;**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <imutilnt.h>

static void
libera_memoria(
	unsigned char *vett,
	short *x_vet,
	short *y_vet
);

/**********************************************************************/

short
imut_disegna_segmenti(
	ROI *roi,
	double angolo,
	double periodo,
	unsigned char colore
)
{
	short i;
	double xi, yi, xf, yf, id, jd;
	double pi = acos(-1.0);
	double pi2 = pi / 2.0;
	double pi4 = pi / 4.0;
	double pi34 = 3.0 * pi4;
	double seno = sin(angolo);
	double coseno = cos(angolo);
	double tangente;
	short xl, yl;
	double step_x, step_y; /* passi in orizzontale e verticale */
	double xld, xld1, yld, yld1;
	double offx, offy;
	short n_punti_utili;
	unsigned char *vett = NULL;
	short *x_vet = NULL, *y_vet = NULL;
	PARAM_SCHEDA param;

/* inizializzazione della struttura che descrive la scheda */

	sea2_init_config(&param);

/* controllo dei parametri */

	if(angolo < 0.0 || angolo > pi)
		return SEAV_ERR_PARAMETER;
	if(imut_check_roi(roi, &param) != SEAV_OK_ALL)
		return SEAV_ERR_PARAMETER;
	xl = roi->xf - roi->xi + 1;
	yl = roi->yf - roi->yi + 1;
	xld = (double) xl;
	xld1 = xld - 1.0;
	yld = (double) yl;
	yld1 = yld - 1.0;

/* allocazione di memoria */

	vett = (unsigned char *) malloc(param.lung_max * sizeof(unsigned char));
	x_vet = (short *) malloc(param.lung_max * sizeof(short));
	y_vet = (short *) malloc(param.lung_max * sizeof(short));
	if(vett == NULL || x_vet == NULL || y_vet == NULL) {
		libera_memoria(vett, x_vet, y_vet);
		return SEAV_ERR_MEMORY;
	}
	memset(
		(unsigned char *) vett, (int) colore,
		param.lung_max * sizeof(unsigned char)
	);

/* calcolo del numero di punti utili del profilo */

	if(angolo < pi34 && angolo >= pi4) { /* parametrizzazione in x */
		tangente = fabs(tan(pi2-angolo));
		offx = yld1 * tangente;
		step_x = periodo / seno;
		n_punti_utili = (short) ((yld * tangente + xld) / step_x) + 1;
	} else {                             /* parametrizzazione in y */
		tangente = fabs(tan(angolo));
		offy = xld1 * tangente;
		step_y = periodo / fabs(coseno);
		n_punti_utili = (short) ((xld * tangente + yld) / step_y) + 1;
	}
#ifdef DEBUG
printf("tangente: %lf\n", tangente);
#endif

/* disegno dei segmenti */

	for(i=0, id=0.0, jd=0.0; i<n_punti_utili; i++, id+=step_y, jd+=step_x) {

/* calcolo dei punti iniziale e finale del singolo segmento da disegnare */

		if(angolo < pi4) {
			yi = yld1 - id;
			yf = yi + offy;
			if(yi < 0.0) {
				xi = -yi / tangente;
				yi = 0.0;
			} else {
				xi = 0.0;
			}
			if(yf > yld1) {
				xf = xld1 - (yf - yld1) / tangente;
				yf = yld1;
			} else {
				xf = xld1;
			}
		} else {
			if(angolo < pi2) {
				xi = xld1 - jd;
				xf = xi + offx;
				if(xi < 0.0) {
					yi = -xi / tangente;
					xi = 0.0;
				} else {
					yi = 0.0;
				}
				if(xf > xld1) {
					yf = yld1 - (xf - xld1) / tangente;
					xf = xld1;
				} else {
					yf = yld1;
				}
			} else {
				if(angolo < pi34) {
					xi = jd;
					xf = xi - offx;
					if(xi > xld1) {
						yi = (xi - xld1) / tangente;
						xi = xld1;
					} else {
						yi = 0.0;
					}
					if(xf < 0.0) {
						yf = xi / tangente;
						xf = 0.0;
					} else {
						yf = yld1;
					}
				} else {
					yi = yld1 - id;
					yf = yi + offy;
					if(yi < 0.0) {
						xi = xld1 + yi / tangente;
						yi = 0.0;
					} else {
						xi = xld1;
					}
					if(yf > yld1) {
						xf = (yf - yld1) / tangente;
						yf = yld1;
					} else {
						xf = 0.0;
					}
				}
			}
		}

#ifdef DEBUG
printf("xi: %lf yi: %lf xf: %lf yf: %lf\n", xi, yi, xf, yf);
#endif

/* disegno del segmento */

		(void) imut_scrivi_segmento(
			(short) ((short) xi + roi->xi), (short) ((short) yi + roi->yi),
			(short) ((short) xf + roi->xi), (short) ((short) yf + roi->yi), roi->fb,
			vett, x_vet, y_vet
		);

	}

	libera_memoria(vett, x_vet, y_vet);
	return SEAV_OK_ALL;
}

/**********************************************************************/

static void
libera_memoria(
	unsigned char *vett,
	short *x_vet,
	short *y_vet
)
{
	if(vett != NULL)
		free(vett);
	if(x_vet != NULL)
		free(x_vet);
	if(y_vet != NULL)
		free(y_vet);

	return;
}




