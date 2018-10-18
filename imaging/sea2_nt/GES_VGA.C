/*
	sea2_set_window_handle
	sea2_how_many_bits_per_pixel_vga
	static CreateIdentityPalette
	static MakeBitmap_8
	static MakeBitmap_16
	static MakeBitmap_24_32
	sea2_copy_bitmap
	sea2_copy_bitmap2
	sea2_copy_bitmap3
	sea2_copy_bitmap_col
	sea2_copy_bitmap2_col
	sea2_copy_bitmap3_col
	sea2_refresh
	sea2_refresh2
	sea2_refresh3
	sea2_refresh_col
	sea2_refresh2_col
	sea2_refresh3_col
	sea2_copy_bitmap_row
	sea2_clear_bitmap
	sea2_direct_vga_mode
	sea2_is_direct_vga_enabled
	sea2_set_zoom_mode
	sea2_display_zoom
	sea2_draw_bitmap_to_dc
	sea2_display_piano_colore
	copia_riga_su_bitmap
	sea2_copy_bitmap_row_col
	static brush_opportuno
	static disegno_opportuno
	sea2_reset_overlay
	sea2_select_overlay_mode
	sea2_overlay_mode
	static sel_penna
	sea2_set_color_mode
	sea2_disegno_su_bitmap
	sea2_disegno_su_bitmap3
	static disegna_primitiva
	sea2_set_n_bit_col16
	sea2_copy_graphic_buffer
	sea2_copy_graphic_fb2fb
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
	#include <malloc.h>
	#include <string.h>
	#include <graph.h>
	#define RGB(r,g,b) ((unsigned long)(((char)(r)|((short)(g)<<8))|(((long)(char)(b))<<16)))
#endif
#include <stdio.h>

/****************************************************************************/

/* variabili globali definite in init.c */

#if defined(MET) || defined(ASS) || defined(MET_MIL) || defined(ORION) || defined(METEOR2)
	extern ROI _roi_fb[N_FB_MAX];
	extern short _roi_enable_display[N_FB_MAX];
	extern short _roi_enable_acq[N_FB_MAX];
	extern unsigned char *_mem_address[N_FB_MAX];
#ifndef __WATCOMC__
	extern HWND _win_handle; /* puntatore alla client area */
	extern HBITMAP _bitmap, _old_bitmap, _bitmap_row;
	extern HPALETTE _hpalApp, _old_palette;
	extern HDC _hdc_screen, _hdc_memory, _hdc_memory_row;
	extern HPEN _penna_nera, _penna_bianca, _penna_rossa, _penna_verde;
	extern HPEN _penna_gialla, _penna_magenta, _penna_ciano, _penna_blu;
	extern HBRUSH _brush_nero;
	static HPEN sel_penna(short r, short g, short b);
	// fine colore RGB
	static HPALETTE CreateIdentityPalette(RGBQUAD aRGB[], int nColors);
#else
	extern unsigned char *_bitmap, *_old_bitmap, *_bitmap_row;
#endif
	extern short _zoom_mode, /* 0:normale, 1:zoom */ _fb_zoom[4];
	/* fb da visualizzare in zoom; se -1 non si visualizza il fb */
	extern unsigned char *_aus_bitmap; /* array ausiliario la cui dimensione
	dipende soltanto dal tipo di display */
	extern long *_offset_riga_display; /* contiene, per ogni riga, l'offset del primo
	pixel; si rifersce al modo con cui si fa il display (1 o 4 byte per pixel)
	e quindi non dipende dal tipo di scheda usata; si usa per indirizzare
	_aus_bitmap */
	extern long *_offset_colonna_display; /* contiene, per ogni colonna, l'offset del primo
	pixel; si riferisce al modo con cui si fa il display (da 1 a 4 byte per pixel)
	e quindi non dipende dal tipo di scheda usata; si usa per indirizzare
	_aus_bitmap */
	extern long _n_byte_per_pixel;  /* si riferisce al modo di display della VGA;
	pertanto NON dipende dalla scheda usata; */
	static long _n_byte_per_riga;  /* dipende sia dal modo di display della VGA;
	sia dalla scheda usata; */
	extern short _n_bit_col16; /* numero di bit usati nel display a 16 bit;
	possono essere 15 (XRRRRRGGGGGBBBBB) o 16 (RRRRRGGGGGGBBBBB) */
	// per il colore RGB
	extern unsigned char *_mem_address_col[N_FB_MAX_COL];
	extern ROI _roi_fb_col[N_FB_MAX_COL];
	extern short _roi_enable_display_col[N_FB_MAX_COL];
	extern short _roi_enable_acq_col[N_FB_MAX_COL];
	extern short _piano_colore; /* per il display: 0:r, 1:g, 2:b, 3:colore */
	static short MakeBitmap_8();
	static short MakeBitmap_16();
	static short MakeBitmap_24_32(short n_bit);
	// variabili per la grafica in overlay
	extern unsigned char *_mem_draw[N_FB_MAX]; // dove memorizzare le primitive
	extern long _primo_byte_libero[N_FB_MAX];
	extern short _overlay_mode;
	extern long _max_n_byte_grafica;
	extern short _colori_approssimati;
#endif // MET || ASS || MET_MIL || ORION || METEOR2

#if defined(MET) || defined(MET_MIL) || defined(ORION) || defined(METEOR2)
	extern MIL_ID _mil_image[N_FB_MAX];
	extern MIL_ID _vga_buffer;
	extern short _direct_vga_possible, _vga_off_x, _vga_off_y, _mode_direct_vga;
	// per il colore RGB
	extern MIL_ID _mil_image_col[N_FB_MAX_COL];
	// fine colore RGB
#endif // MET || MET_MIL || ORION || METEOR2
	extern PARAM_SCHEDA _param_scheda;

static void brush_opportuno(short flag_neri);
static void disegno_opportuno(void);
static long disegna_primitiva(
	unsigned char *mem, long i, short offset_x, short offset_y
);
static void copia_riga_su_bitmap(short yi, unsigned char *buf_dib);

#ifndef __WATCOMC__
static unsigned char buf_dib[LUNG_MAX*3];
#endif

/***********************************************************************

;  Nome:    
	sea2_set_window_handle
;
;  Funzione:
     assegna l'handle della finestra su cui fare il display
;
;  Formato:
	void sea2_set_window_handle(hwnd)
;
;  Argomenti:    
    void near * hwnd           handle della finestra

;
;  Descrizione:
     {\bf sea2_set_window_handle}()
     assegna l'handle della finestra su cui fare il display;
	 sea la routine non va a buon fine mette a 0 _param_scheda.scheda_aperta

;
;**********************************************************************/

short
sea2_set_window_handle(
	void *hwnd
)
{
#ifndef __WATCOMC__
	BOOL ret;
	DEVMODE devmode;

	if(!_param_scheda.scheda_aperta)
		return SEAV_ERR_VGA_INIT;
	_win_handle = (HWND) hwnd;

/* allocazione della bitmap per il refresh a video */

	devmode.dmSize = sizeof(DEVMODE);
	devmode.dmDriverExtra = 0;
	ret = EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devmode);
	switch(devmode.dmBitsPerPel) {
		case 8:
			_n_byte_per_pixel = 1L;
			if(MakeBitmap_8() < 0) {
			 	_param_scheda.scheda_aperta = 0;
				return SEAV_ERR_VGA_INIT;
			}
			break;
		case 15:
			_n_byte_per_pixel = 2L;
			sea2_set_n_bit_col16(15);
			if(MakeBitmap_16() < 0) {
			 	_param_scheda.scheda_aperta = 0;
				return SEAV_ERR_VGA_INIT;
			}
			break;
		case 16:
			_n_byte_per_pixel = 2L;
			sea2_set_n_bit_col16(16);
			if(MakeBitmap_16() < 0) {
			 	_param_scheda.scheda_aperta = 0;
				return SEAV_ERR_VGA_INIT;
			}
			break;
		case 24:
			_n_byte_per_pixel = 3L;
			if(MakeBitmap_24_32(24) < 0) {
			 	_param_scheda.scheda_aperta = 0;
				return SEAV_ERR_VGA_INIT;
			}
			break;
		case 32:
			_n_byte_per_pixel = 4L;
			if(MakeBitmap_24_32(32) < 0) {
			 	_param_scheda.scheda_aperta = 0;
				return SEAV_ERR_VGA_INIT;
			}
			break;
		default:
			return SEAV_ERR_VGA_MODE_NOT_SUPPORTED;
	}
	_n_byte_per_riga = _n_byte_per_pixel * (long) _param_scheda.lung_riga;

/* creo alcune pen ed un brush */

	_penna_nera = CreatePen(PS_SOLID, 0, RGB(0,0,0));
	_penna_rossa = CreatePen(PS_SOLID, 0, RGB(255,0,0));
	_penna_verde = CreatePen(PS_SOLID, 0, RGB(0,255,0));
	_penna_blu = CreatePen(PS_SOLID, 0, RGB(0,0,255));
	_penna_gialla = CreatePen(PS_SOLID, 0, RGB(255,255,0));
	_penna_magenta = CreatePen(PS_SOLID, 0, RGB(255,0,255));
	_penna_ciano = CreatePen(PS_SOLID, 0, RGB(0,255,255));
	_penna_bianca = CreatePen(PS_SOLID, 0, RGB(255,255,255));
	SelectObject(_hdc_memory, _penna_nera);
	_brush_nero = CreateSolidBrush(RGB(0,0,0));
	SelectObject(_hdc_memory, _brush_nero);
#else
	struct videoconfig vc;

	if(!_param_scheda.scheda_aperta)
		return SEAV_ERR_VGA_INIT;
	if(!_setvideomode(_SVRES256COLOR)) {
		if(!_setvideomode(_MAXCOLORMODE)) {
			return SEAV_ERR_VGA_INIT;
		}
	}
	_getvideoconfig(&vc);
	switch(vc.bitsperpixel) {
		case 8:
			_n_byte_per_pixel = 1L;
			if(MakeBitmap_8() < 0) {
			 	_param_scheda.scheda_aperta = 0;
				return SEAV_ERR_VGA_INIT;
			}
			break;
		default:
			return SEAV_ERR_VGA_MODE_NOT_SUPPORTED;
	}
	_n_byte_per_riga = _n_byte_per_pixel * (long) _param_scheda.lung_riga;
#endif

	return SEAV_OK_ALL;
}

/****************************************************************************/

/* ritorna il numero di bit per pixel usati dalla vga oppure il codice
   di errore */

short
sea2_how_many_bits_per_pixel_vga(
	void
)
{
	if(!_param_scheda.scheda_aperta)
		return SEAV_ERR_VGA_INIT;
	switch(_n_byte_per_pixel) {
		case 1:
			return 8;
		case 2:
			if(_n_bit_col16 == 16)
				return 16;
			else
				return 15;
		case 3:
			return 24;
		case 4:
			return 32;
	}
	return SEAV_ERR_VGA_MODE_NOT_SUPPORTED;
}

/****************************************************************************/

#ifndef __WATCOMC__
static HPALETTE CreateIdentityPalette(RGBQUAD aRGB[], int nColors)
{
	int i;
	struct {
		WORD Version;
		WORD NumberOfEntries;
		PALETTEENTRY aEntries[256];
	} Palette =
	{
		0x300,
		256
	};

	/* Just use the screen DC where we need it */
	HDC hdc = GetDC((HWND) NULL);

	/* For SYSPAL_NOSTATIC, just copy the color table into
	   a PALETTEENTRY array and replace the first and last entries
	   with black and white  */
	if (GetSystemPaletteUse(hdc) == SYSPAL_NOSTATIC) {
		/* Fill in the palette with the given values, marking each
		   as PC_NOCOLLAPSE */
		for(i = 0; i < nColors; i++) {
			Palette.aEntries[i].peRed = aRGB[i].rgbRed;
			Palette.aEntries[i].peGreen = aRGB[i].rgbGreen;
			Palette.aEntries[i].peBlue = aRGB[i].rgbBlue;
			Palette.aEntries[i].peFlags = PC_NOCOLLAPSE;
		}

		/* Mark any unused entries PC_NOCOLLAPSE */
		for (; i < 256; ++i)
			Palette.aEntries[i].peFlags = PC_NOCOLLAPSE;

		/* Make sure the last entry is white
		   This may replace an entry in the array! */
		Palette.aEntries[255].peRed = 255;
		Palette.aEntries[255].peGreen = 255;
		Palette.aEntries[255].peBlue = 255;
		Palette.aEntries[255].peFlags = 0;

		/* And the first is black
		   This may replace an entry in the array! */
		Palette.aEntries[0].peRed = 0;
		Palette.aEntries[0].peGreen = 0;
		Palette.aEntries[0].peBlue = 0;
		Palette.aEntries[0].peFlags = 0;

	} else {
	/* For SYSPAL_STATIC, get the twenty static colors into
	   the array, then fill in the empty spaces with the
	   given color table */
		int nStaticColors;
		int nUsableColors;
//char st[20];

		/* Get the static colors from the system palette */
		nStaticColors = GetDeviceCaps(hdc, NUMCOLORS);
//sprintf(st, "n_col_stat: %d", nStaticColors);
//MessageBox(NULL, st, NULL, MB_OK);

		GetSystemPaletteEntries(hdc, 0, 256, Palette.aEntries);

		/* Set the peFlags of the lower static colors to zero */
		nStaticColors = nStaticColors / 2;

		for (i=0; i<nStaticColors; i++)
			Palette.aEntries[i].peFlags = 0;

		/* Fill in the entries from the given color table */
		nUsableColors = nColors - nStaticColors;
		for (; i<nUsableColors; i++) {
			Palette.aEntries[i].peRed = aRGB[i].rgbRed;
			Palette.aEntries[i].peGreen = aRGB[i].rgbGreen;
			Palette.aEntries[i].peBlue = aRGB[i].rgbBlue;
			Palette.aEntries[i].peFlags = PC_NOCOLLAPSE;
		}

		/* Mark any empty entries as PC_NOCOLLAPSE */

		for (; i<256 - nStaticColors; i++)
			Palette.aEntries[i].peFlags = PC_NOCOLLAPSE;

		/* Set the peFlags of the upper static colors to zero */
		for (i = 256 - nStaticColors; i<256; i++)
			Palette.aEntries[i].peFlags = 0;
	}

	/* Remember to release the DC! */
	ReleaseDC((HWND) NULL, hdc);

	/* Return the palette */
	return CreatePalette((LOGPALETTE *)&Palette);
}

/****************************************************************************/

/* realizza la palette su tutti di device, logici e fisici; ovviamente
   la palette deve esistere, ovvero questa funzione puo' essere lanciata
   solo dopo la sea2_set_window_handle(); se il display non e' a 8 bit
   non ha alcun effetto; puo' servire nel caso di switch tra varie applicazioni
   per ripristinare i colori giusti; in tal caso e' sufficiente lanciarla da
   OnActivateView */

void
sea2_realize_palette(
	void
)
{
	if(_n_byte_per_pixel == 1) {
		SelectPalette(_hdc_memory, _hpalApp, FALSE);
		RealizePalette(_hdc_memory);
		SelectPalette(_hdc_memory_row, _hpalApp, FALSE);
		RealizePalette(_hdc_memory_row);
		SelectPalette(_hdc_screen, _hpalApp, FALSE);
		RealizePalette(_hdc_screen);
	}
}
#endif /* __WATCOMC__ */

/****************************************************************************/

/* crea la bitmap ad 8 bit */

static short MakeBitmap_8()
{
#ifndef __WATCOMC__
	RGBQUAD ColorTable[256];
	int i;
	UINT n_ent;
	HPALETTE ret_p;
//char st[20];

	for (i=0; i<256; i++) {
		ColorTable[i].rgbBlue = (BYTE) (i);
		ColorTable[i].rgbRed = (BYTE) (i);
		ColorTable[i].rgbGreen = (BYTE) (i);
		ColorTable[i].rgbReserved = (BYTE) 0;
	}

	_hpalApp = CreateIdentityPalette(ColorTable, 256);
	_bitmap = CreateBitmap(
		_param_scheda.lung_riga, _param_scheda.lung_colonna, 1, 8, NULL
	);
	if(_bitmap == 0)
		return -1;
	_bitmap_row = CreateBitmap(
		_param_scheda.lung_riga, 1, 1, 8, NULL
	);
	if(_bitmap_row == 0)
		return -1;

	_hdc_screen = GetDC(_win_handle);
	_hdc_memory = CreateCompatibleDC(_hdc_screen);
	_hdc_memory_row = CreateCompatibleDC(_hdc_screen);

	SelectObject(_hdc_memory_row, (HBITMAP) _bitmap_row);
	_old_bitmap = SelectObject(_hdc_memory, (HBITMAP) _bitmap);

	_old_palette = SelectPalette(_hdc_memory, _hpalApp, FALSE);
	n_ent = RealizePalette(_hdc_memory);
//sprintf(st, "n_ent: %d  ret_p: %d", n_ent, (int) _old_palette);
//MessageBox(NULL, st, NULL, MB_OK);

	ret_p = SelectPalette(_hdc_memory_row, _hpalApp, FALSE);
	n_ent = RealizePalette(_hdc_memory_row);
//sprintf(st, "n_ent: %d  ret_p: %d", n_ent, (int) ret_p);
//MessageBox(NULL, st, NULL, MB_OK);

	ret_p = SelectPalette(_hdc_screen, _hpalApp, FALSE);
	n_ent = RealizePalette(_hdc_screen);
//sprintf(st, "n_ent: %d  ret_p: %d", n_ent, (int) ret_p);
//MessageBox(NULL, st, NULL, MB_OK);
#else
	int i;
	unsigned long ColorTable[256];
	long size;

	for(i=0; i<64; i++) {
		ColorTable[i*4] = RGB(i,i,i);
		ColorTable[i*4+1] = RGB(i,i,i);
		ColorTable[i*4+2] = RGB(i,i,i);
		ColorTable[i*4+3] = RGB(i,i,i);
	}
	ColorTable[249] = _RED;
	ColorTable[250] = _GREEN;
	ColorTable[251] = _YELLOW;
	ColorTable[252] = _BLUE;
	ColorTable[253] = _MAGENTA;
	ColorTable[254] = _CYAN;
	ColorTable[255] = _BRIGHTWHITE;

	_remapallpalette((long *)ColorTable);

	// preparo la bitmap compatibile
	size = (long)_imagesize(
		0, 0, (short) _param_scheda.lung_riga-1,
		(short) _param_scheda.lung_colonna-1
	);
	_bitmap = (unsigned char *) malloc(size);
	if(_bitmap == NULL)
		return -1;
	_getimage(0, 0, (short) (_param_scheda.lung_riga-1),
		(short) (_param_scheda.lung_colonna-1), (char *)_bitmap );

#endif
	return 0;
}

/****************************************************************************/

#ifndef __WATCOMC__
/* crea la bitmap a 16 bit */

static short MakeBitmap_16()
{
	_bitmap = CreateBitmap(
		_param_scheda.lung_riga, _param_scheda.lung_colonna, 1, 16, NULL
	);
	if(_bitmap == 0)
		return -1;
	_bitmap_row = CreateBitmap(
		_param_scheda.lung_riga, 1, 1, 16, NULL
	);
	if(_bitmap_row == 0)
		return -1;

	_hdc_screen = GetDC(_win_handle);
	_hdc_memory = CreateCompatibleDC(_hdc_screen);
	_hdc_memory_row = CreateCompatibleDC(_hdc_screen);
	SelectObject(_hdc_memory_row, (HBITMAP) _bitmap_row);
	_old_bitmap = SelectObject(_hdc_memory, (HBITMAP) _bitmap);

	return 0;
}

/****************************************************************************/

/* crea la bitmap a 24 o 32 bit */

static short MakeBitmap_24_32(short n_bit)
{
	_bitmap = CreateBitmap(
		_param_scheda.lung_riga, _param_scheda.lung_colonna, 1, n_bit, NULL
	);
	if(_bitmap == 0)
		return -1;
	_bitmap_row = CreateBitmap(
		_param_scheda.lung_riga, 1, 1, n_bit, NULL
	);
	if(_bitmap_row == 0)
		return -1;

	_hdc_screen = GetDC(_win_handle);
	_hdc_memory = CreateCompatibleDC(_hdc_screen);
	_hdc_memory_row = CreateCompatibleDC(_hdc_screen);
	SelectObject(_hdc_memory_row, (HBITMAP) _bitmap_row);
	_old_bitmap = SelectObject(_hdc_memory, (HBITMAP) _bitmap);

	return 0;
}
#endif /* __WATCOMC__ */


/***********************************************************************

;  Nome:    
	sea2_copy_bitmap
;
;  Funzione:
     copia il frame buffer nella bitmap usata per il display
;
;  Formato:
	void sea2_copy_bitmap(flag_neri)
;
;  Argomenti:
    short flag_neri               se 1 si azzera la porzione di video
                                  non occupata da alcun frame buffer

;  Descrizione:
     {\bf sea2_copy_bitmap}() copia il frame buffer nella bitmap usata
     per il display; in caso di zoom copia i frame buffer prescelti
     per la presentazione in zoom; la copia viene fatta tramite
     sottocampionamento 1:2;
     il display avviene di fatto in 3 tempi: dal frame buffer all'array
     _aus_bitmap allocato in init.c, da _aus_bitmap alla bitmap, dalla
     bitmap alla VGA; i primi due passi sono eseguiti da questa routine,
	 il terzo dalla sea2_refresh();     
;
;**********************************************************************/

/* lavora su frame buffer da 8 bit */

void
sea2_copy_bitmap(
	short flag_neri
)
{
	short i, j, lr2;
	unsigned char *ausp;

	if(!_param_scheda.scheda_aperta)
		return;
	lr2 = (short) ((_param_scheda.lung_riga >> 1) * _n_byte_per_pixel);
	if(_overlay_mode & UNDERLAY_MODE) {
		if(_zoom_mode) {  /* zoom */
			for(j=0; j<4; j++) {
				long offset = (long) (j & 1) *
							  (long) (_param_scheda.lung_riga >> 1);
				if(j >= 2)
					offset += (long) (_param_scheda.lung_colonna >> 1) *
							  (long) _param_scheda.lung_riga;
				if(_fb_zoom[j] > -1) {
					sea2_seleziona_buffer(_fb_zoom[j]);
					if(_roi_enable_display[_fb_zoom[j]]) { /* zoom con roi */
						short fb = _fb_zoom[j];
						short xi = _roi_fb[fb].xi;
						short yi = _roi_fb[fb].yi;
						short xf = _roi_fb[fb].xf;
						short yf = _roi_fb[fb].yf;
						short xl = (short) (xf-xi+1);
						ausp = _aus_bitmap + (offset +
							(long) (yi>>1) * (long) _param_scheda.lung_riga +
							(long) (xi>>1)) * _n_byte_per_pixel;
						switch(_n_byte_per_pixel) {
							case 1:
								for(i=yi; i<=yf; i+=2, ausp += _n_byte_per_riga)
									sea2_sotto_c2_riga(xi, i, xl, ausp);
								break;
							case 2:
								for(i=yi; i<=yf; i+=2, ausp += _n_byte_per_riga)
									sea2_sotto_c2_riga2(xi, i, xl, ausp);
								break;
							case 3:
								for(i=yi; i<=yf; i+=2, ausp += _n_byte_per_riga)
									sea2_sotto_c2_riga3(xi, i, xl, ausp);
								break;
							case 4:
								for(i=yi; i<=yf; i+=2, ausp += _n_byte_per_riga)
									sea2_sotto_c2_riga4(xi, i, xl, ausp);
								break;
						}
					} else { /* zoom senza roi */
						for(
							i=0, ausp = _aus_bitmap + (offset * _n_byte_per_pixel);
							i<_param_scheda.lung_colonna;
							i+=2, ausp += _n_byte_per_riga
						) {
							switch(_n_byte_per_pixel) {
								case 1:
									sea2_sotto_c2_riga(
										0, i, _param_scheda.lung_riga, ausp
									);
									/*sea2_sotto_c2_riga(
										0, i, _param_scheda.lung_riga, ausp
									);*/
									break;
								case 2:
									sea2_sotto_c2_riga2(
										0, i, _param_scheda.lung_riga, ausp
									);
									break;
								case 3:
									sea2_sotto_c2_riga3(
										0, i, _param_scheda.lung_riga, ausp
									);
									break;
								case 4:
									sea2_sotto_c2_riga4(
										0, i, _param_scheda.lung_riga, ausp
									);
									break;
							}
						}
					}
				} else if(flag_neri) {
					for(
						i=0, ausp = _aus_bitmap +(offset * _n_byte_per_pixel);
						i<_param_scheda.lung_colonna;
						i+=2, ausp += _n_byte_per_riga
					) {
						memset(ausp, 0, lr2);
					}
				}
			}
		} else { /* no zoom */
			sea2_seleziona_buffer(_param_scheda.fb_display);
			if(_roi_enable_display[_param_scheda.fb_display]) { /* no zoom con roi */
				short fb = _param_scheda.fb_display;
				short xi = _roi_fb[fb].xi;
				short yi = _roi_fb[fb].yi;
				short xf = _roi_fb[fb].xf;
				short yf = _roi_fb[fb].yf;
				short xl = (short) (xf-xi+1);
				ausp = _aus_bitmap + ((long) xi +
					(long) yi * (long) _param_scheda.lung_riga) * _n_byte_per_pixel;
				switch(_n_byte_per_pixel) {
					case 1:
						for(i=yi; i<=yf; i++, ausp += _n_byte_per_riga)
							sea2_leggi_riga(xi, i, xl, ausp);
						break;
					case 2:
						for(i=yi; i<=yf; i++, ausp += _n_byte_per_riga)
							sea2_leggi_riga2(xi, i, xl, ausp);
						break;
					case 3:
						for(i=yi; i<=yf; i++, ausp += _n_byte_per_riga)
							sea2_leggi_riga3(xi, i, xl, ausp);
						break;
					case 4:
						for(i=yi; i<=yf; i++, ausp += _n_byte_per_riga)
							sea2_leggi_riga4(xi, i, xl, ausp);
						break;
				}
			} else { /* no zoom senza roi */
				for(
					i=0, ausp = _aus_bitmap;
					i<_param_scheda.lung_colonna;
					i++, ausp += _n_byte_per_riga
				) {
					switch(_n_byte_per_pixel) {
						case 1:
							sea2_leggi_riga(0, i, _param_scheda.lung_riga, ausp);
							break;
						case 2:
							sea2_leggi_riga2(0, i, _param_scheda.lung_riga, ausp);
							break;
						case 3:
							sea2_leggi_riga3(0, i, _param_scheda.lung_riga, ausp);
							break;
						case 4:
							sea2_leggi_riga4(0, i, _param_scheda.lung_riga, ausp);
							break;
					}
				}
			}
		}
#ifndef __WATCOMC__
		SetBitmapBits(
			_bitmap, _n_byte_per_riga * (long) _param_scheda.lung_colonna,
			_aus_bitmap
		);
		SelectObject(_hdc_memory, _bitmap);
#else
		memcpy(
			_bitmap+6, _aus_bitmap,
			_n_byte_per_riga * (long) _param_scheda.lung_colonna
		);
#endif
	} else { /* faccio il brush sulla bitmap nelle zone opportune */
		brush_opportuno(flag_neri);
	}
	
	/* eventualmente disegno sulla bitmap */
	if(_overlay_mode & OVERLAY_MODE) {
		disegno_opportuno();
	}
}

/************************************************************************/

/* effettua lo stesso lavoro di sea2_copy_bitmap(), ma trascrive solo una riga
   sull'array _aus_bitmap, quella passata come argomento;
   se flag_copy vale 1 copia tutto _aus_bitmap sulla bitmap;
   se flag_copy vale 2 copia sulla bitmap solo la riga yi;
*/

#ifndef __WATCOMC__
void
sea2_copy_bitmap2(
	short flag_neri,
	short yi, /* ordinata della riga su _aus_bitmap */
	short flag_copy
)
{
	short i, j, j1, j2, copia=0, lr2, lc2;
	unsigned char *ausp, *ausp1;
	long nbpp;

	if(!_param_scheda.scheda_aperta)
		return;
	lc2 = _param_scheda.lung_colonna >> 1;
	/* quando flag copy vale 2 e la vga e' settata a 32 bit dobbiamo creare
	   un array a 24 bit da dare a SetDIBits; pertanto in aus_bitmap metto
	   i pixel come se aus_bitmap fosse a 24 bit */
	if(flag_copy == 2 && _n_byte_per_pixel == 4)
		nbpp = 3;
	else
		nbpp = _n_byte_per_pixel;
	lr2 = (short) ((_param_scheda.lung_riga >> 1) * nbpp);
	if(_overlay_mode & UNDERLAY_MODE) { /* se e' abilitato l'underlay */
		if(flag_copy == 2)
			ausp = buf_dib;
		else
			/* ausp e' l'indirizzo del primo byte della riga di aus_bitmap */
			ausp = _aus_bitmap + _offset_riga_display[yi];

		if(_zoom_mode) { /* zoom */
			if(yi < lc2) {
				j1 = 0;
				j2 = 1;
				i = yi << 1; /* ordinata della riga nell'immagine sorgente */
			} else {
				j1 = 2;
				j2 = 3;
				i = (yi-lc2) << 1;
			}
			for(j=j1; j<=j2; j++) {
				/* ausp1 e' l'indirizzo del primo byte su cui si va a scrivere */
				ausp1 = ausp;
				if(j & 1) {
					ausp1 += (long) lr2;
				}
				if(_fb_zoom[j] > -1) { /* fb da presentare */
					sea2_seleziona_buffer(_fb_zoom[j]);
					if(_roi_enable_display[_fb_zoom[j]]) { /* zoom con roi */
						short fb = _fb_zoom[j];
						short xi = _roi_fb[fb].xi;
						short yyi = _roi_fb[fb].yi;
						short xf = _roi_fb[fb].xf;
						short yf = _roi_fb[fb].yf;
						short xl = (short) (xf-xi+1);
						ausp1 += ((long) (xi>>1) * nbpp);
						if(i >= yyi && i <= yf) {
							copia = 1;
							switch(_n_byte_per_pixel) {
								case 1:
									sea2_sotto_c2_riga(xi, i, xl, ausp1);
									break;
								case 2:
									sea2_sotto_c2_riga2(xi, i, xl, ausp1);
									break;
								case 3:
									sea2_sotto_c2_riga3(xi, i, xl, ausp1);
									break;
								case 4:
									if(flag_copy == 2) /* leggo come a 24 bit */
										sea2_sotto_c2_riga3(xi, i, xl, ausp1);
									else
										sea2_sotto_c2_riga4(xi, i, xl, ausp1);
									break;
							}
						}
					} else { /* zoom senza roi */
						copia = 1;
						switch(_n_byte_per_pixel) {
							case 1:
								sea2_sotto_c2_riga(
									0, i, _param_scheda.lung_riga, ausp1
								);
								break;
							case 2:
								sea2_sotto_c2_riga2(
									0, i, _param_scheda.lung_riga, ausp1
								);
								break;
							case 3:
								sea2_sotto_c2_riga3(
									0, i, _param_scheda.lung_riga, ausp1
								);
								break;
							case 4:
								if(flag_copy == 2) /* leggo come a 24 bit */
									sea2_sotto_c2_riga3(
										0, i, _param_scheda.lung_riga, ausp1
									);
								else
									sea2_sotto_c2_riga4(
										0, i, _param_scheda.lung_riga, ausp1
									);
								break;
						}
					}
				} else { /* fb da non presentare */
					if(flag_neri) {
						copia = 1;
						memset(ausp1, 0, lr2);
					}
				}
			} /* for() */
		} else { /* no zoom */
			sea2_seleziona_buffer(_param_scheda.fb_display);
			if(_roi_enable_display[_param_scheda.fb_display]) { /* no zoom con roi */
				short fb = _param_scheda.fb_display;
				short xi = _roi_fb[fb].xi;
				short yyi = _roi_fb[fb].yi;
				short xf = _roi_fb[fb].xf;
				short yf = _roi_fb[fb].yf;
				short xl = (short) (xf-xi+1);
				ausp1 = ausp + (xi*nbpp);
				if(yi >= yyi && yi <= yf) {
					copia = 1;
					switch(_n_byte_per_pixel) {
						case 1:
							sea2_leggi_riga(xi, yi, xl, ausp1);
							break;
						case 2:
							sea2_leggi_riga2(xi, yi, xl, ausp1);
							break;
						case 3:
							sea2_leggi_riga3(xi, yi, xl, ausp1);
							break;
						case 4:
							if(flag_copy == 2) /* leggo come a 24 bit */
								sea2_leggi_riga3(xi, yi, xl, ausp1);
							else
								sea2_leggi_riga4(xi, yi, xl, ausp1);
							break;
					}
				}
			} else { /* no zoom senza roi */
				copia = 1;
				switch(_n_byte_per_pixel) {
					case 1:
						sea2_leggi_riga(0, yi, _param_scheda.lung_riga, ausp);
						break;
					case 2:
						sea2_leggi_riga2(0, yi, _param_scheda.lung_riga, ausp);
						break;
					case 3:
						sea2_leggi_riga3(0, yi, _param_scheda.lung_riga, ausp);
						break;
					case 4:
						if(flag_copy == 2) /* leggo come a 24 bit */
							sea2_leggi_riga3(0, yi, _param_scheda.lung_riga, ausp);
						else
							sea2_leggi_riga4(0, yi, _param_scheda.lung_riga, ausp);
						break;
				}
			}
		}
		if(flag_copy == 1) { /* copio tutto _aus_bitmap sulla bitmap */
			SetBitmapBits(
				_bitmap,
				_n_byte_per_riga * (long) _param_scheda.lung_colonna, _aus_bitmap
			);
		} else if(flag_copy == 2 && copia) { /* copio la riga yi sulla bitmap */
			if(_n_byte_per_pixel >= 3) {
				/* vado direttamente sulla bitmap usando SetDIBits() */
				copia_riga_su_bitmap(yi, ausp);
			} else {
				/* faccio il doppio passaggio per la bitmap intermedia _bitmap_row */
				SetBitmapBits(_bitmap_row, _n_byte_per_riga, ausp);
				BitBlt(
					_hdc_memory, 0, yi, (int) _param_scheda.lung_riga, 1,
					_hdc_memory_row, 0, 0, SRCCOPY
				);
			}
		}
	} else { /* l'underlay non e' abilitato */
		if(flag_copy == 1) {
			/* faccio il brush sulla bitmap nelle zone opportune */ 
			brush_opportuno(flag_neri);
		} else if(flag_copy == 2) {
			/* faccio il brush sulla bitmap solo sulla riga yi */
			SelectObject(_hdc_memory, _penna_nera);
			Rectangle(
				_hdc_memory, _param_scheda.x_min, yi,
				_param_scheda.x_max+1, yi+1
			);
		}
	}

	if(flag_copy == 1) {
		/* eventualmente disegno sulla bitmap */
		if(_overlay_mode & OVERLAY_MODE) {
			disegno_opportuno();
		}
	}
}

/************************************************************************/

/* 2.54.0-7 effettua lo stesso lavoro di sea2_copy_bitmap2(), ma tiene conto del frame
   buffer passato come argomento; se fb=-1 lavora come sea2_copy_bitmap2(),
   altrimenti lavora solo sul fb specificato;
   inoltre non gestisce flag_neri e il flag_copy=1 */

void
sea2_copy_bitmap3(
	short flag_neri,
	short riga, /* ordinata della riga su _aus_bitmap */
	short flag_copy,
	short fb
)
{
	short i, j, j1, j2, copia=0;
	unsigned char *ausp, *ausp1;
	short n_pixel_mezza_riga;
	short n_pixel_mezza_colonna;
	short n_byte_mezza_riga; /* dipende da _n_byte_per_pixel della vga */
	short xi_ab; /* ascissa di scrittura su _aus_bitmap (l'ordinata e' riga) */
	short xl_ab; /* numero di pixel da scrivere su _aus_bitmap */
	short xi, yi, xf, yf, xl; /* coordinate della roi */

	/* se devo fare tutti i fb lancio la vecchia routine */
	if(fb < 0) {
		sea2_copy_bitmap2(flag_neri, riga, flag_copy);
		return;
	}

	if(!_param_scheda.scheda_aperta)
		return;

	n_pixel_mezza_riga = (short) (_param_scheda.lung_riga >> 1);
	n_pixel_mezza_colonna = _param_scheda.lung_colonna >> 1;
	n_byte_mezza_riga = (short) (n_pixel_mezza_riga * _n_byte_per_pixel);

	if(_overlay_mode & UNDERLAY_MODE) { /* se e' abilitato l'underlay */
		/* ausp e' l'indirizzo del primo byte della riga di aus_bitmap */
		ausp = _aus_bitmap + _offset_riga_display[riga];

		if(_zoom_mode) { /* zoom */
			if(riga < n_pixel_mezza_colonna) {
				j1 = 0;
				j2 = 1;
				i = riga << 1; /* ordinata della riga nell'immagine sorgente */
			} else {
				j1 = 2;
				j2 = 3;
				i = (riga-n_pixel_mezza_colonna) << 1;
			}
			for(j=j1; j<=j2; j++) {
				if(_fb_zoom[j] > -1 && _fb_zoom[j] == fb) { /* fb da presentare */
					/* ausp1 e' l'indirizzo del primo byte su cui si va a scrivere */
					ausp1 = ausp;
					if(j & 1)
						ausp1 += (long) n_byte_mezza_riga;
					sea2_seleziona_buffer(fb);
					if(_roi_enable_display[fb]) { /* zoom con roi */
						xi = _roi_fb[fb].xi;
						yi = _roi_fb[fb].yi;
						xf = _roi_fb[fb].xf;
						yf = _roi_fb[fb].yf;
						xl = (short) (xf-xi+1);
						xl_ab = xl >> 1;
						xi_ab = xi >> 1;
						if(j & 1)
							xi_ab += n_pixel_mezza_riga;
						ausp1 += ((long) (xi>>1) * _n_byte_per_pixel);
						if(i >= yi && i <= yf) {
							copia = 1;
							switch(_n_byte_per_pixel) {
								case 1:
									sea2_sotto_c2_riga(xi, i, xl, ausp1);
									break;
								case 2:
									sea2_sotto_c2_riga2(xi, i, xl, ausp1);
									break;
								case 3:
									sea2_sotto_c2_riga3(xi, i, xl, ausp1);
									break;
								case 4:
									sea2_sotto_c2_riga4(xi, i, xl, ausp1);
									break;
							}
						}
					} else { /* zoom senza roi */
						copia = 1;
						xi = 0;
						xl = _param_scheda.lung_riga;
						xl_ab = xl >> 1;
						xi_ab = xi >> 1;
						if(j & 1)
							xi_ab += n_pixel_mezza_riga;
						switch(_n_byte_per_pixel) {
							case 1:
								sea2_sotto_c2_riga(xi, i, xl, ausp1);
								break;
							case 2:
								sea2_sotto_c2_riga2(xi, i, xl, ausp1);
								break;
							case 3:
								sea2_sotto_c2_riga3(xi, i, xl, ausp1);
								break;
							case 4:
								sea2_sotto_c2_riga4(xi, i, xl, ausp1);
								break;
						}
					}
				}
			} /* for() */
		} else { /* no zoom */
			if(fb == _param_scheda.fb_display) {
				sea2_seleziona_buffer(fb);
				if(_roi_enable_display[fb]) { /* no zoom con roi */
					xi = _roi_fb[fb].xi;
					yi = _roi_fb[fb].yi;
					xf = _roi_fb[fb].xf;
					yf = _roi_fb[fb].yf;
					xl = (short) (xf-xi+1);
					ausp1 = ausp + (xi*_n_byte_per_pixel);
					xl_ab = xl;
					xi_ab = xi;
					if(riga >= yi && riga <= yf) {
						copia = 1;
						switch(_n_byte_per_pixel) {
							case 1:
								sea2_leggi_riga(xi, riga, xl, ausp1);
								break;
							case 2:
								sea2_leggi_riga2(xi, riga, xl, ausp1);
								break;
							case 3:
								sea2_leggi_riga3(xi, riga, xl, ausp1);
								break;
							case 4:
								sea2_leggi_riga4(xi, riga, xl, ausp1);
								break;
						}
					}
				} else { /* no zoom senza roi */
					ausp1 = ausp;
					copia = 1;
					xi = 0;
					xl = _param_scheda.lung_riga;
					xl_ab = xl;
					xi_ab = xi;
					switch(_n_byte_per_pixel) {
						case 1:
							sea2_leggi_riga(xi, riga, xl, ausp1);
							break;
						case 2:
							sea2_leggi_riga2(xi, riga, xl, ausp1);
							break;
						case 3:
							sea2_leggi_riga3(xi, riga, xl, ausp1);
							break;
						case 4:
							sea2_leggi_riga4(xi, riga, xl, ausp1);
							break;
					}
				}
			}
		}
		if(flag_copy == 2 && copia) { /* copio la riga sulla bitmap */
			/* faccio il doppio passaggio per la bitmap intermedia _bitmap_row */
			SetBitmapBits(_bitmap_row, xl_ab * _n_byte_per_pixel, ausp1);
			BitBlt(
				_hdc_memory, xi_ab, riga, xl_ab, 1,
				_hdc_memory_row, 0, 0, SRCCOPY
			);
		}
	}
}

/***********************************************************************/

/* copia un segmento sull'array ausiliario _aus_bitmap;
   se flag_copy vale 1 copia tutto _aus_bitmap sulla bitmap;
*/

void
sea2_copy_bitmap_row(
	short xi,       /* coordinate iniziali del segmento da copiare */
	short yi,
	short n_pixel,  /* numero di pixel da copiare da buffer */
	unsigned char *buffer,
	short flag_copy
)
{
	short i;
	unsigned char *ausmem;
	unsigned short *ausmem_s;

	if(!_param_scheda.scheda_aperta)
		return;
	ausmem = _aus_bitmap + _offset_colonna_display[xi] + _offset_riga_display[yi];
	ausmem_s = (unsigned short *) ausmem;
	switch(_n_byte_per_pixel) {
		case 1:
			memcpy(ausmem, buffer, (size_t) n_pixel);
			break;
		case 2:
			if(_n_bit_col16 == 15) {
				for(i=n_pixel; i>0; i--) {
					*ausmem_s++ = SHORT_15(*buffer, *buffer, *buffer);
					buffer++;
				}
			} else {
				for(i=n_pixel; i>0; i--) {
					*ausmem_s++ = SHORT_16(*buffer, *buffer, *buffer);
					buffer++;
				}
			}
			break;
		case 3:
			for(i=n_pixel; i>0; i--) {
				*ausmem++ = *buffer;
				*ausmem++ = *buffer;
				*ausmem++ = *buffer++;
			}
			break;
		case 4:
			for(i=n_pixel; i>0; i--) {
				*ausmem++ = *buffer;
				*ausmem++ = *buffer;
				*ausmem++ = *buffer;
				*ausmem++ = *buffer++;
			}
			break;
	}
	if(flag_copy == 1) {
		SetBitmapBits(
			_bitmap,
			_n_byte_per_riga * (long) _param_scheda.lung_colonna, _aus_bitmap
		);
/*		SelectObject(_hdc_memory, _bitmap);*/
	}
}
#endif /* __WATCOMC__ */

/***********************************************************************/

/* azzera l'array ausiliario _aus_bitmap */

void
sea2_clear_bitmap(
	void
)
{
	short i;
	unsigned char *ausp;

	if(!_param_scheda.scheda_aperta)
		return;
	ausp = _aus_bitmap;
	for(i=0; i<_param_scheda.lung_colonna; i++, ausp+=_n_byte_per_riga)
		memset(ausp, 0, (size_t) _n_byte_per_riga);
}

/***********************************************************************

;  Nome:    
	sea2_refresh
;
;  Funzione:
     copia la bitmap sulla VGA
;
;  Formato:
	void sea2_refresh(xi, yi, xf, yf)

;
;  Argomenti
		short xi
		short yi
		short xf
		short yf
;
;  Descrizione:
     {\bf sea2_refresh}() copia dalla bitmap alla VGA la porzione di
     frame buffer specificata dai quattro argomenti;
     se i quattro parametri sono tutti a zero si copia tutto il frame buffer;
     se e' abilitato il modo direct_vga (e se e' possibile farlo)
     {\bf sea2_refresh}() copia tutto il frame buffer direttamente sulla VGA;
     gli argomenti in questo caso sono ignorati, ma si tiene conto dei parametri
     di pan e scroll impostati;
;
;**********************************************************************/

void
sea2_refresh(
	short xi,
	short yi,
	short xf,
	short yf
)
{
#ifndef __WATCOMC__
	short j;   

	if(!_param_scheda.scheda_aperta)
		return;
#ifdef MET
	if(!_mode_direct_vga) {  /* no direct vga */
#endif
		if(!xi && !yi && !xf && !yf) { /* schermo non parzializzato */
			if(_zoom_mode) { /* zoom */
				for(j=0; j<4; j++) {
					short off_x=0, off_y=0;
					if(j & 1)
						off_x = (short) (_param_scheda.lung_riga >> 1);
					if(j >= 2)
						off_y = (short) (_param_scheda.lung_colonna >> 1);
					if(_fb_zoom[j] > -1) { /* fb da vedere */
						if(_roi_enable_display[_fb_zoom[j]]) {/* zoom con roi */
							short fb = _fb_zoom[j];
							short xxi = (short) (_roi_fb[fb].xi >> 1) + off_x;
							short yyi = (short) (_roi_fb[fb].yi >> 1) + off_y;
							short xxf = (short) (_roi_fb[fb].xf >> 1) + off_x;
							short yyf = (short) (_roi_fb[fb].yf >> 1) + off_y;
							BitBlt(
								_hdc_screen, xxi, yyi,
								(int) (xxf-xxi+1), (int) (yyf-yyi+1),
								_hdc_memory, xxi, yyi, SRCCOPY
							);
						} else { /* zoom senza roi */
							BitBlt(
								_hdc_screen, off_x, off_y,
								(int) (_param_scheda.lung_riga >> 1),
								(int) (_param_scheda.lung_colonna >> 1),
								_hdc_memory, off_x, off_y, SRCCOPY
							);
						}
					}
				}
			} else {  /* no zoom */
				if(_roi_enable_display[_param_scheda.fb_display]) {
					/* no zoom con roi */
					short fb = _param_scheda.fb_display;
					short xxi = _roi_fb[fb].xi;
					short yyi = _roi_fb[fb].yi;
					short xxf = _roi_fb[fb].xf;
					short yyf = _roi_fb[fb].yf;
					BitBlt(
						_hdc_screen, xxi, yyi, (int) (xxf-xxi+1),
						(int) (yyf-yyi+1), _hdc_memory, xxi, yyi, SRCCOPY
					);
				} else { /* no zoom senza roi */
					BitBlt(
						_hdc_screen, 0, 0, (int) _param_scheda.lung_riga,
						(int) _param_scheda.lung_colonna, _hdc_memory,
						0, 0, SRCCOPY
					);
				}
			}
		} else { /* schermo parzializzato */
			if(xi < _param_scheda.x_min)
				xi = _param_scheda.x_min;
			if(yi < _param_scheda.y_min)
				yi = _param_scheda.y_min;
			if(xf > _param_scheda.x_max)
				xf = _param_scheda.x_max;
			if(yf > _param_scheda.y_max)
				yf = _param_scheda.y_max;
			BitBlt(
				_hdc_screen, xi, yi, (int) (xf-xi+1), (int) (yf-yi+1),
				_hdc_memory, xi, yi, SRCCOPY
			);
		}
#ifdef MET
	} else {  /* direct VGA */
		MTbufPut2d(
			_vga_buffer, _vga_off_x - _param_scheda.pan_display,
			_vga_off_y - _param_scheda.scroll_display, _param_scheda.lung_riga,
			_param_scheda.lung_colonna,
			_mem_address[_param_scheda.fb_display]
		);
	} 
#endif
#else /* __WATCOMC__ */
	_putimage(0, 0, (char *) _bitmap, _GPSET);
#endif/* __WATCOMC__ */

}

/***********************************************************************/

/* come sea2_refresh, ma porta sulla VGA solo la porzione opportuna della
   riga passata come argomento */

#ifndef __WATCOMC__
void
sea2_refresh2(
	short yi
)
{
	short j;   
	short lr2 = _param_scheda.lung_riga >> 1;
	short lc2 = _param_scheda.lung_colonna >> 1;
	short j1, j2;

	if(!_param_scheda.scheda_aperta)
		return;
	if(_zoom_mode) { /* zoom */
		if(yi < lc2) {
			j1 = 0;
			j2 = 1;
		} else {
			j1 = 2;
			j2 = 3;
		}
		for(j=j1; j<=j2; j++) {
			if(_fb_zoom[j] > -1) { /* fb da vedere */
				short off_x = (short) ((j & 1) ? lr2 : 0);
				if(_roi_enable_display[_fb_zoom[j]]) { /* zoom con roi */
					short off_y = (short) ((j > 1) ? lc2 : 0);
					short fb = _fb_zoom[j];
					short yyi = (short) (_roi_fb[fb].yi >> 1) + off_y;
					short yyf = (short) (_roi_fb[fb].yf >> 1) + off_y;
					if(yi >= yyi && yi <= yyf) {
						short xxi = (short) (_roi_fb[fb].xi >> 1) + off_x;
						short xxf = (short) (_roi_fb[fb].xf >> 1) + off_x;
						BitBlt(
							_hdc_screen, xxi, yi,
							(int) (xxf-xxi+1), 1,
							_hdc_memory, xxi, yi, SRCCOPY
						);
					}
				} else { /* zoom senza roi */
					BitBlt(
						_hdc_screen, off_x, yi, lr2, 1,
						_hdc_memory, off_x, yi, SRCCOPY
					);
				}
			}
		}
	} else {  /* no zoom */
		if(_roi_enable_display[_param_scheda.fb_display]) {
			/* no zoom con roi */
			short fb = _param_scheda.fb_display;
			if(yi >= _roi_fb[fb].yi && yi <= _roi_fb[fb].yf) {
				short xxi = _roi_fb[fb].xi;
				short xxf = _roi_fb[fb].xf;
				BitBlt(
					_hdc_screen, xxi, yi, (int) (xxf-xxi+1),
					1, _hdc_memory, xxi, yi, SRCCOPY
				);
			}
		} else { /* no zoom senza roi */
			BitBlt(
				_hdc_screen, 0, yi, (int) _param_scheda.lung_riga, 1,
				_hdc_memory, 0, yi, SRCCOPY
			);
		}
	}
}

/***********************************************************************/

/* 2.54.0-7 OCV come sea2_refresh2, ma tiene conto del frame buffer passato come argomento;
   se fb=-1 lavora come sea2_refresh2(),
   altrimenti lavora solo sul fb specificato; */

void
sea2_refresh3(
	short yi,
	short fb
)
{
	short lr2 = _param_scheda.lung_riga >> 1;
	short lc2 = _param_scheda.lung_colonna >> 1;
	short j, j1, j2;

	/* se devo fare tutti i fb lancio la vecchia routine */
	if(fb < 0) {
		sea2_refresh2(yi);
		return;
	}

	if(!_param_scheda.scheda_aperta)
		return;
	if(_zoom_mode) { /* zoom */
		if(yi < lc2) {
			j1 = 0;
			j2 = 1;
		} else {
			j1 = 2;
			j2 = 3;
		}
		for(j=j1; j<=j2; j++) {
			if(_fb_zoom[j] > -1 && _fb_zoom[j] == fb) { /* fb da vedere */
				short off_x = (short) ((j & 1) ? lr2 : 0);
				if(_roi_enable_display[_fb_zoom[j]]) { /* zoom con roi */
					short off_y = (short) ((j > 1) ? lc2 : 0);
					short yyi = (short) (_roi_fb[fb].yi >> 1) + off_y;
					short yyf = (short) (_roi_fb[fb].yf >> 1) + off_y;
					if(yi >= yyi && yi <= yyf) {
						short xxi = (short) (_roi_fb[fb].xi >> 1) + off_x;
						short xxf = (short) (_roi_fb[fb].xf >> 1) + off_x;
						BitBlt(
							_hdc_screen, xxi, yi,
							(int) (xxf-xxi+1), 1,
							_hdc_memory, xxi, yi, SRCCOPY
						);
					}
				} else { /* zoom senza roi */
					BitBlt(
						_hdc_screen, off_x, yi, lr2, 1,
						_hdc_memory, off_x, yi, SRCCOPY
					);
				}
			}
		}
	} else {  /* no zoom */
		if(_param_scheda.fb_display == fb) {
			if(_roi_enable_display[fb]) {
				/* no zoom con roi */
				if(yi >= _roi_fb[fb].yi && yi <= _roi_fb[fb].yf) {
					short xxi = _roi_fb[fb].xi;
					short xxf = _roi_fb[fb].xf;
					BitBlt(
						_hdc_screen, xxi, yi, (int) (xxf-xxi+1),
						1, _hdc_memory, xxi, yi, SRCCOPY
					);
				}
			} else { /* no zoom senza roi */
				BitBlt(
					_hdc_screen, 0, yi, (int) _param_scheda.lung_riga, 1,
					_hdc_memory, 0, yi, SRCCOPY
				);
			}
		}
	}
}
#endif/* __WATCOMC__ */

/***********************************************************************

;  Nome:    
	sea2_direct_vga_mode
;
;  Funzione:
     seleziona il modo di refresh del video "direct vga"
;
;  Formato:
	void sea2_direct_vga_mode(void)
;
;  Descrizione:
     {\bf sea2_direct_vga_mode}() seleziona il modo di refresh del
     video "direct vga";
;
;**********************************************************************/

void
sea2_direct_vga_mode(
	short enable /* 0: non direct, 1: direct */
)
{

#ifdef MET   
	if(_direct_vga_possible)
		_mode_direct_vga = enable;
	else
		_mode_direct_vga = 0; /* questo valore e' anche il default di init */
#endif

}

/******************************************************************************/

/* ritorna 1 se il direct vga e' abilitato */

short
sea2_is_direct_vga_enabled(
	void
)
{
#ifndef ASS
	return _mode_direct_vga;
#endif
	return 0;
}

/******************************************************************************/

/* setta il modo di zoom */

void
sea2_set_zoom_mode(
	short enable /* 0: non zoom, 1: zoom */
)
{
	_zoom_mode = enable;
}

/******************************************************************************/

/* setta i frame buffer da visualizzare in modo zoom */

void
sea2_display_zoom(
	short fb0,
	short fb1,
	short fb2,
	short fb3
)
{
	_fb_zoom[0] = fb0;
	_fb_zoom[1] = fb1;
	_fb_zoom[2] = fb2;
	_fb_zoom[3] = fb3;
}

/******************************************************************************/

/* disegna la porzione di bitmap definita da (xi, yi, x_size, y_size) sul
   device indicato da hdc a partire dalla posizione (x_dest, y_dest) */

#ifndef __WATCOMC__
void
sea2_draw_bitmap_to_dc(
	long hdc,
	short xi,
	short yi,
	short x_size,
	short y_size,
	short x_dest,
	short y_dest
)
{
	if(!_param_scheda.scheda_aperta)
		return;
	BitBlt(
		(HDC) hdc,
		x_dest, y_dest, x_size, y_size, _hdc_memory,
		xi+_param_scheda.pan_display,
		yi+_param_scheda.scroll_display,
		SRCCOPY
	);
}
#endif/* __WATCOMC__ */

/******************************************************************************/

/* setta il piano di colore da visualizzare */

void
sea2_display_piano_colore(
	short piano_colore
)
{
	if(_param_scheda.tipo_tv == TV_RGB)
		_piano_colore = piano_colore;
}

/******************************************************************************/

/* effettua lo stesso lavoro di sea2_copy_bitmap ma a partire da un fb a colori
   a 32 bit */

void
sea2_copy_bitmap_col(
	short flag_neri
)
{
	short i, j;
	unsigned char *ausp;
	short lr2 = (short) ((_param_scheda.lung_riga >> 1) * _n_byte_per_pixel);

	if(!_param_scheda.scheda_aperta)
		return;
	if(_param_scheda.tipo_tv != TV_RGB)
		return;
	if(_overlay_mode & UNDERLAY_MODE) {
		if(_zoom_mode) { /* zoom */
			for(j=0; j<4; j++) {
				long offset = (long) (j & 1) *
				              (long) (_param_scheda.lung_riga >> 1);
				if(j >= 2)
					offset += (long) (_param_scheda.lung_colonna >> 1) *
					          (long) _param_scheda.lung_riga;
				if(_fb_zoom[j] > -1) {
					if(_n_byte_per_pixel == 1) {
						/* leggo il piano di colore prescelto oppure il rosso */
						switch(_piano_colore) {
							/* l'acquisizione e il display sono BGRX */
							case 0: /* rosso */
								sea2_seleziona_buffer((short) ((_fb_zoom[j]<<2) + 2));
								break;
							case 1: /* verde */
								sea2_seleziona_buffer((short) ((_fb_zoom[j]<<2) + 1));
								break;
							case 2: /* blu */
								sea2_seleziona_buffer((short) (_fb_zoom[j]<<2));
								break;
							case 3: /* colore->rosso */
								sea2_seleziona_buffer((short) ((_fb_zoom[j]<<2) + 2));
								break;
						}
					} else {
						switch(_piano_colore) {
							/* l'acquisizione e il display sono BGRX */
							case 0: /* rosso */
								sea2_seleziona_buffer((short) ((_fb_zoom[j]<<2) + 2));
								break;
							case 1: /* verde */
								sea2_seleziona_buffer((short) ((_fb_zoom[j]<<2) + 1));
								break;
							case 2: /* blu */
								sea2_seleziona_buffer((short) (_fb_zoom[j]<<2));
								break;
							case 3: /* colore */
								sea2_seleziona_buffer(_fb_zoom[j]);
								break;
						}
					}
					if(_roi_enable_display_col[_fb_zoom[j]]) { /* zoom con roi */
						short fb = _fb_zoom[j];
						short xi = _roi_fb_col[fb].xi;
						short yi = _roi_fb_col[fb].yi;
						short xf = _roi_fb_col[fb].xf;
						short yf = _roi_fb_col[fb].yf;
						short xl = (short) (xf-xi+1);
						ausp = _aus_bitmap + (offset +
							(long) (yi>>1) * (long) _param_scheda.lung_riga +
							(long) (xi>>1)) * _n_byte_per_pixel;
						switch(_n_byte_per_pixel) {
							case 1:
								for(i=yi; i<=yf; i+=2, ausp += _n_byte_per_riga)
									sea2_sotto_c2_riga(xi, i, xl, ausp);
								break;
							case 2:
								if(_piano_colore == 3) {
									for(i=yi; i<=yf; i+=2, ausp += _n_byte_per_riga) {
										sea2_sotto_c2_riga_col16(xi, i, xl, ausp);
									}
								} else {
									for(i=yi; i<=yf; i+=2, ausp += _n_byte_per_riga) {
										sea2_sotto_c2_riga2(xi, i, xl, ausp);
									}
								}
								break;
							case 3:
								if(_piano_colore == 3) {
									for(i=yi; i<=yf; i+=2, ausp += _n_byte_per_riga) {
										sea2_sotto_c2_riga_col24(xi, i, xl, ausp);
									}
								} else {
									for(i=yi; i<=yf; i+=2, ausp += _n_byte_per_riga) {
										sea2_sotto_c2_riga3(xi, i, xl, ausp);
									}
								}
								break;
							case 4:
								if(_piano_colore == 3) {
									for(i=yi; i<=yf; i+=2, ausp += _n_byte_per_riga) {
										sea2_sotto_c2_riga_col(xi, i, xl, ausp);
									}
								} else {
									for(i=yi; i<=yf; i+=2, ausp += _n_byte_per_riga) {
										sea2_sotto_c2_riga4(xi, i, xl, ausp);
									}
								}
								break;
						}
					} else {  /* zoom senza roi */
						for(
							i=0, ausp = _aus_bitmap + (offset * _n_byte_per_pixel);
							i<_param_scheda.lung_colonna;
							i+=2, ausp += _n_byte_per_riga
						) {
							switch(_n_byte_per_pixel) {
								case 1:
									sea2_sotto_c2_riga(
										0, i, _param_scheda.lung_riga, ausp
									);
									break;
								case 2:
									if(_piano_colore == 3) {
										sea2_sotto_c2_riga_col16(
											0, i, _param_scheda.lung_riga, ausp
										);
									} else {
										sea2_sotto_c2_riga2(
											0, i, _param_scheda.lung_riga, ausp
										);
									}
									break;
								case 3:
									if(_piano_colore == 3) {
										sea2_sotto_c2_riga_col24(
											0, i, _param_scheda.lung_riga, ausp
										);
									} else {
										sea2_sotto_c2_riga3(
											0, i, _param_scheda.lung_riga, ausp
										);
									}
									break;
								case 4:
									if(_piano_colore == 3) {
										sea2_sotto_c2_riga_col(
											0, i, _param_scheda.lung_riga, ausp
										);
									} else {
										sea2_sotto_c2_riga4(
											0, i, _param_scheda.lung_riga, ausp
										);
									}
									break;
							}
						}
					}
				} else if(flag_neri) {
					for(
						i=0, ausp = _aus_bitmap +(offset * _n_byte_per_pixel);
						i<_param_scheda.lung_colonna;
						i+=2, ausp += _n_byte_per_riga
					)
						memset(ausp, 0, lr2);
				}
			}
		} else { /* no zoom */
			if(_n_byte_per_pixel == 1) {
				/* leggo il piano di colore prescelto oppure il rosso */
				switch(_piano_colore) {
					/* l'acquisizione e il display sono BGRX */
					case 0: /* rosso */
						sea2_seleziona_buffer(
							(short) ((_param_scheda.fb_display<<2) + 2)
						);
						break;
					case 1: /* verde */
						sea2_seleziona_buffer(
							(short) ((_param_scheda.fb_display<<2) + 1)
						);
						break;
					case 2: /* blu */
						sea2_seleziona_buffer((short) (_param_scheda.fb_display<<2));
						break;
					case 3: /* colore->rosso */
						sea2_seleziona_buffer(
							(short) ((_param_scheda.fb_display<<2) + 2)
						);
						break;
				}
			} else {
				switch(_piano_colore) {
					/* l'acquisizione e il display sono BGRX */
					case 0: /* rosso */
						sea2_seleziona_buffer(
							(short) ((_param_scheda.fb_display<<2) + 2)
						);
						break;
					case 1: /* verde */
						sea2_seleziona_buffer(
							(short) ((_param_scheda.fb_display<<2) + 1)
						);
						break;
					case 2: /* blu */
						sea2_seleziona_buffer((short) (_param_scheda.fb_display<<2));
						break;
					case 3: /* colore */
						sea2_seleziona_buffer(_param_scheda.fb_display);
						break;
				}
			}
			if(_roi_enable_display_col[_param_scheda.fb_display]) {/* no zoom con roi */
				short fb = _param_scheda.fb_display;
				short xi = _roi_fb_col[fb].xi;
				short yi = _roi_fb_col[fb].yi;
				short xf = _roi_fb_col[fb].xf;
				short yf = _roi_fb_col[fb].yf;
				short xl = (short) (xf-xi+1);
				ausp = _aus_bitmap + ((long) xi +
					(long) yi * (long) _param_scheda.lung_riga) * _n_byte_per_pixel;
				switch(_n_byte_per_pixel) {
					case 1:
						for(i=yi; i<=yf; i++, ausp += _n_byte_per_riga)
							sea2_leggi_riga(xi, i, xl, ausp);
						break;
					case 2:
						if(_piano_colore == 3)
							for(i=yi; i<=yf; i++, ausp += _n_byte_per_riga)
								sea2_leggi_riga_col16(xi, i, xl, ausp);
						else
							for(i=yi; i<=yf; i++, ausp += _n_byte_per_riga)
								sea2_leggi_riga2(xi, i, xl, ausp);
						break;
					case 3:
						if(_piano_colore == 3)
							for(i=yi; i<=yf; i++, ausp += _n_byte_per_riga)
								sea2_leggi_riga_col24(xi, i, xl, ausp);
						else
							for(i=yi; i<=yf; i++, ausp += _n_byte_per_riga)
								sea2_leggi_riga3(xi, i, xl, ausp);
						break;
					case 4:
						if(_piano_colore == 3)
							for(i=yi; i<=yf; i++, ausp += _n_byte_per_riga)
								sea2_leggi_riga_col(xi, i, xl, ausp);
						else
							for(i=yi; i<=yf; i++, ausp += _n_byte_per_riga)
								sea2_leggi_riga4(xi, i, xl, ausp);
						break;
				}
			} else { /* no zoom senza roi */
				for(
					i=0, ausp = _aus_bitmap;
					i<_param_scheda.lung_colonna;
					i++, ausp += _n_byte_per_riga
				) {
					switch(_n_byte_per_pixel) {
						case 1:
							sea2_leggi_riga(0, i, _param_scheda.lung_riga, ausp);
							break;
						case 2:
							if(_piano_colore == 3)
								sea2_leggi_riga_col16(
									0, i, _param_scheda.lung_riga, ausp
								);
							else
								sea2_leggi_riga2(
									0, i, _param_scheda.lung_riga, ausp
								);
							break;
						case 3:
							if(_piano_colore == 3)
								sea2_leggi_riga_col24(
									0, i, _param_scheda.lung_riga, ausp
								);
							else
								sea2_leggi_riga3(
									0, i, _param_scheda.lung_riga, ausp
								);
							break;
						case 4:
							if(_piano_colore == 3)
								sea2_leggi_riga_col(
									0, i, _param_scheda.lung_riga, ausp
								);
							else
								sea2_leggi_riga4(
									0, i, _param_scheda.lung_riga, ausp
								);
							break;
					}
				}
			}
		}
#ifndef __WATCOMC__
		SetBitmapBits(
			_bitmap, _n_byte_per_riga * (long) _param_scheda.lung_colonna,
			_aus_bitmap
		);
#else
		memcpy(
			_bitmap+6, _aus_bitmap,
			_n_byte_per_riga * (long) _param_scheda.lung_colonna
		);
#endif
	} else { /* faccio il brush sulla bitmap nelle zone opportune */
		brush_opportuno(flag_neri);
	}
	
	/* eventualmente disegno sulla bitmap */
	if(_overlay_mode & OVERLAY_MODE) {
		disegno_opportuno();
	}
}

/************************************************************************/

/* effettua lo stesso lavoro di sea2_copy_bitmap2(), ma trascrive solo una riga
   sull'array _aus_bitmap, quella passata come argomento;
   se flag_copy vale 1 copia tutto _aus_bitmap sulla bitmap;
   se flag_copy vale 2 copia sulla bitmap solo la riga yi e non si fa il
   disegno opportuno dell'overlay;
*/

#ifndef __WATCOMC__
void
sea2_copy_bitmap2_col(
	short flag_neri,
	short yi,
	short flag_copy
)
{
	short i, j, copia=0;
	unsigned char *ausp, *ausp1;
	long nbpp;
	short lr2, j1, j2, lc2 = _param_scheda.lung_colonna >> 1;
	

	if(!_param_scheda.scheda_aperta)
		return;
	if(_param_scheda.tipo_tv != TV_RGB)
		return;
	/* quando flag copy vale 2 e la vga e' settata a 32 bit dobbiamo creare
	   un array a 24 bit da dare a SetDIBits; pertanto in aus_bitmap metto
	   i pixel come se aus_bitmap fosse a 24 bit */
	if(flag_copy == 2 && _n_byte_per_pixel == 4)
		nbpp = 3;
	else
		nbpp = _n_byte_per_pixel;
	lr2 = (short) ((_param_scheda.lung_riga >> 1) * nbpp);

	if(_overlay_mode & UNDERLAY_MODE) { /* se e' abilitato l'underlay */
		if(flag_copy == 2)
			ausp = buf_dib;
		else
			/* ausp e' l'indirizzo del primo byte della riga di aus_bitmap */
			ausp = _aus_bitmap + _offset_riga_display[yi];
		if(_zoom_mode) { /* zoom */
			if(yi < lc2) {
				j1 = 0;
				j2 = 1;
				i = yi << 1; /* ordinata della riga nell'immagine sorgente */
			} else {
				j1 = 2;
				j2 = 3;
				i = (yi-lc2) << 1;
			}
			for(j=j1; j<=j2; j++) {
				ausp1 = ausp;
				if(j & 1) {
					ausp1 += (long) lr2;
				}
				if(_fb_zoom[j] > -1) { /* fb da presentare */
					if(_n_byte_per_pixel == 1) {
						/* leggo il piano di colore prescelto oppure il rosso */
						switch(_piano_colore) {
							/* l'acquisizione e il display sono BGRX */
							case 0: /* rosso */
								sea2_seleziona_buffer(
									(short) ((_fb_zoom[j]<<2) + 2)
								);
								break;
							case 1: /* verde */
								sea2_seleziona_buffer(
									(short) ((_fb_zoom[j]<<2) + 1)
								);
								break;
							case 2: /* blu */
								sea2_seleziona_buffer((short) (_fb_zoom[j]<<2));
								break;
							case 3: /* colore->rosso */
								sea2_seleziona_buffer(
									(short) ((_fb_zoom[j]<<2) + 2)
								);
								break;
						}
					} else {
						switch(_piano_colore) {
							/* l'acquisizione e il display sono BGRX */
							case 0: /* rosso */
								sea2_seleziona_buffer(
									(short) ((_fb_zoom[j]<<2) + 2)
								);
								break;
							case 1: /* verde */
								sea2_seleziona_buffer(
									(short) ((_fb_zoom[j]<<2) + 1)
								);
								break;
							case 2: /* blu */
								sea2_seleziona_buffer((short) (_fb_zoom[j]<<2));
								break;
							case 3: /* colore */
								sea2_seleziona_buffer(_fb_zoom[j]);
								break;
						}
					}
					if(_roi_enable_display_col[_fb_zoom[j]]) { /* zoom con roi */
						short fb = _fb_zoom[j];
						short xi = _roi_fb_col[fb].xi;
						short yyi = _roi_fb_col[fb].yi;
						short xf = _roi_fb_col[fb].xf;
						short yf = _roi_fb_col[fb].yf;
						short xl = (short) (xf-xi+1);
						ausp1 += ((long) (xi>>1) * nbpp);
						if(i >= yyi && i <= yf) {
							copia = 1;
							switch(_n_byte_per_pixel) {
								case 1:
									sea2_sotto_c2_riga(
										xi, i, xl, ausp1
									);
									break;
								case 2:
									if(_piano_colore == 3)
										sea2_sotto_c2_riga_col16(
											xi, i, xl, ausp1
										);
									else
										sea2_sotto_c2_riga2(
											xi, i, xl, ausp1
										);
									break;
								case 3:
									if(_piano_colore == 3)
										sea2_sotto_c2_riga_col24(
											xi, i, xl, ausp1
										);
									else
										sea2_sotto_c2_riga3(
											xi, i, xl, ausp1
										);
									break;
								case 4:
									if(_piano_colore == 3) {
										if(flag_copy == 2) /* leggo come a 24 bit */
											sea2_sotto_c2_riga_col24(
												xi, i, xl, ausp1
											);
										else
											sea2_sotto_c2_riga_col(
												xi, i, xl, ausp1
											);
									} else {
										if(flag_copy == 2) /* leggo come a 24 bit */
											sea2_sotto_c2_riga3(
												xi, i, xl, ausp1
											);
										else
											sea2_sotto_c2_riga4(
												xi, i, xl, ausp1
											);
									}
									break;
							}
						}
					} else {  /* zoom senza roi */
						copia = 1;
						switch(_n_byte_per_pixel) {
							case 1:
								sea2_sotto_c2_riga(
									0, i, _param_scheda.lung_riga, ausp1
								);
								break;
							case 2:
								if(_piano_colore == 3)
									sea2_sotto_c2_riga_col16(
										0, i, _param_scheda.lung_riga, ausp1
									);
								else
									sea2_sotto_c2_riga2(
										0, i, _param_scheda.lung_riga, ausp1
									);
								break;
							case 3:
								if(_piano_colore == 3)
									sea2_sotto_c2_riga_col24(
										0, i, _param_scheda.lung_riga, ausp1
									);
								else
									sea2_sotto_c2_riga3(
										0, i, _param_scheda.lung_riga, ausp1
									);
								break;
							case 4:
								if(_piano_colore == 3) {
									if(flag_copy == 2) /* leggo come a 24 bit */
										sea2_sotto_c2_riga_col24(
											0, i, _param_scheda.lung_riga, ausp1
										);
									else
										sea2_sotto_c2_riga_col(
											0, i, _param_scheda.lung_riga, ausp1
										);
								} else {
									if(flag_copy == 2) /* leggo come a 24 bit */
										sea2_sotto_c2_riga3(
											0, i, _param_scheda.lung_riga, ausp1
										);
									else
										sea2_sotto_c2_riga4(
											0, i, _param_scheda.lung_riga, ausp1
										);
								}
								break;
						}
					}
				} else { /* fb da non presentare */
					if(flag_neri) {
						copia = 1;
						memset(ausp1, 0, lr2);
					}
				}
			}
		} else { /* no zoom */
			if(_n_byte_per_pixel == 1) {
				/* leggo il piano di colore prescelto oppure il rosso */
				switch(_piano_colore) {
					/* l'acquisizione e il display sono BGRX */
					case 0: /* rosso */
						sea2_seleziona_buffer(
							(short) ((_param_scheda.fb_display<<2) + 2)
						);
						break;
					case 1: /* verde */
						sea2_seleziona_buffer(
							(short) ((_param_scheda.fb_display<<2) + 1)
						);
						break;
					case 2: /* blu */
						sea2_seleziona_buffer((short) (_param_scheda.fb_display<<2));
						break;
					case 3: /* colore->rosso */
						sea2_seleziona_buffer(
							(short) ((_param_scheda.fb_display<<2) + 2)
						);
						break;
				}
			} else {
				switch(_piano_colore) {
					/* l'acquisizione e il display sono BGRX */
					case 0: /* rosso */
						sea2_seleziona_buffer(
							(short) ((_param_scheda.fb_display<<2) + 2)
						);
						break;
					case 1: /* verde */
						sea2_seleziona_buffer(
							(short) ((_param_scheda.fb_display<<2) + 1)
						);
						break;
					case 2: /* blu */
						sea2_seleziona_buffer((short) (_param_scheda.fb_display<<2));
						break;
					case 3: /* colore */
						sea2_seleziona_buffer(_param_scheda.fb_display);
						break;
				}
			}
			if(_roi_enable_display_col[_param_scheda.fb_display]) {/* no zoom con roi */
				short fb = _param_scheda.fb_display;
				short xi = _roi_fb_col[fb].xi;
				short yyi = _roi_fb_col[fb].yi;
				short xf = _roi_fb_col[fb].xf;
				short yf = _roi_fb_col[fb].yf;
				short xl = (short) (xf-xi+1);
				ausp1 = ausp + (xi*nbpp);
				if(yi >= yyi && yi <= yf) {
					copia = 1;
					switch(_n_byte_per_pixel) {
						case 1:
							sea2_leggi_riga(xi, yi, xl, ausp1);
							break;
						case 2:
							if(_piano_colore == 3)
								sea2_leggi_riga_col16(
									xi, yi, xl, ausp1
								);
							else
								sea2_leggi_riga2(
									xi, yi, xl, ausp1
								);
							break;
						case 3:
							if(_piano_colore == 3)
								sea2_leggi_riga_col24(
									xi, yi, xl, ausp1
								);
							else
								sea2_leggi_riga3(
									xi, yi, xl, ausp1
								);
							break;
						case 4:
							if(_piano_colore == 3) {
								if(flag_copy == 2) /* leggo come a 24 bit */
									sea2_leggi_riga_col24(
										xi, yi, xl, ausp1
									);
								else
									sea2_leggi_riga_col(
										xi, yi, xl, ausp1
									);
							} else {
								if(flag_copy == 2) /* leggo come a 24 bit */
									sea2_leggi_riga3(
										xi, yi, xl, ausp1
									);
								else
									sea2_leggi_riga4(
										xi, yi, xl, ausp1
									);
							}
							break;
					}
				}
			} else {  /* no zoom senza roi */ 
				copia = 1;
				switch(_n_byte_per_pixel) {
					case 1:
						sea2_leggi_riga(0, yi, _param_scheda.lung_riga, ausp);
						break;
					case 2:
						if(_piano_colore == 3)
							sea2_leggi_riga_col16(0, yi, _param_scheda.lung_riga, ausp);
						else
							sea2_leggi_riga2(0, yi, _param_scheda.lung_riga, ausp);
						break;
					case 3:
						if(_piano_colore == 3)
							sea2_leggi_riga_col24(0, yi, _param_scheda.lung_riga, ausp);
						else
							sea2_leggi_riga3(0, yi, _param_scheda.lung_riga, ausp);
						break;
					case 4:
						if(_piano_colore == 3) {
							if(flag_copy == 2) /* leggo come a 24 bit */
								sea2_leggi_riga_col24(0, yi, _param_scheda.lung_riga, ausp);
							else
								sea2_leggi_riga_col(0, yi, _param_scheda.lung_riga, ausp);
						} else {
							if(flag_copy == 2) /* leggo come a 24 bit */
								sea2_leggi_riga3(0, yi, _param_scheda.lung_riga, ausp);
							else
								sea2_leggi_riga4(0, yi, _param_scheda.lung_riga, ausp);
						}
						break;
				}
			}
		}
		if(flag_copy == 1) {
			SetBitmapBits(
				_bitmap,
				_n_byte_per_riga * (long) _param_scheda.lung_colonna, _aus_bitmap
			);
		} else if(flag_copy == 2 && copia) {  /* copio la riga yi sulla bitmap */
			if(_n_byte_per_pixel >= 3) {
				/* vado direttamente sulla bitmap usando SetDIBits() */
				copia_riga_su_bitmap(yi, /*ausp*/buf_dib);
			} else {
				/* faccio il doppio passaggio per la bitmap intermedia _bitmap_row */
				SetBitmapBits(_bitmap_row, _n_byte_per_riga, /*ausp*/buf_dib);
				BitBlt(
					_hdc_memory, 0, yi, (int) _param_scheda.lung_riga, 1,
					_hdc_memory_row, 0, 0, SRCCOPY
				);
			}
		}
	} else { /* l'underlay non e' abilitato */
		if(flag_copy == 1) {
			/* faccio il brush sulla bitmap nelle zone opportune */ 
			brush_opportuno(flag_neri);
		} else if(flag_copy == 2) {
			/* faccio il brush sulla bitmap solo sulla riga yi */
			SelectObject(_hdc_memory, _penna_nera);
			Rectangle(
				_hdc_memory, _param_scheda.x_min, yi,
				_param_scheda.x_max+1, yi+1
			);
		}
	}

	if(flag_copy == 1) {
		/* eventualmente disegno sulla bitmap */
		if(_overlay_mode & OVERLAY_MODE) {
			disegno_opportuno();
		}
	}
}

/************************************************************************/

/* 2.54.0-7 effettua lo stesso lavoro di sea2_copy_bitmap2_col(), ma tiene conto del frame
   buffer passato come argomento; se fb=-1 lavora come sea2_copy_bitmap2_col(),
   altrimenti lavora solo sul fb specificato;
   inoltre non gestisce flag_neri e il flag_copy=1 */

void
sea2_copy_bitmap3_col(
	short flag_neri,
	short riga,
	short flag_copy,
	short fb
)
{
	short i, j, j1, j2, copia=0;
	unsigned char *ausp, *ausp1;
	short n_pixel_mezza_riga;
	short n_pixel_mezza_colonna;
	short n_byte_mezza_riga; /* dipende da _n_byte_per_pixel della vga */
	short xi_ab; /* ascissa di scrittura su _aus_bitmap (l'ordinata e' riga) */
	short xl_ab; /* numero di pixel da scrivere su _aus_bitmap */
	short xi, yi, xf, yf, xl; /* coordinate della roi */

	/* se devo fare tutti i fb lancio la vecchia routine */
	if(fb < 0) {
		sea2_copy_bitmap2_col(flag_neri, riga, flag_copy);
		return;
	}

	if(!_param_scheda.scheda_aperta)
		return;
	if(_param_scheda.tipo_tv != TV_RGB)
		return;

	n_pixel_mezza_riga = (short) (_param_scheda.lung_riga >> 1);
	n_pixel_mezza_colonna = _param_scheda.lung_colonna >> 1;
	n_byte_mezza_riga = (short) (n_pixel_mezza_riga * _n_byte_per_pixel);

	if(_overlay_mode & UNDERLAY_MODE) { /* se e' abilitato l'underlay */
		/* ausp e' l'indirizzo del primo byte della riga di aus_bitmap */
		ausp = _aus_bitmap + _offset_riga_display[riga];

		if(_zoom_mode) { /* zoom */
			if(riga < n_pixel_mezza_colonna) {
				j1 = 0;
				j2 = 1;
				i = riga << 1; /* ordinata della riga nell'immagine sorgente */
			} else {
				j1 = 2;
				j2 = 3;
				i = (riga-n_pixel_mezza_colonna) << 1;
			}
			for(j=j1; j<=j2; j++) {
				if(_fb_zoom[j] > -1 && _fb_zoom[j] == fb) { /* fb da presentare */
					/* ausp1 e' l'indirizzo del primo byte su cui si va a scrivere */
					ausp1 = ausp;
					if(j & 1)
						ausp1 += (long) n_byte_mezza_riga;
					if(_n_byte_per_pixel == 1) {
						/* leggo il piano di colore prescelto oppure il rosso */
						switch(_piano_colore) {
							/* l'acquisizione e il display sono BGRX */
							case 0: /* rosso */
								sea2_seleziona_buffer(
									(short) ((fb<<2) + 2)
								);
								break;
							case 1: /* verde */
								sea2_seleziona_buffer(
									(short) ((fb<<2) + 1)
								);
								break;
							case 2: /* blu */
								sea2_seleziona_buffer((short) (fb<<2));
								break;
							case 3: /* colore->rosso */
								sea2_seleziona_buffer(
									(short) ((fb<<2) + 2)
								);
								break;
						}
					} else {
						switch(_piano_colore) {
							/* l'acquisizione e il display sono BGRX */
							case 0: /* rosso */
								sea2_seleziona_buffer(
									(short) ((fb<<2) + 2)
								);
								break;
							case 1: /* verde */
								sea2_seleziona_buffer(
									(short) ((fb<<2) + 1)
								);
								break;
							case 2: /* blu */
								sea2_seleziona_buffer((short) (fb<<2));
								break;
							case 3: /* colore */
								sea2_seleziona_buffer(fb);
								break;
						}
					}
					if(_roi_enable_display_col[fb]) { /* zoom con roi */
						xi = _roi_fb_col[fb].xi;
						yi = _roi_fb_col[fb].yi;
						xf = _roi_fb_col[fb].xf;
						yf = _roi_fb_col[fb].yf;
						xl = (short) (xf-xi+1);
						xl_ab = xl >> 1;
						xi_ab = xi >> 1;
						if(j & 1)
							xi_ab += n_pixel_mezza_riga;
						ausp1 += ((long) (xi>>1) * _n_byte_per_pixel);
						if(i >= yi && i <= yf) {
							copia = 1;
							switch(_n_byte_per_pixel) {
								case 1:
									sea2_sotto_c2_riga(xi, i, xl, ausp1);
									break;
								case 2:
									if(_piano_colore == 3)
										sea2_sotto_c2_riga_col16(xi, i, xl, ausp1);
									else
										sea2_sotto_c2_riga2(xi, i, xl, ausp1);
									break;
								case 3:
									if(_piano_colore == 3)
										sea2_sotto_c2_riga_col24(xi, i, xl, ausp1);
									else
										sea2_sotto_c2_riga3(xi, i, xl, ausp1);
									break;
								case 4:
									if(_piano_colore == 3)
										sea2_sotto_c2_riga_col(xi, i, xl, ausp1);
									else
										sea2_sotto_c2_riga4(xi, i, xl, ausp1);
									break;
							}
						}
					} else {  /* zoom senza roi */
						copia = 1;
						xi = 0;
						xl = _param_scheda.lung_riga;
						xl_ab = xl >> 1;
						xi_ab = xi >> 1;
						if(j & 1)
							xi_ab += n_pixel_mezza_riga;
						switch(_n_byte_per_pixel) {
							case 1:
								sea2_sotto_c2_riga(xi, i, xl, ausp1);
								break;
							case 2:
								if(_piano_colore == 3)
									sea2_sotto_c2_riga_col16(xi, i, xl, ausp1);
								else
									sea2_sotto_c2_riga2(xi, i, xl, ausp1);
								break;
							case 3:
								if(_piano_colore == 3)
									sea2_sotto_c2_riga_col24(xi, i, xl, ausp1);
								else
									sea2_sotto_c2_riga3(xi, i, xl, ausp1);
								break;
							case 4:
								if(_piano_colore == 3)
									sea2_sotto_c2_riga_col(xi, i, xl, ausp1);
								else
									sea2_sotto_c2_riga4(xi, i, xl, ausp1);
								break;
						}
					}
				}
			}
		} else { /* no zoom */
			if(fb == _param_scheda.fb_display) {
				if(_n_byte_per_pixel == 1) {
					/* leggo il piano di colore prescelto oppure il rosso */
					switch(_piano_colore) {
						/* l'acquisizione e il display sono BGRX */
						case 0: /* rosso */
							sea2_seleziona_buffer((short) ((fb<<2) + 2));
							break;
						case 1: /* verde */
							sea2_seleziona_buffer((short) ((fb<<2) + 1));
							break;
						case 2: /* blu */
							sea2_seleziona_buffer((short) (fb<<2));
							break;
						case 3: /* colore->rosso */
							sea2_seleziona_buffer((short) ((fb<<2) + 2));
							break;
					}
				} else {
					switch(_piano_colore) {
						/* l'acquisizione e il display sono BGRX */
						case 0: /* rosso */
							sea2_seleziona_buffer((short) ((fb<<2) + 2));
							break;
						case 1: /* verde */
							sea2_seleziona_buffer((short) ((fb<<2) + 1));
							break;
						case 2: /* blu */
							sea2_seleziona_buffer((short) (fb<<2));
							break;
						case 3: /* colore */
							sea2_seleziona_buffer(fb);
							break;
					}
				}
				if(_roi_enable_display_col[fb]) {/* no zoom con roi */
					xi = _roi_fb_col[fb].xi;
					yi = _roi_fb_col[fb].yi;
					xf = _roi_fb_col[fb].xf;
					yf = _roi_fb_col[fb].yf;
					xl = (short) (xf-xi+1);
					ausp1 = ausp + (xi*_n_byte_per_pixel);
					xl_ab = xl;
					xi_ab = xi;
					if(riga >= yi && riga <= yf) {
						copia = 1;
						switch(_n_byte_per_pixel) {
							case 1:
								sea2_leggi_riga(xi, riga, xl, ausp1);
								break;
							case 2:
								if(_piano_colore == 3)
									sea2_leggi_riga_col16(xi, riga, xl, ausp1);
								else
									sea2_leggi_riga2(xi, riga, xl, ausp1);
								break;
							case 3:
								if(_piano_colore == 3)
									sea2_leggi_riga_col24(xi, riga, xl, ausp1);
								else
									sea2_leggi_riga3(xi, riga, xl, ausp1);
								break;
							case 4:
								if(_piano_colore == 3)
									sea2_leggi_riga_col(xi, riga, xl, ausp1);
								else
									sea2_leggi_riga4(xi, riga, xl, ausp1);
								break;
						}
					}
				} else {  /* no zoom senza roi */ 
					ausp1 = ausp;
					copia = 1;
					xi = 0;
					xl = _param_scheda.lung_riga;
					xl_ab = xl;
					xi_ab = xi;
					switch(_n_byte_per_pixel) {
						case 1:
							sea2_leggi_riga(xi, riga, xl, ausp);
							break;
						case 2:
							if(_piano_colore == 3)
								sea2_leggi_riga_col16(xi, riga, xl, ausp);
							else
								sea2_leggi_riga2(xi, riga, xl, ausp);
							break;
						case 3:
							if(_piano_colore == 3)
								sea2_leggi_riga_col24(xi, riga, xl, ausp);
							else
								sea2_leggi_riga3(xi, riga, xl, ausp);
							break;
						case 4:
							if(_piano_colore == 3)
								sea2_leggi_riga_col(xi, riga, xl, ausp);
							else
								sea2_leggi_riga4(xi, riga, xl, ausp);
							break;
					}
				}
			}
		}
		if(flag_copy == 2 && copia) {  /* copio la riga sulla bitmap */
			/* faccio il doppio passaggio per la bitmap intermedia _bitmap_row */
			SetBitmapBits(_bitmap_row, xl_ab * _n_byte_per_pixel, ausp1);
			BitBlt(
				_hdc_memory, xi_ab, riga, xl_ab, 1,
				_hdc_memory_row, 0, 0, SRCCOPY
			);
		}
	}
}

/***********************************************************************/

static void		
copia_riga_su_bitmap(
	short yi, /* indice della riga da copiare */
	unsigned char *buf_dib
)
{
	BITMAPINFO gInfo;

	if(!_param_scheda.scheda_aperta)
		return;
	gInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	gInfo.bmiHeader.biWidth = _param_scheda.lung_riga;
	gInfo.bmiHeader.biHeight = _param_scheda.lung_colonna;
	gInfo.bmiHeader.biPlanes = 1;
	gInfo.bmiHeader.biBitCount = 24;
	gInfo.bmiHeader.biCompression = BI_RGB;
	gInfo.bmiHeader.biSizeImage = 0;
	gInfo.bmiHeader.biXPelsPerMeter = 0;
	gInfo.bmiHeader.biYPelsPerMeter = 0;
	gInfo.bmiHeader.biClrUsed = 0;
	gInfo.bmiHeader.biClrImportant = 0;
	SelectObject(_hdc_memory, _old_bitmap);
	/* per le DIB l'origine delle coordinate e' in basso a sx, per cui le
	   y sono invertite */
	SetDIBits(
		_hdc_memory, _bitmap, _param_scheda.lung_colonna-1-yi, 1,
		buf_dib, &gInfo, DIB_RGB_COLORS
	);
	SelectObject(_hdc_memory, _bitmap);
}

/***********************************************************************/

/* copia un segmento sull'array ausiliario _aus_bitmap;
   se flag_copy vale 1 copia tutto _aus_bitmap sulla bitmap;
*/

void
sea2_copy_bitmap_row_col(
	short xi,       /* coordinate iniziali del segmento da copiare */
	short yi,
	short n_pixel,  /* numero di pixel da copiare da buffer */
	unsigned char *buffer,
	short flag_copy
)
{
	short i;
	unsigned char *ausmem;
	unsigned short *ausmem_s;

	if(!_param_scheda.scheda_aperta)
		return;
	if(_param_scheda.tipo_tv != TV_RGB)
		return;
	ausmem = _aus_bitmap + _offset_colonna_display[xi] + _offset_riga_display[yi];
	ausmem_s =  (unsigned short *) ausmem;
	if(_n_byte_per_pixel == 1) {
		if(_piano_colore == 0)
			buffer += 2; /* per beccare il rosso */
		else if(_piano_colore == 1)
			buffer += 1; /* per beccare il verde */
		else if(_piano_colore == 2)
			buffer += 0; /* per beccare il blu */
		else
			buffer += 2; /* per beccare il rosso */
	} else {
		if(_piano_colore == 0)
			buffer += 2; /* per beccare il rosso */
		else if(_piano_colore == 1)
			buffer += 1; /* per beccare il verde */
		else if(_piano_colore == 2)
			buffer += 0; /* per beccare il blu */
	}
	switch(_n_byte_per_pixel) {
		case 1:
			for(i=n_pixel; i>0; i--) {
				*ausmem++ = *buffer;
				buffer+=4;
			}
			break;
		case 2:
			if(_piano_colore == 3) {
				if(_n_bit_col16 == 15) {
					for(i=n_pixel; i>0; i--) {
						*ausmem_s++ = SHORT_15(*(buffer+2), *(buffer+1), *buffer);
						buffer += 4;
					}
				} else {
					for(i=n_pixel; i>0; i--) {
						*ausmem_s++ = SHORT_16(*(buffer+2), *(buffer+1), *buffer);
						buffer += 4;
					}
				}
			} else {
				if(_n_bit_col16 == 15) {
					for(i=n_pixel; i>0; i--) {
						*ausmem_s++ = SHORT_15(*buffer, *buffer, *buffer);
						buffer += 4;
					}
				} else {
					for(i=n_pixel; i>0; i--) {
						*ausmem_s++ = SHORT_16(*buffer, *buffer, *buffer);
						buffer += 4;
					}
				}
			}
			break;
		case 3:
			if(_piano_colore == 3) {
				for(i=n_pixel; i>0; i--) {
					*ausmem++ = *buffer++;
					*ausmem++ = *buffer++;
					*ausmem++ = *buffer++;
					buffer++;
				}
			} else {
				for(i=n_pixel; i>0; i--) {
					*ausmem++ = *buffer;
					*ausmem++ = *buffer;
					*ausmem++ = *buffer;
					buffer += 4;
				}
			}
			break;
		case 4:
			if(_piano_colore == 3) {
				memcpy(ausmem, buffer, (size_t) (n_pixel * _n_byte_per_pixel));
			} else {
				for(i=n_pixel; i>0; i--) {
					*ausmem++ = *buffer;
					*ausmem++ = *buffer;
					*ausmem++ = *buffer;
					*ausmem++ = *buffer;
					buffer += 4;
				}
			}
			break;
	}
	if(flag_copy == 1) {
		SetBitmapBits(
			_bitmap,
			_n_byte_per_riga * (long) _param_scheda.lung_colonna, _aus_bitmap
		);
	}
}
#endif /* __WATCOMC__ */

/***********************************************************************/

/* come sea2_refresh, da usare quando il fb sorgente del display e'
   a colori (come tutte le altre routine _col) */

void
sea2_refresh_col(
	short xi,
	short yi,
	short xf,
	short yf
)
{
	short j;   

	if(!_param_scheda.scheda_aperta)
		return;
	if(_param_scheda.tipo_tv != TV_RGB)
		return;
#ifndef __WATCOMC__
	if(!xi && !yi && !xf && !yf) { /* schermo non parzializzato */
		if(_zoom_mode) { /* zoom */
			for(j=0; j<4; j++) {
				short off_x=0, off_y=0;
				if(j & 1)
					off_x = (short) (_param_scheda.lung_riga >> 1);
				if(j >= 2)
					off_y = (short) (_param_scheda.lung_colonna >> 1);
				if(_fb_zoom[j] > -1) { /* fb da vedere */
					if(_roi_enable_display_col[_fb_zoom[j]]) { /* zoom con roi */
						short fb = _fb_zoom[j];
						short xxi = (short) (_roi_fb_col[fb].xi >> 1) + off_x;
						short yyi = (short) (_roi_fb_col[fb].yi >> 1) + off_y;
						short xxf = (short) (_roi_fb_col[fb].xf >> 1) + off_x;
						short yyf = (short) (_roi_fb_col[fb].yf >> 1) + off_y;
						BitBlt(
							_hdc_screen, xxi, yyi,
							(int) (xxf-xxi+1), (int) (yyf-yyi+1),
							_hdc_memory, xxi, yyi, SRCCOPY
						);
					} else { /* zoom senza roi */
						BitBlt(
							_hdc_screen, off_x, off_y,
							(int) (_param_scheda.lung_riga >> 1),
							(int) (_param_scheda.lung_colonna >> 1),
							_hdc_memory, off_x, off_y, SRCCOPY
						);
					}
				}
			}
		} else {  /* no zoom */
			if(_roi_enable_display_col[_param_scheda.fb_display]) {
				/* no zoom con roi */
				short fb = _param_scheda.fb_display;
				short xxi = _roi_fb_col[fb].xi;
				short yyi = _roi_fb_col[fb].yi;
				short xxf = _roi_fb_col[fb].xf;
				short yyf = _roi_fb_col[fb].yf;
				BitBlt(
					_hdc_screen, xxi, yyi, (int) (xxf-xxi+1),
					(int) (yyf-yyi+1), _hdc_memory, xxi, yyi, SRCCOPY
				);
			} else { /* no zoom senza roi */
				BitBlt(
					_hdc_screen, 0, 0, (int) _param_scheda.lung_riga,
					(int) _param_scheda.lung_colonna, _hdc_memory,
					0, 0, SRCCOPY
				);
			}
		}
	} else { /* schermo parzializzato */
		if(xi < _param_scheda.x_min)
			xi = _param_scheda.x_min;
		if(yi < _param_scheda.y_min)
			yi = _param_scheda.y_min;
		if(xf > _param_scheda.x_max)
			xf = _param_scheda.x_max;
		if(yf > _param_scheda.y_max)
			yf = _param_scheda.y_max;
		BitBlt(
			_hdc_screen, xi, yi, (int) (xf-xi+1), (int) (yf-yi+1),
			_hdc_memory, xi, yi, SRCCOPY
		);
	}
#else
	_putimage(0, 0, (char *) _bitmap, _GPSET);
#endif
}

/***********************************************************************/

/* come sea2_refresh_col, ma porta sulla VGA solo la porzione opportuna della
   riga passata come argomento */

#ifndef __WATCOMC__
void
sea2_refresh2_col(
	short yi
)
{
	short j;   
	short lr2 = _param_scheda.lung_riga >> 1;
	short lc2 = _param_scheda.lung_colonna >> 1;
	short j1, j2;

	if(!_param_scheda.scheda_aperta)
		return;
	if(_param_scheda.tipo_tv != TV_RGB)
		return;
	if(_zoom_mode) { /* zoom */
		if(yi < lc2) {
			j1 = 0;
			j2 = 1;
		} else {
			j1 = 2;
			j2 = 3;
		}
		for(j=j1; j<=j2; j++) {
			if(_fb_zoom[j] > -1) { /* fb da vedere */
				short off_x = (short) ((j & 1) ? lr2 : 0);
				if(_roi_enable_display_col[_fb_zoom[j]]) { /* zoom con roi */
					short off_y = (short) ((j > 1) ? lc2 : 0);
					short fb = _fb_zoom[j];
					short yyi = (short) (_roi_fb_col[fb].yi >> 1) + off_y;
					short yyf = (short) (_roi_fb_col[fb].yf >> 1) + off_y;
					if(yi >= yyi && yi <= yyf) {
						short xxi = (short) (_roi_fb_col[fb].xi >> 1) + off_x;
						short xxf = (short) (_roi_fb_col[fb].xf >> 1) + off_x;
						BitBlt(
							_hdc_screen, xxi, yi,
							(int) (xxf-xxi+1), 1,
							_hdc_memory, xxi, yi, SRCCOPY
						);
					}
				} else { /* zoom senza roi */
					BitBlt(
						_hdc_screen, off_x, yi, lr2, 1,
						_hdc_memory, off_x, yi, SRCCOPY
					);
				}
			}
		}
	} else {  /* no zoom */
		if(_roi_enable_display_col[_param_scheda.fb_display]) {
			/* no zoom con roi */
			short fb = _param_scheda.fb_display;
			if(yi >= _roi_fb_col[fb].yi && yi <= _roi_fb_col[fb].yf) {
				short xxi = _roi_fb_col[fb].xi;
				short xxf = _roi_fb_col[fb].xf;
				BitBlt(
					_hdc_screen, xxi, yi, (int) (xxf-xxi+1),
					1, _hdc_memory, xxi, yi, SRCCOPY
				);
			}
		} else { /* no zoom senza roi */
			BitBlt(
				_hdc_screen, 0, yi, (int) _param_scheda.lung_riga, 1,
				_hdc_memory, 0, yi, SRCCOPY
			);
		}
	}
}

/***********************************************************************/

/* 2.54.0-7 OCV come sea2_refresh2_col, ma tiene conto del frame buffer passato come argomento;
   se fb=-1 lavora come sea2_refresh2_col(),
   altrimenti lavora solo sul fb specificato; */

void
sea2_refresh3_col(
	short yi,
	short fb
)
{
	short j;   
	short lr2 = _param_scheda.lung_riga >> 1;
	short lc2 = _param_scheda.lung_colonna >> 1;
	short j1, j2;

	/* se devo fare tutti i fb lancio la vecchia routine */
	if(fb < 0) {
		sea2_refresh2_col(yi);
		return;
	}

	if(!_param_scheda.scheda_aperta)
		return;
	if(_param_scheda.tipo_tv != TV_RGB)
		return;
	if(_zoom_mode) { /* zoom */
		if(yi < lc2) {
			j1 = 0;
			j2 = 1;
		} else {
			j1 = 2;
			j2 = 3;
		}
		for(j=j1; j<=j2; j++) {
			if(_fb_zoom[j] > -1 && _fb_zoom[j] == fb) { /* fb da vedere */
				short off_x = (short) ((j & 1) ? lr2 : 0);
				if(_roi_enable_display_col[fb]) { /* zoom con roi */
					short off_y = (short) ((j > 1) ? lc2 : 0);
					short yyi = (short) (_roi_fb_col[fb].yi >> 1) + off_y;
					short yyf = (short) (_roi_fb_col[fb].yf >> 1) + off_y;
					if(yi >= yyi && yi <= yyf) {
						short xxi = (short) (_roi_fb_col[fb].xi >> 1) + off_x;
						short xxf = (short) (_roi_fb_col[fb].xf >> 1) + off_x;
						BitBlt(
							_hdc_screen, xxi, yi,
							(int) (xxf-xxi+1), 1,
							_hdc_memory, xxi, yi, SRCCOPY
						);
					}
				} else { /* zoom senza roi */
					BitBlt(
						_hdc_screen, off_x, yi, lr2, 1,
						_hdc_memory, off_x, yi, SRCCOPY
					);
				}
			}
		}
	} else {  /* no zoom */
		if(_param_scheda.fb_display == fb) {
			if(_roi_enable_display_col[fb]) {
				/* no zoom con roi */
				if(yi >= _roi_fb_col[fb].yi && yi <= _roi_fb_col[fb].yf) {
					short xxi = _roi_fb_col[fb].xi;
					short xxf = _roi_fb_col[fb].xf;
					BitBlt(
						_hdc_screen, xxi, yi, (int) (xxf-xxi+1),
						1, _hdc_memory, xxi, yi, SRCCOPY
					);
				}
			} else { /* no zoom senza roi */
				BitBlt(
					_hdc_screen, 0, yi, (int) _param_scheda.lung_riga, 1,
					_hdc_memory, 0, yi, SRCCOPY
				);
			}
		}
	}
}
#endif /* __WATCOMC__ */

/***********************************************************************/

/* azzera l'overlay associato al fb ponendo a 0 l'indice del primo byte libero */

void sea2_reset_overlay(short fb)
{
	if(!_param_scheda.scheda_aperta)
		return;
	_primo_byte_libero[fb] = 0L;
}

/***********************************************************************/

/* assegna il modo di overlay: una qualsiasi combinazione di
   UNDERLAY_MODE e OVERLAY_MODE */

void sea2_select_overlay_mode(short mode)
{
	_overlay_mode = mode;
}

/***********************************************************************/

/* ritorna il modo di overlay: una qualsiasi combinazione di
   UNDERLAY_MODE e OVERLAY_MODE */

short
sea2_overlay_mode(
	void
)
{
	return _overlay_mode;
}

/***********************************************************************/

/* azzera la zona opportuna della bitmap */

static void
brush_opportuno(
	short flag_neri
)
{
#ifndef __WATCOMC__
	short j;

	if(!_param_scheda.scheda_aperta)
		return;

	SelectObject(_hdc_memory, _penna_nera);
	if(_zoom_mode) { /* zoom */
		for(j=0; j<4; j++) {
			short offset_x = (j & 1) * (_param_scheda.lung_riga >> 1);
			short offset_y = ((j & 2) >> 1) * (_param_scheda.lung_colonna >> 1);
			if(_fb_zoom[j] > -1) {
				if(_param_scheda.tipo_tv == TV_RGB) {
					if(_roi_enable_display_col[_fb_zoom[j]]) { /* zoom con roi */
						short fb = _fb_zoom[j];
						short xi = _roi_fb_col[fb].xi >> 1;
						short yi = _roi_fb_col[fb].yi >> 1;
						short xf = _roi_fb_col[fb].xf >> 1;
						short yf = _roi_fb_col[fb].yf >> 1;
						Rectangle(
							_hdc_memory, offset_x+xi, offset_y+yi,
							offset_x+xf+1, offset_y+yf+1
						);
					} else {  /* zoom senza roi */
						Rectangle(
							_hdc_memory, offset_x, offset_y,
							offset_x+(_param_scheda.lung_riga >> 1),
							offset_y+(_param_scheda.lung_colonna >> 1)
						);
					}
				} else {
					if(_roi_enable_display[_fb_zoom[j]]) { /* zoom con roi */
						short fb = _fb_zoom[j];
						short xi = _roi_fb[fb].xi >> 1;
						short yi = _roi_fb[fb].yi >> 1;
						short xf = _roi_fb[fb].xf >> 1;
						short yf = _roi_fb[fb].yf >> 1;
						Rectangle(
							_hdc_memory, offset_x+xi, offset_y+yi,
							offset_x+xf+1, offset_y+yf+1
						);
					} else {  /* zoom senza roi */
						Rectangle(
							_hdc_memory, offset_x, offset_y,
							offset_x+(_param_scheda.lung_riga >> 1),
							offset_y+(_param_scheda.lung_colonna >> 1)
						);
					}
				}
			} else if(flag_neri) {
				Rectangle(
					_hdc_memory, offset_x, offset_y,
					offset_x+(_param_scheda.lung_riga >> 1),
					offset_y+(_param_scheda.lung_colonna >> 1)
				);
			}
		}
	} else { /* no zoom */
		if(_param_scheda.tipo_tv == TV_RGB) {
			if(_roi_enable_display_col[_param_scheda.fb_display]) {/* no zoom con roi */
				short fb = _param_scheda.fb_display;
				short xi = _roi_fb_col[fb].xi, yi = _roi_fb_col[fb].yi;
				short xf = _roi_fb_col[fb].xf, yf = _roi_fb_col[fb].yf;
				Rectangle(_hdc_memory, xi, yi, xf+1, yf+1);
			} else { /* no zoom senza roi */
				Rectangle(
					_hdc_memory, _param_scheda.x_min, _param_scheda.y_min,
					_param_scheda.x_max+1, _param_scheda.y_max+1
				);
			}
		} else {
			if(_roi_enable_display[_param_scheda.fb_display]) {/* no zoom con roi */
				short fb = _param_scheda.fb_display;
				short xi = _roi_fb[fb].xi, yi = _roi_fb[fb].yi;
				short xf = _roi_fb[fb].xf, yf = _roi_fb[fb].yf;
				Rectangle(_hdc_memory, xi, yi, xf+1, yf+1);
			} else { /* no zoom senza roi */
				Rectangle(
					_hdc_memory, _param_scheda.x_min, _param_scheda.y_min,
					_param_scheda.x_max+1, _param_scheda.y_max+1
				);
			}
		}
	}
#endif /* __WATCOMC__ */
}

/***********************************************************************/

/* disegna sulla bitmap le primitive memorizzate: non si fa distinzione tra
   il modo roi e il modo non roi; */

static void
disegno_opportuno(
	void
)
{
	short j, fb;
	long i, primo_libero;
	unsigned char *mem;

	if(!_param_scheda.scheda_aperta)
		return;
	if(_zoom_mode) { /* zoom */
		for(j=0; j<4; j++) {
			short offset_x = (j & 1) * (_param_scheda.lung_riga >> 1);
			short offset_y = ((j & 2) >> 1) * (_param_scheda.lung_colonna >> 1);
			if(_fb_zoom[j] > -1) {
				fb = _fb_zoom[j];
				primo_libero = _primo_byte_libero[fb];
				mem = _mem_draw[fb];
				i = 0L;
				while(i < primo_libero) {
					i = disegna_primitiva(mem, i, offset_x, offset_y);
				}
			}
		}
	} else { /* no zoom */
		fb = _param_scheda.fb_display;
		primo_libero = _primo_byte_libero[fb];
		mem = _mem_draw[fb];
		i = 0L;
		while(i < primo_libero) {
			i = disegna_primitiva(mem, i, 0, 0);
		}
	}
}

/***********************************************************************/

/* disegna sulla bitmap n primitive; ritorna 1 se ci sono ancora primitive
   da disegnare, 0 altrimenti; non si fa distinzione tra
   il modo roi e il modo non roi;
   se n_primitive viene passato a 0 viene resettato il contatore delle
   primitive da disegnare, forzando il disegno a ricominciare dalla prima
   primitiva; se n_primitive viene passato a -1, vengono disegnate tutte le
   primitive rimaste fino all'ultima; */

short
sea2_disegno_su_bitmap(
	short n_primitive
)
{
	short n, j;
	static unsigned char *mem, *mem_zoom[4];
	static long indice, indice_zoom[4], primo_libero, primo_libero_zoom[4];
	static short offset_x[4], offset_y[4];

	if(!_param_scheda.scheda_aperta)
		return 0;
	/* controllo se l'overlay e' abilitato */
	if(!(_overlay_mode & OVERLAY_MODE))
		return 0;

	/* inizializzazione */
	if(n_primitive == 0) {
		if(_zoom_mode) { /* zoom */
			for(j=0; j<4; j++) {
				if(_fb_zoom[j] > -1) {
					offset_x[j] = (j & 1) * (_param_scheda.lung_riga >> 1);
					offset_y[j] = ((j & 2) >> 1) * (_param_scheda.lung_colonna >> 1);
					indice_zoom[j] = 0L;
					primo_libero_zoom[j] =
						_primo_byte_libero[_fb_zoom[j]];
					mem_zoom[j] = _mem_draw[_fb_zoom[j]];
				} else {
					indice_zoom[j] = primo_libero_zoom[j] = 0L;
				}
			}
		} else {
			indice = 0L;
			primo_libero = _primo_byte_libero[_param_scheda.fb_display];
			mem = _mem_draw[_param_scheda.fb_display];
		}
		return 1;
	}

	/* disegno */
	if(_zoom_mode) { /* zoom */
		if(n_primitive == -1) {  /* vado fino in fondo */
			for(j=0; j<4; j++) {
				while(indice_zoom[j] < primo_libero_zoom[j]) {
					indice_zoom[j] = disegna_primitiva(
						mem_zoom[j], indice_zoom[j],
						offset_x[j], offset_y[j]
					);
				}
			}
			return 0;
		} else {
			for(j=0; j<4; j++) {
				while(indice_zoom[j] < primo_libero_zoom[j] && n_primitive) {
					indice_zoom[j] = disegna_primitiva(
						mem_zoom[j], indice_zoom[j],
						offset_x[j], offset_y[j]
					);
					n_primitive--;
				}
			}
			if(n_primitive)
				return 0;
			else
				return 1;
		}
	} else { /* no zoom */
		if(n_primitive == -1) {  /* vado fino in fondo */
			while(indice < primo_libero)
				indice = disegna_primitiva(mem, indice, 0, 0);
			return 0;
		} else {
			for(n=0; n<n_primitive && indice<primo_libero; n++)
				indice = disegna_primitiva(mem, indice, 0, 0);
			if(indice < primo_libero)
				return 1;
			else
				return 0;
		}
	}
	return 0;
}

/***********************************************************************/

/* 2.54.0-7 effettua lo stesso lavoro di sea2_disegno_su_bitmap(), ma tiene conto del frame
   buffer passato come argomento; se fb=-1 lavora come sea2_disegno_su_bitmap(),
   altrimenti lavora solo sul fb specificato;
   a causa della presenza di variabili statiche le due routine (la vecchia e la
   nuova non vanno mescolate; cioe', se si inizia l'overlay di un buffer in un modo
   bisogna continuare con la stessa routine fino al nuovo reset; */

short
sea2_disegno_su_bitmap3(
	short n_primitive,
	short fb
)
{
	short n, j;
	static unsigned char *mem, *mem_zoom[4];
	static long indice, indice_zoom[4], primo_libero, primo_libero_zoom[4];
	static short offset_x[4], offset_y[4];

	/* se devo fare tutti i fb lancio la vecchia routine */
	if(fb < 0) {
		return sea2_disegno_su_bitmap(n_primitive);
	}

	if(!_param_scheda.scheda_aperta)
		return 0;
	/* controllo se l'overlay e' abilitato */
	if(!(_overlay_mode & OVERLAY_MODE))
		return 0;

	/* inizializzazione */
	if(n_primitive == 0) {
		if(_zoom_mode) { /* zoom */
			for(j=0; j<4; j++) {
				if(_fb_zoom[j] > -1 && _fb_zoom[j] == fb) {
					offset_x[j] = (j & 1) * (_param_scheda.lung_riga >> 1);
					offset_y[j] = ((j & 2) >> 1) * (_param_scheda.lung_colonna >> 1);
					indice_zoom[j] = 0L;
					primo_libero_zoom[j] = _primo_byte_libero[_fb_zoom[j]];
					mem_zoom[j] = _mem_draw[_fb_zoom[j]];
				}
			}
		} else {
			if(_param_scheda.fb_display == fb) {
				indice = 0L;
				primo_libero = _primo_byte_libero[_param_scheda.fb_display];
				mem = _mem_draw[_param_scheda.fb_display];
			}
		}
		return 1;
	}

	/* disegno */
	if(_zoom_mode) { /* zoom */
		if(n_primitive == -1) {  /* vado fino in fondo */
			for(j=0; j<4; j++) {
				if(_fb_zoom[j] == fb) {
					while(indice_zoom[j] < primo_libero_zoom[j]) {
						indice_zoom[j] = disegna_primitiva(
							mem_zoom[j], indice_zoom[j],
							offset_x[j], offset_y[j]
						);
					}
				}
			}
			return 0;
		} else {
			for(j=0; j<4; j++) {
				if(_fb_zoom[j] == fb) {
					while(indice_zoom[j] < primo_libero_zoom[j] && n_primitive) {
						indice_zoom[j] = disegna_primitiva(
							mem_zoom[j], indice_zoom[j],
							offset_x[j], offset_y[j]
						);
						n_primitive--;
					}
				}
			}
			if(n_primitive)
				return 0;
			else
				return 1;
		}
	} else { /* no zoom */
		if(_param_scheda.fb_display == fb) {
			if(n_primitive == -1) {  /* vado fino in fondo */
				while(indice < primo_libero)
					indice = disegna_primitiva(mem, indice, 0, 0);
				return 0;
			} else {
				for(n=0; n<n_primitive && indice<primo_libero; n++)
					indice = disegna_primitiva(mem, indice, 0, 0);
				if(indice < primo_libero)
					return 1;
				else
					return 0;
			}
		}
	}
	return 0;
}

/***********************************************************************/

/* se _colori_approssimati == COLORI_VERI crea una penna e la ritorna,
   se _colori_approssimati == COLORI_APPROSSIMATI sceglie una delle
   penne di default e la ritorna */

#ifndef __WATCOMC__
static HPEN
sel_penna(
	short r,
	short g,
	short b
)
{
	if(_colori_approssimati == COLORI_VERI) {
		return CreatePen(PS_SOLID, 0, RGB(r,g,b));
	} else {
		if(r) {
			if(g) {
				if(b) {
					return _penna_bianca;
				} else {
					return _penna_gialla;
				}
			} else {
				if(b) {
					return _penna_magenta;
				} else {
					return _penna_rossa;
				}
			}
		} else {
			if(g) {
				if(b) {
					return _penna_ciano;
				} else {
					return _penna_verde;
				}
			} else {
				if(b) {
					return _penna_blu;
				} else {
					return _penna_nera;
				}
			}
		}
	}
}
#endif /* __WATCOMC__ */

/***********************************************************************/

/* disegna sulla bitmap la primitiva sia in zoom che non; ritorna il
   nuovo valore dell'indice, opportunamente incrementato della
   dimensione della primitiva; */

static long
disegna_primitiva(
	unsigned char *mem, // zona di memoria che contiene le primitive
	long indice,
	short offset_x,
	short offset_y
)
{
#ifndef __WATCOMC__
	short ii, x_pixel, y_pixel, tipo_primitiva;
	short r, g, b, xi, yi, xf, yf, xc, yc, n_punti;
	long jj;
	HPEN penna, old_penna;

	if(!_param_scheda.scheda_aperta)
		return 0;
	tipo_primitiva = (short) mem[indice];
	switch(tipo_primitiva) {
		case PRIM_PUNTO:
			x_pixel = (short) mem[indice+1] |
					  ((short) mem[indice+2] << 8);
			y_pixel = (short) mem[indice+3] |
					  ((short) mem[indice+4] << 8);
			if(_zoom_mode) {
				x_pixel = (x_pixel >> 1) + offset_x;
				y_pixel = (y_pixel >> 1) + offset_y;
			}
			r = (short) mem[indice+5] |
				((short) mem[indice+6] << 8);
			g = (short) mem[indice+7] |
				((short) mem[indice+8] << 8);
			b = (short) mem[indice+9] |
				((short) mem[indice+10] << 8);
			if(_colori_approssimati == COLORI_APPROSSIMATI) {
				if(r < 128) r = 0;
				else r = 255;
				if(g < 128) g = 0;
				else g = 255;
				if(b < 128) b = 0;
				else b = 255;
			}
			SetPixel(_hdc_memory, x_pixel, y_pixel, RGB(r,g,b));
			indice += L_PRIM_PUNTO;
			break;

		case PRIM_FINESTRA:
			xi = (short) mem[indice+1] |
				 ((short) mem[indice+2] << 8);
			yi = (short) mem[indice+3] |
				 ((short) mem[indice+4] << 8);
			xf = (short) mem[indice+5] |
				 ((short) mem[indice+6] << 8);
			yf = (short) mem[indice+7] |
				 ((short) mem[indice+8] << 8);
			if(_zoom_mode) {
				xi = (xi >> 1) + offset_x;
				yi = (yi >> 1) + offset_y;
				xf = (xf >> 1) + offset_x;
				yf = (yf >> 1) + offset_y;
			}
			r = (short) mem[indice+9] |
				((short) mem[indice+10] << 8);
			g = (short) mem[indice+11] |
				((short) mem[indice+12] << 8);
			b = (short) mem[indice+13] |
				((short) mem[indice+14] << 8);
			if(_colori_approssimati == COLORI_APPROSSIMATI) {
				if(r < 128) r = 0;
				else r = 255;
				if(g < 128) g = 0;
				else g = 255;
				if(b < 128) b = 0;
				else b = 255;
			}
			penna = sel_penna(r,g,b);
			old_penna = SelectObject(_hdc_memory, penna);
			MoveToEx(_hdc_memory, xi, yi, NULL);
			LineTo(_hdc_memory, xf, yi);
			LineTo(_hdc_memory, xf, yf);
			LineTo(_hdc_memory, xi, yf);
			LineTo(_hdc_memory, xi, yi);
			SelectObject(_hdc_memory, old_penna);
			if(_colori_approssimati == COLORI_VERI)
				DeleteObject(penna);
			indice += L_PRIM_FINESTRA;
			break;

		case PRIM_LINEA:
			xi = (short) mem[indice+1] |
				 ((short) mem[indice+2] << 8);
			yi = (short) mem[indice+3] |
				 ((short) mem[indice+4] << 8);
			xf = (short) mem[indice+5] |
				 ((short) mem[indice+6] << 8);
			yf = (short) mem[indice+7] |
				 ((short) mem[indice+8] << 8);
			if(_zoom_mode) {
				xi = (xi >> 1) + offset_x;
				yi = (yi >> 1) + offset_y;
				xf = (xf >> 1) + offset_x;
				yf = (yf >> 1) + offset_y;
			}
			r = (short) mem[indice+9] |
				((short) mem[indice+10] << 8);
			g = (short) mem[indice+11] |
				((short) mem[indice+12] << 8);
			b = (short) mem[indice+13] |
				((short) mem[indice+14] << 8);
			if(_colori_approssimati == COLORI_APPROSSIMATI) {
				if(r < 128) r = 0;
				else r = 255;
				if(g < 128) g = 0;
				else g = 255;
				if(b < 128) b = 0;
				else b = 255;
			}
			penna = sel_penna(r,g,b);
			old_penna = SelectObject(_hdc_memory, penna);
			MoveToEx(_hdc_memory, xi, yi, NULL);
			LineTo(_hdc_memory, xf, yf);
			SetPixel(_hdc_memory, xf, yf, RGB(r,g,b));
			SelectObject(_hdc_memory, old_penna);
			if(_colori_approssimati == COLORI_VERI)
				DeleteObject(penna);
			indice += L_PRIM_LINEA;
			break;

		case PRIM_CROCE:
			xc = (short) mem[indice+1] |
				 ((short) mem[indice+2] << 8);
			yc = (short) mem[indice+3] |
				 ((short) mem[indice+4] << 8);
			if(_zoom_mode) {
				xc = (xc >> 1) + offset_x;
				yc = (yc >> 1) + offset_y;
			}
			r = (short) mem[indice+5] |
				((short) mem[indice+6] << 8);
			g = (short) mem[indice+7] |
				((short) mem[indice+8] << 8);
			b = (short) mem[indice+9] |
				((short) mem[indice+10] << 8);
			if(_colori_approssimati == COLORI_APPROSSIMATI) {
				if(r < 128) r = 0;
				else r = 255;
				if(g < 128) g = 0;
				else g = 255;
				if(b < 128) b = 0;
				else b = 255;
			}
			SetPixel(_hdc_memory, xc, yc, RGB(r,g,b));
			if(!_zoom_mode) {
				SetPixel(_hdc_memory, xc-1, yc, RGB(r,g,b));
				SetPixel(_hdc_memory, xc+1, yc, RGB(r,g,b));
				SetPixel(_hdc_memory, xc, yc-1, RGB(r,g,b));
				SetPixel(_hdc_memory, xc, yc+1, RGB(r,g,b));
			}
			indice += L_PRIM_CROCE;
			break;

		case PRIM_QUADRATINO:
			xc = (short) mem[indice+1] |
				 ((short) mem[indice+2] << 8);
			yc = (short) mem[indice+3] |
				 ((short) mem[indice+4] << 8);
			if(_zoom_mode) {
				xc = (xc >> 1) + offset_x;
				yc = (yc >> 1) + offset_y;
			}
			r = (short) mem[indice+5] |
				((short) mem[indice+6] << 8);
			g = (short) mem[indice+7] |
				((short) mem[indice+8] << 8);
			b = (short) mem[indice+9] |
				((short) mem[indice+10] << 8);
			if(_colori_approssimati == COLORI_APPROSSIMATI) {
				if(r < 128) r = 0;
				else r = 255;
				if(g < 128) g = 0;
				else g = 255;
				if(b < 128) b = 0;
				else b = 255;
			}
			SetPixel(_hdc_memory, xc, yc, RGB(r,g,b));
			SetPixel(_hdc_memory, xc-1, yc, RGB(r,g,b));
			SetPixel(_hdc_memory, xc+1, yc, RGB(r,g,b));
			SetPixel(_hdc_memory, xc, yc-1, RGB(r,g,b));
			SetPixel(_hdc_memory, xc, yc+1, RGB(r,g,b));
			SetPixel(_hdc_memory, xc-1, yc-1, RGB(r,g,b));
			SetPixel(_hdc_memory, xc+1, yc+1, RGB(r,g,b));
			SetPixel(_hdc_memory, xc+1, yc-1, RGB(r,g,b));
			SetPixel(_hdc_memory, xc-1, yc+1, RGB(r,g,b));
			indice += L_PRIM_QUADRATINO;
			break;

		case PRIM_POLIGONO:
			n_punti = (short) mem[indice+1] |
				 ((short) mem[indice+2] << 8);
			r = (short) mem[indice+3] |
				((short) mem[indice+4] << 8);
			g = (short) mem[indice+5] |
				((short) mem[indice+6] << 8);
			b = (short) mem[indice+7] |
				((short) mem[indice+8] << 8);
			if(_colori_approssimati == COLORI_APPROSSIMATI) {
				if(r < 128) r = 0;
				else r = 255;
				if(g < 128) g = 0;
				else g = 255;
				if(b < 128) b = 0;
				else b = 255;
			}
			if(n_punti > 1) {
				penna = sel_penna(r,g,b);
				old_penna = SelectObject(_hdc_memory, penna);
				jj = indice + 9L;
				xi = (short) mem[jj] | ((short) mem[jj+1] << 8);
				yi = (short) mem[jj+8] | ((short) mem[jj+3] << 8);
				if(_zoom_mode) {
					xi = (xi >> 1) + offset_x;
					yi = (yi >> 1) + offset_y;
				}
				MoveToEx(_hdc_memory, xi, yi, NULL);
				for(ii=1, jj+=4L; ii<n_punti; ii++, jj+=4L) {
					xc = (short) mem[jj] | ((short) mem[jj+1] << 8);
					yc = (short) mem[jj+2] | ((short) mem[jj+3] << 8);
					if(_zoom_mode) {
						xc = (xc >> 1) + offset_x;
						yc = (yc >> 1) + offset_y;
					}
					LineTo(_hdc_memory, xc, yc);
				}
				LineTo(_hdc_memory, xi, yi);
				SelectObject(_hdc_memory, old_penna);
				if(_colori_approssimati == COLORI_VERI)
					DeleteObject(penna);
			}
			indice += L_PRIM_POLIGONO(n_punti);
			break;
	}
#endif
	return indice;
}

/******************************************************************/

/* il display a 16 bit puo' essere fatto con 15 o 16 bit
   15 bit (byte alto: XRRRRRGG byte basso: GGGBBBBB)
   16 bit (byte alto: RRRRRGGG byte basso: GGGBBBBB); il byte basso va scritto
   prima e il byte alto va scritto dopo (come di solito tutti gli interi
   per l'INTEL; questa routine setta la variabile che tiene memoria del tipo
   di display; il default e' a 16 bit; a seconda del numero di bit le routine
   che leggono il frame buffer ed impaccano i bit nei due byte per il display
   usano una coppia di macro (BYTE_ALTO_15(A,B), BYTE_BASSO_15(A,B))
   o l'altra (BYTE_ALTO_16(A,B), BYTE_BASSO_16(A,B)) (definite in sea2.h) */

void
sea2_set_n_bit_col16(
	short n_bit
)
{
	if(n_bit == 16)
		_n_bit_col16 = 16;
	else if(n_bit == 15)
		_n_bit_col16 = 15;
}

/******************************************************************/

/* setta la variabile _colori_approssimati a COLORI_APPROSSIMATI oppure
   a COLORI_VERI; nel primo caso le primitive disegnate in overlay sono
   tracciate con una delle otto penne di default, nel secondo caso si
   crea una penna per ogni primitiva da disegnare; questa variabile
   influenza tulle le primitive; */

void
sea2_set_color_mode(
	short modo
)
{
	if(_n_byte_per_pixel > 1) {
		if(modo == COLORI_VERI)
			_colori_approssimati = COLORI_VERI;
		else if(modo == COLORI_APPROSSIMATI)
			_colori_approssimati = COLORI_APPROSSIMATI;
	}
}

/***********************************************************************/

/* copia in mem il vettore che contiene le primitive del frame buffer assegnato,
   ritorna il numero di byte copiati; */

long sea2_copy_graphic_buffer(short fb, unsigned char *mem)
{
	if(!_param_scheda.scheda_aperta || _primo_byte_libero[fb] < 0L)
		return 0L;
	memcpy(mem, _mem_draw[fb], _primo_byte_libero[fb]);
	return _primo_byte_libero[fb];
}

/***********************************************************************/

/* versione 2.53.0 di OCV copia da un fb all'altro il vettore che contiene le
   primitive del frame buffer assegnato, ritorna il numero di byte copiati; */

long sea2_copy_graphic_fb2fb(short fb_source, short fb_target)
{
	if(!_param_scheda.scheda_aperta || _primo_byte_libero[fb_source] < 0L)
		return 0L;
	memcpy(_mem_draw[fb_target], _mem_draw[fb_source], _primo_byte_libero[fb_source]);
	_primo_byte_libero[fb_target] = _primo_byte_libero[fb_source];
	return _primo_byte_libero[fb_target];
}


