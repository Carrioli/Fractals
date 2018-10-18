
/*
	sea2_select_board
	sea2_seleziona_buffer
	sea2_pan_display
	sea2_scroll_display
	sea2_set_gain
	sea2_set_offset
	sea2_init_config
	sea2_display
	sea2_set_roi
	sea2_set_roi_col
	sea2_roi_enable_display
	sea2_roi_enable_acq
	sea2_roi_enable_display_col
	sea2_roi_enable_acq_col
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

/****************************************************************************/

/* variabili globali definite in init.c */

#if defined(MET) || defined(MET_MIL) || defined(ORION) || defined(METEOR2)
	extern MIL_ID _mil_camera[N_BOARD_MAX];
#endif
#if defined(MET) || defined(ASS) || defined(MET_MIL) || defined(ORION) || defined(METEOR2)
	extern ROI _roi_fb[N_FB_MAX];
	extern short _roi_enable_display[N_FB_MAX];
	extern short _roi_enable_acq[N_FB_MAX];
	// per il colore RGB
	extern ROI _roi_fb_col[N_FB_MAX_COL];
	extern short _roi_enable_display_col[N_FB_MAX_COL];
	extern short _roi_enable_acq_col[N_FB_MAX_COL];
	// fine colore RGB
#endif
	extern PARAM_SCHEDA _param_scheda;


/***********************************************************************

;  Nome:    
	sea2_select_board
;
;  Funzione:
     seleziona la scheda in un sistema multi-scheda
;
;  Formato:
	void sea2_select_board(n_scheda)

;
;  Argomenti:
    short n_scheda          numero della scheda

;
;  Descrizione:
     {\bf sea2_select_board}() seleziona la scheda da usare
	 per le successive operazioni in un sistema multischeda;
	 se la presente routine non e' mai invocata viene usata
	 la scheda di default (numero 0);

;
;**********************************************************************/

void
sea2_select_board(
	short n_scheda
)
{
	_param_scheda.numero_scheda = n_scheda;
}


/***********************************************************************

;  Nome:    
	sea2_seleziona_buffer

;
;  Funzione:
    seleziona il frame buffer per le operazioni di I/O da host
;
;  Formato:
	short sea2_seleziona_buffer(fb)
;
;  Argomenti:    
	short fb           numero del frame buffer

;
;  Valore di ritorno:
	OK                 nessun errore

;
;  Descrizione:
     {\bf sea2_seleziona_buffer}() seleziona il frame buffer per
	 le operazioni di I/O da host: tutte le routine di I/O non effettuano
	 il controllo dei parametri a loro passati per questioni di efficienza;
;
;**********************************************************************/

void
sea2_seleziona_buffer(
	short fb
)
{
	_param_scheda.fb_lavoro = fb;
}

void
sea2_pan_display(
	short pan
)
{
	_param_scheda.pan_display = pan;
}

void
sea2_scroll_display(
	short scroll
)
{
	_param_scheda.scroll_display = scroll;
}


/***********************************************************************

;  Nome:    
     sea2_set_gain
;
;  Funzione:
     imposta il valore del guadagno per l'acquisizione delle immagini
;
;  Formato:
     short sea2_set_gain(gain)
;
;  Argomenti:    
     short gain   valore del guadagno da impostare

;
;  Valore di ritorno: 
	 OK           nessun errore
     PARAM_ERROR  parametri fuori range
;
;  Descrizione:
     {\bf sea2_set_gain}() imposta il guadagno di ingresso
	 per l'acquisizione delle immagini da telecamera;
;
;**********************************************************************/

short
sea2_set_gain(
	short gain
)
{      
	if(!_param_scheda.scheda_aperta)
		return SEAV_ERR_GRABBER_INIT;
	if(gain > 255 || gain < 0)
		return SEAV_ERR_PARAMETER;

	if(_param_scheda.tipo_tv == TV_RGB) {
#ifdef MET
		/* il default Matrox e' 160; il mio e' 195: vedi init.c */
		MTdigReference(
			_mil_camera[_param_scheda.numero_scheda], M_WHITE_REF, (long) gain
		);
#endif
#ifdef MET_MIL
		/* il default Matrox e' 160; il mio e' 195: vedi init.c */
		MdigReference(
			_mil_camera[_param_scheda.numero_scheda], M_WHITE_REF, (long) gain
		);
#endif
#ifdef ORION
		MdigReference(
			_mil_camera[_param_scheda.numero_scheda], M_WHITE_REF, (long) gain
		);
#endif
#ifdef METEOR2
		MdigReference(
			_mil_camera[_param_scheda.numero_scheda], M_WHITE_REF, (long) gain
		);
#endif
	}
	return SEAV_OK_ALL;
}

/**********************************************************************/

/* setta il livello di nero */

short
sea2_set_offset(
	short offset
)
{
	if(!_param_scheda.scheda_aperta)
		return SEAV_ERR_GRABBER_INIT;
	if(offset > 255 || offset < 0)
		return SEAV_ERR_PARAMETER;
	if(_param_scheda.tipo_tv == TV_RGB) {
		/* il default Matrox e' 80; il mio e' 90: vedi init.c */
#ifdef MET
		MTdigReference(
			_mil_camera[_param_scheda.numero_scheda], M_BLACK_REF, (long) offset
		);
#endif
#ifdef MET_MIL
		MdigReference(
			_mil_camera[_param_scheda.numero_scheda], M_BLACK_REF, (long) offset
		);
#endif
#ifdef ORION
		MdigReference(
			_mil_camera[_param_scheda.numero_scheda], M_BLACK_REF, (long) offset
		);
#endif
#ifdef METEOR2
		MdigReference(
			_mil_camera[_param_scheda.numero_scheda], M_BLACK_REF, (long) offset
		);
#endif
	}
	return SEAV_OK_ALL;
}


/***********************************************************************

;  Nome:    
	sea2_init_config
;
;  Funzione:
     inizializza la struttura che descrive la scheda
;
;  Formato:
	void sea2_init_config(param_scheda)

;
;  Argomenti:
	PARAM_SCHEDA *param_scheda   puntatore alla struttura

;
;  Descrizione:
     {\bf sea2_init_config}() copia nella struttura puntata da param_scheda
	 i valori della struttura _param_scheda (globale) che sono inizializzati
	 dalla sea2_init() e modificati da tutte le routine che agiscono sulla
	 programmazione della scheda (vedi sea2_set_dma());
	 pertanto questa routine non puo' essere chiamata prima della sea2_init();

;
;**********************************************************************/

void
sea2_init_config(
	PARAM_SCHEDA *param_scheda
)
{
	// almeno il nome scheda puo' essere visto prima della init
	_param_scheda.nome_scheda = NOME_SCHEDA;
	memcpy(param_scheda, &_param_scheda, sizeof(PARAM_SCHEDA));
}


/***********************************************************************

;  Nome:    
	sea2_display
;
;  Funzione:
     seleziona il frame buffer da visualizzare
;
;  Formato:
	short sea2_display(fb)
;
;  Argomenti:    
    short fb           numero del frame buffer

;
;  Valore di ritorno: 
	 OK                 nessun errore
     PARAM_ERROR        parametri fuori range
;
;  Descrizione:
     {\bf sea2_display}() abilita la visualizzazione del frame buffer
	 specificato;
;
;**********************************************************************/

short
sea2_display(
	short fb
)
{
	if(!_param_scheda.scheda_aperta)
		return SEAV_ERR_GRABBER_INIT;
	if(fb < _param_scheda.fb_min || fb > _param_scheda.fb_max)
		return SEAV_ERR_PARAMETER;
	_param_scheda.fb_display = fb;

	return SEAV_OK_ALL;
}

/****************************************************************************/

short
sea2_set_roi(
	short fb,
	short xi,
	short yi,
	short xf,
	short yf
)
{
	if(!_param_scheda.scheda_aperta)
		return SEAV_ERR_GRABBER_INIT;
	if(fb < _param_scheda.fb_min || fb > _param_scheda.fb_max)
		return SEAV_ERR_PARAMETER;

	if(xi < _param_scheda.x_min)
		xi = _param_scheda.x_min;
	if(xi > _param_scheda.x_max)
		xi = _param_scheda.x_max;
	if(yi < _param_scheda.y_min)
		yi = _param_scheda.y_min;
	if(yi > _param_scheda.y_max)
		yi = _param_scheda.y_max;
	if(xf < _param_scheda.x_min)
		xf = _param_scheda.x_min;
	if(xf > _param_scheda.x_max)
		xf = _param_scheda.x_max;
	if(yf < _param_scheda.y_min)
		yf = _param_scheda.y_min;
	if(yf > _param_scheda.y_max)
		yf = _param_scheda.y_max;

/* assegnazione */

	_roi_fb[fb].xi = xi;
	_roi_fb[fb].yi = yi;
	_roi_fb[fb].xf = xf;
	_roi_fb[fb].yf = yf;

	return SEAV_OK_ALL;
}

/****************************************************************************/

short
sea2_set_roi_col(
	short fb_col,
	short xi,
	short yi,
	short xf,
	short yf
)
{
	if(!_param_scheda.scheda_aperta)
		return SEAV_ERR_GRABBER_INIT;
	if(_param_scheda.tipo_tv == TV_RGB) {

/* controllo dei parametri */

		if(fb_col < _param_scheda.fb_min_col || fb_col > _param_scheda.fb_max_col)
			return SEAV_ERR_PARAMETER;
		if(xi < _param_scheda.x_min)
			xi = _param_scheda.x_min;
		if(xi > _param_scheda.x_max)
			xi = _param_scheda.x_max;
		if(yi < _param_scheda.y_min)
			yi = _param_scheda.y_min;
		if(yi > _param_scheda.y_max)
			yi = _param_scheda.y_max;
		if(xf < _param_scheda.x_min)
			xf = _param_scheda.x_min;
		if(xf > _param_scheda.x_max)
			xf = _param_scheda.x_max;
		if(yf < _param_scheda.y_min)
			yf = _param_scheda.y_min;
		if(yf > _param_scheda.y_max)
			yf = _param_scheda.y_max;

/* assegnazione */

		_roi_fb_col[fb_col].xi = xi;
		_roi_fb_col[fb_col].yi = yi;
		_roi_fb_col[fb_col].xf = xf;
		_roi_fb_col[fb_col].yf = yf;
	}

	return SEAV_OK_ALL;

}

/****************************************************************************/

short
sea2_roi_enable_display(
	short fb,
	short enable
)
{
	if(!_param_scheda.scheda_aperta)
		return SEAV_ERR_GRABBER_INIT;
	if(fb < _param_scheda.fb_min || fb > _param_scheda.fb_max)
		return SEAV_ERR_PARAMETER;

/* abilitazione disabilitazione */

	if(enable)
		_roi_enable_display[fb] = 1;
	else
		_roi_enable_display[fb] = 0;

	return SEAV_OK_ALL;
}

/****************************************************************************/

short
sea2_roi_enable_acq(
	short fb,
	short enable
)
{
	if(!_param_scheda.scheda_aperta)
		return SEAV_ERR_GRABBER_INIT;
	if(fb < _param_scheda.fb_min || fb > _param_scheda.fb_max)
		return SEAV_ERR_PARAMETER;

/* abilitazione disabilitazione */

	if(enable)
		_roi_enable_acq[fb] = 1;
	else
		_roi_enable_acq[fb] = 0;

	return SEAV_OK_ALL;
}

/****************************************************************************/

short
sea2_roi_enable_display_col(
	short fb_col,
	short enable
)
{
	if(!_param_scheda.scheda_aperta)
		return SEAV_ERR_GRABBER_INIT;
	if(_param_scheda.tipo_tv == TV_RGB) {

/* controllo dei parametri */

		if(fb_col < _param_scheda.fb_min_col || fb_col > _param_scheda.fb_max_col)
			return SEAV_ERR_PARAMETER;

/* abilitazione disabilitazione */

		if(enable)
			_roi_enable_display_col[fb_col] = 1;
		else
			_roi_enable_display_col[fb_col] = 0;
	}

	return SEAV_OK_ALL;
}

/****************************************************************************/

short
sea2_roi_enable_acq_col(
	short fb_col,
	short enable
)
{
	if(!_param_scheda.scheda_aperta)
		return SEAV_ERR_GRABBER_INIT;
	if(_param_scheda.tipo_tv == TV_RGB) {

/* controllo dei parametri */

		if(fb_col < _param_scheda.fb_min_col || fb_col > _param_scheda.fb_max_col)
			return SEAV_ERR_PARAMETER;

/* abilitazione disabilitazione */

		if(enable)
			_roi_enable_acq_col[fb_col] = 1;
		else
			_roi_enable_acq_col[fb_col] = 0;
	}

	return SEAV_OK_ALL;
}

