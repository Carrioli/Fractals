


/***********************************************************************

;  Nome:    
     util_calcola_massimo2
;
;  Funzione:
     calcola il massimo di un vettore
;
;  Formato:
     long util_calcola_massimo2(vet, n_punti, tipo, pos)
;
;  Argomenti:    
     void *vet             vettore del quale calcolare il massimo
     short n_punti         lunghezza di vet
     short tipo            0: vet e' composto da unsigned char, 1: da short,
                               2: da int, 3: da long
         short *pos            indice del massimo trovato
;
;  Valore di ritorno: 
     massimo               massimo del vettore
         NON_TROVATO           se il numero di punti del vettore e' < 1

;
;  Descrizione:
     {\bf util_calcola_massimo2}() calcola il valore massimo di un vettore
         di interi di formato arbitrario; inoltre, a differenza di
     {\bf util_calcola_massimo}(), ritorna la posizione del massimo;
;
;**********************************************************************/

#include <util_nt.h>

long
util_calcola_massimo2(
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
                        unsigned char massimo = aus_p[n_punti-1];
                        short posiz = n_punti-1;

                        for(i=n_punti-2; i>=0; i--) {
                                if(aus_p[i] > massimo) {
                                        massimo = aus_p[i];
                                        posiz = i;
                                }
                        }
                        *pos = posiz;
                        return (long) massimo;
                        break;
                }
                case 1: {
                        short *aus_p = (short *) vet;
                        short massimo = aus_p[n_punti-1];
                        short posiz = n_punti-1;

                        for(i=n_punti-2; i>=0; i--) {
                                if(aus_p[i] > massimo) {
                                        massimo = aus_p[i];
                                        posiz = i;
                                }
                        }
                        *pos = posiz;
                        return (long) massimo;
                        break;
                }
                case 2: {
                        int *aus_p = (int *) vet;
                        int massimo = aus_p[n_punti-1];
                        short posiz = n_punti-1;

                        for(i=n_punti-2; i>=0; i--) {
                                if(aus_p[i] > massimo) {
                                        massimo = aus_p[i];
                                        posiz = i;
                                }
                        }
                        *pos = posiz;
                        return (long) massimo;
                        break;
                }
                case 3: {
                        long *aus_p = (long *) vet;
                        long massimo = aus_p[n_punti-1];
                        short posiz = n_punti-1;

                        for(i=n_punti-2; i>=0; i--) {
                                if(aus_p[i] > massimo) {
                                        massimo = aus_p[i];
                                        posiz = i;
                                }
                        }
                        *pos = posiz;
                        return massimo;
                        break;
                }
        }
        return 0;
}

