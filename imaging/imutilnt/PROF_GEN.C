


/***********************************************************************

;  Nome:    
     imut_profile_gen
;
;  Funzione:
     calcola il profilo lungo una direzione arbitraria
;
;  Formato:
     short imut_profile_gen(finestra, xl, yl, angolo, step_col, step_row
	                       profilo, n_punti_utili)
;
;  Argomenti:    
     unsigned char **finestra  matrice contenente la finestra letta dalla
	                       memoria video
	 short xl              larghezza della finestra
	 short yl              altezza della finestra
	 double angolo         direzione del profilo, in radianti da 0 a pigreco
	 short step_col        distanza tra le colonne che compongono il profilo;
	                       determina il numero di punti utili del profilo
	 short step_row        distanza tra le righe che compongono il profilo;
	                       determina il numero di punti sommati in ogni valore
						   del profilo
     long *profilo         vettore contenente il profilo calcolato dalla routine
	 short *n_punti_utili  numero di punti di cui si compone il profilo

;
;  Valore di ritorno: 
     SEAV_OK_ALL              nessun errore
	 SEAV_ERR_*               codice di errore

;
;  Descrizione:
     {\bf imut_profile_gen}() calcola il profilo di una area di
	 interesse lungo una
	 direzione arbitraria determinata dal valore di "angolo";
	 il profilo e' memorizzato nel vettore "profilo" a partire dal primo
	 elemento; il numero di punti che lo compongono e' restituito nella
	 variabile "*n_punti_utili"; il profilo puo' essere calcolato con sotto-
	 campionamento separato nelle due direzioni assegnando ai parametri
	 "step_col" e "step_row" valori maggiori di 1; le distanze "step_col" e
	 "step_row" sono da intendersi euclidee e si riferiscono rispettivamente
	 alle direzioni ortogonale e parallela a quella determinata dal valore
	 di "angolo";
;
;  Note:
     il profilo non viene normalizzato;
;
;**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <imutilnt.h>


/* prototipi delle funzioni private */

static long un_valore_del_profilo(
	short xi, short yi, short xf, short yf, unsigned char **finestra,
	short *x_vet, short *y_vet, short lung, short n_punti
);
static short calcola_coordinate(
	short *x_vet, short *y_vet, short lung, double angolo, short step
);

/**********************************************************************/

short
imut_profile_gen(
	unsigned char **finestra,
	short xl,
	short yl,
	double angolo,
	short step_col,
	short step_row,
	long *profilo,
	short *n_punti_utili
)
{
	short i, param, n_punti;
	double xi, yi, xf, yf, id, jd;
	double pi = acos(-1.0);
	double pi2 = pi / 2.0;
	double pi4 = pi / 4.0;
	double pi34 = 3.0 * pi4;
	double seno = sin(angolo);
	double coseno = cos(angolo);
	double tangente;
	double step_x, step_y; /* passi in orizzontale e verticale */
	double xld = (double) xl;
	double xld1 = xld - 1.0;
	double yld = (double) yl;
	double yld1 = yld - 1.0;
	double offx, offy;
	short *x_vet=NULL, *y_vet=NULL;
	short lung_max = max(xl, yl);

/* controllo dei parametri */

	if(xl < 1 || yl < 1 || angolo < 0.0 || angolo > pi)
		return SEAV_ERR_PARAMETER;
	if(step_col < 1 || step_row < 1)
		return SEAV_ERR_PARAMETER;

/* allocazione di memoria */

	x_vet = (short *) malloc((size_t) lung_max * sizeof(short));
	if(x_vet == NULL)
		return SEAV_ERR_MEMORY;
	y_vet = (short *) malloc((size_t) lung_max * sizeof(short));
	if(y_vet == NULL) {
		free(x_vet);
		return SEAV_ERR_MEMORY;
	}

/* calcolo del numero di punti utili del profilo */

	if(angolo < pi34 && angolo >= pi4) { /* parametrizzazione in x */
		param = 0;
		tangente = fabs(tan(pi2-angolo));
		offx = yld1 * tangente;
		step_x = (double) step_col / seno;
		*n_punti_utili = (short) ((yld * tangente + xld) / step_x);
	} else {                            /* parametrizzazione in y */
		param = 1;
		tangente = fabs(tan(angolo));
		offy = xld1 * tangente;
		step_y = (double) step_col / fabs(coseno);
		*n_punti_utili = (short) ((xld * tangente + yld) / step_y);
	}

/* azzeramento del vettore */

	memset(profilo, 0, (size_t) (*n_punti_utili * sizeof(long)));

/* calcolo dei vettori delle coordinate */

	n_punti = calcola_coordinate(x_vet, y_vet, lung_max, angolo, step_row);

/* calcolo del profilo */

	for(i=0, id=0.0, jd=0.0; i<*n_punti_utili; i++, id+=step_y, jd+=step_x) {

/* calcolo dei punti iniziale e finale del singolo segmento lungo il quale si
   calcola il profilo */

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

/* calcolo di un valore del profilo lungo il segmento in corso */

		profilo[i] = un_valore_del_profilo(
			(short) xi, (short) yi, (short) xf, (short) yf, finestra,
			x_vet, y_vet, lung_max, n_punti
		);
	}

	free(x_vet);
	free(y_vet);
	return SEAV_OK_ALL;
}

/**********************************************************************/

static long
un_valore_del_profilo(
	short xi,
	short yi,
	short xf,
	short yf,
	unsigned char **finestra,
	short *x_vet,
	short *y_vet,
	short lung,
	short n_punti
)
{
	short i, n_scand, diffy, diffx, *ausx, *ausy;
	long valore = 0L;

/* calcolo quanti punti dei vettori devo scandire */

	if((diffy = abs(yf-yi+1)) > (diffx = abs(xf-xi+1)))
	                          /* parametrizzazione in y */
		n_scand = (short) ((double) diffy * ((double) n_punti / (double) lung));
	else                      /* parametrizzazione in x */
		n_scand = (short) ((double) diffx * ((double) n_punti / (double) lung));

/* calcolo del valore del profilo */

	ausy = y_vet;
	ausx = x_vet;
	for(i=n_scand-7; i>0; i-=8) {
		valore += (long) finestra[*ausy++ + yi][*ausx++ + xi];
		valore += (long) finestra[*ausy++ + yi][*ausx++ + xi];
		valore += (long) finestra[*ausy++ + yi][*ausx++ + xi];
		valore += (long) finestra[*ausy++ + yi][*ausx++ + xi];
		valore += (long) finestra[*ausy++ + yi][*ausx++ + xi];
		valore += (long) finestra[*ausy++ + yi][*ausx++ + xi];
		valore += (long) finestra[*ausy++ + yi][*ausx++ + xi];
		valore += (long) finestra[*ausy++ + yi][*ausx++ + xi];
	}
	for(; i>-7; i--) {
		valore += (long) finestra[*ausy++ + yi][*ausx++ + xi];
	}

	return valore;
}

/**********************************************************************/

static short
calcola_coordinate(
	short *x_vet,
	short *y_vet,
	short lung,
	double angolo,
	short step /* distanza tra i punti */
)
{
	short i, j, n_punti = 0;
	double coeff; /* coefficiente angolare della retta */
	double id, jd;
	short xi=0, yi=0, xf, yf;
	double pi = acos(-1.0);
	double pi2 = pi / 2.0;
	double pi4 = pi / 4.0;
	double pi34 = 3.0 * pi4;

/* calcolo del punto finale */

	if(angolo < pi34 && angolo >= pi4) { /* parametrizzazione in y */
		yf = lung-1;
		xf = (short) ((double) yf * tan(pi2-angolo));
	} else {                             /* parametrizzazione in x */
		if(angolo < pi2)
			xf = lung-1;
		else
			xf = 1-lung;
		yf = (short) ((double) xf * tan(angolo));
	}

/* caso degenere: 1 punto */

	if(xi == xf && yi == yf) {
		x_vet[n_punti] = xi;
		y_vet[n_punti++] = yi;
		return n_punti;
	}

/* caso generale */

	if(abs(yf-yi) > abs(xf-xi)) { /* parametrizzazione in y */
		coeff = (double) step * ((double) (xf - xi) / (double) (yf - yi));
		if(yf > yi){	
			for(i=yi, jd = (double) xi; i<=yf; i+=step, jd+=coeff) {
				x_vet[n_punti] = (short) jd;
				y_vet[n_punti++] = i;
			}
		} else {
			for(i=yi, jd = (double) xi; i>=yf; i-=step, jd-=coeff) {
				x_vet[n_punti] = (short) jd;
				y_vet[n_punti++] = i;
			}
		}
	} else { /* parametrizzazione in x */
		coeff = (double) step * ((double) (yf - yi) / (double) (xf - xi));
		if(xf > xi){	
			for(j=xi, id = (double) yi; j<=xf; j+=step, id+=coeff) {
				x_vet[n_punti] = j;
				y_vet[n_punti++] = (short) id;
			}
		} else {
			for(j=xi, id = (double) yi; j>=xf; j-=step, id-=coeff) {
				x_vet[n_punti] = j;
				y_vet[n_punti++] = (short) id;
			}
		}
	}

	return n_punti;
}

