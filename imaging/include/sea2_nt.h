
#ifndef __SEA2_H__
#define __SEA2_H__


#include "my_err.h"

/************************************************************************/

/* struttura dei parametri della scheda installata */

typedef struct {
    short scheda_aperta;
    short nome_scheda;  /* vedi le define sotto */
    short standard_tv;  /* STANDARD_EU, STANDARD_AM */
    short tipo_tv;      /* TV_MONO, TV_RGB */
    short x_min;        /* parametri caratteristici */
    short x_max;
    short y_min;
    short y_max;
    short chan_min;
    short chan_max;
    short fb_min;
    short fb_max;
    short chan_min_col;
    short chan_max_col;
    short fb_min_col;
    short fb_max_col;
    short fb_dma_min;
    short fb_dma_max;
    short lung_riga;
    short lung_colonna;
    short lung_max;    /* massimo tra lung_riga e lung_colonna */
    short off_fb1;     /* serve alla PIP-640: scroll del fb1 rispetto allo 0 */
    short scheda_min;  /* per i sistemi multi scheda */
    short scheda_max;
    short fb_display;  /* frame buffer attualmente visualizzato */
    short fb_lavoro;   /* frame buffer selezionato per l'I/O */
    short memoria_lavoro;/* MEMORIA_RAM, MEMORIA_SCHEDA */
    short numero_scheda; /* numero della scheda selezionata in
    	                    sistemi multischeda */
    short pan_display; /* valore del pan attuale in display */
    short scroll_display;/* valore dello scroll attuale in display */
    double fattore_dist; /* fattore di distorsione: moltiplica le ascisse per
    	                    renderle omogenee alle ordinate: si fa riferimento
    						ad una telecamera standard CCIR; rapporto
    						tra la risoluzione orizzontale e quella verticale */
} PARAM_SCHEDA;

/************************************************************************/

/* tipi di schede */

#define SCHEDA_METEOR      1
#define SCHEDA_ASSENTE     2
#define SCHEDA_ORION       3
#define SCHEDA_METEOR2_MC  4

/************************************************************************/

/* definizioni varie */

#define STANDARD_EU    0
#define STANDARD_AM    1
#define STANDARD_1024  2
#define STANDARD_4000  3
#define TV_MONO        0
#define TV_RGB         1
#define MEMORIA_SCHEDA 0
#define MEMORIA_RAM    1
#define UNDERLAY_MODE  1
#define OVERLAY_MODE   2
#define GRAFICA_SU_FB  0
#define GRAFICA_SU_OVERLAY  1
#define COLORI_APPROSSIMATI 0
#define COLORI_VERI         1

/************************************************************************/

/* definizioni di strutture varie */

#ifndef __ROI__
typedef struct {   /* area di interesse */
    short fb;
    short xi;
    short yi;
    short xf;
    short yf;
} ROI;
#define __ROI__
#endif
#ifndef __PUNTO__
typedef struct {     /* cartesiane intere short */
    short x;
    short y;
} PUNTO;
#define __PUNTO__
#endif
#ifndef __PUNTOL__
typedef struct {     /* cartesiane intere long */
    long x;
    long y;
} PUNTOL;
#define __PUNTOL__
#endif

#ifndef __PUNTOD__
typedef struct {     /* cartesiane double */
    double x;
    double y;
} PUNTOD;
#define __PUNTOD__
#endif

#ifndef __POLARE__
typedef struct {     /* polari double */
    double r;
    double a;
} POLARE;
#define __POLARE__
#endif

#ifndef __BYTE_ALTO_15__
#define __BYTE_ALTO_15__
#define BYTE_ALTO_15(A,B)  (((A)>>1)&124)|((B)>>6)
#endif

#ifndef __BYTE_BASSO_15__
#define __BYTE_BASSO_15__
#define BYTE_BASSO_15(A,B)  (((A)<<2)&224)|((B)>>3)
#endif

#ifndef __SHORT_15__
#define __SHORT_15__
/*#define SHORT_15(R,G,B)  ((((short)(R)>>3)<<10)|(((short)G>>3)<<5)|((short)B>>3))*/
#define SHORT_15(R,G,B)  ((((short)(R)&248)<<7)|(((short)G&248)<<2)|((short)B>>3))
#endif

#ifndef __BYTE_ALTO_16__
#define __BYTE_ALTO_16__
#define BYTE_ALTO_16(A,B)  ((A)&248)|((B)>>5)
#endif

#ifndef __BYTE_BASSO_16__
#define __BYTE_BASSO_16__
#define BYTE_BASSO_16(A,B)  (((A)<<3)&224)|((B)>>3)
#endif

#ifndef __SHORT_16__
#define __SHORT_16__
/*#define SHORT_16(R,G,B)  ((((short)(R)>>3)<<11)|(((short)G>>2)<<5)|((short)B>>3))*/
#define SHORT_16(R,G,B)  ((((short)(R)&248)<<8)|(((short)G&252)<<3)|((short)B>>3))
#endif


/************************************************************************/

/* prototipi */

short sea2_init(short standard_tv, short tipo_tv, char *nome_dcf);
short sea2_init_multi_board(short n_board, char *nome_dcf);
short sea2_set_offset(short offset);
short sea2_set_gain(short gain);
short sea2_leggi_pixel(short x_pixel, short y_pixel);
void sea2_scrivi_pixel(
    short x_pixel, short y_pixel, unsigned char valore
);
void sea2_leggi_riga(
    short xi, short yi,short n_pixel,unsigned char *riga
);
void sea2_leggi_riga_col(
    short xi, short yi, short n_pixel, unsigned char *riga
);
void sea2_leggi_riga4(
    short xi, short yi,short n_pixel,unsigned char *riga
);
void sea2_scrivi_riga(
    short xi,short yi,short n_pixel,unsigned char *riga
);
void sea2_scrivi_riga_col24(
	short xi, short yi, short n_pixel, unsigned char *riga
);
void sea2_scrivi_riga_col(
    short xi,short yi,short n_pixel,unsigned char *riga
);
void sea2_leggi_colonna(
    short xi, short yi, short n_pixel, unsigned char *colonna
);
void sea2_leggi_colonna_col(
    short xi, short yi, short n_pixel, unsigned char *colonna
);
void sea2_scrivi_colonna(
    short xi, short yi, short n_pixel, unsigned char *colonna
);
void sea2_leggi_finestra(
    short xi, short yi, short xf, short yf, unsigned char *buffer
);
void sea2_leggi_finestra_col(
    short xi, short yi, short xf, short yf, unsigned char *buffer
);
void sea2_scrivi_finestra(
    short xi, short yi, short xf, short yf, unsigned char *buffer
);
short sea2_display(short fb);
short sea2_acq(short fb, short chan, short xi, short yi, short xf, short yf);
short sea2_start_grab(
    short fb, short chan, short xi, short yi, short xf, short yf
);
short sea2_start_grab1(
    short fb, short chan, short xi, short yi, short xf, short yf
);
short sea2_start_grab2(
    short fb, short chan, short xi, short yi, short xf, short yf,
    short inverti_field
);
short sea2_start_grab3(
    short fb, short chan, short xi, short yi, short xf, short yf,
    short tempo_definito
);
short sea2_start_grab4(
    short fb, short xi, short yi, short xf, short yf, short tempo_definito
);
short sea2_start_grab_col(
    short fb, short chan, short xi, short yi, short xf, short yf
);
short sea2_start_grab_col1(
    short fb, short chan, short xi, short yi, short xf, short yf
);
short sea2_start_grab_col2(
    short fb, short chan, short xi, short yi, short xf, short yf,
    short inverti_field
);
short sea2_start_grab_col3(
    short fb, short chan, short xi, short yi, short xf, short yf,
    short tempo_definito
);
short sea2_start_grab_col4(
	short fb, short xi, short yi, short xf, short yf, short tempo_definito
);
void sea2_stop_grab(void);
void sea2_stop_grab1(void);
void sea2_seleziona_buffer(short fb);
void sea2_init_config(PARAM_SCHEDA *param_scheda);
void sea2_disegna_finestra(
    short fb, short xi, short yi, short xf, short yf, short colore
);
void sea2_disegna_linea(
    short fb, short xi, short yi, short xf, short yf, short colore
);
long sea2_wait_v_blank(void);
void sea2_select_board(short n_scheda);
short sea2_leggi_field(void);
void sea2_close(void); 
void sea2_pan_display(short pan);  
void sea2_scroll_display(short scroll);
short sea2_acq_col(
    short fb, short chan, short xi, short yi, short xf, short yf
);
short sea2_acq_dma(
    short fb0, short fb1, short fb_dma0, short fb_dma1, short chan0,
    short chan1, short xi, short yi, short xf, short yf, short n_acq
);
unsigned char *sea2_indirizzo_memoria(short xp, short yp);
unsigned char *sea2_indirizzo_memoria_col(short xp, short yp);
short sea2_set_dma(short n_buffer);
short sea2_set_window_handle(void *hwnd);
short sea2_how_many_bits_per_pixel_vga(void);
void sea2_refresh(short xi, short yi, short xf, short yf);
void sea2_refresh2(short yi);
void sea2_refresh3(short yi, short fb); // 2.54.0-7 OCV
void sea2_refresh_col(short xi, short yi, short xf, short yf);
void sea2_refresh2_col(short yi);
void sea2_refresh3_col(short yi, short fb); // 2.54.0-7 OCV
void sea2_direct_vga_mode(short enable);
void sea2_copy_bitmap(short flag_neri);
short sea2_is_direct_vga_enabled(void);
void sea2_grab_disable(void);
void sea2_grab_enable(void);
short sea2_wait_h_blank(void *punt);
void sea2_copy_bitmap_row(
    short xi, short yi, short n_pixel, unsigned char *buffer, short flag_copy
);
void sea2_set_zoom_mode(short enable);
void sea2_display_zoom(short fb0, short fb1, short fb2, short fb3);
void sea2_sotto_c2_riga(
    short xi, short yi, short n_pixel, unsigned char *riga
);
void sea2_sotto_max2_riga(
    short xi, short yi, short n_pixel, unsigned char *riga
);
void sea2_sotto_c2_riga4(
    short xi, short yi, short n_pixel, unsigned char *riga
);
void sea2_sotto_max2_riga4(
    short xi, short yi, short n_pixel, unsigned char *riga
);
void sea2_sotto_c2_riga_col(
    short xi, short yi, short n_pixel, unsigned char *riga
);
void sea2_sotto_max2_riga_col(
    short xi, short yi, short n_pixel, unsigned char *riga
);
void sea2_sotto_max2_riga4_col(
    short xi, short yi, short n_pixel, unsigned char *riga
);
void sea2_copy_bitmap2(short flag_neri, short yi, short flag_copy);
void sea2_copy_bitmap3(short flag_neri, short yi, short flag_copy, short fb); // 2.54.0-7 OCV
void sea2_copy_bitmap_col(short flag_neri);
void sea2_copy_bitmap2_col(short flag_neri, short yi, short flag_copy);
void sea2_copy_bitmap3_col(short flag_neri, short yi, short flag_copy, short fb); // 2.54.0-7 OCV
void sea2_v_filter_on_off(short on);
short sea2_set_roi(short fb, short xi, short yi, short xf, short yf);
short sea2_set_roi_col(short fb_col, short xi, short yi, short xf, short yf);
short sea2_roi_enable_acq(short fb, short enable);
short sea2_roi_enable_display(short fb, short enable);
short sea2_roi_enable_acq_col(short fb_col, short enable);
short sea2_roi_enable_display_col(short fb_col, short enable);
void sea2_clear_bitmap(void);
void sea2_draw_bitmap_to_dc(
    long hdc, short xi, short yi, short x_size, short y_size, short x_dest,
    short y_dest
);
void sea2_copy_bitmap_row_col(
    short xi, short yi, short n_pixel, unsigned char *buffer, short flag_copy
);
void sea2_display_piano_colore(short piano_colore);
void sea2_scrivi_pixel_col(
    short x_pixel, short y_pixel, unsigned char rosso, unsigned char verde,
    unsigned char blu
);
void sea2_disegna_linea_col(
    short fb, short xi, short yi, short xf, short yf,
    short rosso, short verde, short blu
);
void sea2_disegna_finestra_col(
    short fb, short xi, short yi, short xf, short yf, short rosso,
    short verde, short blu
);
void sea2_reset_overlay(short fb);
void sea2_select_overlay_mode(short mode);
short sea2_overlay_mode(void);
void sea2_disegna_croce_col(
    short xc, short yc, unsigned char rosso, unsigned char verde,
    unsigned char blu
);
void sea2_disegna_quadratino_col(
    short xc, short yc, unsigned char rosso, unsigned char verde,
    unsigned char blu
);
void sea2_disegna_poligono_col(
    short fb, PUNTO *punto, short n_punti, short off_x, short off_y,
    short rosso, short verde, short blu
);
void sea2_scrivi_pixel_overlay(
    short x_pixel, short y_pixel, unsigned char rosso, unsigned char verde,
    unsigned char blu
);
void sea2_disegna_croce_overlay(
    short xc, short yc, unsigned char rosso, unsigned char verde,
    unsigned char blu
);
void sea2_disegna_quadratino_overlay(
    short xc, short yc, unsigned char rosso, unsigned char verde,
    unsigned char blu
);
void sea2_disegna_finestra_overlay(
    short fb, short xi, short yi, short xf, short yf, short rosso, short verde,
    short blu
);
void sea2_disegna_linea_overlay(
    short fb, short xi, short yi, short xf, short yf, short rosso,
    short verde, short blu
);
void sea2_disegna_poligono_overlay(
    short fb, PUNTO *punto, short n_punti, short off_x, short off_y,
    short rosso, short verde, short blu
);
void sea2_leggi_riga2(
    short xi, short yi, short n_pixel, unsigned char *riga
);
void sea2_leggi_riga3(
    short xi, short yi, short n_pixel, unsigned char *riga
);
void sea2_leggi_riga_col16(
    short xi, short yi, short n_pixel, unsigned char *riga
);
void sea2_leggi_riga_col24(
    short xi, short yi, short n_pixel, unsigned char *riga
);
void sea2_sotto_max2_riga2(
    short xi, short yi, short n_pixel, unsigned char *riga
);
void sea2_sotto_max2_riga3(
    short xi, short yi, short n_pixel, unsigned char *riga
);
void sea2_sotto_c2_riga2(
    short xi, short yi, short n_pixel, unsigned char *riga
);
void sea2_sotto_c2_riga3(
    short xi, short yi, short n_pixel, unsigned char *riga
);
void sea2_sotto_c2_riga_col16(
    short xi, short yi, short n_pixel, unsigned char *riga
);
void sea2_sotto_c2_riga_col24(
    short xi, short yi, short n_pixel, unsigned char *riga
);
void sea2_set_n_bit_col16(short n_bit);
void sea2_set_color_mode(short modo);
void sea2_chan(short chan);
void sea2_chan_col(short chan);
short sea2_sync_chan(short chan);
short sea2_disegno_su_bitmap(short n_primitive);
short sea2_disegno_su_bitmap3(short n_primitive, short fb); // 2.54.0-7 OCV
void sea2_realize_palette(void);
short sea2_input_signal_present(void);
void sea2_leggi_finestra_col24(
	short xi, short yi, short xf, short yf, unsigned char *buffer
);
short sea2_is_grab_in_progress();
short sea2_set_exposure_time(long n_ns);
short sea2_start_pulse_width(long n_ns);
short sea2_start_double_pulse(long n_ns);
short sea2_init_serial_port(long speed, long parity, long bits, long stops);
short sea2_are_data_pending_on_serial_port();
char sea2_read_char_from_serial_port();
short sea2_write_string_on_serial_port(char *st);
void sea2_set_pci_latency(int latency);
short sea2_add_dma_buffer(short n_frame_grabber);
short sea2_set_automatic_grab_on_trigger(short flag);
short sea2_add_graphic_buffers(void);
void sea2_set_user_bit_out(short numero_bit, short valore); // 2.31 OCV
long sea2_copy_graphic_buffer(short fb, unsigned char *mem);
long sea2_copy_graphic_fb2fb(short fb_source, short fb_target); // 2.53.0 OCV
short sea2_set_timer_level(short timer/*1,2*/, short level/*0,1*/); // 2.53.0 OCV

#endif

