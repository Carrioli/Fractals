
/*
	sea2_disegna_linea
	sea2_disegna_linea_col
	sea2_disegna_linea_overlay
	static internal_line_draw_mem
	static internal_line_draw_mem_col

	sea2_disegna_poligono_col
	sea2_disegna_poligono_overlay

	sea2_disegna_finestra
	sea2_disegna_finestra_col
	sea2_disegna_finestra_overlay

	sea2_scrivi_pixel
	sea2_scrivi_pixel_col
	sea2_scrivi_pixel_overlay

	sea2_disegna_quadratino_col
	sea2_disegna_quadratino_overlay

	sea2_disegna_croce_col
	sea2_disegna_croce_overlay
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
	extern long *_offset_riga;
		/* contiene, per ogni riga, l'offset del primo pixel */
	extern unsigned char *_mem_address[N_FB_MAX];
	extern unsigned char *_mem_address_col[N_FB_MAX_COL];
	extern unsigned char *_mem_draw[N_FB_MAX];
	extern long _primo_byte_libero[N_FB_MAX];
	extern long _max_n_byte_grafica;
#endif
	extern PARAM_SCHEDA _param_scheda;

/*****************************************************************************/

/* prototipi delle funzioni private */

#if defined(MET) || defined(ASS) || defined(MET_MIL) || defined(ORION) || defined(METEOR2)
	static void internal_line_draw_mem(
		short xi, short yi, short xf, short yf, short colore,
		unsigned char *mem
	);
	// per il colore RGB
	static void internal_line_draw_mem_col(
		short xi, short yi, short xf, short yf, short rosso,
		short verde, short blu, unsigned char *mem
	);
	// fine colore RGB
#endif

/***********************************************************************

;  Nome:    
	sea2_disegna_linea
;
;  Funzione:
	disegna un segmento
;
;  Formato:
	void sea2_disegna_linea(fb, xi, yi, xf, yf, colore)
;
;  Argomenti:    
	short fb           numero del frame buffer
	short xi           ascissa iniziale del segmento
	short yi           ordinata iniziale del segmento
	short xf           ascissa finale del segmento
	short yf           ordinata finale del segmento
	short colore       colore del disegno

;
;  Descrizione:
     {\bf sea2_disegna_linea}() disegna un segmento sulla scheda
	 grafica
;
;**********************************************************************/

void
sea2_disegna_linea(
	short fb,
	short xi,
	short yi,
	short xf,
	short yf,
	short colore
)
{
	unsigned char *mem;
	if(!_param_scheda.scheda_aperta)
		return;
	mem = _mem_address[fb];
	internal_line_draw_mem(xi, yi, xf, yf, colore, mem);
	return;
}

/**********************************************************************/

static void
internal_line_draw_mem(
	short xi,
	short yi,
	short xf,
	short yf,
	short colore,
	unsigned char *mem
)
{
	if(_param_scheda.tipo_tv == TV_RGB) {
		if(xi == xf) {
			short i, yl;
			unsigned char *_ausmem;
			long lung_riga = (long) (_param_scheda.lung_riga << 2);

			if(yi < yf) {
				_ausmem = mem + (long) (xi<<2) + _offset_riga[yi];
				yl = yf - yi + 1;
				for(i=yl; i>0; i--) {
					*_ausmem = (unsigned char) colore;
					_ausmem += lung_riga;
				}
			} else {
				_ausmem = mem + (long) (xi<<2) + _offset_riga[yf];
				yl = yi - yf + 1;
				for(i=yl; i>0; i--) {
					*_ausmem = (unsigned char) colore;
					_ausmem += lung_riga;
				}
			}
			return;
		} else if(yi == yf) {
			short i, xl;
			unsigned char *ausmem;

			if(xi < xf) {
				ausmem = mem + (long) (xi<<2) + _offset_riga[yi];
				xl = xf - xi + 1;
				for(i=xl; i>0; i--) {
					*ausmem = (unsigned char) colore;
					ausmem += 4;
				}
			} else {
				ausmem = mem + (long) (xf<<2) + _offset_riga[yi];
				xl = xi - xf + 1;
				for(i=xl; i>0; i--) {
					*ausmem = (unsigned char) colore;
					ausmem += 4;
				}
			}
			return;
		} else {
			short x1 = xi + xf, x2;
			short y1 = yi + yf, y2;
			if(x1 & 1) {
				x1 >>= 1;
				x2 = x1 + 1;
			} else {
				x1 >>= 1;
				x2 = x1;
			}
			if(y1 & 1) {
				y1 >>= 1;
				y2 = y1 + 1;
			} else {
				y1 >>= 1;
				y2 = y1;
			}
			if(xi < xf) {
				if(yi < yf) {
					internal_line_draw_mem(xi, yi, x1, y1, colore, mem);
					internal_line_draw_mem(x2, y2, xf, yf, colore, mem);
				} else {
					internal_line_draw_mem(xi, yi, x1, y2, colore, mem);
					internal_line_draw_mem(x2, y1, xf, yf, colore, mem);
				}
			} else {
				if(yi < yf) {
					internal_line_draw_mem(xi, yi, x2, y1, colore, mem);
					internal_line_draw_mem(x1, y2, xf, yf, colore, mem);
				} else {
					internal_line_draw_mem(xi, yi, x2, y2, colore, mem);
					internal_line_draw_mem(x1, y1, xf, yf, colore, mem);
				}
			}
		}
	} else {
		if(xi == xf) {
			short i, yl;
			unsigned char *_ausmem;
			long lung_riga = (long) _param_scheda.lung_riga;

			if(yi < yf) {
				_ausmem = mem + (long) xi + _offset_riga[yi];
				yl = yf - yi + 1;
				for(i=yl; i>0; i--) {
					*_ausmem = (unsigned char) colore;
					_ausmem += lung_riga;
				}
			} else {
				_ausmem = mem + (long) xi + _offset_riga[yf];
				yl = yi - yf + 1;
				for(i=yl; i>0; i--) {
					*_ausmem = (unsigned char) colore;
					_ausmem += lung_riga;
				}
			}
			return;
		} else if(yi == yf) {
			if(xi < xf)
				memset(
					mem + (long) xi + _offset_riga[yi],
					(int) colore, (size_t) (xf - xi + 1)
				);
			else
				memset(
					mem + (long) xf + _offset_riga[yi],
					(int) colore, (size_t) (xi - xf + 1)
				);
			return;
		} else {
			short x1 = xi + xf, x2;
			short y1 = yi + yf, y2;
			if(x1 & 1) {
				x1 >>= 1;
				x2 = x1 + 1;
			} else {
				x1 >>= 1;
				x2 = x1;
			}
			if(y1 & 1) {
				y1 >>= 1;
				y2 = y1 + 1;
			} else {
				y1 >>= 1;
				y2 = y1;
			}
			if(xi < xf) {
				if(yi < yf) {
					internal_line_draw_mem(xi, yi, x1, y1, colore, mem);
					internal_line_draw_mem(x2, y2, xf, yf, colore, mem);
				} else {
					internal_line_draw_mem(xi, yi, x1, y2, colore, mem);
					internal_line_draw_mem(x2, y1, xf, yf, colore, mem);
				}
			} else {
				if(yi < yf) {
					internal_line_draw_mem(xi, yi, x2, y1, colore, mem);
					internal_line_draw_mem(x1, y2, xf, yf, colore, mem);
				} else {
					internal_line_draw_mem(xi, yi, x2, y2, colore, mem);
					internal_line_draw_mem(x1, y1, xf, yf, colore, mem);
				}
			}
		}
	}
	return;
}

/*****************************************************************************/

void
sea2_disegna_linea_col(
	short fb,
	short xi,
	short yi,
	short xf,
	short yf,
	short rosso,
	short verde,
	short blu
)
{
	if(!_param_scheda.scheda_aperta)
		return;
	if(_param_scheda.tipo_tv == TV_RGB) {
		unsigned char *mem = _mem_address_col[fb];
		internal_line_draw_mem_col(xi, yi, xf, yf, rosso, verde, blu, mem);
	}
}

/*****************************************************************************/

static void
internal_line_draw_mem_col(
	short xi,
	short yi,
	short xf,
	short yf,
	short rosso,
	short verde,
	short blu,
	unsigned char *mem
)
{
	if(xi == xf) {
		short i, yl;
		unsigned char *_ausmem;
		long lung_riga = (long) (_param_scheda.lung_riga << 2);

		if(yi < yf) {
			_ausmem = mem + (long) (xi<<2) + _offset_riga[yi];
			yl = yf - yi + 1;
			for(i=yl; i>0; i--) {
				*_ausmem = (unsigned char) blu;
				*(_ausmem+1) = (unsigned char) verde;
				*(_ausmem+2) = (unsigned char) rosso;
				_ausmem += lung_riga;
			}
		} else {
			_ausmem = mem + (long) (xi<<2) + _offset_riga[yf];
			yl = yi - yf + 1;
			for(i=yl; i>0; i--) {
				*_ausmem = (unsigned char) blu;
				*(_ausmem+1) = (unsigned char) verde;
				*(_ausmem+2) = (unsigned char) rosso;
				_ausmem += lung_riga;
			}
		}
		return;
	} else if(yi == yf) {
		short i, xl;
		unsigned char *ausmem;

		if(xi < xf) {
			ausmem = mem + (long) (xi<<2) + _offset_riga[yi];
			xl = xf - xi + 1;
			for(i=xl; i>0; i--) {
				*ausmem++ = (unsigned char) blu;
				*ausmem++ = (unsigned char) verde;
				*ausmem++ = (unsigned char) rosso;
				ausmem ++;
			}
		} else {
			ausmem = mem + (long) (xf<<2) + _offset_riga[yi];
			xl = xi - xf + 1;
			for(i=xl; i>0; i--) {
				*ausmem++ = (unsigned char) blu;
				*ausmem++ = (unsigned char) verde;
				*ausmem++ = (unsigned char) rosso;
				ausmem ++;
			}
		}
		return;
	} else {
		short x1 = xi + xf, x2;
		short y1 = yi + yf, y2;
		if(x1 & 1) {
			x1 >>= 1;
			x2 = x1 + 1;
		} else {
			x1 >>= 1;
			x2 = x1;
		}
		if(y1 & 1) {
			y1 >>= 1;
			y2 = y1 + 1;
		} else {
			y1 >>= 1;
			y2 = y1;
		}
		if(xi < xf) {
			if(yi < yf) {
				internal_line_draw_mem_col(xi, yi, x1, y1, rosso,verde,blu,mem);
				internal_line_draw_mem_col(x2, y2, xf, yf, rosso,verde,blu,mem);
			} else {
				internal_line_draw_mem_col(xi, yi, x1, y2, rosso,verde,blu,mem);
				internal_line_draw_mem_col(x2, y1, xf, yf, rosso,verde,blu,mem);
			}
		} else {
			if(yi < yf) {
				internal_line_draw_mem_col(xi, yi, x2, y1, rosso,verde,blu,mem);
				internal_line_draw_mem_col(x1, y2, xf, yf, rosso,verde,blu,mem);
			} else {
				internal_line_draw_mem_col(xi, yi, x2, y2, rosso,verde,blu,mem);
				internal_line_draw_mem_col(x1, y1, xf, yf, rosso,verde,blu,mem);
			}
		}
	}

	return;
}

/*****************************************************************************/

void
sea2_disegna_poligono_col(
	short fb,
	PUNTO *punto,
	short n_punti,
	short off_x, /* da sommare alle coordinate */
	short off_y,
	short rosso,
	short verde,
	short blu
)
{
	if(!_param_scheda.scheda_aperta)
		return;
	if(_param_scheda.tipo_tv == TV_RGB) {
		short i;
		for(i=1; i<n_punti; i++)
			sea2_disegna_linea_col(
				fb, (short) (off_x+punto[i-1].x), (short) (off_y+punto[i-1].y),
				(short) (off_x+punto[i].x), (short) (off_y+punto[i].y),
				rosso, verde, blu
			);
		if(n_punti > 0)
			sea2_disegna_linea_col(
				fb, (short) (off_x+punto[n_punti-1].x),
				(short) (off_y+punto[n_punti-1].y),
				(short) (off_x+punto[0].x), (short) (off_y+punto[0].y),
				rosso, verde, blu
			);
	}
}

/*****************************************************************************/

void
sea2_disegna_linea_overlay(
	short fb,
	short xi,
	short yi,
	short xf,
	short yf,
	short rosso,
	short verde,
	short blu
)
{
	long primo_libero;

	if(!_param_scheda.scheda_aperta)
		return;
	primo_libero = _primo_byte_libero[fb];
	if(primo_libero + L_PRIM_LINEA < _max_n_byte_grafica) {        
		unsigned char *mem = _mem_draw[fb];
		long primo_libero_fisico = primo_libero;
		mem[primo_libero_fisico] = PRIM_LINEA;
		mem[primo_libero_fisico+1] = (unsigned char) xi;
		mem[primo_libero_fisico+2] = (unsigned char) (xi >> 8);
		mem[primo_libero_fisico+3] = (unsigned char) yi;
		mem[primo_libero_fisico+4] = (unsigned char) (yi >> 8);
		mem[primo_libero_fisico+5] = (unsigned char) xf;
		mem[primo_libero_fisico+6] = (unsigned char) (xf >> 8);
		mem[primo_libero_fisico+7] = (unsigned char) yf;
		mem[primo_libero_fisico+8] = (unsigned char) (yf >> 8);
		mem[primo_libero_fisico+9] = (unsigned char) rosso;
		mem[primo_libero_fisico+10] = (unsigned char) (rosso >> 8);
		mem[primo_libero_fisico+11] = (unsigned char) verde;
		mem[primo_libero_fisico+12] = (unsigned char) (verde >> 8);
		mem[primo_libero_fisico+13] = (unsigned char) blu;
		mem[primo_libero_fisico+14] = (unsigned char) (blu >> 8);
		_primo_byte_libero[fb] =
			primo_libero + L_PRIM_LINEA;
	}
}

/*****************************************************************************/

void
sea2_disegna_poligono_overlay(
	short fb,
	PUNTO *punto,
	short n_punti,
	short off_x, /* da sommare alle coordinate */
	short off_y,
	short rosso,
	short verde,
	short blu
)
{
	long primo_libero;

	if(!_param_scheda.scheda_aperta)
		return;
	primo_libero = _primo_byte_libero[fb];
	if(primo_libero + L_PRIM_POLIGONO(n_punti) < _max_n_byte_grafica) {        
		unsigned char *mem = _mem_draw[fb];
		long primo_libero_fisico = primo_libero;
		short i, aus;
		long j;
		mem[primo_libero_fisico] = PRIM_POLIGONO;
		mem[primo_libero_fisico+1] = (unsigned char) n_punti;
		mem[primo_libero_fisico+2] = (unsigned char) (n_punti >> 8);
		mem[primo_libero_fisico+3] = (unsigned char) rosso;
		mem[primo_libero_fisico+4] = (unsigned char) (rosso >> 8);
		mem[primo_libero_fisico+5] = (unsigned char) verde;
		mem[primo_libero_fisico+6] = (unsigned char) (verde >> 8);
		mem[primo_libero_fisico+7] = (unsigned char) blu;
		mem[primo_libero_fisico+8] = (unsigned char) (blu >> 8);
		for(i=0, j=primo_libero_fisico+9L; i<n_punti; i++, j+=4L) {
			aus = punto[i].x + off_x;
			mem[j] = (unsigned char) aus;
			mem[j+1] = (unsigned char) (aus >> 8);
			aus = punto[i].y + off_y;
			mem[j+2] = (unsigned char) aus;
			mem[j+3] = (unsigned char) (aus >> 8);
		}
		_primo_byte_libero[fb] =
			primo_libero + L_PRIM_POLIGONO(n_punti);
	}
}

/***********************************************************************

;  Nome:    
	sea2_disegna_finestra
;
;  Funzione:
	disegna un rettangolo
;
;  Formato:
	void sea2_disegna_finestra(fb, xi, yi, xf, yf, colore)
;
;  Argomenti:    
	short fb           numero del frame buffer
	short xi           ascissa iniziale della finestra
	short yi           ordinata iniziale della finestra
	short xf           ascissa finale della finestra
	short yf           ordinata finale della finestra
	short colore       colore del disegno

;
;  Descrizione:
     {\bf sea2_disegna_finestra}() disegna un rettangolo sulla scheda
	 grafica
;
;**********************************************************************/

void
sea2_disegna_finestra(
	short fb,
	short xi,
	short yi,
	short xf,
	short yf,
	short colore
)
{
	if(!_param_scheda.scheda_aperta)
		return;
	if(_param_scheda.tipo_tv == TV_RGB) {
		short xl = xf - xi + 1;
		short yl = yf - yi + 1;
		unsigned char *mem = _mem_address[fb];
		unsigned char *ausmem0 = mem + (long) (xi<<2) + _offset_riga[yi];
		unsigned char *ausmem1 = ausmem0 + _offset_riga[yl-1];
		long offset = (long) (_param_scheda.lung_riga << 2);
		short j;

		for(j=xl; j>0; j--) {
			*ausmem0 = (unsigned char) colore;
			*ausmem1 = (unsigned char) colore;
			ausmem0+=4;
			ausmem1+=4;
		}
		ausmem0 = mem + (long) (xi<<2) + _offset_riga[yi];
		ausmem1 = ausmem0 + (long) ((xl-1)<<2);
		for(j=yl; j>0; j--) {
			*ausmem0 = (unsigned char) colore;
			*ausmem1 = (unsigned char) colore;
			ausmem0 += offset;
			ausmem1 += offset;
		}
	} else {
		short xl = xf - xi + 1;
		short yl = yf - yi + 1;
		unsigned char *mem = _mem_address[fb];
		unsigned char *ausmem = mem + (long) xi + _offset_riga[yi];
		short i;

		memset(ausmem, (int) colore, (size_t) xl);
		memset(ausmem + _offset_riga[yl - 1], (int) colore, (size_t) xl);
		for(i=yl; i>0; i--) {
			ausmem[0] = (unsigned char) colore;
			ausmem[xl-1] = (unsigned char) colore;
			ausmem += (long) _param_scheda.lung_riga;
		}
	}

	return;
}

/*****************************************************************************/

void
sea2_disegna_finestra_col(
	short fb,
	short xi,
	short yi,
	short xf,
	short yf,
	short rosso,
	short verde,
	short blu
)
{
	if(!_param_scheda.scheda_aperta)
		return;
	if(_param_scheda.tipo_tv == TV_RGB) {
		short xl = xf - xi + 1;
		short yl = yf - yi + 1;
		unsigned char *mem = _mem_address_col[fb];
		unsigned char *ausmem0 = mem + (long) (xi<<2) + _offset_riga[yi];
		unsigned char *ausmem1 = ausmem0 + _offset_riga[yl-1];
		long offset = (long) ((_param_scheda.lung_riga << 2) - 2);
		unsigned char r = (unsigned char) rosso;
		unsigned char g = (unsigned char) verde;
		unsigned char b = (unsigned char) blu;
		short j;

		for(j=xl; j>0; j--) {
			*ausmem0++ = b;
			*ausmem0++ = g;
			*ausmem0++ = r;
			ausmem0++;
			*ausmem1++ = b;
			*ausmem1++ = g;
			*ausmem1++ = r;
			ausmem1++;
		}
		ausmem0 = mem + (long) (xi<<2) + _offset_riga[yi];
		ausmem1 = ausmem0 + (long) ((xl-1)<<2);
		for(j=yl; j>0; j--) {
			*ausmem0++ = b;
			*ausmem0++ = g;
			*ausmem0 = r;
			ausmem0 += offset;
			*ausmem1++ = b;
			*ausmem1++ = g;
			*ausmem1 = r;
			ausmem1 += offset;
		}
	}
}

/*****************************************************************************/

void
sea2_disegna_finestra_overlay(
	short fb,
	short xi,
	short yi,
	short xf,
	short yf,
	short rosso,
	short verde,
	short blu
)
{
	long primo_libero;

	if(!_param_scheda.scheda_aperta)
		return;
	primo_libero = _primo_byte_libero[fb];
	if(primo_libero + L_PRIM_FINESTRA < _max_n_byte_grafica) {
		unsigned char *mem = _mem_draw[fb];
		long primo_libero_fisico = primo_libero;
		mem[primo_libero_fisico] = PRIM_FINESTRA;
		mem[primo_libero_fisico+1] = (unsigned char) xi;
		mem[primo_libero_fisico+2] = (unsigned char) (xi >> 8);
		mem[primo_libero_fisico+3] = (unsigned char) yi;
		mem[primo_libero_fisico+4] = (unsigned char) (yi >> 8);
		mem[primo_libero_fisico+5] = (unsigned char) xf;
		mem[primo_libero_fisico+6] = (unsigned char) (xf >> 8);
		mem[primo_libero_fisico+7] = (unsigned char) yf;
		mem[primo_libero_fisico+8] = (unsigned char) (yf >> 8);
		mem[primo_libero_fisico+9] = (unsigned char) rosso;
		mem[primo_libero_fisico+10] = (unsigned char) (rosso >> 8);
		mem[primo_libero_fisico+11] = (unsigned char) verde;
		mem[primo_libero_fisico+12] = (unsigned char) (verde >> 8);
		mem[primo_libero_fisico+13] = (unsigned char) blu;
		mem[primo_libero_fisico+14] = (unsigned char) (blu >> 8);
		_primo_byte_libero[fb] =
			primo_libero + L_PRIM_FINESTRA;
	}
}

/***********************************************************************

;  Nome:    
	sea2_scrivi_pixel
;
;  Funzione:
	scrive un pixel sul frame buffer selezionato
;
;  Formato:
	void sea2_scrivi_pixel(x_pixel, y_pixel, valore)
;
;  Argomenti:    
	short x_pixel           ascissa del pixel
	short y_pixel           ordinata del pixel
	unsigned char valore    livello di grigio da assegnare al pixel

;
;  Descrizione:
     {\bf sea2_scrivi_pixel}() scrive un pixel sul
	 frame buffer selezionato con la routine {\bf sea2_seleziona_buffer}();
;
;  Note:
     le coordinate del pixel possono eccedere le dimensioni del
	 frame buffer in quanto l'accesso alla memoria e' realizzato
	 modulo le dimensioni della medesima;
;
;**********************************************************************/

void
sea2_scrivi_pixel(
	short x_pixel,
	short y_pixel,
	unsigned char valore
)
{
	if(!_param_scheda.scheda_aperta)
		return;
	if(_param_scheda.tipo_tv == TV_RGB) {
		unsigned char *mem = _mem_address[_param_scheda.fb_lavoro];
		unsigned char *ausmem = mem+(long)(x_pixel<<2) + _offset_riga[y_pixel];

		*ausmem = valore;
	} else {
		unsigned char *mem = _mem_address[_param_scheda.fb_lavoro];
		unsigned char *ausmem = mem + (long) x_pixel + _offset_riga[y_pixel];

		*ausmem = valore;
	}
	return;
}

/*********************************************************************/

void
sea2_scrivi_pixel_col(
	short x_pixel,
	short y_pixel,
	unsigned char rosso,
	unsigned char verde,
	unsigned char blu
)
{
	if(!_param_scheda.scheda_aperta)
		return;
	if(_param_scheda.tipo_tv == TV_RGB) {
		unsigned char *mem = _mem_address_col[_param_scheda.fb_lavoro];
		unsigned char *ausmem = mem + (long) (x_pixel<<2) + _offset_riga[y_pixel];

		*ausmem++ = blu;
		*ausmem++ = verde;
		*ausmem = rosso;
	}
}

/*********************************************************************/

void
sea2_disegna_croce_col(
	short xc,
	short yc,
	unsigned char rosso,
	unsigned char verde,
	unsigned char blu
)
{
	if(!_param_scheda.scheda_aperta)
		return;
	if(_param_scheda.tipo_tv == TV_RGB) {
		unsigned char *mem = _mem_address_col[_param_scheda.fb_lavoro];
		unsigned char *ausmem = mem + (long) ((xc-1)<<2) + _offset_riga[yc];

		*ausmem++ = blu;
		*ausmem++ = verde;
		*ausmem++ = rosso;
		ausmem++;
		*ausmem++ = blu;
		*ausmem++ = verde;
		*ausmem++ = rosso;
		ausmem++;
		*ausmem++ = blu;
		*ausmem++ = verde;
		*ausmem = rosso;
		ausmem = mem + (long) (xc<<2) + _offset_riga[yc-1];
		*ausmem++ = blu;
		*ausmem++ = verde;
		*ausmem = rosso;
		ausmem = mem + (long) (xc<<2) + _offset_riga[yc+1];
		*ausmem++ = blu;
		*ausmem++ = verde;
		*ausmem = rosso;
	}
}

/*********************************************************************/

void
sea2_disegna_quadratino_col(
	short xc,
	short yc,
	unsigned char rosso,
	unsigned char verde,
	unsigned char blu
)
{
	if(!_param_scheda.scheda_aperta)
		return;
	if(_param_scheda.tipo_tv == TV_RGB) {
		unsigned char *mem = _mem_address_col[_param_scheda.fb_lavoro];
		unsigned char *ausmem = mem + (long) ((xc-1)<<2) + _offset_riga[yc-1];

		*ausmem++ = blu;
		*ausmem++ = verde;
		*ausmem++ = rosso;
		ausmem++;
		*ausmem++ = blu;
		*ausmem++ = verde;
		*ausmem++ = rosso;
		ausmem++;
		*ausmem++ = blu;
		*ausmem++ = verde;
		*ausmem = rosso;
		ausmem = mem + (long) ((xc-1)<<2) + _offset_riga[yc];
		*ausmem++ = blu;
		*ausmem++ = verde;
		*ausmem++ = rosso;
		ausmem++;
		*ausmem++ = blu;
		*ausmem++ = verde;
		*ausmem++ = rosso;
		ausmem++;
		*ausmem++ = blu;
		*ausmem++ = verde;
		*ausmem = rosso;
		ausmem = mem + (long) ((xc-1)<<2) + _offset_riga[yc+1];
		*ausmem++ = blu;
		*ausmem++ = verde;
		*ausmem++ = rosso;
		ausmem++;
		*ausmem++ = blu;
		*ausmem++ = verde;
		*ausmem++ = rosso;
		ausmem++;
		*ausmem++ = blu;
		*ausmem++ = verde;
		*ausmem = rosso;
	}
}

/*********************************************************************/

void
sea2_scrivi_pixel_overlay(
	short x_pixel,
	short y_pixel,
	unsigned char rosso,
	unsigned char verde,
	unsigned char blu
)
{
	long primo_libero;

	if(!_param_scheda.scheda_aperta)
		return;
	primo_libero = _primo_byte_libero[_param_scheda.fb_lavoro];
	if(primo_libero + L_PRIM_PUNTO < _max_n_byte_grafica) {
		unsigned char *mem = _mem_draw[_param_scheda.fb_lavoro];
		long primo_libero_fisico = primo_libero;
		mem[primo_libero_fisico] = PRIM_PUNTO;
		mem[primo_libero_fisico+1] = (unsigned char) x_pixel;
		mem[primo_libero_fisico+2] = (unsigned char) (x_pixel >> 8);
		mem[primo_libero_fisico+3] = (unsigned char) y_pixel;
		mem[primo_libero_fisico+4] = (unsigned char) (y_pixel >> 8);
		mem[primo_libero_fisico+5] = (unsigned char) rosso;
		mem[primo_libero_fisico+6] = (unsigned char) 0;
		mem[primo_libero_fisico+7] = (unsigned char) verde;
		mem[primo_libero_fisico+8] = (unsigned char) 0;
		mem[primo_libero_fisico+9] = (unsigned char) blu;
		mem[primo_libero_fisico+10] = (unsigned char) 0;
		_primo_byte_libero[_param_scheda.fb_lavoro] =
			primo_libero + L_PRIM_PUNTO;
	}
}

/*********************************************************************/

void
sea2_disegna_croce_overlay(
	short xc,
	short yc,
	unsigned char rosso,
	unsigned char verde,
	unsigned char blu
)
{
	long primo_libero;

	if(!_param_scheda.scheda_aperta)
		return;
	primo_libero = _primo_byte_libero[_param_scheda.fb_lavoro];
	if(primo_libero + L_PRIM_CROCE < _max_n_byte_grafica) {
		unsigned char *mem = _mem_draw[_param_scheda.fb_lavoro];
		long primo_libero_fisico = primo_libero;
		mem[primo_libero_fisico] = PRIM_CROCE;
		mem[primo_libero_fisico+1] = (unsigned char) xc;
		mem[primo_libero_fisico+2] = (unsigned char) (xc >> 8);
		mem[primo_libero_fisico+3] = (unsigned char) yc;
		mem[primo_libero_fisico+4] = (unsigned char) (yc >> 8);
		mem[primo_libero_fisico+5] = (unsigned char) rosso;
		mem[primo_libero_fisico+6] = (unsigned char) 0;
		mem[primo_libero_fisico+7] = (unsigned char) verde;
		mem[primo_libero_fisico+8] = (unsigned char) 0;
		mem[primo_libero_fisico+9] = (unsigned char) blu;
		mem[primo_libero_fisico+10] = (unsigned char) 0;
		_primo_byte_libero[_param_scheda.fb_lavoro] =
			primo_libero + L_PRIM_CROCE;
	}
}

/*********************************************************************/

void
sea2_disegna_quadratino_overlay(
	short xc,
	short yc,
	unsigned char rosso,
	unsigned char verde,
	unsigned char blu
)
{
	long primo_libero;

	if(!_param_scheda.scheda_aperta)
		return;
	primo_libero = _primo_byte_libero[_param_scheda.fb_lavoro];
	if(primo_libero + L_PRIM_QUADRATINO < _max_n_byte_grafica) {
		unsigned char *mem = _mem_draw[_param_scheda.fb_lavoro];
		long primo_libero_fisico = primo_libero;
		mem[primo_libero_fisico] = PRIM_QUADRATINO;
		mem[primo_libero_fisico+1] = (unsigned char) xc;
		mem[primo_libero_fisico+2] = (unsigned char) (xc >> 8);
		mem[primo_libero_fisico+3] = (unsigned char) yc;
		mem[primo_libero_fisico+4] = (unsigned char) (yc >> 8);
		mem[primo_libero_fisico+5] = (unsigned char) rosso;
		mem[primo_libero_fisico+6] = (unsigned char) 0;
		mem[primo_libero_fisico+7] = (unsigned char) verde;
		mem[primo_libero_fisico+8] = (unsigned char) 0;
		mem[primo_libero_fisico+9] = (unsigned char) blu;
		mem[primo_libero_fisico+10] = (unsigned char) 0;
		_primo_byte_libero[_param_scheda.fb_lavoro] =
			primo_libero + L_PRIM_QUADRATINO;
	}
}

