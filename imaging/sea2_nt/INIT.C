
/*
	sea2_init
	sea2_init_multi_board
	sea2_close
	sea2_set_dma
*/

// ultime modifiche con Harle 7.61

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
	#include <stdlib.h>
	#include <graph.h>
#endif

/****************************************************************************/

/* variabili globali definite qui */

#if defined(MET) || defined(ASS) || defined(MET_MIL) || defined(ORION) || defined(METEOR2)
	ROI _roi_fb[N_FB_MAX];
	short _roi_enable_display[N_FB_MAX];
	short _roi_enable_acq[N_FB_MAX];
	unsigned char *_mem_address[N_FB_MAX];
	#ifndef __WATCOMC__
		HWND _win_handle = 0; /* puntatore alla client area */
		HBITMAP _bitmap, _old_bitmap, _bitmap_row;
		HPALETTE _hpalApp, _old_palette;
		HDC _hdc_screen, _hdc_memory, _hdc_memory_row;
		HPEN _penna_nera, _penna_bianca, _penna_rossa, _penna_verde;
		HPEN _penna_gialla, _penna_magenta, _penna_ciano, _penna_blu;
		HBRUSH _brush_nero;
	#else
		unsigned char *_bitmap, *_old_bitmap, *_bitmap_row;
	#endif
	short _zoom_mode; /* 0:normale, 1:zoom */
	short _fb_zoom[4];
	/* fb da visualizzare in zoom; se -1 non si visualizza il fb */
	unsigned char *_aus_bitmap=NULL; /* array ausiliario la cui dimensione
	dipende soltanto dal tipo di display */
	long *_offset_riga_display; /* contiene, per ogni riga, l'offset del primo
	pixel; si rifersce al modo con cui si fa il display (da 1 a 4 byte per pixel)
	e quindi non dipende dal tipo di scheda usata; si usa per indirizzare
	_aus_bitmap */
	long *_offset_colonna_display; /* contiene, per ogni colonna, l'offset del primo
	pixel; si rifersce al modo con cui si fa il display (da 1 a 4 byte per pixel)
	e quindi non dipende dal tipo di scheda usata; si usa per indirizzare
	_aus_bitmap */
	long *_offset_riga; /* contiene, per ogni riga, l'offset del primo pixel; si
	rifersce al modo con cui sono memorizzati i pixel in memoria e quindi
	dipende solo dal tipo di scheda usata */
	short _n_bit_col16 = 16; /* numero di bit usati nel display a 16 bit;
	possono essere 15 (XRRRRRGGGGGBBBBB) o 16 (RRRRRGGGGGGBBBBB) */
	long _n_byte_per_pixel;
	// per il colore RGB
	unsigned char *_mem_address_col[N_FB_MAX_COL];
	ROI _roi_fb_col[N_FB_MAX_COL];
	short _roi_enable_display_col[N_FB_MAX_COL];
	short _roi_enable_acq_col[N_FB_MAX_COL];
	short _piano_colore=3; /* per il display: 0:r, 1:g, 2:b, 3:colore */
	// fine colore RGB
	// variabili per la grafica in overlay
	unsigned char *_mem_draw[N_FB_MAX]; // dove memorizzare le primitive
	long _primo_byte_libero[N_FB_MAX];
	short _overlay_mode;
	long _max_n_byte_grafica;
	short _colori_approssimati;
#endif // MET || ASS || MET_MIL || ORION || METEOR2

#if defined(MET) || defined(MET_MIL) || defined(ORION) || defined(METEOR2)
	MIL_ID _mil_application, _vga_buffer, _mil_image[N_FB_MAX];
	MIL_ID _mil_system[N_BOARD_MAX], _mil_camera[N_BOARD_MAX];
	short _direct_vga_possible, _vga_off_x, _vga_off_y, _mode_direct_vga;
	// per il colore RGB
	MIL_ID _mil_image_col[N_FB_MAX_COL];
	// fine colore RGB
#endif // MET || MET_MIL || ORION || METEOR2

#if defined(METEOR2) || defined(ORION)
	short _tempo_definito=0;
#endif

	PARAM_SCHEDA _param_scheda;

/***********************************************************************/

short
sea2_init(
	short standard_tv, /* STANDARD_EU, STANDARD_AM */
	short tipo_tv,      /* TV_MONO, TV_RGB */
	char *nome_dcf // puo' essere NULL
)
{
	short i;

	/* inizializzazione della struttura */
	_param_scheda.scheda_aperta = 0;
	_param_scheda.nome_scheda = NOME_SCHEDA;
	_param_scheda.standard_tv = standard_tv;
	_param_scheda.tipo_tv = tipo_tv;
	if(standard_tv == STANDARD_AM) {
		_param_scheda.x_max = X_MAX_AM;
		_param_scheda.y_max = Y_MAX_AM;
		_param_scheda.lung_riga = LUNG_RIGA_AM;
		_param_scheda.lung_colonna = LUNG_COLONNA_AM;
		_param_scheda.lung_max = LUNG_MAX_AM;
		_param_scheda.fattore_dist = FATTORE_DIST_AM;
	} else if(standard_tv == STANDARD_EU) {
		_param_scheda.x_max = X_MAX_EU;
		_param_scheda.y_max = Y_MAX_EU;
		_param_scheda.lung_riga = LUNG_RIGA_EU;
		_param_scheda.lung_colonna = LUNG_COLONNA_EU;
		_param_scheda.lung_max = LUNG_MAX_EU;
		_param_scheda.fattore_dist = FATTORE_DIST_EU;
	} else if (standard_tv == STANDARD_1024) {
		_param_scheda.x_max = 1023;
		_param_scheda.y_max = 767;
		_param_scheda.lung_riga = 1024;
		_param_scheda.lung_colonna = 768;
		_param_scheda.lung_max = 1024;
		_param_scheda.fattore_dist = 1;
	} else if (standard_tv == STANDARD_4000) {
		_param_scheda.x_max = 3039;
		_param_scheda.y_max = 1899;
		_param_scheda.lung_riga = 3040;
		_param_scheda.lung_colonna = 1900;
		_param_scheda.lung_max = 3040;
		_param_scheda.fattore_dist = 1;
	}
	_param_scheda.x_min = X_MIN;
	_param_scheda.y_min = Y_MIN;
	_param_scheda.chan_min = CHAN_MIN;
	_param_scheda.chan_max = CHAN_MAX;
	_param_scheda.fb_min = FB_MIN;
	_param_scheda.fb_max = FB_MAX;
	_param_scheda.chan_min_col = CHAN_MIN_COL;
	_param_scheda.chan_max_col = CHAN_MAX_COL;
	_param_scheda.fb_min_col = FB_MIN_COL;
	_param_scheda.fb_max_col = FB_MAX_COL;
	_param_scheda.fb_dma_min = FB_DMA_MIN;
	_param_scheda.fb_dma_max = FB_DMA_MAX;
	_param_scheda.numero_scheda = 0;
	_param_scheda.scheda_min = 0;
	_param_scheda.scheda_max = 0;
	_param_scheda.fb_display = FB_MIN;
	_param_scheda.fb_lavoro = FB_MIN;
	_param_scheda.memoria_lavoro = MEMORIA_LAVORO;
	_param_scheda.pan_display = 0;
	_param_scheda.scroll_display = 0;

	// 7.61-7: inizializzazione di _mem_draw: cosi' si possono fare le free
	// senza problemi
	for(i=0; i<N_FB_MAX; i++)
		_mem_draw[i] = NULL;

#ifdef MET
	{
		long j, n_byte;
#ifndef __WATCOMC__
		HDC hdc;
#endif
	
/* allocazione dell'applicazione */
	
		if(MTappAlloc(M_DEFAULT, &_mil_application) == M_NULL)
			return SEAV_ERR_GRABBER_INIT;

/* disabilito l'interrupt */

		MTappControl(M_IRQ_CONTROL, M_DISABLE);

/* disabilito il check dei parametri */
	
		MTappControl(M_PARAMETER_CHECK, M_DISABLE);

/* allocazione del sistema di default (0) */
	
		if(MTsysAlloc(M_DEFAULT, M_DEV0, M_DEFAULT, _mil_system) == M_NULL)
			return SEAV_ERR_GRABBER_INIT;

/* allocazione del digitizer di default (0) */

		if(tipo_tv == TV_RGB) {
			if(standard_tv == STANDARD_AM)
				MTdigAlloc(_mil_system[0], M_DEV0, "M_NTSC_RGB", M_DEFAULT,_mil_camera);
			else
				MTdigAlloc(_mil_system[0], M_DEV0, "M_PAL_RGB", M_DEFAULT, _mil_camera);
		} else {
			if(standard_tv == STANDARD_AM)
				MTdigAlloc(_mil_system[0], M_DEV0, "M_RS170", M_DEFAULT, _mil_camera);
			else
				MTdigAlloc(_mil_system[0], M_DEV0, "M_CCIR", M_DEFAULT, _mil_camera);
		}

/* selezione del canale di ingresso di default */

		MTdigChannel(_mil_camera[0], M_CH1);

/* si fa si' che le funzioni escano subito dopo aver lanciato il grab */
	
		MTdigControl(_mil_camera[0], M_GRAB_MODE, M_ASYNCHRONOUS);
	
/* si acquisisce un solo frame */
	
		MTdigControl(_mil_camera[0], M_GRAB_FRAME_NUM, 1.0);
	
/* si acquisisce un solo field */
	
		MTdigControl(_mil_camera[0], M_GRAB_FIELD_NUM, 1.0);
	
		if(tipo_tv == TV_RGB) {

/* si cambiano i livelli di digitalizzazione per evitare i valori 0-10
   245-255; i default sono 80 e 160 */
	
			MTdigReference(_mil_camera[0], M_BLACK_REF, 90L);
			MTdigReference(_mil_camera[0], M_WHITE_REF, 195L);
		} else {

/* si clippa la dinamica di ingresso alla scheda dalla telecamera tra 10
   e 245: non funziona per l'RGB; forse e' meglio riprogrammare il
   digitizer come sopra; */
	
			MTdigControl(_mil_camera[0], M_GRAB_WINDOWS_RANGE, M_ENABLE);
		}

/* serve se la fase orizzontale della telecamera e' disturbata */
	
		MTdigControl(_mil_camera[0], M_VCR_INPUT_TYPE, M_ENABLE);
	
/* mumble mumble ... */

//		MTsysControl(_mil_system[0], M_FAST_PCI_TO_MEM, M_ENABLE);
//		MTsysControl(_mil_system[0], M_ALLOC_BUF_RGB888_AS_RGB555, M_DISABLE);

/* non si fa il direct_vga */

		_direct_vga_possible = 0;
		_mode_direct_vga = 0;
		_vga_off_x = 0;  
		_vga_off_y = 0; 

/* per default non si fa lo zoom */

		_zoom_mode = 0;

/* per default in modo zoom non si visualizza niente */

		_fb_zoom[0] = _fb_zoom[1] = _fb_zoom[2] = _fb_zoom[3] = -1;

/* inizializzazione della grafica: per default si abilitano sia
   l'overlay che l'underlay; per default si usano in overlay i colori delle
   8 penne di default; e' comunque possibile usare i colori veri settando
   _colori_approssimati a COLORI_VERI tramite la routine sea2_set_color_mode,
   ma solo con il display a colori */

		_overlay_mode = UNDERLAY_MODE | OVERLAY_MODE;
		_colori_approssimati = COLORI_APPROSSIMATI;
		_max_n_byte_grafica = (long) _param_scheda.lung_riga *
			(long) _param_scheda.lung_colonna;

/* allocazione dei frame buffer di default insieme con i buffer per la grafica */

		if(tipo_tv == TV_RGB) {
			_param_scheda.fb_max = _param_scheda.fb_dma_max =
				(_param_scheda.fb_max_col - _param_scheda.fb_min_col + 1) * 4 - 1 +
				_param_scheda.fb_min;
			for(i=_param_scheda.fb_min_col; i<=_param_scheda.fb_max_col; i++) {
				if(MTbufAllocColor(
					_mil_system[0], 4L, _param_scheda.lung_riga,
					_param_scheda.lung_colonna,
					8 + M_UNSIGNED, M_IMAGE,
					_mil_image_col + i
				) == M_NULL)
					return SEAV_ERR_GRABBER_MEMORY;
				MTbufInquire(
					_mil_image_col[i], M_HOST_ADDRESS, (long *) (_mem_address_col+i)
				);
				_mem_address[i*4] = _mem_address_col[i];
				_mem_address[i*4 + 1] = _mem_address_col[i] + 1;
				_mem_address[i*4 + 2] = _mem_address_col[i] + 2;
				_mem_address[i*4 + 3] = _mem_address_col[i] + 3;
				_mem_draw[i] = (unsigned char *) malloc(
					_max_n_byte_grafica * sizeof(unsigned char)
				);
				if(!_mem_draw[i])
					return SEAV_ERR_MEMORY;
				_primo_byte_libero[i] = 0L;
			}
		} else {
			for(i=_param_scheda.fb_min; i<=_param_scheda.fb_max; i++) {
				if(MTbufAlloc2d(
					_mil_system[0], _param_scheda.lung_riga,
					_param_scheda.lung_colonna,
					8 + M_UNSIGNED, M_IMAGE, _mil_image + i
				) == M_NULL)
					return SEAV_ERR_GRABBER_MEMORY;
				MTbufInquire(
					_mil_image[i], M_HOST_ADDRESS, (long *) (_mem_address+i)
				);
				_mem_draw[i] = (unsigned char *) malloc(
					_max_n_byte_grafica * sizeof(unsigned char)
				);
				if(!_mem_draw[i])
					return SEAV_ERR_MEMORY;
				_primo_byte_libero[i] = 0L;
			}
		}

/* inizializzazione delle ROI; per default non si abilita la roi */

		for(i=0; i<N_FB_MAX; i++) {
			_roi_fb[i].fb = i;
			_roi_fb[i].xi = _param_scheda.x_min;
			_roi_fb[i].xf = _param_scheda.x_max;
			_roi_fb[i].yi = _param_scheda.y_min;
			_roi_fb[i].yf = _param_scheda.y_max;
			_roi_enable_display[i] = 0;
			_roi_enable_acq[i] = 0;
		}

		if(tipo_tv == TV_RGB) {
			for(i=0; i<N_FB_MAX_COL; i++) {
				_roi_fb_col[i].fb = i;
				_roi_fb_col[i].xi = _param_scheda.x_min;
				_roi_fb_col[i].xf = _param_scheda.x_max;
				_roi_fb_col[i].yi = _param_scheda.y_min;
				_roi_fb_col[i].yf = _param_scheda.y_max;
				_roi_enable_display_col[i] = 0;
				_roi_enable_acq_col[i] = 0;
			}
		}

/* start grab */

		if(tipo_tv == TV_RGB)
			MTdigGrabContinuous(_mil_camera[0], _mil_image_col[0]);
		else
			MTdigGrabContinuous(_mil_camera[0], _mil_image[0]);
		for(j=0; j<10; j++)
			sea2_wait_v_blank();
		MTdigGrabStopMio(_mil_system[0]);

/* allocazione di _aus_bitmap per la scrittura della bitmap */

#ifndef __WATCOMC__
		hdc = GetDC((HWND) NULL);
		i = GetDeviceCaps(hdc, BITSPIXEL);
		ReleaseDC((HWND) NULL, hdc);
		switch(i) {
			case 8:
				n_byte = 1L;
				break;
			case 16:
				n_byte = 2L;
				break;
			case 24:
				n_byte = 3L;
				break;
			case 32:
				n_byte = 4L;
				break;
			default:
				return SEAV_ERR_VGA_MODE_NOT_SUPPORTED;
		}
#else
		n_byte = 1L;
#endif

		_aus_bitmap = (unsigned char *) malloc(
			(long)_param_scheda.lung_riga *
			(long)_param_scheda.lung_colonna * n_byte * sizeof(unsigned char)
		);
		if(!_aus_bitmap)
			return SEAV_ERR_MEMORY;
		sea2_clear_bitmap();

/* allocazione di _offset_riga_display: contiene l'offset del primo pixel di
   ogni riga di _aus_bitmap */

		_offset_riga_display = (long *) malloc(
			_param_scheda.lung_colonna * sizeof(long)
		);
		if(!_offset_riga_display)
			return SEAV_ERR_MEMORY;
		for(j=0; j<_param_scheda.lung_colonna; j++)
			_offset_riga_display[j] = (long) j *
			(long) _param_scheda.lung_riga * n_byte;

/* allocazione di _offset_colonna_display: contiene l'offset del primo pixel di
   ogni colonna di _aus_bitmap */

		_offset_colonna_display = (long *) malloc(
			_param_scheda.lung_riga * sizeof(long)
		);
		if(!_offset_colonna_display)
			return SEAV_ERR_MEMORY;
		for(j=0; j<_param_scheda.lung_riga; j++)
			_offset_colonna_display[j] = (long) j * n_byte;

/* allocazione di _offset_riga: contiene l'offset del primo pixel di
   ogni riga in memoria */

		_offset_riga = (long *) malloc(
			_param_scheda.lung_colonna * sizeof(long)
		);
		if(!_offset_riga)
			return SEAV_ERR_MEMORY;
		if(tipo_tv == TV_RGB) {
			for(j=0; j<_param_scheda.lung_colonna; j++)
				_offset_riga[j] = (long) j * (long) _param_scheda.lung_riga * 4L;
		} else {
			for(j=0; j<_param_scheda.lung_colonna; j++)
				_offset_riga[j] = (long) j * (long) _param_scheda.lung_riga;
		}               
	}               

#endif /* MET */

#ifdef ASS
	{
		long j, n_byte;
#ifndef __WATCOMC__
		HDC hdc;
#endif
	
/* per default non si fa lo zoom */

		_zoom_mode = 0;

/* per default in modo zoom non si visualizza niente */

		_fb_zoom[0] = _fb_zoom[1] = _fb_zoom[2] = _fb_zoom[3] = -1;

/* inizializzazione della grafica: per default si abilitano sia
   l'overlay che l'underlay; per default si usano in overlay i colori delle
   8 penne di default; e' comunque possibile usare i colori veri settando
   _colori_approssimati a COLORI_VERI tramite la routine sea2_set_color_mode,
   ma solo con il display a colori */

		_overlay_mode = UNDERLAY_MODE | OVERLAY_MODE;
		_colori_approssimati = COLORI_APPROSSIMATI;
		_max_n_byte_grafica = (long) _param_scheda.lung_riga *
			(long) _param_scheda.lung_colonna;

/* allocazione dei frame buffer di default insieme con i buffer per la grafica */

		if(tipo_tv == TV_RGB) {
			_param_scheda.fb_max = _param_scheda.fb_dma_max =
				(_param_scheda.fb_max_col - _param_scheda.fb_min_col + 1) * 4 - 1 +
				_param_scheda.fb_min;
			for(i=_param_scheda.fb_min_col; i<=_param_scheda.fb_max_col; i++) {
				if((_mem_address_col[i] = (unsigned char *) malloc(
					(long)_param_scheda.lung_riga *
					(long)_param_scheda.lung_colonna * 4L * sizeof(unsigned char)
				)) == NULL)
					return SEAV_ERR_GRABBER_MEMORY;
				_mem_address[i*4] = _mem_address_col[i];
				_mem_address[i*4 + 1] = _mem_address_col[i] + 1;
				_mem_address[i*4 + 2] = _mem_address_col[i] + 2;
				_mem_address[i*4 + 3] = _mem_address_col[i] + 3;
				_mem_draw[i] = (unsigned char *) malloc(
					_max_n_byte_grafica * sizeof(unsigned char)
				);
				if(!_mem_draw[i])
					return SEAV_ERR_MEMORY;
				_primo_byte_libero[i] = 0L;
			}
		} else {
			for(i=_param_scheda.fb_min; i<=_param_scheda.fb_max; i++) {
				if((_mem_address[i] = (unsigned char *) malloc(
					(long)_param_scheda.lung_riga *
					(long)_param_scheda.lung_colonna * sizeof(unsigned char)
				)) == NULL)
					return SEAV_ERR_GRABBER_MEMORY;
				_mem_draw[i] = (unsigned char *) malloc(
					_max_n_byte_grafica * sizeof(unsigned char)
				);
				if(!_mem_draw[i])
					return SEAV_ERR_MEMORY;
				_primo_byte_libero[i] = 0L;
			}
		}

/* inizializzazione delle ROI; per default non si abilita la roi */

		for(i=0; i<N_FB_MAX; i++) {
			_roi_fb[i].fb = i;
			_roi_fb[i].xi = _param_scheda.x_min;
			_roi_fb[i].xf = _param_scheda.x_max;
			_roi_fb[i].yi = _param_scheda.y_min;
			_roi_fb[i].yf = _param_scheda.y_max;
			_roi_enable_display[i] = 0;
			_roi_enable_acq[i] = 0;
		}
		if(tipo_tv == TV_RGB) {
			for(i=0; i<N_FB_MAX_COL; i++) {
				_roi_fb_col[i].fb = i;
				_roi_fb_col[i].xi = _param_scheda.x_min;
				_roi_fb_col[i].xf = _param_scheda.x_max;
				_roi_fb_col[i].yi = _param_scheda.y_min;
				_roi_fb_col[i].yf = _param_scheda.y_max;
				_roi_enable_display_col[i] = 0;
				_roi_enable_acq_col[i] = 0;
			}
		}

/* allocazione di _aus_bitmap per la scrittura della bitmap */

#ifndef __WATCOMC__
		hdc = GetDC((HWND) NULL);
		i = GetDeviceCaps(hdc, BITSPIXEL);
		ReleaseDC((HWND) NULL, hdc);
		switch(i) {
			case 8:
				n_byte = 1L;
				break;
			case 16:
				n_byte = 2L;
				break;
			case 24:
				n_byte = 3L;
				break;
			case 32:
				n_byte = 4L;
				break;
			default:
				return SEAV_ERR_VGA_MODE_NOT_SUPPORTED;
		}
#else
		n_byte = 1L;
#endif

		_aus_bitmap = (unsigned char *) malloc(
			(long)_param_scheda.lung_riga *
			(long)_param_scheda.lung_colonna * n_byte * sizeof(unsigned char)
		);
		if(!_aus_bitmap)
			return SEAV_ERR_MEMORY;
		sea2_clear_bitmap();

/* allocazione di _offset_riga_display: contiene l'offset del primo pixel di
   ogni riga di _aus_bitmap */

		_offset_riga_display = (long *) malloc(
			_param_scheda.lung_colonna * sizeof(long)
		);
		if(!_offset_riga_display)
			return SEAV_ERR_MEMORY;
		for(j=0; j<_param_scheda.lung_colonna; j++)
			_offset_riga_display[j] = (long) j *
			(long) _param_scheda.lung_riga * n_byte;


/* allocazione di _offset_colonna_display: contiene l'offset del primo pixel di
   ogni colonna di _aus_bitmap */

		_offset_colonna_display = (long *) malloc(
			_param_scheda.lung_riga * sizeof(long)
		);
		if(!_offset_colonna_display)
			return SEAV_ERR_MEMORY;
		for(j=0; j<_param_scheda.lung_riga; j++)
			_offset_colonna_display[j] = (long) j * n_byte;

/* allocazione di _offset_riga: contiene l'offset del primo pixel di
   ogni riga */

		_offset_riga = (long *) malloc(
			_param_scheda.lung_colonna * sizeof(long)
		);
		if(!_offset_riga)
			return SEAV_ERR_MEMORY;
		if(tipo_tv == TV_RGB) {
			for(j=0; j<_param_scheda.lung_colonna; j++)
				_offset_riga[j] = (long) j * (long) _param_scheda.lung_riga * 4L;
		} else {
			for(j=0; j<_param_scheda.lung_colonna; j++)
				_offset_riga[j] = (long) j * (long) _param_scheda.lung_riga;
		}
	}

#endif /* ASS */

#ifdef MET_MIL
	{
		long j, n_byte;
		HDC hdc;
		MIL_ID ret;
	
/* allocazione dell'applicazione */
	
		if(MappAlloc(M_DEFAULT, &_mil_application) == M_NULL)
			return SEAV_ERR_GRABBER_INIT;

/* disabilito l'interrupt */

		//MappControl(M_IRQ_CONTROL, M_DISABLE); // ?

/* disabilito la stampa degli errori */
	
		MappControl(M_ERROR, M_PRINT_DISABLE);

/* disabilito il check dei parametri */
	
		MappControl(M_PARAMETER, M_CHECK_DISABLE);

/* allocazione del sistema di default (0) */
	
		if(MsysAlloc(M_SYSTEM_METEOR, M_DEV0, M_DEFAULT, _mil_system) == M_NULL)
			return SEAV_ERR_GRABBER_INIT;

/* allocazione del digitizer di default (0) */

		if(tipo_tv == TV_RGB) {
			if(standard_tv == STANDARD_AM)
				ret = MdigAlloc(
					_mil_system[0], M_DEV0, "M_NTSC_RGB", M_DEFAULT,_mil_camera
				);
			else
				ret = MdigAlloc(
					_mil_system[0], M_DEV0, "M_PAL_RGB", M_DEFAULT, _mil_camera
				);
		} else {
			if(standard_tv == STANDARD_AM)
				ret = MdigAlloc(
					_mil_system[0], M_DEV0, "M_RS170", M_DEFAULT, _mil_camera
				);
			else
				ret = MdigAlloc(
					_mil_system[0], M_DEV0, "M_CCIR", M_DEFAULT, _mil_camera
				);
		}
		if(ret == M_NULL)
			return SEAV_ERR_GRABBER_INIT;

/* selezione del canale di ingresso di default */

		MdigChannel(_mil_camera[0], M_CH1);

/* si fa si' che le funzioni escano subito dopo aver lanciato il grab */
	
		MdigControl(_mil_camera[0], M_GRAB_MODE, M_ASYNCHRONOUS);
	
/* si acquisisce un solo frame */
	
		MdigControl(_mil_camera[0], M_GRAB_FRAME_NUM, 1.0);
	
		if(tipo_tv == TV_RGB) {

/* si cambiano i livelli di digitalizzazione per evitare i valori 0-10
   245-255; i default sono 80 e 160 */
	
//			MdigReference(_mil_camera[0], M_BLACK_REF, 90L);
//			MdigReference(_mil_camera[0], M_WHITE_REF, 195L);
		} else {

/* si clippa la dinamica di ingresso alla scheda dalla telecamera tra 10
   e 245: non funziona per l'RGB; forse e' meglio riprogrammare il
   digitizer come sopra; */
	
			MdigControl(_mil_camera[0], M_GRAB_WINDOWS_RANGE, M_ENABLE);
		}

/* serve se la fase orizzontale della telecamera e' disturbata */
	
		MdigControl(_mil_camera[0], M_VCR_INPUT_TYPE, M_ENABLE);
	
/* non si fa il direct_vga */

		_direct_vga_possible = 0;
		_mode_direct_vga = 0;
		_vga_off_x = 0;  
		_vga_off_y = 0; 

/* per default non si fa lo zoom */

		_zoom_mode = 0;

/* per default in modo zoom non si visualizza niente */

		_fb_zoom[0] = _fb_zoom[1] = _fb_zoom[2] = _fb_zoom[3] = -1;

/* inizializzazione della grafica: per default si abilitano sia
   l'overlay che l'underlay; per default si usano in overlay i colori delle
   8 penne di default; e' comunque possibile usare i colori veri settando
   _colori_approssimati a COLORI_VERI tramite la routine sea2_set_color_mode,
   ma solo con il display a colori */

		_overlay_mode = UNDERLAY_MODE | OVERLAY_MODE;
		_colori_approssimati = COLORI_APPROSSIMATI;
		_max_n_byte_grafica = (long) _param_scheda.lung_riga *
			(long) _param_scheda.lung_colonna;

/* allocazione dei frame buffer di default insieme con i buffer per la grafica */

		if(tipo_tv == TV_RGB) {
			_param_scheda.fb_max = _param_scheda.fb_dma_max =
				(_param_scheda.fb_max_col - _param_scheda.fb_min_col + 1) * 4 - 1 +
				_param_scheda.fb_min;
			for(i=_param_scheda.fb_min_col; i<=_param_scheda.fb_max_col; i++) {
				if(MbufAllocColor(
					_mil_system[0], 4L, _param_scheda.lung_riga,
					_param_scheda.lung_colonna,
					8 + M_UNSIGNED, M_IMAGE+M_GRAB,
					_mil_image_col + i
				) == M_NULL)
					return SEAV_ERR_GRABBER_MEMORY;
				MbufInquire(
					_mil_image_col[i], M_HOST_ADDRESS, (long *) (_mem_address_col+i)
				);
				_mem_address[i*4] = _mem_address_col[i];
				_mem_address[i*4 + 1] = _mem_address_col[i] + 1;
				_mem_address[i*4 + 2] = _mem_address_col[i] + 2;
				_mem_address[i*4 + 3] = _mem_address_col[i] + 3;
				_mem_draw[i] = (unsigned char *) malloc(
					_max_n_byte_grafica * sizeof(unsigned char)
				);
				if(!_mem_draw[i])
					return SEAV_ERR_MEMORY;
				_primo_byte_libero[i] = 0L;
			}
		} else {
			for(i=_param_scheda.fb_min; i<=_param_scheda.fb_max; i++) {
				if(MbufAlloc2d(
					_mil_system[0], _param_scheda.lung_riga,
					_param_scheda.lung_colonna,
					8 + M_UNSIGNED, M_IMAGE+M_GRAB, _mil_image + i
				) == M_NULL)
					return SEAV_ERR_GRABBER_MEMORY;
				MbufInquire(
					_mil_image[i], M_HOST_ADDRESS, (long *) (_mem_address+i)
				);
				_mem_draw[i] = (unsigned char *) malloc(
					_max_n_byte_grafica * sizeof(unsigned char)
				);
				if(!_mem_draw[i])
					return SEAV_ERR_MEMORY;
				_primo_byte_libero[i] = 0L;
			}
		}

/* inizializzazione delle ROI; per default non si abilita la roi */

		for(i=0; i<N_FB_MAX; i++) {
			_roi_fb[i].fb = i;
			_roi_fb[i].xi = _param_scheda.x_min;
			_roi_fb[i].xf = _param_scheda.x_max;
			_roi_fb[i].yi = _param_scheda.y_min;
			_roi_fb[i].yf = _param_scheda.y_max;
			_roi_enable_display[i] = 0;
			_roi_enable_acq[i] = 0;
		}

		if(tipo_tv == TV_RGB) {
			for(i=0; i<N_FB_MAX_COL; i++) {
				_roi_fb_col[i].fb = i;
				_roi_fb_col[i].xi = _param_scheda.x_min;
				_roi_fb_col[i].xf = _param_scheda.x_max;
				_roi_fb_col[i].yi = _param_scheda.y_min;
				_roi_fb_col[i].yf = _param_scheda.y_max;
				_roi_enable_display_col[i] = 0;
				_roi_enable_acq_col[i] = 0;
			}
		}

/* start grab */

#if 0
		if(tipo_tv == TV_RGB)
			MdigGrabContinuous(_mil_camera[0], _mil_image_col[0]);
		else
			MdigGrabContinuous(_mil_camera[0], _mil_image[0]);
		for(j=0; j<10; j++)
			sea2_wait_v_blank();
		MdigHalt(_mil_camera[0]);
#endif

/* allocazione di _aus_bitmap per la scrittura della bitmap */

		hdc = GetDC((HWND) NULL);
		i = GetDeviceCaps(hdc, BITSPIXEL);
		ReleaseDC((HWND) NULL, hdc);
		switch(i) {
			case 8:
				n_byte = 1L;
				break;
			case 16:
				n_byte = 2L;
				break;
			case 24:
				n_byte = 3L;
				break;
			case 32:
				n_byte = 4L;
				break;
			default:
				return SEAV_ERR_VGA_MODE_NOT_SUPPORTED;
		}

		_aus_bitmap = (unsigned char *) malloc(
			(long)_param_scheda.lung_riga *
			(long)_param_scheda.lung_colonna * n_byte * sizeof(unsigned char)
		);
		if(!_aus_bitmap)
			return SEAV_ERR_MEMORY;
		sea2_clear_bitmap();

/* allocazione di _offset_riga_display: contiene l'offset del primo pixel di
   ogni riga di _aus_bitmap */

		_offset_riga_display = (long *) malloc(
			_param_scheda.lung_colonna * sizeof(long)
		);
		if(!_offset_riga_display)
			return SEAV_ERR_MEMORY;
		for(j=0; j<_param_scheda.lung_colonna; j++)
			_offset_riga_display[j] = (long) j *
			(long) _param_scheda.lung_riga * n_byte;

/* allocazione di _offset_colonna_display: contiene l'offset del primo pixel di
   ogni colonna di _aus_bitmap */

		_offset_colonna_display = (long *) malloc(
			_param_scheda.lung_riga * sizeof(long)
		);
		if(!_offset_colonna_display)
			return SEAV_ERR_MEMORY;
		for(j=0; j<_param_scheda.lung_riga; j++)
			_offset_colonna_display[j] = (long) j * n_byte;

/* allocazione di _offset_riga: contiene l'offset del primo pixel di
   ogni riga in memoria */

		_offset_riga = (long *) malloc(
			_param_scheda.lung_colonna * sizeof(long)
		);
		if(!_offset_riga)
			return SEAV_ERR_MEMORY;
		if(tipo_tv == TV_RGB) {
			for(j=0; j<_param_scheda.lung_colonna; j++)
				_offset_riga[j] = (long) j * (long) _param_scheda.lung_riga * 4L;
		} else {
			for(j=0; j<_param_scheda.lung_colonna; j++)
				_offset_riga[j] = (long) j * (long) _param_scheda.lung_riga;
		}               
	}               

#endif /* MET_MIL */

#ifdef ORION
	{
		long j, n_byte;
		HDC hdc;
		MIL_ID ret;
		char *dcf;

/* allocazione dell'applicazione */

		if(MappAlloc(M_DEFAULT, &_mil_application) == M_NULL)
			return SEAV_ERR_GRABBER_INIT;

/* disabilito il check dei parametri */

		MappControl(M_PARAMETER, M_CHECK_DISABLE);

/* disabilito il display degli errori */

		MappControl(M_ERROR, M_PRINT_DISABLE);

/* disabilito l'interrupt */

		//MappControl(M_IRQ_CONTROL, M_DISABLE); ?

/* allocazione del sistema di default (0) */

		if(MsysAlloc(M_SYSTEM_ORION, M_DEV0, M_COMPLETE, _mil_system) == M_NULL)
			return SEAV_ERR_GRABBER_INIT;

/* allocazione del digitizer di default (0) */

		if(nome_dcf) {
			dcf = nome_dcf;
		} else {
			if(tipo_tv == TV_RGB) {
				if(standard_tv == STANDARD_AM)
					dcf = "M_NTSC_RGB";
				else
					dcf = "M_PAL_RGB";
			} else {
				if(standard_tv == STANDARD_AM)
					dcf = "M_RS170";
				else
					dcf = "M_CCIR";
			}
		}
		ret = MdigAlloc(
			_mil_system[0], M_DEV0, dcf, M_DEFAULT, _mil_camera
		);
		if(ret == M_NULL)
			return SEAV_ERR_GRABBER_INIT;

/* selezione del canale di ingresso di default */

		//MdigChannel(_mil_camera[0], M_CH1);
//MessageBox((HWND) NULL, "1", NULL, MB_OK|MB_APPLMODAL);

/* si fa si' che le funzioni escano subito dopo aver lanciato il grab */
	
		MdigControl(_mil_camera[0], M_GRAB_MODE, M_ASYNCHRONOUS);
	
/* si acquisisce un solo frame */
	
		MdigControl(_mil_camera[0], M_GRAB_FRAME_NUM, 1.0);
	
/* la scheda e' limitata a 2 field */
	
		MdigControl(_mil_camera[0], M_GRAB_FIELD_NUM, 2.0);

/* aggiungo il controllo sullo start grab: il grab parte su qualunque field */

		MdigControl(_mil_camera[0], M_GRAB_START_MODE, M_FIELD_START);

/* aggiungo il controllo sullo stop grab: il grab si ferma su qualunque field */

		MdigControl(_mil_camera[0], M_GRAB_HALT_ON_NEXT_FIELD, M_ENABLE);

/* aggiungo il controllo sul trigger */

		MdigControl(_mil_camera[0], M_GRAB_TRIGGER_SOURCE, M_NULL);

/* aggiungo il controllo sul timeout di acq */

		//MdigControl(_mil_camera[0], M_GRAB_TIMEOUT, 500);

/* aggiungo il controllo sulla lut di ingresso */

		MdigLut(_mil_camera[0], M_DEFAULT);

		if(tipo_tv == TV_RGB) {

/* si cambiano i livelli di digitalizzazione per evitare i valori 0-10
   245-255; i default sono 80 e 160 */
	
			//MdigReference(_mil_camera[0], M_BLACK_REF, 90L);
			//MdigReference(_mil_camera[0], M_WHITE_REF, 195L);
		} else {

/* si clippa la dinamica di ingresso alla scheda dalla telecamera tra 10
   e 245: non funziona per l'RGB; forse e' meglio riprogrammare il
   digitizer come sopra; */
	
			MdigControl(_mil_camera[0], M_GRAB_WINDOWS_RANGE, M_ENABLE);
		}

/* serve se la fase orizzontale della telecamera e' disturbata */
	
		//MdigControl(_mil_camera[0], M_VCR_INPUT_TYPE, M_ENABLE); ?

/* non si fa il direct_vga */

		_direct_vga_possible = 0;
		_mode_direct_vga = 0;
		_vga_off_x = 0;  
		_vga_off_y = 0; 

/* per default non si fa lo zoom */

		_zoom_mode = 0;

/* per default in modo zoom non si visualizza niente */

		_fb_zoom[0] = _fb_zoom[1] = _fb_zoom[2] = _fb_zoom[3] = -1;

/* inizializzazione della grafica: per default si abilitano sia
   l'overlay che l'underlay; per default si usano in overlay i colori delle
   8 penne di default; e' comunque possibile usare i colori veri settando
   _colori_approssimati a COLORI_VERI tramite la routine sea2_set_color_mode,
   ma solo con il display a colori */

		_overlay_mode = UNDERLAY_MODE | OVERLAY_MODE;
		_colori_approssimati = COLORI_APPROSSIMATI;
		_max_n_byte_grafica = (long) _param_scheda.lung_riga *
			(long) _param_scheda.lung_colonna;

/* allocazione dei frame buffer di default insieme con i buffer per la grafica */

		if(tipo_tv == TV_RGB) {
			_param_scheda.fb_max = _param_scheda.fb_dma_max =
				(_param_scheda.fb_max_col - _param_scheda.fb_min_col + 1) * 4 - 1 +
				_param_scheda.fb_min;
			for(i=_param_scheda.fb_min_col; i<=_param_scheda.fb_max_col; i++) {
				if(MbufAllocColor(
					_mil_system[0], 3L, _param_scheda.lung_riga,
					_param_scheda.lung_colonna,
					8 + M_UNSIGNED, M_IMAGE+M_GRAB+M_BGR32+M_PACKED,
					_mil_image_col + i
				) == M_NULL)
					return SEAV_ERR_GRABBER_MEMORY;
				MbufInquire(
					_mil_image_col[i], M_HOST_ADDRESS, (long *) (_mem_address_col+i)
				);
				_mem_address[i*4] = _mem_address_col[i];
				_mem_address[i*4 + 1] = _mem_address_col[i] + 1;
				_mem_address[i*4 + 2] = _mem_address_col[i] + 2;
				_mem_address[i*4 + 3] = _mem_address_col[i] + 3;
				_mem_draw[i] = (unsigned char *) malloc(
					_max_n_byte_grafica * sizeof(unsigned char)
				);
				if(!_mem_draw[i])
					return SEAV_ERR_MEMORY;
				_primo_byte_libero[i] = 0L;
			}
		} else {
			for(i=_param_scheda.fb_min; i<=_param_scheda.fb_max; i++) {
				if(MbufAlloc2d(
					_mil_system[0], _param_scheda.lung_riga,
					_param_scheda.lung_colonna,
					8 + M_UNSIGNED, M_IMAGE+M_GRAB, _mil_image + i
				) == M_NULL)
					return SEAV_ERR_GRABBER_MEMORY;
				MbufInquire(
					_mil_image[i], M_HOST_ADDRESS, (long *) (_mem_address+i)
				);
				_mem_draw[i] = (unsigned char *) malloc(
					_max_n_byte_grafica * sizeof(unsigned char)
				);
				if(!_mem_draw[i])
					return SEAV_ERR_MEMORY;
				_primo_byte_libero[i] = 0L;
			}
		}

/* inizializzazione delle ROI; per default non si abilita la roi */

		for(i=0; i<N_FB_MAX; i++) {
			_roi_fb[i].fb = i;
			_roi_fb[i].xi = _param_scheda.x_min;
			_roi_fb[i].xf = _param_scheda.x_max;
			_roi_fb[i].yi = _param_scheda.y_min;
			_roi_fb[i].yf = _param_scheda.y_max;
			_roi_enable_display[i] = 0;
			_roi_enable_acq[i] = 0;
		}

		if(tipo_tv == TV_RGB) {
			for(i=0; i<N_FB_MAX_COL; i++) {
				_roi_fb_col[i].fb = i;
				_roi_fb_col[i].xi = _param_scheda.x_min;
				_roi_fb_col[i].xf = _param_scheda.x_max;
				_roi_fb_col[i].yi = _param_scheda.y_min;
				_roi_fb_col[i].yf = _param_scheda.y_max;
				_roi_enable_display_col[i] = 0;
				_roi_enable_acq_col[i] = 0;
			}
		}

/* start grab */

#if 0
		if(tipo_tv == TV_RGB)
			MdigGrabContinuous(_mil_camera[0], _mil_image_col[0]);
		else
			MdigGrabContinuous(_mil_camera[0], _mil_image[0]);
		for(j=0; j<10; j++)
			sea2_wait_v_blank();
		MTdigHalt(_mil_camera[0]);
#endif

/* allocazione di _aus_bitmap per la scrittura della bitmap */

		hdc = GetDC((HWND) NULL);
		i = GetDeviceCaps(hdc, BITSPIXEL);
		ReleaseDC((HWND) NULL, hdc);
		switch(i) {
			case 8:
				n_byte = 1L;
				break;
			case 16:
				n_byte = 2L;
				break;
			case 24:
				n_byte = 3L;
				break;
			case 32:
				n_byte = 4L;
				break;
			default:
				return SEAV_ERR_VGA_MODE_NOT_SUPPORTED;
		}

		_aus_bitmap = (unsigned char *) malloc(
			(long)_param_scheda.lung_riga *
			(long)_param_scheda.lung_colonna * n_byte * sizeof(unsigned char)
		);
		if(!_aus_bitmap)
			return SEAV_ERR_MEMORY;
		sea2_clear_bitmap();

/* allocazione di _offset_riga_display: contiene l'offset del primo pixel di
   ogni riga di _aus_bitmap */

		_offset_riga_display = (long *) malloc(
			_param_scheda.lung_colonna * sizeof(long)
		);
		if(!_offset_riga_display)
			return SEAV_ERR_MEMORY;
		for(j=0; j<_param_scheda.lung_colonna; j++)
			_offset_riga_display[j] = (long) j *
			(long) _param_scheda.lung_riga * n_byte;

/* allocazione di _offset_colonna_display: contiene l'offset del primo pixel di
   ogni colonna di _aus_bitmap */

		_offset_colonna_display = (long *) malloc(
			_param_scheda.lung_riga * sizeof(long)
		);
		if(!_offset_colonna_display)
			return SEAV_ERR_MEMORY;
		for(j=0; j<_param_scheda.lung_riga; j++)
			_offset_colonna_display[j] = (long) j * n_byte;

/* allocazione di _offset_riga: contiene l'offset del primo pixel di
   ogni riga in memoria */

		_offset_riga = (long *) malloc(
			_param_scheda.lung_colonna * sizeof(long)
		);
		if(!_offset_riga)
			return SEAV_ERR_MEMORY;
		if(tipo_tv == TV_RGB) {
			for(j=0; j<_param_scheda.lung_colonna; j++)
				_offset_riga[j] = (long) j * (long) _param_scheda.lung_riga * 4L;
		} else {
			for(j=0; j<_param_scheda.lung_colonna; j++)
				_offset_riga[j] = (long) j * (long) _param_scheda.lung_riga;
		}               
	}               

#endif /* ORION */

#ifdef METEOR2
	{
//long pippo, latenza;
		long j, n_byte;
		HDC hdc;
		MIL_ID ret;
		char *dcf;

/* allocazione dell'applicazione */

		if(MappAlloc(M_DEFAULT, &_mil_application) == M_NULL)
			return SEAV_ERR_GRABBER_INIT;

/* disabilito il check dei parametri */

		MappControl(M_PARAMETER, M_CHECK_DISABLE);
//		MappControl(M_PARAMETER, M_CHECK_ENABLE);

/* disabilito il display degli errori */

		MappControl(M_ERROR, M_PRINT_DISABLE);
//		MappControl(M_ERROR, M_PRINT_ENABLE);

/* disabilito l'interrupt */

		//MappControl(M_IRQ_CONTROL, M_DISABLE); ?

/* allocazione del sistema di default (0) */

		if(MsysAlloc(M_SYSTEM_METEOR_II, M_DEV0, M_COMPLETE, _mil_system) == M_NULL)
			return SEAV_ERR_GRABBER_INIT;

/* imposto la latenza a 8 (il default e' 128) altrimenti la meteor ruba
   troppo tempo per il trasferimento sul bus e gli altri accessi da CPU
   (per leggere l'I/O o il timer di sistema) vengono stoppati; con latenza
   troppo bassa (per esempio a 0) l'acquisizione dura troppo perche'
   la meteor bufferizza in memoria on-board e poi trasferisce sul bus;
   l'overhead in questo caso e' di circa 50 ms; con latenza a 8 l'overhead e'
   di pochi ms; probabilmente con 2 schede in parallelo bisogna aumentare la
   latenza a scapito della frequenza del plc interno; forse converra' fare
   una funzione per impostare la latenza a seconda dei casi; */

		MsysControl(_mil_system[0], M_PCI_LATENCY, 8);
		//latenza = MsysInquire(_mil_system[0], M_PCI_LATENCY, M_NULL);
		//sprintf(stringa, "%ld", latenza);
		//MessageBox((HWND) NULL, stringa, NULL, MB_OK|MB_APPLMODAL);

/* allocazione del digitizer di default (0) */

		if(nome_dcf) {
			dcf = nome_dcf;
		} else {
			if(tipo_tv == TV_RGB) {
				if(standard_tv == STANDARD_AM)
					dcf = "M_NTSC_RGB";
				else
					dcf = "M_PAL_RGB";
			} else {
				if(standard_tv == STANDARD_AM)
					dcf = "M_RS170";
				else
					dcf = "M_CCIR";
			}
		}
		ret = MdigAlloc(
			_mil_system[0], M_DEV0, dcf, M_DEFAULT, _mil_camera
		);
		if(ret == M_NULL)
			return SEAV_ERR_GRABBER_INIT;

/* selezione del canale di ingresso di default */

//MessageBox((HWND) NULL, "1", NULL, MB_OK|MB_APPLMODAL);
		if(tipo_tv == TV_RGB)
			MdigChannel(_mil_camera[0], M_CH0);
		else
			MdigChannel(_mil_camera[0], M_CH1);

/* si fa si' che le funzioni escano subito dopo aver lanciato il grab */
	
		MdigControl(_mil_camera[0], M_GRAB_MODE, M_ASYNCHRONOUS);
//pippo = MdigInquire(_mil_camera[0], M_GRAB_MODE, M_NULL);
/* si acquisisce un solo frame */
	
		//MdigControl(_mil_camera[0], M_GRAB_FRAME_NUM, 1);
	
/* si acquisiscono due field; se si specifica 1, esso viene messo sempre
   a partire dalla riga 0 del frame buffer a prescindere se sia pari o dispari */
	
#if 1
		MdigControl(_mil_camera[0], M_GRAB_FIELD_NUM, 2);

/* aggiungo il controllo sullo start grab: il grab parte su qualunque field */

		MdigControl(_mil_camera[0], M_GRAB_START_MODE, M_FIELD_START);

/* aggiungo il controllo sullo stop grab: il grab si ferma su qualunque field */

		MdigControl(_mil_camera[0], M_GRAB_HALT_ON_NEXT_FIELD, M_ENABLE);

/* aggiungo il controllo sul trigger e sull'esposizione */

		MdigControl(_mil_camera[0], M_GRAB_TRIGGER, M_DISABLE);
		MdigControl(_mil_camera[0], M_GRAB_TRIGGER_SOURCE, M_NULL);
		MdigControl(_mil_camera[0], M_GRAB_EXPOSURE_BYPASS, M_ENABLE);
//		MdigControl(_mil_camera[0], M_GRAB_EXPOSURE+M_TIMER1, M_ENABLE);
//		MdigControl(_mil_camera[0], M_GRAB_EXPOSURE+M_TIMER2, M_ENABLE);
		MdigControl(_mil_camera[0], M_GRAB_EXPOSURE_SOURCE+M_TIMER1, M_SOFTWARE);
		MdigControl(_mil_camera[0], M_GRAB_EXPOSURE_SOURCE+M_TIMER2, M_TIMER1);
		MdigControl(_mil_camera[0], M_GRAB_EXPOSURE_MODE+M_TIMER1, M_DEFAULT); // modifica 2.53.0 OCV: da M_LEVEL_LOW a M_DEFAULT
		MdigControl(_mil_camera[0], M_GRAB_EXPOSURE_MODE+M_TIMER2, M_DEFAULT);
#endif

/* aggiungo il controllo sulla lut di ingresso */

		MdigLut(_mil_camera[0], M_DEFAULT);

		if(tipo_tv == TV_RGB) {

/* si cambiano i livelli di digitalizzazione per evitare i valori 0-10
   245-255; i default sono 80 e 160 */
	
			//MdigReference(_mil_camera[0], M_BLACK_REF, 90L);
			//MdigReference(_mil_camera[0], M_WHITE_REF, 195L);
		} else {

/* si clippa la dinamica di ingresso alla scheda dalla telecamera tra 10
   e 245: non funziona per l'RGB; forse e' meglio riprogrammare il
   digitizer come sopra; */
	
			MdigControl(_mil_camera[0], M_GRAB_WINDOWS_RANGE, M_ENABLE);
		}

/* serve se la fase orizzontale della telecamera e' disturbata */
	
		//MdigControl(_mil_camera[0], M_VCR_INPUT_TYPE, M_ENABLE); ?

/* non si fa il direct_vga */

		_direct_vga_possible = 0;
		_mode_direct_vga = 0;
		_vga_off_x = 0;  
		_vga_off_y = 0; 

/* per default non si fa lo zoom */

		_zoom_mode = 0;

/* per default in modo zoom non si visualizza niente */

		_fb_zoom[0] = _fb_zoom[1] = _fb_zoom[2] = _fb_zoom[3] = -1;

/* inizializzazione della grafica: per default si abilitano sia
   l'overlay che l'underlay; per default si usano in overlay i colori delle
   8 penne di default; e' comunque possibile usare i colori veri settando
   _colori_approssimati a COLORI_VERI tramite la routine sea2_set_color_mode,
   ma solo con il display a colori */

		_overlay_mode = UNDERLAY_MODE | OVERLAY_MODE;
		_colori_approssimati = COLORI_APPROSSIMATI;
		_max_n_byte_grafica = (long) _param_scheda.lung_riga *
			(long) _param_scheda.lung_colonna;

/* allocazione dei frame buffer di default insieme con i buffer per la grafica */

		if(tipo_tv == TV_RGB) {
			_param_scheda.fb_max = _param_scheda.fb_dma_max =
				(_param_scheda.fb_max_col - _param_scheda.fb_min_col + 1) * 4 - 1 +
				_param_scheda.fb_min;
			for(i=_param_scheda.fb_min_col; i<=_param_scheda.fb_max_col; i++) {
				if(MbufAllocColor(
					_mil_system[0], 3L, _param_scheda.lung_riga,
					_param_scheda.lung_colonna,
					8 + M_UNSIGNED, M_IMAGE+M_GRAB+M_BGR32+M_PACKED+M_NON_PAGED+M_OFF_BOARD,
					_mil_image_col + i
				) == M_NULL)
					return SEAV_ERR_GRABBER_MEMORY;
				MbufInquire(
					_mil_image_col[i], M_HOST_ADDRESS, (long *) (_mem_address_col+i)
				);
				_mem_address[i*4] = _mem_address_col[i];
				_mem_address[i*4 + 1] = _mem_address_col[i] + 1;
				_mem_address[i*4 + 2] = _mem_address_col[i] + 2;
				_mem_address[i*4 + 3] = _mem_address_col[i] + 3;
				_mem_draw[i] = (unsigned char *) malloc(
					_max_n_byte_grafica * sizeof(unsigned char)
				);
				if(!_mem_draw[i])
					return SEAV_ERR_MEMORY;
				_primo_byte_libero[i] = 0L;
			}
		} else {
			for(i=_param_scheda.fb_min; i<=_param_scheda.fb_max; i++) {
				if(MbufAlloc2d(
					_mil_system[0], _param_scheda.lung_riga,
					_param_scheda.lung_colonna,
					8 + M_UNSIGNED, M_IMAGE+M_GRAB+M_NON_PAGED+M_OFF_BOARD, _mil_image + i
				) == M_NULL)
					return SEAV_ERR_GRABBER_MEMORY;
				MbufInquire(
					_mil_image[i], M_HOST_ADDRESS, (long *) (_mem_address+i)
				);
				_mem_draw[i] = (unsigned char *) malloc(
					_max_n_byte_grafica * sizeof(unsigned char)
				);
				if(!_mem_draw[i])
					return SEAV_ERR_MEMORY;
				_primo_byte_libero[i] = 0L;
			}
		}

/* inizializzazione delle ROI; per default non si abilita la roi */

		for(i=0; i<N_FB_MAX; i++) {
			_roi_fb[i].fb = i;
			_roi_fb[i].xi = _param_scheda.x_min;
			_roi_fb[i].xf = _param_scheda.x_max;
			_roi_fb[i].yi = _param_scheda.y_min;
			_roi_fb[i].yf = _param_scheda.y_max;
			_roi_enable_display[i] = 0;
			_roi_enable_acq[i] = 0;
		}

		if(tipo_tv == TV_RGB) {
			for(i=0; i<N_FB_MAX_COL; i++) {
				_roi_fb_col[i].fb = i;
				_roi_fb_col[i].xi = _param_scheda.x_min;
				_roi_fb_col[i].xf = _param_scheda.x_max;
				_roi_fb_col[i].yi = _param_scheda.y_min;
				_roi_fb_col[i].yf = _param_scheda.y_max;
				_roi_enable_display_col[i] = 0;
				_roi_enable_acq_col[i] = 0;
			}
		}

/* allocazione di _aus_bitmap per la scrittura della bitmap */

		hdc = GetDC((HWND) NULL);
		i = GetDeviceCaps(hdc, BITSPIXEL);
		ReleaseDC((HWND) NULL, hdc);
		switch(i) {
			case 8:
				n_byte = 1L;
				break;
			case 16:
				n_byte = 2L;
				break;
			case 24:
				n_byte = 3L;
				break;
			case 32:
				n_byte = 4L;
				break;
			default:
				return SEAV_ERR_VGA_MODE_NOT_SUPPORTED;
		}

		_aus_bitmap = (unsigned char *) malloc(
			(long)_param_scheda.lung_riga *
			(long)_param_scheda.lung_colonna * n_byte * sizeof(unsigned char)
		);
		if(!_aus_bitmap)
			return SEAV_ERR_MEMORY;
		sea2_clear_bitmap();

/* allocazione di _offset_riga_display: contiene l'offset del primo pixel di
   ogni riga di _aus_bitmap */

		_offset_riga_display = (long *) malloc(
			_param_scheda.lung_colonna * sizeof(long)
		);
		if(!_offset_riga_display)
			return SEAV_ERR_MEMORY;
		for(j=0; j<_param_scheda.lung_colonna; j++)
			_offset_riga_display[j] = (long) j *
			(long) _param_scheda.lung_riga * n_byte;

/* allocazione di _offset_colonna_display: contiene l'offset del primo pixel di
   ogni colonna di _aus_bitmap */

		_offset_colonna_display = (long *) malloc(
			_param_scheda.lung_riga * sizeof(long)
		);
		if(!_offset_colonna_display)
			return SEAV_ERR_MEMORY;
		for(j=0; j<_param_scheda.lung_riga; j++)
			_offset_colonna_display[j] = (long) j * n_byte;

/* allocazione di _offset_riga: contiene l'offset del primo pixel di
   ogni riga in memoria */

		_offset_riga = (long *) malloc(
			_param_scheda.lung_colonna * sizeof(long)
		);
		if(!_offset_riga)
			return SEAV_ERR_MEMORY;
		if(tipo_tv == TV_RGB) {
			for(j=0; j<_param_scheda.lung_colonna; j++)
				_offset_riga[j] = (long) j * (long) _param_scheda.lung_riga * 4L;
		} else {
			for(j=0; j<_param_scheda.lung_colonna; j++)
				_offset_riga[j] = (long) j * (long) _param_scheda.lung_riga;
		}               
	}               

#endif /* METEOR2 */

	_param_scheda.scheda_aperta = 1;
	return SEAV_OK_ALL;
}

/******************************************************************/

/* inizializza n_board aggiuntive oltre a quella di default; come la
   sea2_init() puo' essere lanciata una sola volta; per poterla lanciare
   una seconda volta e' necessario chiamare prima la sea2_close();
   ritorna OK o BOARD_ERROR;
   i buffer di memoria sono allocati sempre al system 0 (di default)
   perche' e' possibile acquisire in ogni caso su qualsiasi buffer da
   qualsiasi system; */

short
sea2_init_multi_board(
	short n_board,
	char *nome_dcf // puo' essere NULL
)
{
#if defined(MET) || defined(MET_MIL) || defined(METEOR2)
	short i;
	char *dcf;
#endif
#if defined(MET) || defined(MET_MIL)
	short j;
#endif

	if(!_param_scheda.scheda_aperta)
		return SEAV_ERR_GRABBER_INIT;
	if(n_board + 1 > N_BOARD_MAX)
		return SEAV_ERR_PARAMETER;
	if(!n_board)
		return SEAV_OK_ALL;

	_param_scheda.scheda_aperta = 0;

#ifdef MET
	for(i=1; i<n_board+1; i++) {

		/* allocazione del sistema */
		if(!MTsysAlloc(M_DEFAULT, (long) i, M_DEFAULT, _mil_system+i))
			return SEAV_ERR_GRABBER_INIT;

		/* allocazione del digitizer */
		if(_param_scheda.tipo_tv == TV_RGB) {
			if(_param_scheda.standard_tv == STANDARD_AM) {
				if(!MTdigAlloc(
					_mil_system[i], (long) i, "M_NTSC_RGB", M_DEFAULT, _mil_camera+i
				))
					return SEAV_ERR_GRABBER_INIT;
			} else {
				if(!MTdigAlloc(
					_mil_system[i], (long) i, "M_PAL_RGB", M_DEFAULT, _mil_camera+i
				))
					return SEAV_ERR_GRABBER_INIT;
			}
		} else {
			if(_param_scheda.standard_tv == STANDARD_AM) {
				if(!MTdigAlloc(
					_mil_system[i], (long) i, "M_RS170", M_DEFAULT, _mil_camera+i
				))
					return SEAV_ERR_GRABBER_INIT;
			} else {
				if(!MTdigAlloc(
					_mil_system[i], (long) i, "M_CCIR", M_DEFAULT, _mil_camera+i
				))
					return SEAV_ERR_GRABBER_INIT;
			}
		}

		/* selezione del canale di ingresso di default */
		MTdigChannel(_mil_camera[i], M_CH1);

		/* si fa si' che le funzioni escano subito dopo aver lanciato il grab */
		MTdigControl(_mil_camera[i], M_GRAB_MODE, M_ASYNCHRONOUS);
	
		/* si acquisisce un solo frame */
		MTdigControl(_mil_camera[i], M_GRAB_FRAME_NUM, 1.0);
	
		/* si acquisisce un solo field */
		MTdigControl(_mil_camera[i], M_GRAB_FIELD_NUM, 1.0);
	
		if(_param_scheda.tipo_tv == TV_RGB) {
			/* si cambiano i livelli di digitalizzazione per evitare i valori 0-10
			   245-255; i default sono 80 e 160 */
			MTdigReference(_mil_camera[i], M_BLACK_REF, 90L);
			MTdigReference(_mil_camera[i], M_WHITE_REF, 195L);
		} else {

			/* si clippa la dinamica di ingresso alla scheda dalla telecamera tra 10
			   e 245: non funziona per l'RGB; forse e' meglio riprogrammare il
			   digitizer come sopra; */
			MTdigControl(_mil_camera[i], M_GRAB_WINDOWS_RANGE, M_ENABLE);
		}

		/* serve se la fase orizzontale della telecamera e' disturbata */
		MTdigControl(_mil_camera[i], M_VCR_INPUT_TYPE, M_ENABLE);

		/* start grab */
		if(_param_scheda.tipo_tv == TV_RGB)
			MTdigGrabContinuous(_mil_camera[i], _mil_image_col[0]);
		else
			MTdigGrabContinuous(_mil_camera[i], _mil_image[0]);
		for(j=0; j<10; j++)
			sea2_wait_v_blank();
		sea2_select_board(i);
		sea2_grab_disable();
	}
	sea2_select_board(0);
	_param_scheda.scheda_max = n_board;
#endif /* MET */

#ifdef MET_MIL
	for(i=1; i<n_board+1; i++) {

		/* allocazione del sistema */
		if(!MsysAlloc(M_SYSTEM_METEOR, (long) i, M_DEFAULT, _mil_system+i))
			return SEAV_ERR_GRABBER_INIT;

		/* allocazione del digitizer */
		if(_param_scheda.tipo_tv == TV_RGB) {
			if(_param_scheda.standard_tv == STANDARD_AM) {
				if(!MdigAlloc(
					_mil_system[i], (long) i, "M_NTSC_RGB", M_DEFAULT, _mil_camera+i
				))
					return SEAV_ERR_GRABBER_INIT;
			} else {
				if(!MdigAlloc(
					_mil_system[i], (long) i, "M_PAL_RGB", M_DEFAULT, _mil_camera+i
				))
					return SEAV_ERR_GRABBER_INIT;
			}
		} else {
			if(_param_scheda.standard_tv == STANDARD_AM) {
				if(!MdigAlloc(
					_mil_system[i], (long) i, "M_RS170", M_DEFAULT, _mil_camera+i
				))
					return SEAV_ERR_GRABBER_INIT;
			} else {
				if(!MdigAlloc(
					_mil_system[i], (long) i, "M_CCIR", M_DEFAULT, _mil_camera+i
				))
					return SEAV_ERR_GRABBER_INIT;
			}
		}

		/* selezione del canale di ingresso di default */
		MdigChannel(_mil_camera[i], M_CH1);

		/* si fa si' che le funzioni escano subito dopo aver lanciato il grab */
		MdigControl(_mil_camera[i], M_GRAB_MODE, M_ASYNCHRONOUS);
	
		/* si acquisisce un solo frame */
		MdigControl(_mil_camera[i], M_GRAB_FRAME_NUM, 1.0);
	
		if(_param_scheda.tipo_tv == TV_RGB) {
			/* si cambiano i livelli di digitalizzazione per evitare i valori 0-10
			   245-255; i default sono 80 e 160 */
//			MdigReference(_mil_camera[i], M_BLACK_REF, 90L);
//			MdigReference(_mil_camera[i], M_WHITE_REF, 195L);
		} else {

			/* si clippa la dinamica di ingresso alla scheda dalla telecamera tra 10
			   e 245: non funziona per l'RGB; forse e' meglio riprogrammare il
			   digitizer come sopra; */
			MdigControl(_mil_camera[i], M_GRAB_WINDOWS_RANGE, M_ENABLE);
		}

		/* serve se la fase orizzontale della telecamera e' disturbata */
		MdigControl(_mil_camera[i], M_VCR_INPUT_TYPE, M_ENABLE);

#if 0
		/* start grab */
		if(_param_scheda.tipo_tv == TV_RGB)
			MdigGrabContinuous(_mil_camera[i], _mil_image_col[0]);
		else
			MdigGrabContinuous(_mil_camera[i], _mil_image[0]);
		for(j=0; j<10; j++)
			sea2_wait_v_blank();
		sea2_select_board(i);
		sea2_grab_disable();
#endif
	}
	sea2_select_board(0);
	_param_scheda.scheda_max = n_board;
#endif /* MET_MIL */

#ifdef ORION
	if(n_board)
		return SEAV_ERR_BOARD;
#endif /* ORION */

#ifdef METEOR2

/* imposto la latenza a 8 (il default e' 128) altrimenti la meteor ruba
   troppo tempo per il trasferimento sul bus e gli altri accessi da CPU
   (per leggere l'I/O o il timer di sistema) vengono stoppati; con latenza
   troppo bassa (per esempio a 0) l'acquisizione dura troppo perche'
   la meteor bufferizza in memoria on-board e poi trasferisce sul bus;
   l'overhead in questo caso e' di circa 50 ms; con latenza a 8 l'overhead e'
   di pochi ms; probabilmente con 2 schede in parallelo bisogna aumentare la
   latenza a scapito della frrequenza del plc interno; forse converra' fare
   una funzione per impostare la latenza a seconda dei casi; */

	if(nome_dcf) {
		dcf = nome_dcf;
	} else {
		if(_param_scheda.tipo_tv == TV_RGB) {
			if(_param_scheda.standard_tv == STANDARD_AM)
				dcf = "M_NTSC_RGB";
			else
				dcf = "M_PAL_RGB";
		} else {
			if(_param_scheda.standard_tv == STANDARD_AM)
				dcf = "M_RS170";
			else
				dcf = "M_CCIR";
		}
	}
	for(i=1; i<n_board+1; i++) {

		/* allocazione del sistema */
		if(!MsysAlloc(M_SYSTEM_METEOR_II, (long) i, M_COMPLETE, _mil_system+i))
			return SEAV_ERR_GRABBER_INIT;

		/* imposto la latenza a 8 (il default e' 128) vedi commento nella init */
		MsysControl(_mil_system[i], M_PCI_LATENCY, 8);

		/* allocazione del digitizer 2.54.0 OCV: corretto un baco: si mette M_DEV0
		   al posto di i: si alloca sempre il primo digitizer della scheda */
		if(!MdigAlloc(
			_mil_system[i], M_DEV0, dcf, M_DEFAULT, _mil_camera+i
		))
			return SEAV_ERR_GRABBER_INIT;


		/* selezione del canale di ingresso di default */
		if(_param_scheda.tipo_tv == TV_RGB)
			MdigChannel(_mil_camera[i], M_CH0);
		else
			MdigChannel(_mil_camera[i], M_CH1);

		/* si fa si' che le funzioni escano subito dopo aver lanciato il grab */
		MdigControl(_mil_camera[i], M_GRAB_MODE, M_ASYNCHRONOUS);
	
		/* si acquisisce un solo frame */
		//MdigControl(_mil_camera[i], M_GRAB_FRAME_NUM, 1.0);
	
		/* si acquisiscono due field */
		MdigControl(_mil_camera[i], M_GRAB_FIELD_NUM, 2.0);

		/* aggiungo il controllo sullo start grab: il grab parte su qualunque field */
		MdigControl(_mil_camera[i], M_GRAB_START_MODE, M_FIELD_START);

		/* aggiungo il controllo sullo stop grab: il grab si ferma su qualunque field */
		MdigControl(_mil_camera[i], M_GRAB_HALT_ON_NEXT_FIELD, M_ENABLE);

		/* aggiungo il controllo sul trigger e sull'esposizione */
		MdigControl(_mil_camera[i], M_GRAB_TRIGGER, M_DISABLE);
		MdigControl(_mil_camera[i], M_GRAB_TRIGGER_SOURCE, M_NULL);
		MdigControl(_mil_camera[i], M_GRAB_EXPOSURE_BYPASS, M_ENABLE);
		MdigControl(_mil_camera[i], M_GRAB_EXPOSURE_SOURCE+M_TIMER1, M_SOFTWARE);
		MdigControl(_mil_camera[i], M_GRAB_EXPOSURE_MODE+M_TIMER1, M_DEFAULT); // modifica 2.53.0 OCV: da M_LEVEL_LOW a M_DEFAULT
		MdigControl(_mil_camera[i], M_GRAB_EXPOSURE_MODE+M_TIMER2, M_DEFAULT);

		/* aggiungo il controllo sulla lut di ingresso */
		MdigLut(_mil_camera[i], M_DEFAULT);

		if(_param_scheda.tipo_tv == TV_RGB) {
			/* si cambiano i livelli di digitalizzazione per evitare i valori 0-10
			   245-255; i default sono 80 e 160 */
//			MdigReference(_mil_camera[i], M_BLACK_REF, 90L);
//			MdigReference(_mil_camera[i], M_WHITE_REF, 195L);
		} else {

			/* si clippa la dinamica di ingresso alla scheda dalla telecamera tra 10
			   e 245: non funziona per l'RGB; forse e' meglio riprogrammare il
			   digitizer come sopra; */
			MdigControl(_mil_camera[i], M_GRAB_WINDOWS_RANGE, M_ENABLE);
		}

		/* serve se la fase orizzontale della telecamera e' disturbata */
		//MdigControl(_mil_camera[i], M_VCR_INPUT_TYPE, M_ENABLE); ??

	}
	sea2_select_board(0);
	_param_scheda.scheda_max = n_board;
#endif /* METEOR2 */

	_param_scheda.scheda_aperta = 1;
	return SEAV_OK_ALL;
}

/***********************************************************************

;  Nome:    
	sea2_close
;
;  Funzione:
     termina l'utilizzo della scheda
;
;  Formato:
	void sea2_close(void)
;
;**********************************************************************/

void
sea2_close(
	void
)
{
	short i;

	if(!_param_scheda.scheda_aperta)
		return;

#ifdef MET
	for(i=_param_scheda.scheda_min; i<=_param_scheda.scheda_max; i++)
		MTdigGrabStopMio(_mil_system[i]);
	sea2_wait_v_blank();
	sea2_wait_v_blank();
	sea2_wait_v_blank();
#endif // MET

#if defined(MET_MIL) || defined(ORION) || defined(METEOR2)
	for(i=_param_scheda.scheda_min; i<=_param_scheda.scheda_max; i++)
		MdigHalt(_mil_camera[i]);
	sea2_wait_v_blank();
	sea2_wait_v_blank();
	sea2_wait_v_blank();
#endif // MET_MIL

#if defined(MET) || defined(ASS) || defined(MET_MIL) || defined(ORION) || defined(METEOR2)
	free(_aus_bitmap);
	free(_offset_riga);
	free(_offset_riga_display);
	free(_offset_colonna_display);
	// 7.61-7: si fa la free di tutti
	for(i=0; i<N_FB_MAX; i++) {
		free(_mem_draw[i]);
		_mem_draw[i] = NULL;
	}
#ifndef __WATCOMC__
	if(_n_byte_per_pixel == 1) {
		SelectPalette(_hdc_memory, _old_palette, FALSE);
		SelectPalette(_hdc_screen, _old_palette, FALSE);
		DeleteObject(_hpalApp);
	}
	DeleteObject(SelectObject(_hdc_memory_row, _old_bitmap));
	DeleteDC(_hdc_memory_row);
	DeleteObject(_penna_nera);
	DeleteObject(_penna_bianca);
	DeleteObject(_penna_rossa);
	DeleteObject(_penna_verde);
	DeleteObject(_penna_gialla);
	DeleteObject(_penna_magenta);
	DeleteObject(_penna_ciano);
	DeleteObject(_penna_blu);
	DeleteObject(_brush_nero);
	DeleteObject(SelectObject(_hdc_memory, _old_bitmap));
	DeleteDC(_hdc_memory);
	ReleaseDC(_win_handle, _hdc_screen);
#else
	free(_bitmap);
	_setvideomode(_DEFAULTMODE);
#endif
#endif // MET || ASS || MET_MIL || ORION || METEOR2

#ifdef MET
	if(_param_scheda.tipo_tv == TV_RGB) {
		for(i=_param_scheda.fb_min_col; i<=_param_scheda.fb_max_col; i++)
			MTbufFree(_mil_image_col[i]);
	} else {
		for(i=_param_scheda.fb_min; i<=_param_scheda.fb_max; i++)
			MTbufFree(_mil_image[i]);
	}
	for(i=_param_scheda.scheda_min; i<=_param_scheda.scheda_max; i++) {
		MTdigFree(_mil_camera[i]);
		MTsysFree(_mil_system[i]);
	}
	MTappFree(_mil_application);
#endif // MET

#ifdef MET_MIL
	if(_param_scheda.tipo_tv == TV_RGB) {
		for(i=_param_scheda.fb_min_col; i<=_param_scheda.fb_max_col; i++)
			MbufFree(_mil_image_col[i]);
	} else {
		for(i=_param_scheda.fb_min; i<=_param_scheda.fb_max; i++)
			MbufFree(_mil_image[i]);
	}
	for(i=_param_scheda.scheda_min; i<=_param_scheda.scheda_max; i++) {
		MdigFree(_mil_camera[i]);
		MsysFree(_mil_system[i]);
	}
	MappFree(_mil_application);
#endif // MET_MIL

#ifdef ASS
	if(_param_scheda.tipo_tv == TV_RGB) {
		for(i=_param_scheda.fb_min_col; i<=_param_scheda.fb_max_col; i++)
			free(_mem_address_col[i]);
	} else {
		for(i=_param_scheda.fb_min; i<=_param_scheda.fb_max; i++)
			free(_mem_address[i]);
	}
#endif

#ifdef ORION
	if(_param_scheda.tipo_tv == TV_RGB) {
		for(i=_param_scheda.fb_min_col; i<=_param_scheda.fb_max_col; i++)
			MbufFree(_mil_image_col[i]);
	} else {
		for(i=_param_scheda.fb_min; i<=_param_scheda.fb_max; i++)
			MbufFree(_mil_image[i]);
	}
	for(i=_param_scheda.scheda_min; i<=_param_scheda.scheda_max; i++) {
		MdigFree(_mil_camera[i]);
		MsysFree(_mil_system[i]);
	}
	MappFree(_mil_application);
#endif // ORION

#ifdef METEOR2
	if(_param_scheda.tipo_tv == TV_RGB) {
		for(i=_param_scheda.fb_min_col; i<=_param_scheda.fb_max_col; i++)
			MbufFree(_mil_image_col[i]);
	} else {
		for(i=_param_scheda.fb_min; i<=_param_scheda.fb_max; i++)
			MbufFree(_mil_image[i]);
	}
	for(i=_param_scheda.scheda_min; i<=_param_scheda.scheda_max; i++) {
		MdigFree(_mil_camera[i]);
		MsysFree(_mil_system[i]);
	}
	MappFree(_mil_application);
#endif // METEOR2

	return;
}

/***********************************************************************

;  Nome:    
	sea2_set_dma
;
;  Funzione:
     configura i buffer per il dma, allocando la relativa memoria
;
;  Formato:
	short sea2_set_dma(n_buffer)
;
;  Argomenti:    
	short n_buffer           numero di frame buffer da allocare
;
;  Valore di ritorno: 
	 OK                 nessun errore
     PARAM_ERROR        parametri fuori range
	 MEMORY_ERROR       mancata allocazione
;
;  Descrizione:
     {\bf sea2_set_dma}() alloca la memoria per n_buffer dopo aver rilascato
	 quella allocata per default da {\bf sea2_init}();

;
;**********************************************************************/

short
sea2_set_dma(
	short n_buffer
)
{
	if(!_param_scheda.scheda_aperta)
		return SEAV_ERR_GRABBER_INIT;

#ifdef MET

/* versione senza disallocazione se il numero di buffer aumenta */

	if(_param_scheda.tipo_tv == TV_RGB) {
		short i, old_n_fb = _param_scheda.fb_max_col + 1;

		_param_scheda.fb_min_col = 0;
		_param_scheda.fb_max_col = n_buffer - 1;
		_param_scheda.fb_min = _param_scheda.fb_dma_min = 0;
		_param_scheda.fb_max = _param_scheda.fb_dma_max = 4*n_buffer - 1;
		if(n_buffer > old_n_fb) {
			for(i=old_n_fb; i<=_param_scheda.fb_max_col; i++) {
				if(MTbufAllocColor(
					_mil_system[0], 4L, _param_scheda.lung_riga,
					_param_scheda.lung_colonna,
					8 + M_UNSIGNED, M_IMAGE,
					_mil_image_col + i
				) == M_NULL)
					return SEAV_ERR_GRABBER_MEMORY;
				MTbufInquire(
					_mil_image_col[i], M_HOST_ADDRESS, (long *) (_mem_address_col+i)
				);
				_mem_address[i*4] = _mem_address_col[i];
				_mem_address[i*4 + 1] = _mem_address_col[i] + 1;
				_mem_address[i*4 + 2] = _mem_address_col[i] + 2;
				_mem_address[i*4 + 3] = _mem_address_col[i] + 3;
				_mem_draw[i] = (unsigned char *) malloc(
					_max_n_byte_grafica * sizeof(unsigned char)
				);
				if(!_mem_draw[i])
					return SEAV_ERR_MEMORY;
				_primo_byte_libero[i] = 0L;
			}              
		} else {
			for(i=old_n_fb-1; i>_param_scheda.fb_max_col; i--) {
				MTbufFree(_mil_image_col[i]);
				free(_mem_draw[i]);
				_mem_draw[i] = NULL;
			}
		}
	} else {
		short i, old_n_fb = _param_scheda.fb_max + 1;

		_param_scheda.fb_min = _param_scheda.fb_dma_min = 0;
		_param_scheda.fb_max = _param_scheda.fb_dma_max = n_buffer - 1;
		if(n_buffer > old_n_fb) {
			for(i=old_n_fb; i<=_param_scheda.fb_max; i++) {
				if(MTbufAlloc2d(
					_mil_system[0], _param_scheda.lung_riga,
					_param_scheda.lung_colonna,
					8 + M_UNSIGNED, M_IMAGE, _mil_image + i
				) == M_NULL)
					return SEAV_ERR_GRABBER_MEMORY;
				MTbufInquire(
					_mil_image[i], M_HOST_ADDRESS, (long *) (_mem_address+i)
				);
				_mem_draw[i] = (unsigned char *) malloc(
					_max_n_byte_grafica * sizeof(unsigned char)
				);
				if(!_mem_draw[i])
					return SEAV_ERR_MEMORY;
				_primo_byte_libero[i] = 0L;
			}
		} else {
			for(i=old_n_fb-1; i>_param_scheda.fb_max; i--) {
				MTbufFree(_mil_image[i]);
				free(_mem_draw[i]);
				_mem_draw[i] = NULL;
			}
		}
	}
#endif  /* MET */

#ifdef MET_MIL

/* versione senza disallocazione se il numero di buffer aumenta */

	if(_param_scheda.tipo_tv == TV_RGB) {
		short i, old_n_fb = _param_scheda.fb_max_col + 1;

		_param_scheda.fb_min_col = 0;
		_param_scheda.fb_max_col = n_buffer - 1;
		_param_scheda.fb_min = _param_scheda.fb_dma_min = 0;
		_param_scheda.fb_max = _param_scheda.fb_dma_max = 4*n_buffer - 1;
		if(n_buffer > old_n_fb) {
			for(i=old_n_fb; i<=_param_scheda.fb_max_col; i++) {
				if(MbufAllocColor(
					_mil_system[0], 4L, _param_scheda.lung_riga,
					_param_scheda.lung_colonna,
					8 + M_UNSIGNED, M_IMAGE+M_GRAB,
					_mil_image_col + i
				) == M_NULL)
					return SEAV_ERR_GRABBER_MEMORY;
				MbufInquire(
					_mil_image_col[i], M_HOST_ADDRESS, (long *) (_mem_address_col+i)
				);
				_mem_address[i*4] = _mem_address_col[i];
				_mem_address[i*4 + 1] = _mem_address_col[i] + 1;
				_mem_address[i*4 + 2] = _mem_address_col[i] + 2;
				_mem_address[i*4 + 3] = _mem_address_col[i] + 3;
				_mem_draw[i] = (unsigned char *) malloc(
					_max_n_byte_grafica * sizeof(unsigned char)
				);
				if(!_mem_draw[i])
					return SEAV_ERR_MEMORY;
				_primo_byte_libero[i] = 0L;
			}              
		} else {
			for(i=old_n_fb-1; i>_param_scheda.fb_max_col; i--) {
				MbufFree(_mil_image_col[i]);
				free(_mem_draw[i]);
				_mem_draw[i] = NULL;
			}
		}
	} else {
		short i, old_n_fb = _param_scheda.fb_max + 1;

		_param_scheda.fb_min = _param_scheda.fb_dma_min = 0;
		_param_scheda.fb_max = _param_scheda.fb_dma_max = n_buffer - 1;
		if(n_buffer > old_n_fb) {
			for(i=old_n_fb; i<=_param_scheda.fb_max; i++) {
				if(MbufAlloc2d(
					_mil_system[0], _param_scheda.lung_riga,
					_param_scheda.lung_colonna,
					8 + M_UNSIGNED, M_IMAGE+M_GRAB, _mil_image + i
				) == M_NULL)
					return SEAV_ERR_GRABBER_MEMORY;
				MbufInquire(
					_mil_image[i], M_HOST_ADDRESS, (long *) (_mem_address+i)
				);
				_mem_draw[i] = (unsigned char *) malloc(
					_max_n_byte_grafica * sizeof(unsigned char)
				);
				if(!_mem_draw[i])
					return SEAV_ERR_MEMORY;
				_primo_byte_libero[i] = 0L;
			}
		} else {
			for(i=old_n_fb-1; i>_param_scheda.fb_max; i--) {
				MbufFree(_mil_image[i]);
				free(_mem_draw[i]);
				_mem_draw[i] = NULL;
			}
		}
	}
#endif  /* MET_MIL */

#if defined(ASS) || defined(ORION) || defined(METEOR2)
	if(_param_scheda.tipo_tv == TV_RGB) {
		short i, diff, old_n_fb = _param_scheda.fb_max_col + 1;
		diff = n_buffer - old_n_fb;
		if(diff > 0) {
			for(i=0; i<diff; i++)
				sea2_add_dma_buffer(0);
		} else {
			for(i=old_n_fb-1; i>_param_scheda.fb_max_col; i--) {
				#ifdef ASS
				free(_mem_address_col[i]);
				#endif
				#ifdef ORION
				MbufFree(_mil_image_col[i]);
				#endif
				#ifdef METEOR2
				MbufFree(_mil_image_col[i]);
				#endif
				free(_mem_draw[i]);
				_mem_draw[i] = NULL;
			}
			_param_scheda.fb_max_col = n_buffer - 1;
			_param_scheda.fb_min = _param_scheda.fb_dma_min = 0; // 7.61-7
			_param_scheda.fb_max = _param_scheda.fb_dma_max = 4*n_buffer - 1; // 7.61-7
		}
	} else {
		short i, diff, old_n_fb = _param_scheda.fb_max + 1;
		diff = n_buffer - old_n_fb;
		if(diff > 0) {
			for(i=0; i<diff; i++)
				sea2_add_dma_buffer(0);
		} else {
			for(i=old_n_fb-1; i>_param_scheda.fb_max; i--) {
				#ifdef ASS
				free(_mem_address[i]);
				#endif
				#ifdef ORION
				MbufFree(_mil_image[i]);
				#endif
				#ifdef METEOR2
				MbufFree(_mil_image[i]);
				#endif
				free(_mem_draw[i]);
				_mem_draw[i] = NULL;
			}
			_param_scheda.fb_max = n_buffer - 1;
			_param_scheda.fb_dma_max = _param_scheda.fb_max;
		}
	}
#endif  /* ASS || ORION || METEOR2 */

	return SEAV_OK_ALL;
}

/**********************************************************************/

// alloca un frame buffer per l'acquisizione oltre a quelli gia' presenti
short
sea2_add_dma_buffer(
	short n_frame_grabber // numero della scheda a cui assegnare il nuovo buffer
)
{
	short new_fb; // indice del nuovo fb
	short n_buffer; // numero di buffer totali allocati

	if(!_param_scheda.scheda_aperta)
		return SEAV_ERR_GRABBER_INIT;

	if(_param_scheda.tipo_tv == TV_RGB) {
		_param_scheda.fb_max_col ++;
		new_fb = _param_scheda.fb_max_col;
		n_buffer = _param_scheda.fb_max_col + 1;

		_param_scheda.fb_max = _param_scheda.fb_dma_max = 4*n_buffer - 1;
#ifdef MET
		if(MTbufAllocColor(
			_mil_system[n_frame_grabber], 4L, _param_scheda.lung_riga,
			_param_scheda.lung_colonna,
			8 + M_UNSIGNED, M_IMAGE,
			_mil_image_col + new_fb
		) == M_NULL)
			return SEAV_ERR_GRABBER_MEMORY;
		MTbufInquire(
			_mil_image_col[new_fb], M_HOST_ADDRESS,
			(long *) (_mem_address_col+new_fb)
		);
#endif  /* MET */
#ifdef ASS
		if((_mem_address_col[new_fb] = (unsigned char *) malloc(
			(long)_param_scheda.lung_riga *
			(long)_param_scheda.lung_colonna * 4L *
			sizeof(unsigned char)
		)) == NULL)
			return SEAV_ERR_GRABBER_MEMORY;
#endif  /* ASS */
#ifdef ORION
		if(MbufAllocColor(
			_mil_system[n_frame_grabber], 3L, _param_scheda.lung_riga,
			_param_scheda.lung_colonna,
			8 + M_UNSIGNED, M_IMAGE+M_GRAB+M_BGR32+M_PACKED,
			_mil_image_col + new_fb
		) == M_NULL)
			return SEAV_ERR_GRABBER_MEMORY;
		MbufInquire(
			_mil_image_col[new_fb], M_HOST_ADDRESS,
			(long *) (_mem_address_col+new_fb)
		);
#endif  /* ORION */
#ifdef METEOR2
		if(MbufAllocColor(
			_mil_system[n_frame_grabber], 3L, _param_scheda.lung_riga,
			_param_scheda.lung_colonna,
			8 + M_UNSIGNED, M_IMAGE+M_GRAB+M_BGR32+M_PACKED+M_NON_PAGED+M_OFF_BOARD,
			_mil_image_col + new_fb
		) == M_NULL)
			return SEAV_ERR_GRABBER_MEMORY;
		MbufInquire(
			_mil_image_col[new_fb], M_HOST_ADDRESS,
			(long *) (_mem_address_col+new_fb)
		);
#endif  /* METEOR2 */
		_mem_address[new_fb*4] = _mem_address_col[new_fb];
		_mem_address[new_fb*4 + 1] = _mem_address_col[new_fb] + 1;
		_mem_address[new_fb*4 + 2] = _mem_address_col[new_fb] + 2;
		_mem_address[new_fb*4 + 3] = _mem_address_col[new_fb] + 3;
	} else { // tv monocromatica
		_param_scheda.fb_dma_max ++;
		_param_scheda.fb_max = _param_scheda.fb_dma_max;
		new_fb = _param_scheda.fb_dma_max;
		n_buffer = _param_scheda.fb_dma_max + 1;

#ifdef MET
		if(MTbufAlloc2d(
			_mil_system[n_frame_grabber], _param_scheda.lung_riga,
			_param_scheda.lung_colonna, 8 + M_UNSIGNED, M_IMAGE,
			_mil_image + new_fb
		) == M_NULL)
			return SEAV_ERR_GRABBER_MEMORY;
		MTbufInquire(
			_mil_image[new_fb], M_HOST_ADDRESS,
			(long *) (_mem_address+new_fb)
		);
#endif  /* MET */
#ifdef ASS
		if((_mem_address[new_fb] = (unsigned char *) malloc(
			(long)_param_scheda.lung_riga *
			(long)_param_scheda.lung_colonna *
			sizeof(unsigned char)
		)) == NULL)
			return SEAV_ERR_GRABBER_MEMORY;
#endif  /* ASS */
#ifdef ORION
		if(MbufAlloc2d(
			_mil_system[n_frame_grabber], _param_scheda.lung_riga,
			_param_scheda.lung_colonna,
			8 + M_UNSIGNED, M_IMAGE+M_GRAB, _mil_image + new_fb
		) == M_NULL)
			return SEAV_ERR_GRABBER_MEMORY;
		MbufInquire(
			_mil_image[new_fb], M_HOST_ADDRESS, (long *) (_mem_address+new_fb)
		);
#endif  /* ORION */
#ifdef METEOR2
		if(MbufAlloc2d(
			_mil_system[n_frame_grabber], _param_scheda.lung_riga,
			_param_scheda.lung_colonna,
			8 + M_UNSIGNED, M_IMAGE+M_GRAB+M_NON_PAGED+M_OFF_BOARD, _mil_image + new_fb
		) == M_NULL)
			return SEAV_ERR_GRABBER_MEMORY;
		MbufInquire(
			_mil_image[new_fb], M_HOST_ADDRESS, (long *) (_mem_address+new_fb)
		);
#endif  /* METEOR2 */
	}

	// allocazione del buffer per l'overlay
	_mem_draw[new_fb] = (unsigned char *) malloc(
		_max_n_byte_grafica * sizeof(unsigned char)
	);
	if(!_mem_draw[new_fb])
		return SEAV_ERR_MEMORY;
	_primo_byte_libero[new_fb] = 0L;

	return SEAV_OK_ALL;
}

/**********************************************************************/

// 7.61-7
// alloca 3*n frame buffer per la grafica oltre a quelli gia' presenti (n),
// n=numero di buffer a colori allocati; serve per portare il numero di
// buffer di grafica pari al numero di buffer monocromatici quando l'acq.
// e' di tipo a colori;

short sea2_add_graphic_buffers(void)
{
	int i, n_buffer_grafica_allocati = 0, n_buffer_grafica_da_allocare;
	int n_buffer_grafica_totali = (_param_scheda.fb_max_col + 1) * 4;

	if(_param_scheda.tipo_tv == TV_MONO) // non c'e' niente da fare
		return SEAV_OK_ALL;

	// conto i buffer allocati (hanno il puntatore diverso da NULL)
	for(i=0; i<N_FB_MAX; i++) {
		if(_mem_draw[i])
			n_buffer_grafica_allocati ++;
	}
	n_buffer_grafica_da_allocare = n_buffer_grafica_totali - n_buffer_grafica_allocati;
	if(n_buffer_grafica_da_allocare <= 0) // non c'e' niente da fare
		return SEAV_OK_ALL;

	// alloco
	for(i=n_buffer_grafica_allocati; i<n_buffer_grafica_totali; i++) {
		_mem_draw[i] = (unsigned char *) malloc(
			_max_n_byte_grafica * sizeof(unsigned char)
		);
		if(!_mem_draw[i])
			return SEAV_ERR_MEMORY;
		_primo_byte_libero[i] = 0L;
	}

	return SEAV_OK_ALL;
}
