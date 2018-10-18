


/***********************************************************************

;  Nome:    
     util_calcola_minimo2
;
;  Funzione:
     calcola il minimo di un vettore
;
;  Formato:
     long util_calcola_minimo2(vet, n_punti, tipo, pos)
;
;  Argomenti:    
     void *vet             vettore del quale calcolare il minimo
     short n_punti         lunghezza di vet
     short tipo            0: vet e' composto da unsigned char, 1: da short,
                               2: da int, 3: da long
         short *pos            indice del minimo trovato

;
;  Valore di ritorno: 
     minimo               minimo del vettore

;
;  Descrizione:
     {\bf util_calcola_minimo2}() calcola il valore minimo di un vettore
         di interi di formato arbitrario; inoltre, a differenza di
     {\bf util_calcola_minimo}(), ritorna la posizione del minimo;
;
;**********************************************************************/

#include <util_nt.h>

long
util_calcola_minimo2(
        void *vet,
        short n_punti,
        short tipo,
        short *pos
)
{
        short i;
        if(n_punti <= 0)
                return (long) SEAV_ERR_PARAMETER;

        switch(tipo) {
                case 0: {
                        unsigned char *aus_p = (unsigned char *) vet;
                        unsigned char minimo = aus_p[n_punti-1];
                        short posiz = n_punti-1;

                        for(i=n_punti-2; i>=0; i--) {
                                if(aus_p[i] < minimo) {
                                        minimo = aus_p[i];
                                        posiz = i;
                                }
                        }
                        *pos = posiz;
                        return (long) minimo;
                        break;
                }
                case 1: {
                        short *aus_p = (short *) vet;
                        short minimo = aus_p[n_punti-1];
                        short posiz = n_punti-1;

                        for(i=n_punti-2; i>=0; i--) {
                                if(aus_p[i] < minimo) {
                                        minimo = aus_p[i];
                                        posiz = i;
                                }
                        }
                        *pos = posiz;
                        return (long) minimo;
                        break;
                }
                case 2: {
                        int *aus_p = (int *) vet;
                        int minimo = aus_p[n_punti-1];
                        short posiz = n_punti-1;

                        for(i=n_punti-2; i>=0; i--) {
                                if(aus_p[i] < minimo) {
                                        minimo = aus_p[i];
                                        posiz = i;
                                }
                        }
                        *pos = posiz;
                        return (long) minimo;
                        break;
                }
                case 3: {
                        long *aus_p = (long *) vet;
                        long minimo = aus_p[n_punti-1];
                        short posiz = n_punti-1;

                        for(i=n_punti-2; i>=0; i--) {
                                if(aus_p[i] < minimo) {
                                        minimo = aus_p[i];
                                        posiz = i;
                                }
                        }
                        *pos = posiz;
                        return minimo;
                        break;
                }
        }
        return 0;
}


