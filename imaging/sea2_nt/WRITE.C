
/*
	sea2_scrivi_colonna
	sea2_scrivi_riga
	sea2_scrivi_riga_col
	sea2_scrivi_finestra
	sea2_scrivi_riga_col24
*/

#include <string.h>
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

/*****************************************************************************/

/* variabili globali definite in init.c */

#if defined(MET) || defined(ASS) || defined(MET_MIL) || defined(ORION) || defined(METEOR2)
	extern unsigned char *_mem_address[N_FB_MAX];
	extern long *_offset_riga;
	/* contiene, per ogni riga, l'offset del primo pixel */
	// per il colore RGB
	extern unsigned char *_mem_address_col[N_FB_MAX_COL];
	extern long _primo_byte_libero_col[N_FB_MAX_COL];
	extern long _max_n_byte_grafica_col;
	// fine colore RGB
#endif
	extern PARAM_SCHEDA _param_scheda;


/***********************************************************************

;  Nome:    
	sea2_scrivi_colonna
;
;  Funzione:
	scrive un segmento verticale sul frame buffer selezionato
;
;  Formato:
	void sea2_scrivi_colonna(xi, yi, n_pixel, colonna)
;
;  Argomenti:    
	short xi           ascissa iniziale
	short yi           ordinata iniziale
	short n_pixel      numero di pixel da scrivere
	unsigned char *colonna  buffer nel quale sono contenuti i pixel
	                        da trasferire sulla scheda

;
;  Descrizione:
     {\bf sea2_scrivi_colonna}() scrive un segmento verticale sul
	 frame buffer selezionato con la routine {\bf sea2_seleziona_buffer}();
;
;**********************************************************************/

void
sea2_scrivi_colonna(
	short xi,
	short yi,
	short n_pixel,
	unsigned char *colonna
)
{
	short yf;
	if(!_param_scheda.scheda_aperta)
		return;
	if(xi < _param_scheda.x_min || xi > _param_scheda.x_max) // 8.28-2
		return;
	if(yi < _param_scheda.y_min) { // 8.28-2
		colonna += (_param_scheda.y_min - yi);
		n_pixel -= (_param_scheda.y_min - yi);
		yi = _param_scheda.y_min;
	}
	yf = yi + n_pixel - 1;
	if(yf > _param_scheda.y_max) { // 8.28-2
		n_pixel -= (yf - _param_scheda.y_max);
	}

	if(_param_scheda.tipo_tv == TV_RGB) {
		long lung_riga = (long) _param_scheda.lung_riga * 4L;
		unsigned char *mem = _mem_address[_param_scheda.fb_lavoro];
		unsigned char *ausmem = mem + (long) (xi<<2) + _offset_riga[yi];
		short i;

		for(i=n_pixel-7; i>0; i-=8) {
			*ausmem = *colonna++; ausmem += lung_riga;
			*ausmem = *colonna++; ausmem += lung_riga;
			*ausmem = *colonna++; ausmem += lung_riga;
			*ausmem = *colonna++; ausmem += lung_riga;
			*ausmem = *colonna++; ausmem += lung_riga;
			*ausmem = *colonna++; ausmem += lung_riga;
			*ausmem = *colonna++; ausmem += lung_riga;
			*ausmem = *colonna++; ausmem += lung_riga;
		}
		for(; i>-7; i--) {
			*ausmem = *colonna++; ausmem += lung_riga;
		}
	} else {
		long lung_riga = (long) _param_scheda.lung_riga;
		unsigned char *mem = _mem_address[_param_scheda.fb_lavoro];
		unsigned char *ausmem = mem + (long) xi + _offset_riga[yi];
		short i;

		for(i=n_pixel-7; i>0; i-=8) {
			*ausmem = *colonna++; ausmem += lung_riga;
			*ausmem = *colonna++; ausmem += lung_riga;
			*ausmem = *colonna++; ausmem += lung_riga;
			*ausmem = *colonna++; ausmem += lung_riga;
			*ausmem = *colonna++; ausmem += lung_riga;
			*ausmem = *colonna++; ausmem += lung_riga;
			*ausmem = *colonna++; ausmem += lung_riga;
			*ausmem = *colonna++; ausmem += lung_riga;
		}
		for(; i>-7; i--) {
			*ausmem = *colonna++; ausmem += lung_riga;
		}
	}

	return;
}


/***********************************************************************

;  Nome:    
	sea2_scrivi_riga
;
;  Funzione:
	scrive un segmento orizzontale sul frame buffer selezionato
;
;  Formato:
	void sea2_scrivi_riga(xi, yi, n_pixel, riga)
;
;  Argomenti:    
	short xi           ascissa iniziale
	short yi           ordinata iniziale
	short n_pixel      numero di pixel da scrivere
	unsigned char *riga  buffer nel quale sono contenuti i pixel
	                        da trasferire sulla scheda

;
;  Descrizione:
     {\bf sea2_scrivi_riga}() scrive un segmento orizzontale sul
	 frame buffer selezionato con la routine {\bf sea2_seleziona_buffer}();
;
;**********************************************************************/

void
sea2_scrivi_riga(
	short xi,
	short yi,
	short n_pixel,
	unsigned char *riga
)
{
	short xf;
	if(!_param_scheda.scheda_aperta)
		return;
	if(yi < _param_scheda.y_min || yi > _param_scheda.y_max) // 8.28-2
		return;
	if(xi < _param_scheda.x_min) { // 8.28-2
		riga += (_param_scheda.x_min - xi);
		n_pixel -= (_param_scheda.x_min - xi);
		xi = _param_scheda.x_min;
	}
	xf = xi + n_pixel - 1;
	if(xf > _param_scheda.x_max) { // 8.28-2
		n_pixel -= (xf - _param_scheda.x_max);
	}
	
	if(_param_scheda.tipo_tv == TV_RGB) {
		short i;
		unsigned char *mem = _mem_address[_param_scheda.fb_lavoro];
		unsigned char *ausm = mem + (long) (xi<<2) + _offset_riga[yi];
		unsigned char *ausr = riga;

		for(i=n_pixel-7; i>0; i-=8) {
			*ausm = *ausr++; ausm += 4;
			*ausm = *ausr++; ausm += 4;
			*ausm = *ausr++; ausm += 4;
			*ausm = *ausr++; ausm += 4;
			*ausm = *ausr++; ausm += 4;
			*ausm = *ausr++; ausm += 4;
			*ausm = *ausr++; ausm += 4;
			*ausm = *ausr++; ausm += 4;
		}
		for(; i>-7; i--) {
			*ausm = *ausr++; ausm += 4;
		}
	} else {
		unsigned char *mem = _mem_address[_param_scheda.fb_lavoro];
		unsigned char *ausmem = mem + (long) xi + _offset_riga[yi];

		memcpy(ausmem, riga, (size_t) n_pixel);
	}

	return;
}

/*****************************************************************************/

/* scrive una riga in un frame buffer a colori (32 bit per pixel) */

void
sea2_scrivi_riga_col(
	short xi,
	short yi,
	short n_pixel,
	unsigned char *riga
)
{
	short xf;
	if(!_param_scheda.scheda_aperta)
		return;
	if(yi < _param_scheda.y_min || yi > _param_scheda.y_max) // 8.28-2
		return;
	if(xi < _param_scheda.x_min) { // 8.28-2
		riga += (_param_scheda.x_min - xi);
		n_pixel -= (_param_scheda.x_min - xi);
		xi = _param_scheda.x_min;
	}
	xf = xi + n_pixel - 1;
	if(xf > _param_scheda.x_max) { // 8.28-2
		n_pixel -= (xf - _param_scheda.x_max);
	}
	if(_param_scheda.tipo_tv == TV_RGB) {
		unsigned char *mem = _mem_address_col[_param_scheda.fb_lavoro];
		unsigned char *ausmem = mem + (long) (xi<<2) + _offset_riga[yi];

		memcpy(ausmem, riga, (size_t) (n_pixel<<2));
	}
}


/***********************************************************************

;  Nome:    
	sea2_scrivi_finestra
;
;  Funzione:
	scrive una porzione rettangolare di immagine sul frame buffer selezionato
;
;  Formato:
	void sea2_scrivi_finestra(xi, yi, xf, yf, buffer)
;
;  Argomenti:    
	short xi           ascissa iniziale della finestra
	short yi           ordinata iniziale della finestra
	short xf           ascissa finale della finestra
	short yf           ordinata finale della finestra
	unsigned char *buffer  buffer nel quale sono contenuti i pixel
	                       da trasferire sulla scheda

;
;  Descrizione:
     {\bf sea2_scrivi_finestra}() scrive una porzione rettangolare di
	 immagine sul
	 frame buffer selezionato con la routine {\bf sea2_seleziona_buffer}();
;
;**********************************************************************/

void
sea2_scrivi_finestra(
	short xi,
	short yi,
	short xf,
	short yf,
	unsigned char *buffer
)
{
	if(!_param_scheda.scheda_aperta)
		return;
	if(_param_scheda.tipo_tv == TV_RGB ||
		xi < _param_scheda.x_min ||
		xf > _param_scheda.x_max ||
		yi < _param_scheda.y_min ||
		yf > _param_scheda.y_max) { // 8.28-2
		short i, xl = xf - xi + 1;

		for(i=yi; i<=yf; i++) {
			sea2_scrivi_riga(xi, i, xl, buffer);
			buffer += (long) xl;
		}
	} else {
		long lung_riga = _param_scheda.lung_riga;
		unsigned char *mem = _mem_address[_param_scheda.fb_lavoro];
		unsigned char *ausmem = mem + (long) xi + _offset_riga[yi];
		short i, xl = xf - xi + 1;

		for(i=yf-yi+1; i>0; i--) {
			memcpy(ausmem, buffer, (size_t) xl);
			buffer += (long) xl;
			ausmem += lung_riga;
		}
	}

	return;
}

/*****************************************************************************/

/* scrive una riga in un frame buffer a colori (32 bit per pixel): il buffer
   riga ha 24 bit per pixel; pertanto il vettore riga deve essere lungo almeno
   3*n_pixel */

void
sea2_scrivi_riga_col24(
	short xi,
	short yi,
	short n_pixel,
	unsigned char *riga
)
{
	short xf;
	if(!_param_scheda.scheda_aperta)
		return;
	if(yi < _param_scheda.y_min || yi > _param_scheda.y_max) // 8.28-2
		return;
	if(xi < _param_scheda.x_min) { // 8.28-2
		riga += (_param_scheda.x_min - xi);
		n_pixel -= (_param_scheda.x_min - xi);
		xi = _param_scheda.x_min;
	}
	xf = xi + n_pixel - 1;
	if(xf > _param_scheda.x_max) { // 8.28-2
		n_pixel -= (xf - _param_scheda.x_max);
	}
	if(_param_scheda.tipo_tv == TV_RGB) {
		short i;
		unsigned char *mem = _mem_address_col[_param_scheda.fb_lavoro];
		unsigned char *ausm = mem + (long) (xi<<2) + _offset_riga[yi];

		for(i=n_pixel-7; i>0; i-=8) {
			*ausm++ = *riga++; *ausm++ = *riga++; *ausm++ = *riga++; ausm++;
			*ausm++ = *riga++; *ausm++ = *riga++; *ausm++ = *riga++; ausm++;
			*ausm++ = *riga++; *ausm++ = *riga++; *ausm++ = *riga++; ausm++;
			*ausm++ = *riga++; *ausm++ = *riga++; *ausm++ = *riga++; ausm++;
			*ausm++ = *riga++; *ausm++ = *riga++; *ausm++ = *riga++; ausm++;
			*ausm++ = *riga++; *ausm++ = *riga++; *ausm++ = *riga++; ausm++;
			*ausm++ = *riga++; *ausm++ = *riga++; *ausm++ = *riga++; ausm++;
			*ausm++ = *riga++; *ausm++ = *riga++; *ausm++ = *riga++; ausm++;
		}
		for(; i>-7; i--) {
			*ausm++ = *riga++; *ausm++ = *riga++; *ausm++ = *riga++; ausm++;
		}
	}
	return;
}

