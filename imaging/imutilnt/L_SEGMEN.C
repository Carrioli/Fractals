


/***********************************************************************

;  Nome:    
     imut_leggi_segmento
;
;  Funzione:
     lettura di un segmento dalla memoria video
;
;  Formato:
     short imut_leggi_segmento(xi, yi, xf, yf, fb, vett, x_vet, y_vet)
;
;  Argomenti:    
     short xi              ascissa del punto iniziale
     short yi              ordinata del punto iniziale
     short xf              ascissa del punto finale
     short yf              ordinata del punto finale
     short fb              numero del frame buffer
     unsigned char *vett   vettore contenente i pixel letti
     short *x_vet          vettore contenente le ascisse dei pixel letti
     short *y_vet          vettore contenente le ordinate dei pixel letti

;
;  Valore di ritorno: 
     $n$                   numero di punti letti
         SEAV_ERR_*               codice di errore

;
;  Descrizione:
     {\bf imut_leggi_segmento}() legge un segmento dal frame buffer specificato;
     il punto iniziale ed il punto finale sono compresi;
         i vettori "x_vet" ed "y_vet" conterranno le coordinate dei pixel letti
         dall'immagine;
;
;**********************************************************************/

#include <math.h>
#include <stdlib.h>
#include <imutilnt.h>

short
imut_leggi_segmento(
        short xi,
        short yi,
        short xf,
        short yf,
        short fb,
        unsigned char *vett,
        short *x_vet,
        short *y_vet
)
{
        short i, j, n_pixel;
        double coeff; /* coefficiente angolare della retta */
        double id, jd;
        PARAM_SCHEDA param;

/* inizializzazione della struttura che descrive la scheda */

        sea2_init_config(&param);

/* controllo dei parametri */

        if(xi < param.x_min || xi > param.x_max ||
           yi < param.y_min || yi > param.y_max)
                return SEAV_ERR_PARAMETER;
        if(xf < param.x_min || xf > param.x_max ||
           yf < param.y_min || yf > param.y_max)
                return SEAV_ERR_PARAMETER;
        if(fb < param.fb_min || fb > param.fb_max)
                return SEAV_ERR_PARAMETER;

/* selezione del buffer */

        sea2_seleziona_buffer(fb);

/* caso degenere: 1 punto */

        if(xi == xf && yi == yf) {
                vett[0] = (unsigned char) sea2_leggi_pixel(xi, yi);
                x_vet[0] = xi;
                y_vet[0] = yi;
                return (short) 1;
        }

/* caso generale */

        if(abs(yf-yi) > abs(xf-xi)) { /* parametrizzazione in y */
                coeff = (double) (xf - xi) / (double) (yf - yi);
                if(yf > yi){    
                        n_pixel =  yf - yi + 1;
                        for(i=yi, jd = (double) xi; i<yf; i++, jd+=coeff){
                                *vett++ = (unsigned char) sea2_leggi_pixel(j=(short) jd, i);
                                *x_vet++ = j;
                                *y_vet++ = i;
                        }
                } else {
                        n_pixel =  yi - yf + 1;
                        for(i=yi, jd = (double) xi; i>yf; i--, jd-=coeff){
                                *vett++ = (unsigned char) sea2_leggi_pixel(j=(short) jd, i);
                                *x_vet++ = j;
                                *y_vet++ = i;
                        }
                }
        } else { /* parametrizzazione in x */
                coeff = (double) (yf - yi) / (double) (xf - xi);
                if(xf > xi){    
                        n_pixel = xf - xi + 1;
                        for(j=xi, id = (double) yi; j<xf; j++, id+=coeff){
                                *vett++ = (unsigned char) sea2_leggi_pixel(j, i=(short) id);
                                *x_vet++ = j;
                                *y_vet++ = i;
                        }
                } else {
                        n_pixel = xi - xf + 1;
                        for(j=xi, id = (double) yi; j>xf; j--, id-=coeff){
                                *vett++ = (unsigned char) sea2_leggi_pixel(j, i=(short) id);
                                *x_vet++ = j;
                                *y_vet++ = i;
                        }
                }
        }

/* per evitare guai con gli arrotondamenti forzo l'ultimo punto a (xf,yf) */

        *vett++ = (unsigned char) sea2_leggi_pixel(xf, yf);
        *x_vet++ = xf;
        *y_vet++ = yf;

        return n_pixel;
}

