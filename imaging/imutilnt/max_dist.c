

/***********************************************************************

;  Nome:    
     imut_max_dist_border
;
;  Funzione:
     calcola la distanza massima dal background
;
;  Formato:
     short imut_max_dist_border(roi, flag_conn)
;
;  Argomenti:    
     ROI roi      regione di interesse
	 short flag_conn    0:connettivita' 4, 1:connettivita' 8

;
;  Valore di ritorno: 
     n             distanza
	 SEAV_ERR_*               codice di errore

;
;  Descrizione:
     {\bf imut_max_dist_border}() calcola la massima distanza dal background;
	 sono considerati appartenenti al background soltanto i punti a 0
	 nella ROI di ingresso; la distanza puo' essere calcolata in metrica MAX
	 (flag_conn=1) oppure in metrica SUM (flag_conn=0);

;
;**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <imutilnt.h>


short
imut_max_dist_border(
	ROI *roi,
	short flag_conn
)
{
	unsigned short **dist_mat=NULL, v_min_vicini;
	unsigned char *riga=NULL;
	short fb, xi, yi, xf, yf;
	short n_pixel_riga, n_pixel_colonna, dist_max;
	short i, j;
	PARAM_SCHEDA param;

/* controllo dei parametri */

	sea2_init_config(&param);
	if(imut_check_roi(roi, &param) != SEAV_OK_ALL)
		return SEAV_ERR_PARAMETER;

	fb = roi->fb;
	xi = roi->xi;
	yi = roi->yi;
	xf = roi->xf;
	yf = roi->yf;
	n_pixel_riga = xf - xi + 1;
	n_pixel_colonna = yf - yi + 1;

/* allocazione di memoria */

	dist_mat = (unsigned short **) util_array2(
		(long) (n_pixel_colonna + 2), (long) (n_pixel_riga + 2),
		(long) sizeof(unsigned short)
	);
	riga = (unsigned char *) malloc(n_pixel_riga * sizeof(unsigned char));
	if(!dist_mat || !riga) {
		free(riga);
		util_free2((char **) dist_mat);
		return SEAV_ERR_MEMORY;
	}

/* azzero la cornice */

	for(i=0; i<n_pixel_colonna+2; i++) {
		dist_mat[i][0] = 0;
		dist_mat[i][n_pixel_riga + 1] = 0;
	}
	for(j=0; j<n_pixel_riga+2; j++) {
		dist_mat[0][j] = 0;
		dist_mat[n_pixel_colonna + 1][j] = 0;
	}

/* inizializzo i punti di dist_mat con 0xffff (massimo) in corrispondenza
   del foreground e 0 in corrispondenza del background */

	sea2_seleziona_buffer(fb);
	for(i=0; i<n_pixel_colonna; i++) {
		sea2_leggi_riga(xi, (short) (yi+i), n_pixel_riga, riga);
		for(j=0; j<n_pixel_riga; j++) {
			if(riga[j])
				dist_mat[i+1][j+1] = 0xffff;
			else
				dist_mat[i+1][j+1] = 0;
		}
	}

/* prima passata: dall'alto a sinistra */

	for(i=1; i<n_pixel_colonna+1; i++) {
		for(j=1; j<n_pixel_riga+1; j++) {
			if(dist_mat[i][j]) {
				v_min_vicini = dist_mat[i-1][j];
				if(dist_mat[i][j-1] < v_min_vicini)
					v_min_vicini = dist_mat[i][j-1];
				if(flag_conn == CONNECTIVITY_8) {
					if(dist_mat[i-1][j-1] < v_min_vicini)
						v_min_vicini = dist_mat[i-1][j-1];
					if(dist_mat[i-1][j+1] < v_min_vicini)
						v_min_vicini = dist_mat[i-1][j+1];
				}
				v_min_vicini++;
				if(v_min_vicini < dist_mat[i][j])
					dist_mat[i][j] = v_min_vicini;
			}
		}
	}

/* seconda passata: dal basso a destra */

	for(i=n_pixel_colonna; i>0; i--) {
		for(j=n_pixel_riga; j>0; j--) {
			if(dist_mat[i][j]) {
				v_min_vicini = dist_mat[i+1][j];
				if(dist_mat[i][j+1] < v_min_vicini)
					v_min_vicini = dist_mat[i][j+1];
				if(flag_conn == CONNECTIVITY_8) {
					if(dist_mat[i+1][j-1] < v_min_vicini)
						v_min_vicini = dist_mat[i+1][j-1];
					if(dist_mat[i+1][j+1] < v_min_vicini)
						v_min_vicini = dist_mat[i+1][j+1];
				}
				v_min_vicini++;
				if(v_min_vicini < dist_mat[i][j])
					dist_mat[i][j] = v_min_vicini;
			}
		}
	}

/* calcolo il valore massimo della trasformata */

	dist_max = 0;
	for(i=1; i<n_pixel_colonna+1; i++) {
		for(j=1; j<n_pixel_riga+1; j++) {
			if(dist_mat[i][j] > dist_max)
				dist_max = dist_mat[i][j];
		}
	}

/* uscita */

	free(riga);
	util_free2((char **) dist_mat);

	return dist_max;
}

