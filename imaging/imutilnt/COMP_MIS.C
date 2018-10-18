

/***********************************************************************

;  Nome:    
     imut_conn_measure
;
;  Funzione:
     calcola i descrittori di una componente connessa
;
;  Formato:
     short imut_conn_measure(roi, livello, momenti, invarianti,
                                 matrice, nr, nc)
;
;  Argomenti:    
     ROI *roi        regione contenente la componente
         short livello   livello che identifica la componente all'interno della
                         regione, se livello=-1 sono considerati appartenenti alla
                                         componente tutti i punti diversi da 0
         MOMENTS *momenti        momenti calcolati
         INVARIANTS *invarianti  invarianti calcolati
         long **matrice          matrice a risoluzione ridotta calcolata
         short nr                numero di righe di matrice
         short nc                numero di colonne di matrice

;
;  Valore di ritorno: 
     OK_ALL        nessun errore
     SEAV_OK_ALL              nessun errore
         SEAV_ERR_*               codice di errore

;
;  Descrizione:
     {\bf imut_conn_measure}() calcola i descrittori di una componente
         connessa contenuta nella regione assegnata e identificata dal
         livello dato (se livello vale -1 tutti i punti diversi da 0 sono
         considerati appartenere alla componente); se la componente si
         estende anche al di fuori della regione, i punti esterni alla
         regione sono ignorati; i descrittori sono: i
         momenti (fino al terzo ordine), gli invarianti calcolati sui
         momenti, la matrice a risoluzione ridotta i cui valori sono
         ottenuti con medie locali dei punti della regione;
         la matrice a risoluzione ridotta e' calcolata mappando su nr e nc
         la sottoregione che effettivamente contiene la componente; nr e nc
         non possono essere minori della regione che contiene la componente;
         ai fini del calcolo della matrice i punti che appartengono alla
         componente hanno peso WEIGHT_P, gli altri 0; in tal modo il valore
         massimo che un punto della matrice puo' assumere e' WEIGHT_P;
;
;**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <imutilnt.h>
#ifndef __WATCOMC__
	#include <windows.h>
#endif

short
imut_conn_measure(
        ROI *roi,
        short livello,
        MOMENTS *momenti,
        INVARIANTS *invarianti,
        long **matrice,
        short nr,
        short nc
)
{
        unsigned char *riga=NULL;
        short fb, xi, yi, xf, yf;
        short i, j, x_min, x_max, y_min, y_max;
        short n_pixel_riga, n_pixel_colonna;
        PARAM_SCHEDA param;
        long m00=0L, m10=0L, m01=0L;
        double x_bari, y_bari, norm2, norm3;
        double m11=0.0, m20=0.0, m02=0.0;
        double m12=0.0, m21=0.0, m30=0.0, m03=0.0;
        double *tab1x=NULL, *tab2x=NULL, *tab3x=NULL;
        double *tab1y=NULL, *tab2y=NULL, *tab3y=NULL;
        double aus1, aus2, aus3, aus4, aus5, aus6, aus55, aus66;
        double nprd, npcd, n_pixel_per_puntod;
        short nprs, npcs, ii, jj, prima_riga, prima_colonna;

/* controllo dei parametri */

        sea2_init_config(&param);
        if(imut_check_roi(roi, &param) != SEAV_OK_ALL)
                return SEAV_ERR_PARAMETER;
        fb = roi->fb;
        xi = roi->xi; yi = roi->yi;
        xf = roi->xf; yf = roi->yf;
        n_pixel_riga = xf - xi + 1;
        n_pixel_colonna = yf - yi + 1;

/* allocazione di memoria */

        riga = (unsigned char *) malloc(n_pixel_riga);
        if(riga == NULL)
                return SEAV_ERR_MEMORY;

/* individuo la sottoregione che contiene la componente e ne calcolo
   area e baricentro */

        sea2_seleziona_buffer(fb);
        x_min = n_pixel_riga; y_min = n_pixel_colonna;
        x_max = 0; y_max = 0;
        for(i=n_pixel_colonna-1; i>=0; i--) {
                sea2_leggi_riga(xi, (short) (yi + i), n_pixel_riga, riga);
                if(livello >= 0) {
                        for(j=n_pixel_riga-1; j>=0; j--) {
                                if((short) riga[j] == livello) {
                                        m00 ++;
                                        m10 += (long) j;
                                        m01 += (long) i;
                                        if(i < y_min)
                                                y_min = i;
                                        if(i > y_max)
                                                y_max = i;
                                        if(j < x_min)
                                                x_min = j;
                                        if(j > x_max)
                                                x_max = j;
                                }
                        }
                } else {
                        for(j=n_pixel_riga-1; j>=0; j--) {
                                if(riga[j]) {
                                        m00 ++;
                                        m10 += (long) j;
                                        m01 += (long) i;
                                        if(i < y_min)
                                                y_min = i;
                                        if(i > y_max)
                                                y_max = i;
                                        if(j < x_min)
                                                x_min = j;
                                        if(j > x_max)
                                                x_max = j;
                                }
                        }
                }
        }
        if(!m00) { /* la roi e' vuota */
                free(riga);
                return SEAV_ERR_NOT_FOUND;
        }
        x_min += xi;
        x_max += xi;
        y_min += yi;
        y_max += yi;
        n_pixel_riga = x_max - x_min + 1;
        n_pixel_colonna = y_max - y_min + 1;
        if(/*n_pixel_riga < nc ||*/ n_pixel_colonna < nr) {
                /* la roi e' troppo piccola */
                free(riga);
                return SEAV_ERR_NOT_FOUND;
        }
        momenti->m00 = (double) m00;
        momenti->m10 = (double) m10 / (double) m00 + (double) xi;
        momenti->m01 = (double) m01 / (double) m00 + (double) yi;
        x_bari = momenti->m10 - (double) x_min;
        y_bari = momenti->m01 - (double) y_min;

/* calcolo le tabelle con le potenze */

        tab1x = (double *) malloc((size_t) n_pixel_riga * sizeof(double));
        tab2x = (double *) malloc((size_t) n_pixel_riga * sizeof(double));
        tab3x = (double *) malloc((size_t) n_pixel_riga * sizeof(double));
        tab1y = (double *) malloc((size_t) n_pixel_colonna * sizeof(double));
        tab2y = (double *) malloc((size_t) n_pixel_colonna * sizeof(double));
        tab3y = (double *) malloc((size_t) n_pixel_colonna * sizeof(double));
        if(tab1x == NULL || tab2x == NULL || tab3x == NULL ||
           tab1y == NULL || tab2y == NULL || tab3y == NULL) {
                free(riga);
                free(tab1x);
                free(tab2x);
                free(tab3x);
                free(tab1y);
                free(tab2y);
                free(tab3y);
                return SEAV_ERR_MEMORY;
        }
        for(j=n_pixel_riga-1; j>=0; j--) {
                tab1x[j] = (double) j - x_bari;
                tab2x[j] = tab1x[j] * tab1x[j];
                tab3x[j] = tab1x[j] * tab2x[j];
        }
        for(i=n_pixel_colonna-1; i>=0; i--) {
                tab1y[i] = (double) i - y_bari;
                tab2y[i] = tab1y[i] * tab1y[i];
                tab3y[i] = tab1y[i] * tab2y[i];
        }

/* calcolo dei momenti centrali normalizzati del secondo e terzo ordine */

        for(i=n_pixel_colonna-1; i>=0; i--) {
                sea2_leggi_riga(x_min, (short) (y_min + i), n_pixel_riga, riga);
                if(livello >= 0) {
                        for(j=n_pixel_riga-1; j>=0; j--) {
                                if((short) riga[j] == livello) {
                                        m11 += tab1x[j] * tab1y[i];
                                        m20 += tab2x[j];
                                        m02 += tab2y[i];
                                        m21 += tab2x[j] * tab1y[i];
                                        m12 += tab1x[j] * tab2y[i];
                                        m30 += tab3x[j];
                                        m03 += tab3y[i];
                                }
                        }
                } else {
                        for(j=n_pixel_riga-1; j>=0; j--) {
                                if(riga[j]) {
                                        m11 += tab1x[j] * tab1y[i];
                                        m20 += tab2x[j];
                                        m02 += tab2y[i];
                                        m21 += tab2x[j] * tab1y[i];
                                        m12 += tab1x[j] * tab2y[i];
                                        m30 += tab3x[j];
                                        m03 += tab3y[i];
                                }
                        }
                }
        }
        norm2 = momenti->m00 * momenti->m00;
        norm3 = pow(momenti->m00, 2.5);
        momenti->m11 = (double) m11 / norm2;
        momenti->m20 = (double) m20 / norm2;
        momenti->m02 = (double) m02 / norm2;
        momenti->m21 = (double) m21 / norm3;
        momenti->m12 = (double) m12 / norm3;
        momenti->m30 = (double) m30 / norm3;
        momenti->m03 = (double) m03 / norm3;

/* calcolo degli invarianti */

        invarianti->phi0 = momenti->m00;
        invarianti->phi1 = momenti->m20 + momenti->m02;
        aus1 = momenti->m20 - momenti->m02;
        aus2 = momenti->m11 + momenti->m11;
        invarianti->phi2 = aus1 * aus1 + aus2 * aus2;
        aus3 = momenti->m30 - 3.0 * momenti->m12;
        aus4 = momenti->m03 - 3.0 * momenti->m21;
        aus5 = momenti->m30 + momenti->m12;
        aus55 = aus5 * aus5;
        aus6 = momenti->m03 + momenti->m21;
        aus66 = aus6 * aus6;
        invarianti->phi3 = aus3 * aus3 + aus4 * aus4;
        invarianti->phi4 = aus55 + aus66;
        invarianti->phi5 = aus3 * aus5 * (aus55 - 3.0 * aus66) +
                                           aus4 * aus6 * (aus66 - 3.0 * aus55);
        invarianti->phi6 = aus1 * (aus55 - aus66) + 2.0 * aus2 * aus5 * aus6;
        invarianti->phi7 = aus4 * aus5 * (3.0 * aus66 - aus55) +
                                           aus3 * aus6 * (aus66 - 3.0 * aus55);

/* calcolo della matrice a risoluzione ridotta */

        nprd = (double) n_pixel_riga / (double) nc;
        npcd = (double) n_pixel_colonna / (double) nr;
        nprs = (short) ceil(nprd);
        npcs = (short) ceil(npcd);
        n_pixel_per_puntod = (double) nprs * (double) npcs;
        for(ii=0; ii<nr; ii++)
                for(jj=0; jj<nc; jj++)
                        matrice[ii][jj] = 0L;

        for(ii=0; ii<nr; ii++) {
                prima_riga = (short) ((double) ii * npcd);
                while(prima_riga + npcs >= n_pixel_colonna)
                        prima_riga --;
                for(i=0; i<npcs; i++) {
                        sea2_leggi_riga(
                                x_min, (short) (y_min + prima_riga + i), n_pixel_riga, riga
                        );
                        for(jj=0; jj<nc; jj++) {
                                prima_colonna = (short) ((double) jj * nprd);
                                while(prima_colonna + nprs >= n_pixel_riga)
                                        prima_colonna --;
                                if(livello >= 0) {
                                        for(j=0; j<nprs; j++) {
                                                if((short) riga[prima_colonna + j] == livello)
                                                        matrice[ii][jj] ++;
                                        }
                                } else {
                                        for(j=0; j<nprs; j++) {
                                                if(riga[prima_colonna + j])
                                                        matrice[ii][jj] ++;
                                        }
                                }
                        }
                }
        }
        for(ii=0; ii<nr; ii++)
                for(jj=0; jj<nc; jj++)
                        matrice[ii][jj] = (long) (
                                ((double) matrice[ii][jj] * WEIGHT_P) / n_pixel_per_puntod
                        );

/* uscita */

        free(tab3y);
        free(tab2y);
        free(tab1y);
        free(tab3x);
        free(tab2x);
        free(tab1x);
        free(riga);
        return SEAV_OK_ALL;
}

