
/*
	mparse()             pubblica
	mparse2()            privata

	mparse e' l'unica routine di questa libreria il cui nome, per
	ragioni storiche non inizia con util_
*/


/***********************************************************************

;  Nome:    
     mparse
;
;  Funzione:
     effettua il parsing della linea di comando
;
;  Formato:
     int mparse(argc, argv, par)
;
;  Argomenti:    
     int argc             numero di stringhe della linea di comando
     char **argv          stringhe della linea di comando
     struct params par[]  struttura che definisce la sintassi della riga
	                      di comando (vedi util.h)
;
;  Valore di ritorno: 
     $n$            numero di stringhe matched

;
;  Descrizione:
     {\bf mparse}() effettua il parsing della linea di comando confrontando
	 ogni stringa di ingresso con il campo "match" della struttura "par"
	 che deve essere opportunamente inizializzata; se non vi sono errori
	 la porzione di stringa che contiene l'informazione viene posta,
	 per ogni stringa, nel secondo campo (val) della struttura "par";
	 in caso di errore viene settato il campo "val" dell'ultima struttura
	 (ERRFLAG);
;
;**********************************************************************/

#include <util_nt.h>

/* prototipi delle routine private */

static int mparse2(char *aa, char *bb, int *mm);
static int mparse2();

/**********************************************************************/

/* mparse2: fa il match della parte iniziale della stringa;
   ritorna 1 se c'e' il match pieno tra tutti i caratteri della stringa
   di riconoscimento (if=  fb=  etc.) 0 altrimenti */

static int
mparse2(
	char *a, /* stringa di riconoscimento primo campo della struttura params */
	char *b, /* stringa che viene da tastiera */
	int *m   /* numero di caratteri iniziali della stringa che sono matchati
	            es.: if= fb= etc.. */
)
{
	*m = 0;
	while((*a != '\0') && (*a == *b++)) {
		a++;
		(*m)++;
	}
	return *a == '\0';
}

/*****************************************************************************/

/* parsing dei parametri di ingresso: ritorna il numero di campi convertiti */

int
mparse(
	int argc,
	char **argv,
	struct params par[]
)
{
	struct params *p;
	int count=0, mm=0;

/* initialize value pointers to zero */

	for(p=par; p->match != 0; p++->val = 0)
		;

/* now scan for params */

	while(--argc > 0) {
		argv++;

/* search for match */

		for(p=par;
		    (p->match != 0) && (p->val || !mparse2(p->match, *argv, &mm));
		    p++)
			;

/* if there is a match, set value */

		if(p->match != 0) {      /* if we didn't hit the end: OK */
			count++;
			p->val = *argv + mm;
		}
	}
	return count;
}

