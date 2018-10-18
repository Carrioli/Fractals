


/***********************************************************************

;  Nome:    
     util_flip_short
;
;  Funzione:
     inverte l'ordine dei byte
;
;  Formato:
     short util_flip_short(valore)
;
;  Argomenti:    
     short valore           intero da invertire

;
;  Valore di ritorno: 
     $n$                   intero invertito

;
;  Descrizione:
     {\bf util_flip_short}() inverte l'ordine dei byte in interi
	 di 2 byte;
;
;**********************************************************************/

#include <util_nt.h>

short
util_flip_short(
	short valore
)
{
	return ((valore >> 8) & 255) | (valore << 8);
}

