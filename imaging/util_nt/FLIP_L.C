



/***********************************************************************

;  Nome:    
     util_flip_long
;
;  Funzione:
     inverte l'ordine dei byte
;
;  Formato:
     long util_flip_long(valore)
;
;  Argomenti:    
     long valore           intero da invertire

;
;  Valore di ritorno: 
     $n$                   intero invertito

;
;  Descrizione:
     {\bf util_flip_long}() inverte l'ordine dei byte in interi
	 di 4 byte;
;
;**********************************************************************/

#include <util_nt.h>

long
util_flip_long(
	long valore
)
{
	return ((valore >> 24) & 0x000000ffL) | ((valore >> 8) & 0x0000ff00L) |
           ((valore << 8) & 0x00ff0000L)  | (valore << 24);
}

