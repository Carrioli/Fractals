
/*
	sea2_leggi_riga
	sea2_leggi_riga2
	sea2_leggi_riga3
	sea2_leggi_riga4
	sea2_leggi_riga_col
	sea2_leggi_riga_col16
	sea2_leggi_riga_col24
	sea2_sotto_c2_riga
	sea2_sotto_c2_riga2
	sea2_sotto_c2_riga3
	sea2_sotto_c2_riga4
	sea2_sotto_c2_riga_col
	sea2_sotto_c2_riga_col16
	sea2_sotto_c2_riga_col24
	sea2_sotto_max2_riga
	sea2_sotto_max2_riga2
	sea2_sotto_max2_riga3
	sea2_sotto_max2_riga4
	sea2_sotto_max2_riga_col
	sea2_sotto_max2_riga4_col

	sea2_leggi_pixel
	sea2_leggi_colonna
	sea2_leggi_colonna_col
	sea2_leggi_finestra
	sea2_leggi_finestra_col
	sea2_leggi_finestra_col24
*/

#include <sea2_nt.h>
#ifdef MET
	#include "libmetnt.h"
#endif                    
#ifdef MET_MIL
	#include "libmetnt.h"
#endif                    
#ifdef ASS
	#include "libassnt.h"
#endif
#ifdef ORION
	#include "liborion.h"
#endif                    
#ifdef METEOR2
	#include "libmeteor2.h"
#endif                    
#ifndef __WATCOMC__
	#include <windows.h>
#else
	#include <string.h>
#endif

/*****************************************************************************/

/* variabili globali definite in init.c */

#if defined(MET) || defined(ASS) || defined(MET_MIL) || defined(ORION) || defined(METEOR2)
	extern unsigned char *_mem_address[N_FB_MAX];
	extern long *_offset_riga;
	/* contiene, per ogni riga, l'offset del primo pixel */
	extern short _n_bit_col16; /* numero di bit usati nel display a 16 bit;
	possono essere 15 (XRRRRRGGGGGBBBBB) o 16 (RRRRRGGGGGGBBBBB) */
	// per il colore RGB
	extern unsigned char *_mem_address_col[N_FB_MAX_COL];
	// fine colore RGB
#endif
	extern PARAM_SCHEDA _param_scheda;


/***********************************************************************

;  Nome:    
	sea2_leggi_riga
;
;  Funzione:
	legge un segmento orizzontale dal frame buffer selezionato
;
;  Formato:
	void sea2_leggi_riga(xi, yi, n_pixel, riga)
;
;  Argomenti:    
	short xi           ascissa iniziale
	short yi           ordinata iniziale
	short n_pixel      numero di pixel da leggere
	unsigned char *riga  buffer nel quale sono trasferiti i pixel
	                     letti dalla scheda

;
;  Descrizione:
     {\bf sea2_leggi_riga}() legge un segmento orizzontale dal
	 frame buffer selezionato con la routine {\bf sea2_seleziona_buffer}();
;
;**********************************************************************/

void
sea2_leggi_riga(
	short xi,
	short yi,
	short n_pixel,
	unsigned char *riga
)
{
	if(!_param_scheda.scheda_aperta)
		return;
	if(_param_scheda.tipo_tv == TV_RGB) {
		short i;
		unsigned char *mem = _mem_address[_param_scheda.fb_lavoro];
		unsigned char *ausm = mem + (long) (xi<<2) + _offset_riga[yi];
		unsigned char *ausr = riga;
		for(i=n_pixel-7; i>0; i-=8) {
			*ausr++ = *ausm; ausm += 4;
			*ausr++ = *ausm; ausm += 4;
			*ausr++ = *ausm; ausm += 4;
			*ausr++ = *ausm; ausm += 4;
			*ausr++ = *ausm; ausm += 4;
			*ausr++ = *ausm; ausm += 4;
			*ausr++ = *ausm; ausm += 4;
			*ausr++ = *ausm; ausm += 4;
		}
		for(; i>-7; i--) {
			*ausr++ = *ausm; ausm += 4;
		}
	} else {
		unsigned char *mem = _mem_address[_param_scheda.fb_lavoro];
		unsigned char *ausmem = mem + (long) xi + _offset_riga[yi];

		memcpy(riga, ausmem, (size_t) n_pixel);
	}

	return;
}

/*****************************************************************************/

/* legge una riga da un frame buffer a colori (32 bit per pixel) */

void
sea2_leggi_riga_col(
	short xi,
	short yi,
	short n_pixel,
	unsigned char *riga
)
{
	if(!_param_scheda.scheda_aperta)
		return;
	if(_param_scheda.tipo_tv == TV_RGB) {
		unsigned char *mem = _mem_address_col[_param_scheda.fb_lavoro];
		unsigned char *ausmem = mem + (long) (xi<<2) + _offset_riga[yi];

		memcpy(riga, ausmem, (size_t) (n_pixel<<2));
	}
}

/*****************************************************************************/

/* copia in riga i pixel della riga yi sottocampionati a passo 2;
   n_pixel si riferisce alla lunghezza della riga da sottocampionare: di
   fatto sono copiati n_pixel/2 valori; */

void
sea2_sotto_c2_riga(
	short xi,
	short yi,
	short n_pixel,
	unsigned char *riga
)
{
	if(!_param_scheda.scheda_aperta)
		return;
	if(_param_scheda.tipo_tv == TV_RGB) {
		register short j;
		unsigned char *mem = _mem_address[_param_scheda.fb_lavoro];
		unsigned char *ausmem0 = mem + (long) (xi<<2) + _offset_riga[yi];
		unsigned char *ausr = riga;

		for(j=(n_pixel>>1)-7; j>0; j-=8) {
			*ausr++ = *ausmem0; ausmem0+=8;
			*ausr++ = *ausmem0; ausmem0+=8;
			*ausr++ = *ausmem0; ausmem0+=8;
			*ausr++ = *ausmem0; ausmem0+=8;
			*ausr++ = *ausmem0; ausmem0+=8;
			*ausr++ = *ausmem0; ausmem0+=8;
			*ausr++ = *ausmem0; ausmem0+=8;
			*ausr++ = *ausmem0; ausmem0+=8;
		}
		for(; j>-7; j--) {
			*ausr++ = *ausmem0; ausmem0+=8;
		}
	} else {
		register short j;
		unsigned char *mem = _mem_address[_param_scheda.fb_lavoro];
		unsigned char *ausmem0 = mem + (long) xi + _offset_riga[yi];
		unsigned char *ausr = riga;

		for(j=(n_pixel>>1)-7; j>0; j-=8) {
			*ausr++ = *ausmem0++; ausmem0++;
			*ausr++ = *ausmem0++; ausmem0++;
			*ausr++ = *ausmem0++; ausmem0++;
			*ausr++ = *ausmem0++; ausmem0++;
			*ausr++ = *ausmem0++; ausmem0++;
			*ausr++ = *ausmem0++; ausmem0++;
			*ausr++ = *ausmem0++; ausmem0++;
			*ausr++ = *ausmem0++; ausmem0++;
		}
		for(; j>-7; j--) {
			*ausr++ = *ausmem0++; ausmem0++;
		}
	}
}

/*****************************************************************************/

/* come sea2_sotto_c2_riga, ma ogni valore letto e' replicato 3 volte
   in 16 bit; */

void
sea2_sotto_c2_riga2(
	short xi,
	short yi,
	short n_pixel,
	unsigned char *riga
)
{
	if(!_param_scheda.scheda_aperta)
		return;
	if(_param_scheda.tipo_tv == TV_RGB) {
		register short j;
		unsigned char *mem = _mem_address[_param_scheda.fb_lavoro];
		unsigned char *ausmem0 = mem + (long) (xi<<2) + _offset_riga[yi];
		unsigned short *ausr = (unsigned short *) riga;

		if(_n_bit_col16 == 15) {
			for(j=(n_pixel>>1)-7; j>0; j-=8) {
				*ausr++ = SHORT_15(*ausmem0, *ausmem0, *ausmem0);
				ausmem0+=8;
				*ausr++ = SHORT_15(*ausmem0, *ausmem0, *ausmem0);
				ausmem0+=8;
				*ausr++ = SHORT_15(*ausmem0, *ausmem0, *ausmem0);
				ausmem0+=8;
				*ausr++ = SHORT_15(*ausmem0, *ausmem0, *ausmem0);
				ausmem0+=8;
				*ausr++ = SHORT_15(*ausmem0, *ausmem0, *ausmem0);
				ausmem0+=8;
				*ausr++ = SHORT_15(*ausmem0, *ausmem0, *ausmem0);
				ausmem0+=8;
				*ausr++ = SHORT_15(*ausmem0, *ausmem0, *ausmem0);
				ausmem0+=8;
				*ausr++ = SHORT_15(*ausmem0, *ausmem0, *ausmem0);
				ausmem0+=8;
			}
			for(; j>-7; j--) {
				*ausr++ = SHORT_15(*ausmem0, *ausmem0, *ausmem0);
				ausmem0+=8;
			}
		} else {
			for(j=(n_pixel>>1)-7; j>0; j-=8) {
				*ausr++ = SHORT_16(*ausmem0, *ausmem0, *ausmem0);
				ausmem0+=8;
				*ausr++ = SHORT_16(*ausmem0, *ausmem0, *ausmem0);
				ausmem0+=8;
				*ausr++ = SHORT_16(*ausmem0, *ausmem0, *ausmem0);
				ausmem0+=8;
				*ausr++ = SHORT_16(*ausmem0, *ausmem0, *ausmem0);
				ausmem0+=8;
				*ausr++ = SHORT_16(*ausmem0, *ausmem0, *ausmem0);
				ausmem0+=8;
				*ausr++ = SHORT_16(*ausmem0, *ausmem0, *ausmem0);
				ausmem0+=8;
				*ausr++ = SHORT_16(*ausmem0, *ausmem0, *ausmem0);
				ausmem0+=8;
				*ausr++ = SHORT_16(*ausmem0, *ausmem0, *ausmem0);
				ausmem0+=8;
			}
			for(; j>-7; j--) {
				*ausr++ = SHORT_16(*ausmem0, *ausmem0, *ausmem0);
				ausmem0+=8;
			}
		}
	} else {
		register short j;
		unsigned char *mem = _mem_address[_param_scheda.fb_lavoro];
		unsigned char *ausmem0 = mem + (long) xi + _offset_riga[yi];
		unsigned short *ausr = (unsigned short *) riga;

		if(_n_bit_col16 == 15) {
			for(j=(n_pixel>>1)-7; j>0; j-=8) {
				*ausr++ = SHORT_15(*ausmem0, *ausmem0, *ausmem0);
				ausmem0+=2;
				*ausr++ = SHORT_15(*ausmem0, *ausmem0, *ausmem0);
				ausmem0+=2;
				*ausr++ = SHORT_15(*ausmem0, *ausmem0, *ausmem0);
				ausmem0+=2;
				*ausr++ = SHORT_15(*ausmem0, *ausmem0, *ausmem0);
				ausmem0+=2;
				*ausr++ = SHORT_15(*ausmem0, *ausmem0, *ausmem0);
				ausmem0+=2;
				*ausr++ = SHORT_15(*ausmem0, *ausmem0, *ausmem0);
				ausmem0+=2;
				*ausr++ = SHORT_15(*ausmem0, *ausmem0, *ausmem0);
				ausmem0+=2;
				*ausr++ = SHORT_15(*ausmem0, *ausmem0, *ausmem0);
				ausmem0+=2;
			}
			for(; j>-7; j--) {
				*ausr++ = SHORT_15(*ausmem0, *ausmem0, *ausmem0);
				ausmem0+=2;
			}
		} else {
			for(j=(n_pixel>>1)-7; j>0; j-=8) {
				*ausr++ = SHORT_16(*ausmem0, *ausmem0, *ausmem0);
				ausmem0+=2;
				*ausr++ = SHORT_16(*ausmem0, *ausmem0, *ausmem0);
				ausmem0+=2;
				*ausr++ = SHORT_16(*ausmem0, *ausmem0, *ausmem0);
				ausmem0+=2;
				*ausr++ = SHORT_16(*ausmem0, *ausmem0, *ausmem0);
				ausmem0+=2;
				*ausr++ = SHORT_16(*ausmem0, *ausmem0, *ausmem0);
				ausmem0+=2;
				*ausr++ = SHORT_16(*ausmem0, *ausmem0, *ausmem0);
				ausmem0+=2;
				*ausr++ = SHORT_16(*ausmem0, *ausmem0, *ausmem0);
				ausmem0+=2;
				*ausr++ = SHORT_16(*ausmem0, *ausmem0, *ausmem0);
				ausmem0+=2;
			}
			for(; j>-7; j--) {
				*ausr++ = SHORT_16(*ausmem0, *ausmem0, *ausmem0);
				ausmem0+=2;
			}
		}
	}
}

/*****************************************************************************/

/* come sea2_sotto_c2_riga_col, ma ogni pixel a colori letto (4 byte)
   e' impaccato in 16 bit; */

void
sea2_sotto_c2_riga_col16(
	short xi,
	short yi,
	short n_pixel,
	unsigned char *riga
)
{
	if(!_param_scheda.scheda_aperta)
		return;
	if(_param_scheda.tipo_tv == TV_RGB) {
		register short j;
		unsigned char *mem = _mem_address_col[_param_scheda.fb_lavoro];
		unsigned char *ausmem0 = mem + (long) (xi<<2) + _offset_riga[yi];
		unsigned short *ausr = (unsigned short *) riga;

		if(_n_bit_col16 == 15) {
			for(j=(n_pixel>>1)-7; j>0; j-=8) {
				*ausr++ = SHORT_15(*(ausmem0+2), *(ausmem0+1), *ausmem0);
				ausmem0+=8;
				*ausr++ = SHORT_15(*(ausmem0+2), *(ausmem0+1), *ausmem0);
				ausmem0+=8;
				*ausr++ = SHORT_15(*(ausmem0+2), *(ausmem0+1), *ausmem0);
				ausmem0+=8;
				*ausr++ = SHORT_15(*(ausmem0+2), *(ausmem0+1), *ausmem0);
				ausmem0+=8;
				*ausr++ = SHORT_15(*(ausmem0+2), *(ausmem0+1), *ausmem0);
				ausmem0+=8;
				*ausr++ = SHORT_15(*(ausmem0+2), *(ausmem0+1), *ausmem0);
				ausmem0+=8;
				*ausr++ = SHORT_15(*(ausmem0+2), *(ausmem0+1), *ausmem0);
				ausmem0+=8;
				*ausr++ = SHORT_15(*(ausmem0+2), *(ausmem0+1), *ausmem0);
				ausmem0+=8;
			}
			for(; j>-7; j--) {
				*ausr++ = SHORT_15(*(ausmem0+2), *(ausmem0+1), *ausmem0);
				ausmem0+=8;
			}
		} else {
			for(j=(n_pixel>>1)-7; j>0; j-=8) {
				*ausr++ = SHORT_16(*(ausmem0+2), *(ausmem0+1), *ausmem0);
				ausmem0+=8;
				*ausr++ = SHORT_16(*(ausmem0+2), *(ausmem0+1), *ausmem0);
				ausmem0+=8;
				*ausr++ = SHORT_16(*(ausmem0+2), *(ausmem0+1), *ausmem0);
				ausmem0+=8;
				*ausr++ = SHORT_16(*(ausmem0+2), *(ausmem0+1), *ausmem0);
				ausmem0+=8;
				*ausr++ = SHORT_16(*(ausmem0+2), *(ausmem0+1), *ausmem0);
				ausmem0+=8;
				*ausr++ = SHORT_16(*(ausmem0+2), *(ausmem0+1), *ausmem0);
				ausmem0+=8;
				*ausr++ = SHORT_16(*(ausmem0+2), *(ausmem0+1), *ausmem0);
				ausmem0+=8;
				*ausr++ = SHORT_16(*(ausmem0+2), *(ausmem0+1), *ausmem0);
				ausmem0+=8;
			}
			for(; j>-7; j--) {
				*ausr++ = SHORT_16(*(ausmem0+2), *(ausmem0+1), *ausmem0);
				ausmem0+=8;
			}
		}
	}
}

/*****************************************************************************/

/* come sea2_sotto_c2_riga_col, ma ogni pixel a colori letto (4 byte)
   e' impaccato in 24 bit; */

void
sea2_sotto_c2_riga_col24(
	short xi,
	short yi,
	short n_pixel,
	unsigned char *riga
)
{
	if(!_param_scheda.scheda_aperta)
		return;
	if(_param_scheda.tipo_tv == TV_RGB) {
		register short j;
		unsigned char *mem = _mem_address_col[_param_scheda.fb_lavoro];
		unsigned char *ausmem0 = mem + (long) (xi<<2) + _offset_riga[yi];
		unsigned char *ausr = riga;

		for(j=(n_pixel>>1)-7; j>0; j-=8) {
			*ausr++ = *ausmem0++; *ausr++ = *ausmem0++; *ausr++ = *ausmem0++;
			ausmem0+=5;
			*ausr++ = *ausmem0++; *ausr++ = *ausmem0++; *ausr++ = *ausmem0++;
			ausmem0+=5;
			*ausr++ = *ausmem0++; *ausr++ = *ausmem0++; *ausr++ = *ausmem0++;
			ausmem0+=5;
			*ausr++ = *ausmem0++; *ausr++ = *ausmem0++; *ausr++ = *ausmem0++;
			ausmem0+=5;
			*ausr++ = *ausmem0++; *ausr++ = *ausmem0++; *ausr++ = *ausmem0++;
			ausmem0+=5;
			*ausr++ = *ausmem0++; *ausr++ = *ausmem0++; *ausr++ = *ausmem0++;
			ausmem0+=5;
			*ausr++ = *ausmem0++; *ausr++ = *ausmem0++; *ausr++ = *ausmem0++;
			ausmem0+=5;
			*ausr++ = *ausmem0++; *ausr++ = *ausmem0++; *ausr++ = *ausmem0++;
			ausmem0+=5;
		}
		for(; j>-7; j--) {
			*ausr++ = *ausmem0++; *ausr++ = *ausmem0++; *ausr++ = *ausmem0++;
			ausmem0+=5;
		}
	}
}

/*****************************************************************************/

/* come sea2_sotto_c2_riga, ma ogni valore letto e' replicato 3 volte in
   24 bit; */

void
sea2_sotto_c2_riga3(
	short xi,
	short yi,
	short n_pixel,
	unsigned char *riga
)
{
	if(!_param_scheda.scheda_aperta)
		return;
	if(_param_scheda.tipo_tv == TV_RGB) {
		register short j;
		unsigned char *mem = _mem_address[_param_scheda.fb_lavoro];
		unsigned char *ausmem0 = mem + (long) (xi<<2) + _offset_riga[yi];
		unsigned char *ausr = riga;

		for(j=(n_pixel>>1)-7; j>0; j-=8) {
			*ausr++ = *ausmem0; *ausr++ = *ausmem0; *ausr++ = *ausmem0;
			ausmem0+=8;
			*ausr++ = *ausmem0; *ausr++ = *ausmem0; *ausr++ = *ausmem0;
			ausmem0+=8;
			*ausr++ = *ausmem0; *ausr++ = *ausmem0; *ausr++ = *ausmem0;
			ausmem0+=8;
			*ausr++ = *ausmem0; *ausr++ = *ausmem0; *ausr++ = *ausmem0;
			ausmem0+=8;
			*ausr++ = *ausmem0; *ausr++ = *ausmem0; *ausr++ = *ausmem0;
			ausmem0+=8;
			*ausr++ = *ausmem0; *ausr++ = *ausmem0; *ausr++ = *ausmem0;
			ausmem0+=8;
			*ausr++ = *ausmem0; *ausr++ = *ausmem0; *ausr++ = *ausmem0;
			ausmem0+=8;
			*ausr++ = *ausmem0; *ausr++ = *ausmem0; *ausr++ = *ausmem0;
			ausmem0+=8;
		}
		for(; j>-7; j--) {
			*ausr++ = *ausmem0; *ausr++ = *ausmem0; *ausr++ = *ausmem0;
			ausmem0+=8;
		}
	} else {
		register short j;
		unsigned char *mem = _mem_address[_param_scheda.fb_lavoro];
		unsigned char *ausmem0 = mem + (long) xi + _offset_riga[yi];
		unsigned char *ausr = riga;

		for(j=(n_pixel>>1)-7; j>0; j-=8) {
			*ausr++ = *ausmem0; *ausr++ = *ausmem0; *ausr++ = *ausmem0;
			ausmem0+=2;
			*ausr++ = *ausmem0; *ausr++ = *ausmem0; *ausr++ = *ausmem0;
			ausmem0+=2;
			*ausr++ = *ausmem0; *ausr++ = *ausmem0; *ausr++ = *ausmem0;
			ausmem0+=2;
			*ausr++ = *ausmem0; *ausr++ = *ausmem0; *ausr++ = *ausmem0;
			ausmem0+=2;
			*ausr++ = *ausmem0; *ausr++ = *ausmem0; *ausr++ = *ausmem0;
			ausmem0+=2;
			*ausr++ = *ausmem0; *ausr++ = *ausmem0; *ausr++ = *ausmem0;
			ausmem0+=2;
			*ausr++ = *ausmem0; *ausr++ = *ausmem0; *ausr++ = *ausmem0;
			ausmem0+=2;
			*ausr++ = *ausmem0; *ausr++ = *ausmem0; *ausr++ = *ausmem0;
			ausmem0+=2;
		}
		for(; j>-7; j--) {
			*ausr++ = *ausmem0; *ausr++ = *ausmem0; *ausr++ = *ausmem0;
			ausmem0+=2;
		}
	}
}

/*****************************************************************************/

/* sottocampiona l'intorno (2x2) formato dalla riga yi e yi+1; se nell'intorno
   ci sono punti di valore superiore a 245 questi sono trasferiti al posto
   del pixel normale;
   n_pixel si riferisce alla lunghezza della riga da sottocampionare: di
   fatto sono copiati n_pixel/2 valori; */

void
sea2_sotto_max2_riga(
	short xi,
	short yi,
	short n_pixel,
	unsigned char *riga
)
{
	if(!_param_scheda.scheda_aperta)
		return;
	if(_param_scheda.tipo_tv == TV_RGB) {
		register short j;
		unsigned char *mem = _mem_address[_param_scheda.fb_lavoro];
		unsigned char *ausmem0 = mem + (long) (xi<<2) + _offset_riga[yi];
		unsigned char *ausmem1 = ausmem0 + (long) (_param_scheda.lung_riga<<2);
		unsigned char *ausr = riga;

		for(j=(n_pixel>>1)-7; j>0; j-=8) {
			*ausr++ = *ausmem0; ausmem0+=4;
			if(*ausmem0 > 245) *(ausr-1) = *ausmem0; ausmem0+=4;
			if(*ausmem1 > 245) *(ausr-1) = *ausmem1; ausmem1+=4;
			if(*ausmem1 > 245) *(ausr-1) = *ausmem1; ausmem1+=4;
			*ausr++ = *ausmem0; ausmem0+=4;
			if(*ausmem0 > 245) *(ausr-1) = *ausmem0; ausmem0+=4;
			if(*ausmem1 > 245) *(ausr-1) = *ausmem1; ausmem1+=4;
			if(*ausmem1 > 245) *(ausr-1) = *ausmem1; ausmem1+=4;
			*ausr++ = *ausmem0; ausmem0+=4;
			if(*ausmem0 > 245) *(ausr-1) = *ausmem0; ausmem0+=4;
			if(*ausmem1 > 245) *(ausr-1) = *ausmem1; ausmem1+=4;
			if(*ausmem1 > 245) *(ausr-1) = *ausmem1; ausmem1+=4;
			*ausr++ = *ausmem0; ausmem0+=4;
			if(*ausmem0 > 245) *(ausr-1) = *ausmem0; ausmem0+=4;
			if(*ausmem1 > 245) *(ausr-1) = *ausmem1; ausmem1+=4;
			if(*ausmem1 > 245) *(ausr-1) = *ausmem1; ausmem1+=4;
			*ausr++ = *ausmem0; ausmem0+=4;
			if(*ausmem0 > 245) *(ausr-1) = *ausmem0; ausmem0+=4;
			if(*ausmem1 > 245) *(ausr-1) = *ausmem1; ausmem1+=4;
			if(*ausmem1 > 245) *(ausr-1) = *ausmem1; ausmem1+=4;
			*ausr++ = *ausmem0; ausmem0+=4;
			if(*ausmem0 > 245) *(ausr-1) = *ausmem0; ausmem0+=4;
			if(*ausmem1 > 245) *(ausr-1) = *ausmem1; ausmem1+=4;
			if(*ausmem1 > 245) *(ausr-1) = *ausmem1; ausmem1+=4;
			*ausr++ = *ausmem0; ausmem0+=4;
			if(*ausmem0 > 245) *(ausr-1) = *ausmem0; ausmem0+=4;
			if(*ausmem1 > 245) *(ausr-1) = *ausmem1; ausmem1+=4;
			if(*ausmem1 > 245) *(ausr-1) = *ausmem1; ausmem1+=4;
			*ausr++ = *ausmem0; ausmem0+=4;
			if(*ausmem0 > 245) *(ausr-1) = *ausmem0; ausmem0+=4;
			if(*ausmem1 > 245) *(ausr-1) = *ausmem1; ausmem1+=4;
			if(*ausmem1 > 245) *(ausr-1) = *ausmem1; ausmem1+=4;
		}
		for(; j>-7; j--) {
			*ausr++ = *ausmem0; ausmem0+=4;
			if(*ausmem0 > 245) *(ausr-1) = *ausmem0; ausmem0+=4;
			if(*ausmem1 > 245) *(ausr-1) = *ausmem1; ausmem1+=4;
			if(*ausmem1 > 245) *(ausr-1) = *ausmem1; ausmem1+=4;
		}
	} else {
		register short j;
		unsigned char *mem = _mem_address[_param_scheda.fb_lavoro];
		unsigned char *ausmem0 = mem + (long) xi + _offset_riga[yi];
		unsigned char *ausmem1 = ausmem0 + (long) _param_scheda.lung_riga;
		unsigned char *ausr = riga;

		for(j=(n_pixel>>1)-7; j>0; j-=8) {
			*ausr++ = *ausmem0++;
			if(*ausmem0++ > 245) *(ausr-1) = *(ausmem0-1);
			if(*ausmem1++ > 245) *(ausr-1) = *(ausmem1-1);
			if(*ausmem1++ > 245) *(ausr-1) = *(ausmem1-1);
			*ausr++ = *ausmem0++;
			if(*ausmem0++ > 245) *(ausr-1) = *(ausmem0-1);
			if(*ausmem1++ > 245) *(ausr-1) = *(ausmem1-1);
			if(*ausmem1++ > 245) *(ausr-1) = *(ausmem1-1);
			*ausr++ = *ausmem0++;
			if(*ausmem0++ > 245) *(ausr-1) = *(ausmem0-1);
			if(*ausmem1++ > 245) *(ausr-1) = *(ausmem1-1);
			if(*ausmem1++ > 245) *(ausr-1) = *(ausmem1-1);
			*ausr++ = *ausmem0++;
			if(*ausmem0++ > 245) *(ausr-1) = *(ausmem0-1);
			if(*ausmem1++ > 245) *(ausr-1) = *(ausmem1-1);
			if(*ausmem1++ > 245) *(ausr-1) = *(ausmem1-1);
			*ausr++ = *ausmem0++;
			if(*ausmem0++ > 245) *(ausr-1) = *(ausmem0-1);
			if(*ausmem1++ > 245) *(ausr-1) = *(ausmem1-1);
			if(*ausmem1++ > 245) *(ausr-1) = *(ausmem1-1);
			*ausr++ = *ausmem0++;
			if(*ausmem0++ > 245) *(ausr-1) = *(ausmem0-1);
			if(*ausmem1++ > 245) *(ausr-1) = *(ausmem1-1);
			if(*ausmem1++ > 245) *(ausr-1) = *(ausmem1-1);
			*ausr++ = *ausmem0++;
			if(*ausmem0++ > 245) *(ausr-1) = *(ausmem0-1);
			if(*ausmem1++ > 245) *(ausr-1) = *(ausmem1-1);
			if(*ausmem1++ > 245) *(ausr-1) = *(ausmem1-1);
			*ausr++ = *ausmem0++;
			if(*ausmem0++ > 245) *(ausr-1) = *(ausmem0-1);
			if(*ausmem1++ > 245) *(ausr-1) = *(ausmem1-1);
			if(*ausmem1++ > 245) *(ausr-1) = *(ausmem1-1);
		}
		for(; j>-7; j--) {
			*ausr++ = *ausmem0++;
			if(*ausmem0++ > 245) *(ausr-1) = *(ausmem0-1);
			if(*ausmem1++ > 245) *(ausr-1) = *(ausmem1-1);
			if(*ausmem1++ > 245) *(ausr-1) = *(ausmem1-1);
		}
	}
}

/*****************************************************************************/

/* come sea2_sotto_max2_riga, ma ogni pixel prelevato e' replicato 3 volte in
   16 bit;
   n_pixel si riferisce alla lunghezza della riga da sottocampionare: di
   fatto sono copiati n_pixel/2 valori; */

void
sea2_sotto_max2_riga2(
	short xi,
	short yi,
	short n_pixel,
	unsigned char *riga
)
{
	if(!_param_scheda.scheda_aperta)
		return;
	if(_param_scheda.tipo_tv == TV_RGB) {
		register short j;
		unsigned char ausc;
		unsigned char *mem = _mem_address[_param_scheda.fb_lavoro];
		unsigned char *ausmem0 = mem + (long) (xi<<2) + _offset_riga[yi];
		unsigned char *ausmem1 = ausmem0 + (long) (_param_scheda.lung_riga<<2);
		unsigned short *ausr = (unsigned short *) riga;

		if(_n_bit_col16 == 15) {
			for(j=(n_pixel>>1)-7; j>0; j-=8) {
				ausc = *ausmem0; ausmem0+=4;
				if(*ausmem0 > 245) ausc = *ausmem0; ausmem0+=4;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1+=4;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1+=4;
				*ausr++ = SHORT_15(ausc, ausc, ausc);
				ausc = *ausmem0; ausmem0+=4;
				if(*ausmem0 > 245) ausc = *ausmem0; ausmem0+=4;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1+=4;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1+=4;
				*ausr++ = SHORT_15(ausc, ausc, ausc);
				ausc = *ausmem0; ausmem0+=4;
				if(*ausmem0 > 245) ausc = *ausmem0; ausmem0+=4;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1+=4;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1+=4;
				*ausr++ = SHORT_15(ausc, ausc, ausc);
				ausc = *ausmem0; ausmem0+=4;
				if(*ausmem0 > 245) ausc = *ausmem0; ausmem0+=4;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1+=4;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1+=4;
				*ausr++ = SHORT_15(ausc, ausc, ausc);
				ausc = *ausmem0; ausmem0+=4;
				if(*ausmem0 > 245) ausc = *ausmem0; ausmem0+=4;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1+=4;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1+=4;
				*ausr++ = SHORT_15(ausc, ausc, ausc);
				ausc = *ausmem0; ausmem0+=4;
				if(*ausmem0 > 245) ausc = *ausmem0; ausmem0+=4;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1+=4;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1+=4;
				*ausr++ = SHORT_15(ausc, ausc, ausc);
				ausc = *ausmem0; ausmem0+=4;
				if(*ausmem0 > 245) ausc = *ausmem0; ausmem0+=4;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1+=4;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1+=4;
				*ausr++ = SHORT_15(ausc, ausc, ausc);
				ausc = *ausmem0; ausmem0+=4;
				if(*ausmem0 > 245) ausc = *ausmem0; ausmem0+=4;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1+=4;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1+=4;
				*ausr++ = SHORT_15(ausc, ausc, ausc);
			}
			for(; j>-7; j--) {
				ausc = *ausmem0; ausmem0+=4;
				if(*ausmem0 > 245) ausc = *ausmem0; ausmem0+=4;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1+=4;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1+=4;
				*ausr++ = SHORT_15(ausc, ausc, ausc);
			}
		} else {
			for(j=(n_pixel>>1)-7; j>0; j-=8) {
				ausc = *ausmem0; ausmem0+=4;
				if(*ausmem0 > 245) ausc = *ausmem0; ausmem0+=4;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1+=4;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1+=4;
				*ausr++ = SHORT_16(ausc, ausc, ausc);
				ausc = *ausmem0; ausmem0+=4;
				if(*ausmem0 > 245) ausc = *ausmem0; ausmem0+=4;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1+=4;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1+=4;
				*ausr++ = SHORT_16(ausc, ausc, ausc);
				ausc = *ausmem0; ausmem0+=4;
				if(*ausmem0 > 245) ausc = *ausmem0; ausmem0+=4;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1+=4;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1+=4;
				*ausr++ = SHORT_16(ausc, ausc, ausc);
				ausc = *ausmem0; ausmem0+=4;
				if(*ausmem0 > 245) ausc = *ausmem0; ausmem0+=4;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1+=4;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1+=4;
				*ausr++ = SHORT_16(ausc, ausc, ausc);
				ausc = *ausmem0; ausmem0+=4;
				if(*ausmem0 > 245) ausc = *ausmem0; ausmem0+=4;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1+=4;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1+=4;
				*ausr++ = SHORT_16(ausc, ausc, ausc);
				ausc = *ausmem0; ausmem0+=4;
				if(*ausmem0 > 245) ausc = *ausmem0; ausmem0+=4;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1+=4;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1+=4;
				*ausr++ = SHORT_16(ausc, ausc, ausc);
				ausc = *ausmem0; ausmem0+=4;
				if(*ausmem0 > 245) ausc = *ausmem0; ausmem0+=4;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1+=4;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1+=4;
				*ausr++ = SHORT_16(ausc, ausc, ausc);
				ausc = *ausmem0; ausmem0+=4;
				if(*ausmem0 > 245) ausc = *ausmem0; ausmem0+=4;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1+=4;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1+=4;
				*ausr++ = SHORT_16(ausc, ausc, ausc);
			}
			for(; j>-7; j--) {
				ausc = *ausmem0; ausmem0+=4;
				if(*ausmem0 > 245) ausc = *ausmem0; ausmem0+=4;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1+=4;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1+=4;
				*ausr++ = SHORT_16(ausc, ausc, ausc);
			}
		}
	} else {
		register short j;
		unsigned char ausc;
		unsigned char *mem = _mem_address[_param_scheda.fb_lavoro];
		unsigned char *ausmem0 = mem + (long) xi + _offset_riga[yi];
		unsigned char *ausmem1 = ausmem0 + (long) _param_scheda.lung_riga;
		unsigned short *ausr = (unsigned short *) riga;

		if(_n_bit_col16 == 15) {
			for(j=(n_pixel>>1)-7; j>0; j-=8) {
				ausc = *ausmem0++;
				if(*ausmem0 > 245) ausc = *ausmem0; ausmem0++;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1++;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1++;
				*ausr++ = SHORT_15(ausc, ausc, ausc);
				ausc = *ausmem0++;
				if(*ausmem0 > 245) ausc = *ausmem0; ausmem0++;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1++;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1++;
				*ausr++ = SHORT_15(ausc, ausc, ausc);
				ausc = *ausmem0++;
				if(*ausmem0 > 245) ausc = *ausmem0; ausmem0++;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1++;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1++;
				*ausr++ = SHORT_15(ausc, ausc, ausc);
				ausc = *ausmem0++;
				if(*ausmem0 > 245) ausc = *ausmem0; ausmem0++;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1++;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1++;
				*ausr++ = SHORT_15(ausc, ausc, ausc);
				ausc = *ausmem0++;
				if(*ausmem0 > 245) ausc = *ausmem0; ausmem0++;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1++;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1++;
				*ausr++ = SHORT_15(ausc, ausc, ausc);
				ausc = *ausmem0++;
				if(*ausmem0 > 245) ausc = *ausmem0; ausmem0++;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1++;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1++;
				*ausr++ = SHORT_15(ausc, ausc, ausc);
				ausc = *ausmem0++;
				if(*ausmem0 > 245) ausc = *ausmem0; ausmem0++;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1++;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1++;
				*ausr++ = SHORT_15(ausc, ausc, ausc);
				ausc = *ausmem0++;
				if(*ausmem0 > 245) ausc = *ausmem0; ausmem0++;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1++;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1++;
				*ausr++ = SHORT_15(ausc, ausc, ausc);
			}
			for(; j>-7; j--) {
				ausc = *ausmem0++;
				if(*ausmem0 > 245) ausc = *ausmem0; ausmem0++;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1++;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1++;
				*ausr++ = SHORT_15(ausc, ausc, ausc);
			}
		} else {
			for(j=(n_pixel>>1)-7; j>0; j-=8) {
				ausc = *ausmem0++;
				if(*ausmem0 > 245) ausc = *ausmem0; ausmem0++;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1++;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1++;
				*ausr++ = SHORT_16(ausc, ausc, ausc);
				ausc = *ausmem0++;
				if(*ausmem0 > 245) ausc = *ausmem0; ausmem0++;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1++;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1++;
				*ausr++ = SHORT_16(ausc, ausc, ausc);
				ausc = *ausmem0++;
				if(*ausmem0 > 245) ausc = *ausmem0; ausmem0++;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1++;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1++;
				*ausr++ = SHORT_16(ausc, ausc, ausc);
				ausc = *ausmem0++;
				if(*ausmem0 > 245) ausc = *ausmem0; ausmem0++;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1++;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1++;
				*ausr++ = SHORT_16(ausc, ausc, ausc);
				ausc = *ausmem0++;
				if(*ausmem0 > 245) ausc = *ausmem0; ausmem0++;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1++;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1++;
				*ausr++ = SHORT_16(ausc, ausc, ausc);
				ausc = *ausmem0++;
				if(*ausmem0 > 245) ausc = *ausmem0; ausmem0++;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1++;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1++;
				*ausr++ = SHORT_16(ausc, ausc, ausc);
				ausc = *ausmem0++;
				if(*ausmem0 > 245) ausc = *ausmem0; ausmem0++;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1++;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1++;
				*ausr++ = SHORT_16(ausc, ausc, ausc);
				ausc = *ausmem0++;
				if(*ausmem0 > 245) ausc = *ausmem0; ausmem0++;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1++;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1++;
				*ausr++ = SHORT_16(ausc, ausc, ausc);
			}
			for(; j>-7; j--) {
				ausc = *ausmem0++;
				if(*ausmem0 > 245) ausc = *ausmem0; ausmem0++;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1++;
				if(*ausmem1 > 245) ausc = *ausmem1; ausmem1++;
				*ausr++ = SHORT_16(ausc, ausc, ausc);
			}
		}
	}
}

/*****************************************************************************/

/* come sea2_sotto_max2_riga, ma ogni pixel prelevato e' replicato 3 volte in
   24 bit;
   n_pixel si riferisce alla lunghezza della riga da sottocampionare: di
   fatto sono copiati n_pixel/2 valori; */

void
sea2_sotto_max2_riga3(
	short xi,
	short yi,
	short n_pixel,
	unsigned char *riga
)
{
	if(!_param_scheda.scheda_aperta)
		return;
	if(_param_scheda.tipo_tv == TV_RGB) {
		register short j;
		unsigned char ausc;
		unsigned char *mem = _mem_address[_param_scheda.fb_lavoro];
		unsigned char *ausmem0 = mem + (long) (xi<<2) + _offset_riga[yi];
		unsigned char *ausmem1 = ausmem0 + (long) (_param_scheda.lung_riga<<2);
		unsigned char *ausr = riga;

		for(j=(n_pixel>>1)-7; j>0; j-=8) {
			ausc = *ausmem0; ausmem0+=4;
			if(*ausmem0 > 245) ausc = *ausmem0; ausmem0+=4;
			if(*ausmem1 > 245) ausc = *ausmem1; ausmem1+=4;
			if(*ausmem1 > 245) ausc = *ausmem1; ausmem1+=4;
			*ausr++ = ausc; *ausr++ = ausc; *ausr++ = ausc;
			ausc = *ausmem0; ausmem0+=4;
			if(*ausmem0 > 245) ausc = *ausmem0; ausmem0+=4;
			if(*ausmem1 > 245) ausc = *ausmem1; ausmem1+=4;
			if(*ausmem1 > 245) ausc = *ausmem1; ausmem1+=4;
			*ausr++ = ausc; *ausr++ = ausc; *ausr++ = ausc;
			ausc = *ausmem0; ausmem0+=4;
			if(*ausmem0 > 245) ausc = *ausmem0; ausmem0+=4;
			if(*ausmem1 > 245) ausc = *ausmem1; ausmem1+=4;
			if(*ausmem1 > 245) ausc = *ausmem1; ausmem1+=4;
			*ausr++ = ausc; *ausr++ = ausc; *ausr++ = ausc;
			ausc = *ausmem0; ausmem0+=4;
			if(*ausmem0 > 245) ausc = *ausmem0; ausmem0+=4;
			if(*ausmem1 > 245) ausc = *ausmem1; ausmem1+=4;
			if(*ausmem1 > 245) ausc = *ausmem1; ausmem1+=4;
			*ausr++ = ausc; *ausr++ = ausc; *ausr++ = ausc;
			ausc = *ausmem0; ausmem0+=4;
			if(*ausmem0 > 245) ausc = *ausmem0; ausmem0+=4;
			if(*ausmem1 > 245) ausc = *ausmem1; ausmem1+=4;
			if(*ausmem1 > 245) ausc = *ausmem1; ausmem1+=4;
			*ausr++ = ausc; *ausr++ = ausc; *ausr++ = ausc;
			ausc = *ausmem0; ausmem0+=4;
			if(*ausmem0 > 245) ausc = *ausmem0; ausmem0+=4;
			if(*ausmem1 > 245) ausc = *ausmem1; ausmem1+=4;
			if(*ausmem1 > 245) ausc = *ausmem1; ausmem1+=4;
			*ausr++ = ausc; *ausr++ = ausc; *ausr++ = ausc;
			ausc = *ausmem0; ausmem0+=4;
			if(*ausmem0 > 245) ausc = *ausmem0; ausmem0+=4;
			if(*ausmem1 > 245) ausc = *ausmem1; ausmem1+=4;
			if(*ausmem1 > 245) ausc = *ausmem1; ausmem1+=4;
			*ausr++ = ausc; *ausr++ = ausc; *ausr++ = ausc;
			ausc = *ausmem0; ausmem0+=4;
			if(*ausmem0 > 245) ausc = *ausmem0; ausmem0+=4;
			if(*ausmem1 > 245) ausc = *ausmem1; ausmem1+=4;
			if(*ausmem1 > 245) ausc = *ausmem1; ausmem1+=4;
			*ausr++ = ausc; *ausr++ = ausc; *ausr++ = ausc;
		}
		for(; j>-7; j--) {
			ausc = *ausmem0; ausmem0+=4;
			if(*ausmem0 > 245) ausc = *ausmem0; ausmem0+=4;
			if(*ausmem1 > 245) ausc = *ausmem1; ausmem1+=4;
			if(*ausmem1 > 245) ausc = *ausmem1; ausmem1+=4;
			*ausr++ = ausc; *ausr++ = ausc; *ausr++ = ausc;
		}
	} else {
		register short j;
		unsigned char ausc;
		unsigned char *mem = _mem_address[_param_scheda.fb_lavoro];
		unsigned char *ausmem0 = mem + (long) xi + _offset_riga[yi];
		unsigned char *ausmem1 = ausmem0 + (long) _param_scheda.lung_riga;
		unsigned char *ausr = riga;

		for(j=(n_pixel>>1)-7; j>0; j-=8) {
			ausc = *ausmem0++;
			if(*ausmem0 > 245) ausc = *ausmem0; ausmem0++;
			if(*ausmem1 > 245) ausc = *ausmem1; ausmem1++;
			if(*ausmem1 > 245) ausc = *ausmem1; ausmem1++;
			*ausr++ = ausc; *ausr++ = ausc; *ausr++ = ausc;
			ausc = *ausmem0++;
			if(*ausmem0 > 245) ausc = *ausmem0; ausmem0++;
			if(*ausmem1 > 245) ausc = *ausmem1; ausmem1++;
			if(*ausmem1 > 245) ausc = *ausmem1; ausmem1++;
			*ausr++ = ausc; *ausr++ = ausc; *ausr++ = ausc;
			ausc = *ausmem0++;
			if(*ausmem0 > 245) ausc = *ausmem0; ausmem0++;
			if(*ausmem1 > 245) ausc = *ausmem1; ausmem1++;
			if(*ausmem1 > 245) ausc = *ausmem1; ausmem1++;
			*ausr++ = ausc; *ausr++ = ausc; *ausr++ = ausc;
			ausc = *ausmem0++;
			if(*ausmem0 > 245) ausc = *ausmem0; ausmem0++;
			if(*ausmem1 > 245) ausc = *ausmem1; ausmem1++;
			if(*ausmem1 > 245) ausc = *ausmem1; ausmem1++;
			*ausr++ = ausc; *ausr++ = ausc; *ausr++ = ausc;
			ausc = *ausmem0++;
			if(*ausmem0 > 245) ausc = *ausmem0; ausmem0++;
			if(*ausmem1 > 245) ausc = *ausmem1; ausmem1++;
			if(*ausmem1 > 245) ausc = *ausmem1; ausmem1++;
			*ausr++ = ausc; *ausr++ = ausc; *ausr++ = ausc;
			ausc = *ausmem0++;
			if(*ausmem0 > 245) ausc = *ausmem0; ausmem0++;
			if(*ausmem1 > 245) ausc = *ausmem1; ausmem1++;
			if(*ausmem1 > 245) ausc = *ausmem1; ausmem1++;
			*ausr++ = ausc; *ausr++ = ausc; *ausr++ = ausc;
			ausc = *ausmem0++;
			if(*ausmem0 > 245) ausc = *ausmem0; ausmem0++;
			if(*ausmem1 > 245) ausc = *ausmem1; ausmem1++;
			if(*ausmem1 > 245) ausc = *ausmem1; ausmem1++;
			*ausr++ = ausc; *ausr++ = ausc; *ausr++ = ausc;
			ausc = *ausmem0++;
			if(*ausmem0 > 245) ausc = *ausmem0; ausmem0++;
			if(*ausmem1 > 245) ausc = *ausmem1; ausmem1++;
			if(*ausmem1 > 245) ausc = *ausmem1; ausmem1++;
			*ausr++ = ausc; *ausr++ = ausc; *ausr++ = ausc;
		}
		for(; j>-7; j--) {
			ausc = *ausmem0++;
			if(*ausmem0 > 245) ausc = *ausmem0; ausmem0++;
			if(*ausmem1 > 245) ausc = *ausmem1; ausmem1++;
			if(*ausmem1 > 245) ausc = *ausmem1; ausmem1++;
			*ausr++ = ausc; *ausr++ = ausc; *ausr++ = ausc;
		}
	}
}

/*****************************************************************************/

/* sottocampiona l'intorno (2x2) formato dalla riga yi e yi+1; se vi e' un
   pixel con valori a 0 o a 255, viene prelevato;
   n_pixel si riferisce alla lunghezza della riga da sottocampionare: di
   fatto sono copiati n_pixel/2 pixel; */

void
sea2_sotto_max2_riga_col(
	short xi,
	short yi,
	short n_pixel,
	unsigned char *riga
)
{
	if(!_param_scheda.scheda_aperta)
		return;
	if(_param_scheda.tipo_tv == TV_RGB) {
		register short j;
		unsigned char *mem = _mem_address_col[_param_scheda.fb_lavoro];
		unsigned char *ausmem0 = mem + (long) (xi<<2) + _offset_riga[yi];
		unsigned char *ausmem1 = ausmem0 + (long) (_param_scheda.lung_riga<<2);
		unsigned char *ausr = riga;

		for(j=n_pixel>>1; j>0; j--) {
			*ausr++ = *ausmem0++; *ausr++ = *ausmem0++;
			*ausr++ = *ausmem0++; *ausr++ = *ausmem0++;
			if(*ausmem0 == 255 || *ausmem0 == 0) {
				*(ausr-4) = *ausmem0++; *(ausr-3) = *ausmem0++;
				*(ausr-2) = *ausmem0++; *(ausr-1) = *ausmem0++;
			} else {
				ausmem0+=4;
			}
			if(*ausmem1 == 255 || *ausmem1 == 0) {
				*(ausr-4) = *ausmem1++; *(ausr-3) = *ausmem1++;
				*(ausr-2) = *ausmem1++; *(ausr-1) = *ausmem1++;
			} else {
				ausmem1+=4;
			}
			if(*ausmem1 == 255 || *ausmem1 == 0) {
				*(ausr-4) = *ausmem1++; *(ausr-3) = *ausmem1++;
				*(ausr-2) = *ausmem1++; *(ausr-1) = *ausmem1++;
			} else {
				ausmem1+=4;
			}
		}
	}
}

/**************************************************************************/

/* come sea2_leggi_riga, ma replica 4 volte ogni pixel letto; pertanto
   il vettore riga deve essere lungo almeno 4*n_pixel */

void
sea2_leggi_riga4(
	short xi,
	short yi,
	short n_pixel,
	unsigned char *riga
)
{
	if(!_param_scheda.scheda_aperta)
		return;
	if(_param_scheda.tipo_tv == TV_RGB) {
		short i;
		unsigned char *mem = _mem_address[_param_scheda.fb_lavoro];
		unsigned char *ausm = mem + (long) (xi<<2) + _offset_riga[yi];
		unsigned char *ausr = riga;

		for(i=n_pixel-7; i>0; i-=8) {
			*ausr++ = *ausm; *ausr++ = *ausm; *ausr++ = *ausm; *ausr++ = *ausm;
			ausm += 4;
			*ausr++ = *ausm; *ausr++ = *ausm; *ausr++ = *ausm; *ausr++ = *ausm;
			ausm += 4;
			*ausr++ = *ausm; *ausr++ = *ausm; *ausr++ = *ausm; *ausr++ = *ausm;
			ausm += 4;
			*ausr++ = *ausm; *ausr++ = *ausm; *ausr++ = *ausm; *ausr++ = *ausm;
			ausm += 4;
			*ausr++ = *ausm; *ausr++ = *ausm; *ausr++ = *ausm; *ausr++ = *ausm;
			ausm += 4;
			*ausr++ = *ausm; *ausr++ = *ausm; *ausr++ = *ausm; *ausr++ = *ausm;
			ausm += 4;
			*ausr++ = *ausm; *ausr++ = *ausm; *ausr++ = *ausm; *ausr++ = *ausm;
			ausm += 4;
			*ausr++ = *ausm; *ausr++ = *ausm; *ausr++ = *ausm; *ausr++ = *ausm;
			ausm += 4;
		}
		for(; i>-7; i--) {
			*ausr++ = *ausm; *ausr++ = *ausm; *ausr++ = *ausm; *ausr++ = *ausm;
			ausm += 4;
		}
	} else {
		short i;
		unsigned char *mem = _mem_address[_param_scheda.fb_lavoro];
		unsigned char *ausm = mem + (long) xi + _offset_riga[yi];
		unsigned char *ausr = riga;

		for(i=n_pixel-7; i>0; i-=8) {
			*ausr++ = *ausm; *ausr++ = *ausm; *ausr++ = *ausm; *ausr++ = *ausm++;
			*ausr++ = *ausm; *ausr++ = *ausm; *ausr++ = *ausm; *ausr++ = *ausm++;
			*ausr++ = *ausm; *ausr++ = *ausm; *ausr++ = *ausm; *ausr++ = *ausm++;
			*ausr++ = *ausm; *ausr++ = *ausm; *ausr++ = *ausm; *ausr++ = *ausm++;
			*ausr++ = *ausm; *ausr++ = *ausm; *ausr++ = *ausm; *ausr++ = *ausm++;
			*ausr++ = *ausm; *ausr++ = *ausm; *ausr++ = *ausm; *ausr++ = *ausm++;
			*ausr++ = *ausm; *ausr++ = *ausm; *ausr++ = *ausm; *ausr++ = *ausm++;
			*ausr++ = *ausm; *ausr++ = *ausm; *ausr++ = *ausm; *ausr++ = *ausm++;
		}
		for(; i>-7; i--) {
			*ausr++ = *ausm; *ausr++ = *ausm; *ausr++ = *ausm; *ausr++ = *ausm++;
		}
	}
	return;
}

/**************************************************************************/

/* come sea2_leggi_riga, ma ogni pixel letto viene replicato 3 volte in 16
   bit; pertanto il vettore riga deve essere lungo almeno 2*n_pixel */

void
sea2_leggi_riga2(
	short xi,
	short yi,
	short n_pixel,
	unsigned char *riga
)
{
	if(!_param_scheda.scheda_aperta)
		return;
	if(_param_scheda.tipo_tv == TV_RGB) {
		short i;
		unsigned char *mem = _mem_address[_param_scheda.fb_lavoro];
		unsigned char *ausm = mem + (long) (xi<<2) + _offset_riga[yi];
		unsigned short *ausr = (unsigned short *) riga;

		if(_n_bit_col16 == 15) {
			for(i=n_pixel-7; i>0; i-=8) {
				*ausr++ = SHORT_15(*ausm, *ausm, *ausm);
				ausm += 4;
				*ausr++ = SHORT_15(*ausm, *ausm, *ausm);
				ausm += 4;
				*ausr++ = SHORT_15(*ausm, *ausm, *ausm);
				ausm += 4;
				*ausr++ = SHORT_15(*ausm, *ausm, *ausm);
				ausm += 4;
				*ausr++ = SHORT_15(*ausm, *ausm, *ausm);
				ausm += 4;
				*ausr++ = SHORT_15(*ausm, *ausm, *ausm);
				ausm += 4;
				*ausr++ = SHORT_15(*ausm, *ausm, *ausm);
				ausm += 4;
				*ausr++ = SHORT_15(*ausm, *ausm, *ausm);
				ausm += 4;
			}
			for(; i>-7; i--) {
				*ausr++ = SHORT_15(*ausm, *ausm, *ausm);
				ausm += 4;
			}
		} else {
			for(i=n_pixel-7; i>0; i-=8) {
				*ausr++ = SHORT_16(*ausm, *ausm, *ausm);
				ausm += 4;
				*ausr++ = SHORT_16(*ausm, *ausm, *ausm);
				ausm += 4;
				*ausr++ = SHORT_16(*ausm, *ausm, *ausm);
				ausm += 4;
				*ausr++ = SHORT_16(*ausm, *ausm, *ausm);
				ausm += 4;
				*ausr++ = SHORT_16(*ausm, *ausm, *ausm);
				ausm += 4;
				*ausr++ = SHORT_16(*ausm, *ausm, *ausm);
				ausm += 4;
				*ausr++ = SHORT_16(*ausm, *ausm, *ausm);
				ausm += 4;
				*ausr++ = SHORT_16(*ausm, *ausm, *ausm);
				ausm += 4;
			}
			for(; i>-7; i--) {
				*ausr++ = SHORT_16(*ausm, *ausm, *ausm);
				ausm += 4;
			}
		}
	} else {
		short i;
		unsigned char *mem = _mem_address[_param_scheda.fb_lavoro];
		unsigned char *ausm = mem + (long) xi + _offset_riga[yi];
		unsigned short *ausr = (unsigned short *) riga;

		if(_n_bit_col16 == 15) {
			for(i=n_pixel-7; i>0; i-=8) {
				*ausr++ = SHORT_15(*ausm, *ausm, *ausm);
				ausm++;
				*ausr++ = SHORT_15(*ausm, *ausm, *ausm);
				ausm++;
				*ausr++ = SHORT_15(*ausm, *ausm, *ausm);
				ausm++;
				*ausr++ = SHORT_15(*ausm, *ausm, *ausm);
				ausm++;
				*ausr++ = SHORT_15(*ausm, *ausm, *ausm);
				ausm++;
				*ausr++ = SHORT_15(*ausm, *ausm, *ausm);
				ausm++;
				*ausr++ = SHORT_15(*ausm, *ausm, *ausm);
				ausm++;
				*ausr++ = SHORT_15(*ausm, *ausm, *ausm);
				ausm++;
			}
			for(; i>-7; i--) {
				*ausr++ = SHORT_15(*ausm, *ausm, *ausm);
				ausm++;
			}
		} else {
			for(i=n_pixel-7; i>0; i-=8) {
				*ausr++ = SHORT_16(*ausm, *ausm, *ausm);
				ausm++;
				*ausr++ = SHORT_16(*ausm, *ausm, *ausm);
				ausm++;
				*ausr++ = SHORT_16(*ausm, *ausm, *ausm);
				ausm++;
				*ausr++ = SHORT_16(*ausm, *ausm, *ausm);
				ausm++;
				*ausr++ = SHORT_16(*ausm, *ausm, *ausm);
				ausm++;
				*ausr++ = SHORT_16(*ausm, *ausm, *ausm);
				ausm++;
				*ausr++ = SHORT_16(*ausm, *ausm, *ausm);
				ausm++;
				*ausr++ = SHORT_16(*ausm, *ausm, *ausm);
				ausm++;
			}
			for(; i>-7; i--) {
				*ausr++ = SHORT_16(*ausm, *ausm, *ausm);
				ausm++;
			}
		}
	}
	return;
}

/**************************************************************************/

/* ogni pixel a colori letto (4 byte) viene impaccato in 16
   bit; pertanto il vettore riga deve essere lungo almeno 2*n_pixel */

void
sea2_leggi_riga_col16(
	short xi,
	short yi,
	short n_pixel,
	unsigned char *riga
)
{
	if(!_param_scheda.scheda_aperta)
		return;
	if(_param_scheda.tipo_tv == TV_RGB) {
		short i;
		unsigned char *mem = _mem_address_col[_param_scheda.fb_lavoro];
		unsigned char *ausm = mem + (long) (xi<<2) + _offset_riga[yi];
		unsigned short *ausr = (unsigned short *) riga;

		if(_n_bit_col16 == 15) {
			for(i=n_pixel-7; i>0; i-=8) {
				*ausr++ = SHORT_15(*(ausm+2), *(ausm+1), *ausm);
				ausm += 4;
				*ausr++ = SHORT_15(*(ausm+2), *(ausm+1), *ausm);
				ausm += 4;
				*ausr++ = SHORT_15(*(ausm+2), *(ausm+1), *ausm);
				ausm += 4;
				*ausr++ = SHORT_15(*(ausm+2), *(ausm+1), *ausm);
				ausm += 4;
				*ausr++ = SHORT_15(*(ausm+2), *(ausm+1), *ausm);
				ausm += 4;
				*ausr++ = SHORT_15(*(ausm+2), *(ausm+1), *ausm);
				ausm += 4;
				*ausr++ = SHORT_15(*(ausm+2), *(ausm+1), *ausm);
				ausm += 4;
				*ausr++ = SHORT_15(*(ausm+2), *(ausm+1), *ausm);
				ausm += 4;
			}
			for(; i>-7; i--) {
				*ausr++ = SHORT_15(*(ausm+2), *(ausm+1), *ausm);
				ausm += 4;
			}
		} else {
			for(i=n_pixel-7; i>0; i-=8) {
				*ausr++ = SHORT_16(*(ausm+2), *(ausm+1), *ausm);
				ausm += 4;
				*ausr++ = SHORT_16(*(ausm+2), *(ausm+1), *ausm);
				ausm += 4;
				*ausr++ = SHORT_16(*(ausm+2), *(ausm+1), *ausm);
				ausm += 4;
				*ausr++ = SHORT_16(*(ausm+2), *(ausm+1), *ausm);
				ausm += 4;
				*ausr++ = SHORT_16(*(ausm+2), *(ausm+1), *ausm);
				ausm += 4;
				*ausr++ = SHORT_16(*(ausm+2), *(ausm+1), *ausm);
				ausm += 4;
				*ausr++ = SHORT_16(*(ausm+2), *(ausm+1), *ausm);
				ausm += 4;
				*ausr++ = SHORT_16(*(ausm+2), *(ausm+1), *ausm);
				ausm += 4;
			}
			for(; i>-7; i--) {
				*ausr++ = SHORT_16(*(ausm+2), *(ausm+1), *ausm);
				ausm += 4;
			}
		}
	}
	return;
}

/**************************************************************************/

/* come sea2_leggi_riga, ma ogni pixel letto viene replicato 3 volte in 24
   bit; pertanto il vettore riga deve essere lungo almeno 3*n_pixel */

void
sea2_leggi_riga3(
	short xi,
	short yi,
	short n_pixel,
	unsigned char *riga
)
{
	if(!_param_scheda.scheda_aperta)
		return;
	if(_param_scheda.tipo_tv == TV_RGB) {
		short i;
		unsigned char *mem = _mem_address[_param_scheda.fb_lavoro];
		unsigned char *ausm = mem + (long) (xi<<2) + _offset_riga[yi];
		unsigned char *ausr = riga;

		for(i=n_pixel-7; i>0; i-=8) {
			*ausr++ = *ausm; *ausr++ = *ausm; *ausr++ = *ausm; ausm += 4;
			*ausr++ = *ausm; *ausr++ = *ausm; *ausr++ = *ausm; ausm += 4;
			*ausr++ = *ausm; *ausr++ = *ausm; *ausr++ = *ausm; ausm += 4;
			*ausr++ = *ausm; *ausr++ = *ausm; *ausr++ = *ausm; ausm += 4;
			*ausr++ = *ausm; *ausr++ = *ausm; *ausr++ = *ausm; ausm += 4;
			*ausr++ = *ausm; *ausr++ = *ausm; *ausr++ = *ausm; ausm += 4;
			*ausr++ = *ausm; *ausr++ = *ausm; *ausr++ = *ausm; ausm += 4;
			*ausr++ = *ausm; *ausr++ = *ausm; *ausr++ = *ausm; ausm += 4;
		}
		for(; i>-7; i--) {
			*ausr++ = *ausm; *ausr++ = *ausm; *ausr++ = *ausm; ausm += 4;
		}
	} else {
		short i;
		unsigned char *mem = _mem_address[_param_scheda.fb_lavoro];
		unsigned char *ausm = mem + (long) xi + _offset_riga[yi];
		unsigned char *ausr = riga;

		for(i=n_pixel-7; i>0; i-=8) {
			*ausr++ = *ausm; *ausr++ = *ausm; *ausr++ = *ausm++;
			*ausr++ = *ausm; *ausr++ = *ausm; *ausr++ = *ausm++;
			*ausr++ = *ausm; *ausr++ = *ausm; *ausr++ = *ausm++;
			*ausr++ = *ausm; *ausr++ = *ausm; *ausr++ = *ausm++;
			*ausr++ = *ausm; *ausr++ = *ausm; *ausr++ = *ausm++;
			*ausr++ = *ausm; *ausr++ = *ausm; *ausr++ = *ausm++;
			*ausr++ = *ausm; *ausr++ = *ausm; *ausr++ = *ausm++;
			*ausr++ = *ausm; *ausr++ = *ausm; *ausr++ = *ausm++;
		}
		for(; i>-7; i--) {
			*ausr++ = *ausm; *ausr++ = *ausm; *ausr++ = *ausm++;
		}
	}
	return;
}

/**************************************************************************/

/* ogni pixel a colori letto (4 byte) viene impaccato in 24
   bit; pertanto il vettore riga deve essere lungo almeno 3*n_pixel */

void
sea2_leggi_riga_col24(
	short xi,
	short yi,
	short n_pixel,
	unsigned char *riga
)
{
	if(!_param_scheda.scheda_aperta)
		return;
	if(_param_scheda.tipo_tv == TV_RGB) {
		short i;
		unsigned char *mem = _mem_address_col[_param_scheda.fb_lavoro];
		unsigned char *ausm = mem + (long) (xi<<2) + _offset_riga[yi];
		unsigned char *ausr = riga;

		for(i=n_pixel-7; i>0; i-=8) {
			*ausr++ = *ausm++; *ausr++ = *ausm++; *ausr++ = *ausm++; ausm++;
			*ausr++ = *ausm++; *ausr++ = *ausm++; *ausr++ = *ausm++; ausm++;
			*ausr++ = *ausm++; *ausr++ = *ausm++; *ausr++ = *ausm++; ausm++;
			*ausr++ = *ausm++; *ausr++ = *ausm++; *ausr++ = *ausm++; ausm++;
			*ausr++ = *ausm++; *ausr++ = *ausm++; *ausr++ = *ausm++; ausm++;
			*ausr++ = *ausm++; *ausr++ = *ausm++; *ausr++ = *ausm++; ausm++;
			*ausr++ = *ausm++; *ausr++ = *ausm++; *ausr++ = *ausm++; ausm++;
			*ausr++ = *ausm++; *ausr++ = *ausm++; *ausr++ = *ausm++; ausm++;
		}
		for(; i>-7; i--) {
			*ausr++ = *ausm++; *ausr++ = *ausm++; *ausr++ = *ausm++; ausm++;
		}
	}
	return;
}

/***************************************************************************/

/* come sea2_sotto_max2_riga, ma ogni pixel e' replicato 4 volte; pertanto
   il vettore riga deve essere 4 volte piu' lungo */

void
sea2_sotto_max2_riga4(
	short xi,
	short yi,
	short n_pixel,
	unsigned char *riga
)
{
	if(!_param_scheda.scheda_aperta)
		return;
	if(_param_scheda.tipo_tv == TV_RGB) {
		register short j;
		unsigned char ch;
		unsigned char *mem = _mem_address[_param_scheda.fb_lavoro];
		unsigned char *ausmem0 = mem + (long) (xi<<2) + _offset_riga[yi];
		unsigned char *ausmem1 = ausmem0 + (long) (_param_scheda.lung_riga<<2);
		unsigned char *ausr = riga;

		for(j=(n_pixel>>1)-7; j>0; j-=8) {
			ch = *ausmem0; ausmem0+=4;
			if(*ausmem0 > 245) ch = *ausmem0; ausmem0+=4;
			if(*ausmem1 > 245) ch = *ausmem1; ausmem1+=4;
			if(*ausmem1 > 245) ch = *ausmem1; ausmem1+=4;
			*ausr++ = ch; *ausr++ = ch; *ausr++ = ch; *ausr++ = ch;
			ch = *ausmem0; ausmem0+=4;
			if(*ausmem0 > 245) ch = *ausmem0; ausmem0+=4;
			if(*ausmem1 > 245) ch = *ausmem1; ausmem1+=4;
			if(*ausmem1 > 245) ch = *ausmem1; ausmem1+=4;
			*ausr++ = ch; *ausr++ = ch; *ausr++ = ch; *ausr++ = ch;
			ch = *ausmem0; ausmem0+=4;
			if(*ausmem0 > 245) ch = *ausmem0; ausmem0+=4;
			if(*ausmem1 > 245) ch = *ausmem1; ausmem1+=4;
			if(*ausmem1 > 245) ch = *ausmem1; ausmem1+=4;
			*ausr++ = ch; *ausr++ = ch; *ausr++ = ch; *ausr++ = ch;
			ch = *ausmem0; ausmem0+=4;
			if(*ausmem0 > 245) ch = *ausmem0; ausmem0+=4;
			if(*ausmem1 > 245) ch = *ausmem1; ausmem1+=4;
			if(*ausmem1 > 245) ch = *ausmem1; ausmem1+=4;
			*ausr++ = ch; *ausr++ = ch; *ausr++ = ch; *ausr++ = ch;
			ch = *ausmem0; ausmem0+=4;
			if(*ausmem0 > 245) ch = *ausmem0; ausmem0+=4;
			if(*ausmem1 > 245) ch = *ausmem1; ausmem1+=4;
			if(*ausmem1 > 245) ch = *ausmem1; ausmem1+=4;
			*ausr++ = ch; *ausr++ = ch; *ausr++ = ch; *ausr++ = ch;
			ch = *ausmem0; ausmem0+=4;
			if(*ausmem0 > 245) ch = *ausmem0; ausmem0+=4;
			if(*ausmem1 > 245) ch = *ausmem1; ausmem1+=4;
			if(*ausmem1 > 245) ch = *ausmem1; ausmem1+=4;
			*ausr++ = ch; *ausr++ = ch; *ausr++ = ch; *ausr++ = ch;
			ch = *ausmem0; ausmem0+=4;
			if(*ausmem0 > 245) ch = *ausmem0; ausmem0+=4;
			if(*ausmem1 > 245) ch = *ausmem1; ausmem1+=4;
			if(*ausmem1 > 245) ch = *ausmem1; ausmem1+=4;
			*ausr++ = ch; *ausr++ = ch; *ausr++ = ch; *ausr++ = ch;
			ch = *ausmem0; ausmem0+=4;
			if(*ausmem0 > 245) ch = *ausmem0; ausmem0+=4;
			if(*ausmem1 > 245) ch = *ausmem1; ausmem1+=4;
			if(*ausmem1 > 245) ch = *ausmem1; ausmem1+=4;
			*ausr++ = ch; *ausr++ = ch; *ausr++ = ch; *ausr++ = ch;
		}
		for(; j>-7; j--) {
			ch = *ausmem0; ausmem0+=4;
			if(*ausmem0 > 245) ch = *ausmem0; ausmem0+=4;
			if(*ausmem1 > 245) ch = *ausmem1; ausmem1+=4;
			if(*ausmem1 > 245) ch = *ausmem1; ausmem1+=4;
			*ausr++ = ch; *ausr++ = ch; *ausr++ = ch; *ausr++ = ch;
		}
	} else {
		register short j;
		unsigned char ch;
		unsigned char *mem = _mem_address[_param_scheda.fb_lavoro];
		unsigned char *ausmem0 = mem + (long) xi + _offset_riga[yi];
		unsigned char *ausmem1 = ausmem0 + (long) _param_scheda.lung_riga;
		unsigned char *ausr = riga;

		for(j=(n_pixel>>1)-7; j>0; j-=8) {
			ch = *ausmem0++;
			if(*ausmem0++ > 245) ch = *(ausmem0-1);
			if(*ausmem1++ > 245) ch = *(ausmem1-1);
			if(*ausmem1++ > 245) ch = *(ausmem1-1);
			*ausr++ = ch; *ausr++ = ch; *ausr++ = ch; *ausr++ = ch;
			ch = *ausmem0++;
			if(*ausmem0++ > 245) ch = *(ausmem0-1);
			if(*ausmem1++ > 245) ch = *(ausmem1-1);
			if(*ausmem1++ > 245) ch = *(ausmem1-1);
			*ausr++ = ch; *ausr++ = ch; *ausr++ = ch; *ausr++ = ch;
			ch = *ausmem0++;
			if(*ausmem0++ > 245) ch = *(ausmem0-1);
			if(*ausmem1++ > 245) ch = *(ausmem1-1);
			if(*ausmem1++ > 245) ch = *(ausmem1-1);
			*ausr++ = ch; *ausr++ = ch; *ausr++ = ch; *ausr++ = ch;
			ch = *ausmem0++;
			if(*ausmem0++ > 245) ch = *(ausmem0-1);
			if(*ausmem1++ > 245) ch = *(ausmem1-1);
			if(*ausmem1++ > 245) ch = *(ausmem1-1);
			*ausr++ = ch; *ausr++ = ch; *ausr++ = ch; *ausr++ = ch;
			ch = *ausmem0++;
			if(*ausmem0++ > 245) ch = *(ausmem0-1);
			if(*ausmem1++ > 245) ch = *(ausmem1-1);
			if(*ausmem1++ > 245) ch = *(ausmem1-1);
			*ausr++ = ch; *ausr++ = ch; *ausr++ = ch; *ausr++ = ch;
			ch = *ausmem0++;
			if(*ausmem0++ > 245) ch = *(ausmem0-1);
			if(*ausmem1++ > 245) ch = *(ausmem1-1);
			if(*ausmem1++ > 245) ch = *(ausmem1-1);
			*ausr++ = ch; *ausr++ = ch; *ausr++ = ch; *ausr++ = ch;
			ch = *ausmem0++;
			if(*ausmem0++ > 245) ch = *(ausmem0-1);
			if(*ausmem1++ > 245) ch = *(ausmem1-1);
			if(*ausmem1++ > 245) ch = *(ausmem1-1);
			*ausr++ = ch; *ausr++ = ch; *ausr++ = ch; *ausr++ = ch;
			ch = *ausmem0++;
			if(*ausmem0++ > 245) ch = *(ausmem0-1);
			if(*ausmem1++ > 245) ch = *(ausmem1-1);
			if(*ausmem1++ > 245) ch = *(ausmem1-1);
			*ausr++ = ch; *ausr++ = ch; *ausr++ = ch; *ausr++ = ch;
		}
		for(; j>-7; j--) {
			ch = *ausmem0++;
			if(*ausmem0++ > 245) ch = *(ausmem0-1);
			if(*ausmem1++ > 245) ch = *(ausmem1-1);
			if(*ausmem1++ > 245) ch = *(ausmem1-1);
			*ausr++ = ch; *ausr++ = ch; *ausr++ = ch; *ausr++ = ch;
		}
	}
}

/***************************************************************************/

/* come sea2_sotto_max2_riga4, ma con una diversa gestione dei punti
   a 0 e a 255; */

void
sea2_sotto_max2_riga4_col(
	short xi,
	short yi,
	short n_pixel,
	unsigned char *riga
)
{
	if(!_param_scheda.scheda_aperta)
		return;
	if(_param_scheda.tipo_tv == TV_RGB) {
		register short j;
		unsigned char ch;
		unsigned char *mem = _mem_address[_param_scheda.fb_lavoro];
		unsigned char *ausmem0 = mem + (long) (xi<<2) + _offset_riga[yi];
		unsigned char *ausmem1 = ausmem0 + (long) (_param_scheda.lung_riga<<2);
		unsigned char *ausr = riga;

		for(j=(n_pixel>>1)-7; j>0; j-=8) {
			ch = *ausmem0; ausmem0+=4;
			if(*ausmem0 == 255 || !*ausmem0) ch = *ausmem0; ausmem0+=4;
			if(*ausmem1 == 255 || !*ausmem1) ch = *ausmem1; ausmem1+=4;
			if(*ausmem1 == 255 || !*ausmem1) ch = *ausmem1; ausmem1+=4;
			*ausr++ = ch; *ausr++ = ch; *ausr++ = ch; *ausr++ = ch;
			ch = *ausmem0; ausmem0+=4;
			if(*ausmem0 == 255 || !*ausmem0) ch = *ausmem0; ausmem0+=4;
			if(*ausmem1 == 255 || !*ausmem1) ch = *ausmem1; ausmem1+=4;
			if(*ausmem1 == 255 || !*ausmem1) ch = *ausmem1; ausmem1+=4;
			*ausr++ = ch; *ausr++ = ch; *ausr++ = ch; *ausr++ = ch;
			ch = *ausmem0; ausmem0+=4;
			if(*ausmem0 == 255 || !*ausmem0) ch = *ausmem0; ausmem0+=4;
			if(*ausmem1 == 255 || !*ausmem1) ch = *ausmem1; ausmem1+=4;
			if(*ausmem1 == 255 || !*ausmem1) ch = *ausmem1; ausmem1+=4;
			*ausr++ = ch; *ausr++ = ch; *ausr++ = ch; *ausr++ = ch;
			ch = *ausmem0; ausmem0+=4;
			if(*ausmem0 == 255 || !*ausmem0) ch = *ausmem0; ausmem0+=4;
			if(*ausmem1 == 255 || !*ausmem1) ch = *ausmem1; ausmem1+=4;
			if(*ausmem1 == 255 || !*ausmem1) ch = *ausmem1; ausmem1+=4;
			*ausr++ = ch; *ausr++ = ch; *ausr++ = ch; *ausr++ = ch;
			ch = *ausmem0; ausmem0+=4;
			if(*ausmem0 == 255 || !*ausmem0) ch = *ausmem0; ausmem0+=4;
			if(*ausmem1 == 255 || !*ausmem1) ch = *ausmem1; ausmem1+=4;
			if(*ausmem1 == 255 || !*ausmem1) ch = *ausmem1; ausmem1+=4;
			*ausr++ = ch; *ausr++ = ch; *ausr++ = ch; *ausr++ = ch;
			ch = *ausmem0; ausmem0+=4;
			if(*ausmem0 == 255 || !*ausmem0) ch = *ausmem0; ausmem0+=4;
			if(*ausmem1 == 255 || !*ausmem1) ch = *ausmem1; ausmem1+=4;
			if(*ausmem1 == 255 || !*ausmem1) ch = *ausmem1; ausmem1+=4;
			*ausr++ = ch; *ausr++ = ch; *ausr++ = ch; *ausr++ = ch;
			ch = *ausmem0; ausmem0+=4;
			if(*ausmem0 == 255 || !*ausmem0) ch = *ausmem0; ausmem0+=4;
			if(*ausmem1 == 255 || !*ausmem1) ch = *ausmem1; ausmem1+=4;
			if(*ausmem1 == 255 || !*ausmem1) ch = *ausmem1; ausmem1+=4;
			*ausr++ = ch; *ausr++ = ch; *ausr++ = ch; *ausr++ = ch;
			ch = *ausmem0; ausmem0+=4;
			if(*ausmem0 == 255 || !*ausmem0) ch = *ausmem0; ausmem0+=4;
			if(*ausmem1 == 255 || !*ausmem1) ch = *ausmem1; ausmem1+=4;
			if(*ausmem1 == 255 || !*ausmem1) ch = *ausmem1; ausmem1+=4;
			*ausr++ = ch; *ausr++ = ch; *ausr++ = ch; *ausr++ = ch;
		}
		for(; j>-7; j--) {
			ch = *ausmem0; ausmem0+=4;
			if(*ausmem0 == 255 || !*ausmem0) ch = *ausmem0; ausmem0+=4;
			if(*ausmem1 == 255 || !*ausmem1) ch = *ausmem1; ausmem1+=4;
			if(*ausmem1 == 255 || !*ausmem1) ch = *ausmem1; ausmem1+=4;
			*ausr++ = ch; *ausr++ = ch; *ausr++ = ch; *ausr++ = ch;
		}
	} else {
		register short j;
		unsigned char ch;
		unsigned char *mem = _mem_address[_param_scheda.fb_lavoro];
		unsigned char *ausmem0 = mem + (long) xi + _offset_riga[yi];
		unsigned char *ausmem1 = ausmem0 + (long) _param_scheda.lung_riga;
		unsigned char *ausr = riga;

		for(j=(n_pixel>>1)-7; j>0; j-=8) {
			ch = *ausmem0++;
			if(*ausmem0++ == 255 || !*ausmem0) ch = *(ausmem0-1);
			if(*ausmem1++ == 255 || !*ausmem1) ch = *(ausmem1-1);
			if(*ausmem1++ == 255 || !*ausmem1) ch = *(ausmem1-1);
			*ausr++ = ch; *ausr++ = ch; *ausr++ = ch; *ausr++ = ch;
			ch = *ausmem0++;
			if(*ausmem0++ == 255 || !*ausmem0) ch = *(ausmem0-1);
			if(*ausmem1++ == 255 || !*ausmem1) ch = *(ausmem1-1);
			if(*ausmem1++ == 255 || !*ausmem1) ch = *(ausmem1-1);
			*ausr++ = ch; *ausr++ = ch; *ausr++ = ch; *ausr++ = ch;
			ch = *ausmem0++;
			if(*ausmem0++ == 255 || !*ausmem0) ch = *(ausmem0-1);
			if(*ausmem1++ == 255 || !*ausmem1) ch = *(ausmem1-1);
			if(*ausmem1++ == 255 || !*ausmem1) ch = *(ausmem1-1);
			*ausr++ = ch; *ausr++ = ch; *ausr++ = ch; *ausr++ = ch;
			ch = *ausmem0++;
			if(*ausmem0++ == 255 || !*ausmem0) ch = *(ausmem0-1);
			if(*ausmem1++ == 255 || !*ausmem1) ch = *(ausmem1-1);
			if(*ausmem1++ == 255 || !*ausmem1) ch = *(ausmem1-1);
			*ausr++ = ch; *ausr++ = ch; *ausr++ = ch; *ausr++ = ch;
			ch = *ausmem0++;
			if(*ausmem0++ == 255 || !*ausmem0) ch = *(ausmem0-1);
			if(*ausmem1++ == 255 || !*ausmem1) ch = *(ausmem1-1);
			if(*ausmem1++ == 255 || !*ausmem1) ch = *(ausmem1-1);
			*ausr++ = ch; *ausr++ = ch; *ausr++ = ch; *ausr++ = ch;
			ch = *ausmem0++;
			if(*ausmem0++ == 255 || !*ausmem0) ch = *(ausmem0-1);
			if(*ausmem1++ == 255 || !*ausmem1) ch = *(ausmem1-1);
			if(*ausmem1++ == 255 || !*ausmem1) ch = *(ausmem1-1);
			*ausr++ = ch; *ausr++ = ch; *ausr++ = ch; *ausr++ = ch;
			ch = *ausmem0++;
			if(*ausmem0++ == 255 || !*ausmem0) ch = *(ausmem0-1);
			if(*ausmem1++ == 255 || !*ausmem1) ch = *(ausmem1-1);
			if(*ausmem1++ == 255 || !*ausmem1) ch = *(ausmem1-1);
			*ausr++ = ch; *ausr++ = ch; *ausr++ = ch; *ausr++ = ch;
			ch = *ausmem0++;
			if(*ausmem0++ == 255 || !*ausmem0) ch = *(ausmem0-1);
			if(*ausmem1++ == 255 || !*ausmem1) ch = *(ausmem1-1);
			if(*ausmem1++ == 255 || !*ausmem1) ch = *(ausmem1-1);
			*ausr++ = ch; *ausr++ = ch; *ausr++ = ch; *ausr++ = ch;
		}
		for(; j>-7; j--) {
			ch = *ausmem0++;
			if(*ausmem0++ == 255 || !*ausmem0) ch = *(ausmem0-1);
			if(*ausmem1++ == 255 || !*ausmem1) ch = *(ausmem1-1);
			if(*ausmem1++ == 255 || !*ausmem1) ch = *(ausmem1-1);
			*ausr++ = ch; *ausr++ = ch; *ausr++ = ch; *ausr++ = ch;
		}
	}
}

/*****************************************************************************/

/* sottocampiona l'intorno (2x2) formato dalla riga yi e yi+1;
   n_pixel si riferisce alla lunghezza della riga da sottocampionare: di
   fatto sono copiati n_pixel/2 pixel; */

void
sea2_sotto_c2_riga_col(
	short xi,
	short yi,
	short n_pixel,
	unsigned char *riga
)
{
	if(!_param_scheda.scheda_aperta)
		return;
	if(_param_scheda.tipo_tv == TV_RGB) {
		register short j;
		unsigned char *mem = _mem_address_col[_param_scheda.fb_lavoro];
		unsigned char *ausmem0 = mem + (long) (xi<<2) + _offset_riga[yi];
		unsigned char *ausr = riga;

		for(j=n_pixel>>1; j>0; j--) {
			*ausr++ = *ausmem0++; *ausr++ = *ausmem0++;
			*ausr++ = *ausmem0++; *ausr++ = *ausmem0++;
			ausmem0+=4;
		}
	}
}

/***************************************************************************/

/* come sea2_sotto_c2_riga, ma ogni pixel e' replicato 4 volte; pertanto
   il vettore riga deve essere 4 volte piu' lungo */

void
sea2_sotto_c2_riga4(
	short xi,
	short yi,
	short n_pixel,
	unsigned char *riga
)
{
	if(!_param_scheda.scheda_aperta)
		return;
	if(_param_scheda.tipo_tv == TV_RGB) {
		register short j;
		unsigned char ch;
		unsigned char *mem = _mem_address[_param_scheda.fb_lavoro];
		unsigned char *ausmem0 = mem + (long) (xi<<2) + _offset_riga[yi];
		unsigned char *ausr = riga;

		for(j=(n_pixel>>1)-7; j>0; j-=8) {
			ch = *ausmem0; ausmem0+=8;
			*ausr++ = ch; *ausr++ = ch; *ausr++ = ch; *ausr++ = ch;
			ch = *ausmem0; ausmem0+=8;
			*ausr++ = ch; *ausr++ = ch; *ausr++ = ch; *ausr++ = ch;
			ch = *ausmem0; ausmem0+=8;
			*ausr++ = ch; *ausr++ = ch; *ausr++ = ch; *ausr++ = ch;
			ch = *ausmem0; ausmem0+=8;
			*ausr++ = ch; *ausr++ = ch; *ausr++ = ch; *ausr++ = ch;
			ch = *ausmem0; ausmem0+=8;
			*ausr++ = ch; *ausr++ = ch; *ausr++ = ch; *ausr++ = ch;
			ch = *ausmem0; ausmem0+=8;
			*ausr++ = ch; *ausr++ = ch; *ausr++ = ch; *ausr++ = ch;
			ch = *ausmem0; ausmem0+=8;
			*ausr++ = ch; *ausr++ = ch; *ausr++ = ch; *ausr++ = ch;
			ch = *ausmem0; ausmem0+=8;
			*ausr++ = ch; *ausr++ = ch; *ausr++ = ch; *ausr++ = ch;
		}
		for(; j>-7; j--) {
			ch = *ausmem0; ausmem0+=8;
			*ausr++ = ch; *ausr++ = ch; *ausr++ = ch; *ausr++ = ch;
		}
	} else {
		register short j;
		unsigned char ch;
		unsigned char *mem = _mem_address[_param_scheda.fb_lavoro];
		unsigned char *ausmem0 = mem + (long) xi + _offset_riga[yi];
		unsigned char *ausr = riga;

		for(j=(n_pixel>>1)-7; j>0; j-=8) {
			ch = *ausmem0++; ausmem0++;
			*ausr++ = ch; *ausr++ = ch; *ausr++ = ch; *ausr++ = ch;
			ch = *ausmem0++; ausmem0++;
			*ausr++ = ch; *ausr++ = ch; *ausr++ = ch; *ausr++ = ch;
			ch = *ausmem0++; ausmem0++;
			*ausr++ = ch; *ausr++ = ch; *ausr++ = ch; *ausr++ = ch;
			ch = *ausmem0++; ausmem0++;
			*ausr++ = ch; *ausr++ = ch; *ausr++ = ch; *ausr++ = ch;
			ch = *ausmem0++; ausmem0++;
			*ausr++ = ch; *ausr++ = ch; *ausr++ = ch; *ausr++ = ch;
			ch = *ausmem0++; ausmem0++;
			*ausr++ = ch; *ausr++ = ch; *ausr++ = ch; *ausr++ = ch;
			ch = *ausmem0++; ausmem0++;
			*ausr++ = ch; *ausr++ = ch; *ausr++ = ch; *ausr++ = ch;
			ch = *ausmem0++; ausmem0++;
			*ausr++ = ch; *ausr++ = ch; *ausr++ = ch; *ausr++ = ch;
		}
		for(; j>-7; j--) {
			ch = *ausmem0++; ausmem0++;
			*ausr++ = ch; *ausr++ = ch; *ausr++ = ch; *ausr++ = ch;
		}
	}
}


/***********************************************************************

;  Nome:    
	sea2_leggi_pixel
;
;  Funzione:
	legge un pixel dal frame buffer selezionato
;
;  Formato:
	short sea2_leggi_pixel(x_pixel, y_pixel)
;
;  Argomenti:    
	short x_pixel           ascissa del pixel
	short y_pixel           ordinata del pixel

;
;  Valore di ritorno: 
	n        livello di grigio del pixel

;
;  Descrizione:
     {\bf sea2_leggi_pixel}() legge un pixel dal
	 frame buffer selezionato con la routine {\bf sea2_seleziona_buffer}();
;
;  Note:
     le coordinate del pixel possono eccedere le dimensioni del
	 frame buffer in quanto l'accesso alla memoria e' realizzato
	 modulo le dimensioni della medesima;
;
;**********************************************************************/

short
sea2_leggi_pixel(
	short x_pixel,
	short y_pixel
)
{
	if(!_param_scheda.scheda_aperta)
		return SEAV_ERR_GRABBER_INIT;
	if(_param_scheda.tipo_tv == TV_RGB) {
		unsigned char *mem = _mem_address[_param_scheda.fb_lavoro];
		unsigned char *ausmem = mem+(long)(x_pixel<<2) + _offset_riga[y_pixel];
		return (short) *ausmem;
	} else {
		unsigned char *mem = _mem_address[_param_scheda.fb_lavoro];
		unsigned char *ausmem = mem + (long) x_pixel + _offset_riga[y_pixel];
		return (short) *ausmem;
	}
}


/***********************************************************************

;  Nome:    
	sea2_leggi_colonna
;
;  Funzione:
	legge un segmento verticale dal frame buffer selezionato
;
;  Formato:
	void sea2_leggi_colonna(xi, yi, n_pixel, colonna)
;
;  Argomenti:    
	short xi           ascissa iniziale
	short yi           ordinata iniziale
	short n_pixel      numero di pixel da leggere
	unsigned char *colonna  buffer nel quale sono trasferiti i pixel
	                        letti dalla scheda

;
;  Descrizione:
     {\bf sea2_leggi_colonna}() legge un segmento verticale dal
	 frame buffer selezionato con la routine {\bf sea2_seleziona_buffer}();
;
;**********************************************************************/

void
sea2_leggi_colonna(
	short xi,
	short yi,
	short n_pixel,
	unsigned char *colonna
)
{
	if(!_param_scheda.scheda_aperta)
		return;
	if(_param_scheda.tipo_tv == TV_RGB) {
		long lung_riga = (long) _param_scheda.lung_riga * 4L;
		unsigned char *mem = _mem_address[_param_scheda.fb_lavoro];
		unsigned char *ausmem = mem + (long) xi * 4L + _offset_riga[yi];
		short i;

		for(i=n_pixel-7; i>0; i-=8) {
			*colonna++ = *ausmem; ausmem += lung_riga;
			*colonna++ = *ausmem; ausmem += lung_riga;
			*colonna++ = *ausmem; ausmem += lung_riga;
			*colonna++ = *ausmem; ausmem += lung_riga;
			*colonna++ = *ausmem; ausmem += lung_riga;
			*colonna++ = *ausmem; ausmem += lung_riga;
			*colonna++ = *ausmem; ausmem += lung_riga;
			*colonna++ = *ausmem; ausmem += lung_riga;
		}
		for(; i>-7; i--) {
			*colonna++ = *ausmem; ausmem += lung_riga;
		}
	} else {
		long lung_riga = (long) _param_scheda.lung_riga;
		unsigned char *mem = _mem_address[_param_scheda.fb_lavoro];
		unsigned char *ausmem = mem + (long) xi + _offset_riga[yi];
		short i;

		for(i=n_pixel-7; i>0; i-=8) {
			*colonna++ = *ausmem; ausmem += lung_riga;
			*colonna++ = *ausmem; ausmem += lung_riga;
			*colonna++ = *ausmem; ausmem += lung_riga;
			*colonna++ = *ausmem; ausmem += lung_riga;
			*colonna++ = *ausmem; ausmem += lung_riga;
			*colonna++ = *ausmem; ausmem += lung_riga;
			*colonna++ = *ausmem; ausmem += lung_riga;
			*colonna++ = *ausmem; ausmem += lung_riga;
		}
		for(; i>-7; i--) {
			*colonna++ = *ausmem; ausmem += lung_riga;
		}
	}

	return;
}

/*****************************************************************************/

/* legge una colonna da un frame buffer a colori (32 bit per pixel) */

void
sea2_leggi_colonna_col(
	short xi,
	short yi,
	short n_pixel,
	unsigned char *colonna
)
{
	if(!_param_scheda.scheda_aperta)
		return;
	if(_param_scheda.tipo_tv == TV_RGB) {
		long lung_riga = (long) _param_scheda.lung_riga * 4L - 4L;
		unsigned char *mem = _mem_address_col[_param_scheda.fb_lavoro];
		unsigned char *ausmem = mem + (long) (xi<<2) + _offset_riga[yi];
		short i;

		for(i=n_pixel-7; i>0; i-=8) {
			*colonna++ = *ausmem++; *colonna++ = *ausmem++;
			*colonna++ = *ausmem++; *colonna++ = *ausmem++;
			ausmem += lung_riga;
			*colonna++ = *ausmem++; *colonna++ = *ausmem++;
			*colonna++ = *ausmem++; *colonna++ = *ausmem++;
			ausmem += lung_riga;
			*colonna++ = *ausmem++; *colonna++ = *ausmem++;
			*colonna++ = *ausmem++; *colonna++ = *ausmem++;
			ausmem += lung_riga;
			*colonna++ = *ausmem++; *colonna++ = *ausmem++;
			*colonna++ = *ausmem++; *colonna++ = *ausmem++;
			ausmem += lung_riga;
			*colonna++ = *ausmem++; *colonna++ = *ausmem++;
			*colonna++ = *ausmem++; *colonna++ = *ausmem++;
			ausmem += lung_riga;
			*colonna++ = *ausmem++; *colonna++ = *ausmem++;
			*colonna++ = *ausmem++; *colonna++ = *ausmem++;
			ausmem += lung_riga;
			*colonna++ = *ausmem++; *colonna++ = *ausmem++;
			*colonna++ = *ausmem++; *colonna++ = *ausmem++;
			ausmem += lung_riga;
			*colonna++ = *ausmem++; *colonna++ = *ausmem++;
			*colonna++ = *ausmem++; *colonna++ = *ausmem++;
			ausmem += lung_riga;
		}
		for(; i>-7; i--) {
			*colonna++ = *ausmem++; *colonna++ = *ausmem++;
			*colonna++ = *ausmem++; *colonna++ = *ausmem++;
			ausmem += lung_riga;
		}
	}
}


/***********************************************************************

;  Nome:    
	sea2_leggi_finestra
;
;  Funzione:
	legge una porzione rettangolare di immagine dal frame buffer selezionato
;
;  Formato:
	void sea2_leggi_finestra(xi, yi, xf, yf, buffer)
;
;  Argomenti:    
	short xi           ascissa iniziale della finestra
	short yi           ordinata iniziale della finestra
	short xf           ascissa finale della finestra
	short yf           ordinata finale della finestra
	unsigned char *buffer  buffer nel quale sono trasferiti i pixel
	                        letti dalla scheda

;
;  Descrizione:
     {\bf sea2_leggi_finestra}() legge una porzione rettangolare di
	 immagine dal
	 frame buffer selezionato con la routine {\bf sea2_seleziona_buffer}();
;
;**********************************************************************/

void
sea2_leggi_finestra(
	short xi,
	short yi,
	short xf,
	short yf,
	unsigned char *buffer
)
{
	if(!_param_scheda.scheda_aperta)
		return;
	if(_param_scheda.tipo_tv == TV_RGB) {
		short i, xl = xf - xi + 1;

		for(i=yi; i<=yf; i++) {
			sea2_leggi_riga(xi, i, xl, buffer);
			buffer += (long) xl;
		}
	} else {
		long lung_riga = (long) _param_scheda.lung_riga;
		unsigned char *mem = _mem_address[_param_scheda.fb_lavoro];
		unsigned char *ausmem = mem + (long) xi + _offset_riga[yi];
		short i, xl = xf - xi + 1;

		for(i=yf-yi+1; i>0; i--) {
			memcpy(buffer, ausmem, (size_t) xl);
			buffer += (long) xl;
			ausmem += lung_riga;
		}
	}
	return;
}

/***************************************************************************/

void
sea2_leggi_finestra_col(
	short xi,
	short yi,
	short xf,
	short yf,
	unsigned char *buffer
)
{
	if(!_param_scheda.scheda_aperta)
		return;
	if(_param_scheda.tipo_tv == TV_RGB) {
		short i, xl = xf - xi + 1;
		short x_size = xl * 4;

		for(i=yi; i<=yf; i++) {
			sea2_leggi_riga_col(xi, i, xl, buffer);
			buffer += (long) x_size;
		}
	}
	return;
}

/*****************************************************************************/

/* i pixel sono impaccati in 24 bit */

void
sea2_leggi_finestra_col24(
	short xi,
	short yi,
	short xf,
	short yf,
	unsigned char *buffer
)
{
	if(_param_scheda.tipo_tv == TV_RGB) {
		short i, xl = xf - xi + 1;
		long xl3 = xl*3;

		for(i=yi; i<=yf; i++) {
			sea2_leggi_riga_col24(xi, i, xl, buffer);
			buffer += xl3;
		}
	}
	return;
}
