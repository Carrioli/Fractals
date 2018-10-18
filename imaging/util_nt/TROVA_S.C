


/***********************************************************************

;  Nome:    
     util_trova_segmenti
;
;  Funzione:
     segmenta un vettore
;
;  Formato:
     short util_trova_segmenti(buf, len, soglia, lung_min, dist_min, inizio,
	                           fine, lunghezza)
;
;  Argomenti:    
	short *buf          vettore di pixel
	short len           lunghezza di "buf"
	short soglia        i pixel dei segmenti hanno livello maggiore di "soglia"
	short lung_min      lunghezza minima dei segmenti
	short dist_min      distanza minima tra 2 segmenti
	short *inizio       indice di inizio del segmento (incluso)
	short *fine         indice di fine del segmento (incluso)
	short *lunghezza    lunghezza del segmento: fine-inizio+1
;
;  Valore di ritorno: 
     $n$                 numero di segmenti individuati

;
;  Descrizione:
     {\bf util_trova_segmenti}() segmenta in modo robusto un vettore
	 di pixel attraverso le seguenti fasi: estrazione di tutti i segmenti
	 tramite binarizzazione, fusione tra segmenti che si trovano a
	 distanza minore di "dist_min" (la fusione ha luogo anche quando i
	 segmenti sono piu' corti di "lung_min"), eliminazione dei segmenti
	 piu' corti di "lung_min";
;
;**********************************************************************/

#include <util_nt.h>

short
util_trova_segmenti(
	short *buf,      /* vettore di pixel */
	short len,       /* lunghezza di buf */
	short soglia,    /* soglia */
	short lung_min,  /* lunghezza minima dei segmenti */
	short dist_min,  /* distanza minima tra 2 segmenti: se la distanza
	                    tra 2 segmenti adiacenti e' minore di dist_min essi
						vengono fusi in un unico segmento; la fusione ha luogo
	                    anche quando i segmenti sono piu' corti di lung_min */
	short *inizio,   /* indice di inizio del segmento (incluso) */
	short *fine,     /* indice di fine del segmento (incluso) */
	short *lunghezza /* lunghezza del segmento: fine-inizio+1 */
)
{
	short i,n_segmenti,inizio_segmento,primo=0,ultimo,indice;

/* prima passata: si trovano tutti i segmenti indipendentemente dalla
   loro lunghezza */

	n_segmenti = 0;
	inizio_segmento = 0;
	for(i=0; i<len; i++) {
		if(buf[i]>soglia && !inizio_segmento) {
			inizio_segmento = 1;
			inizio[n_segmenti] = i;
		}
		if(inizio_segmento && buf[i]<=soglia) {
			inizio_segmento = 0;
			fine[n_segmenti++] = i-1;
		}
	}
	if(inizio_segmento == 1) { /* caso in cui l'ultimo pixel e' sopra soglia */
		inizio_segmento = 0;
		fine[n_segmenti++] = len-1;
	}

/* seconda passata: si fanno le eventuali fusioni: la fusione e' una partizione
   dell'insieme dei segmenti; viene usato il vettore lunghezza come buffer
   ausiliario che, per ogni segmento dice se esso va fuso col successivo */

	for(i=0; i<n_segmenti-1; i++) {
		if(inizio[i+1] - fine[i] < dist_min)
			lunghezza[i] = 1;
		else
			lunghezza[i] = 0;
	}
	lunghezza[n_segmenti-1] = 0;

/* terza passata: si scandisce lunghezza e si effettuano fisicamente
   le fusioni */

	inizio_segmento = 0;
	for(i=0,indice=0; i<n_segmenti; i++) {
		if(lunghezza[i]==0 && inizio_segmento==0) {
			inizio[indice] = inizio[i];
			fine[indice++] = fine[i];
		}
		if(lunghezza[i]==1 && inizio_segmento==0) {
			inizio_segmento = 1;
			primo = i;
		}
		if(lunghezza[i]==0 && inizio_segmento==1) {
			inizio_segmento = 0;
			ultimo = i;
			inizio[indice] = inizio[primo];
			fine[indice++] = fine[ultimo];
		}
	}
	n_segmenti = indice;

/* quarta passata: si aggiornano le lunghezze */

	for(i=0; i<n_segmenti; i++) {
		lunghezza[i] = fine[i] - inizio[i] + 1;
	}

/* quinta passata: vengono individuati e scartati i segmenti troppo corti */

	for(i=0,indice=0; i<n_segmenti; i++) {
		if(lunghezza[i] >= lung_min) {
			if(indice != i) { /* sposto il segmento da i a indice */
				inizio[indice] = inizio[i];
				fine[indice] = fine[i];
				lunghezza[indice] = lunghezza[i];
			}
			indice ++;
		}
	}
	n_segmenti = indice;

	return n_segmenti;
}

