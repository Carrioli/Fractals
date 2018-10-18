
/*
	sea2_acq
	sea2_acq_col
	sea2_acq_dma
*/

#include <sea2_nt.h>
#include <util_nt.h>
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

/****************************************************************************/

/* variabili globali definite in init.c */

#if defined(MET) || defined(MET_MIL) || defined(ORION) || defined(METEOR2)
	extern MIL_ID _mil_camera[N_BOARD_MAX];
	extern MIL_ID _mil_image[N_FB_MAX];
#endif
#if defined(MET_MIL) || defined(ORION) || defined(METEOR2)
	extern MIL_ID _mil_image_col[N_FB_MAX_COL];
#endif

extern PARAM_SCHEDA _param_scheda;

/***********************************************************************/

void (*sea2_acq_dma_call_back)(
	short fb_dma, short xi, short yi, short xf, short yf, short indice_frame
);

////////////////////////////////////////////////////////////////////////

short
sea2_acq(
	short fb,
	short chan,
	short xi,
	short yi,
	short xf,
	short yf
)
{
	if(!_param_scheda.scheda_aperta)
		return SEAV_ERR_GRABBER_INIT;
	if(_param_scheda.tipo_tv == TV_MONO) {
		short ret=SEAV_OK_ALL;

/* controllo dei parametri */

		if(fb < _param_scheda.fb_min || fb > _param_scheda.fb_max)
			return SEAV_ERR_PARAMETER;
		if(chan < _param_scheda.chan_min || chan > _param_scheda.chan_max)
			return SEAV_ERR_PARAMETER;
		if(xi < _param_scheda.x_min || xi > _param_scheda.x_max)
			return SEAV_ERR_PARAMETER;
		if(yi < _param_scheda.y_min || yi > _param_scheda.y_max)
			return SEAV_ERR_PARAMETER;
		if(xf < _param_scheda.x_min || xf > _param_scheda.x_max)
			return SEAV_ERR_PARAMETER;
		if(yf < _param_scheda.y_min || yf > _param_scheda.y_max)
			return SEAV_ERR_PARAMETER;

#ifdef MET
		ret = sea2_start_grab(fb, chan, xi, yi, xf, yf);
		if(ret != SEAV_OK_ALL)
			return ret;
		sea2_wait_v_blank();
		sea2_stop_grab();
#endif
#ifdef MET_MIL
		sea2_chan(chan);
		MdigGrab(
			_mil_camera[_param_scheda.numero_scheda], _mil_image[fb]
		);
		MdigGrabWait(
			_mil_camera[_param_scheda.numero_scheda], M_GRAB_END
		);
#endif
#ifdef ORION
		sea2_chan(chan);
		MdigGrab(
			_mil_camera[_param_scheda.numero_scheda], _mil_image[fb]
		);
		MdigGrabWait(
			_mil_camera[_param_scheda.numero_scheda], M_GRAB_END
		);
#endif
#ifdef METEOR2
		sea2_chan(chan);
		MdigGrab(
			_mil_camera[_param_scheda.numero_scheda], _mil_image[fb]
		);
		MdigGrabWait(
			_mil_camera[_param_scheda.numero_scheda], M_GRAB_END
		);
#endif
	} else {
		return SEAV_ERR_OP_NOT_ALLOWED;
	}

	return SEAV_OK_ALL;
}

/////////////////////////////////////////////////////////////////////////

short
sea2_acq_col(
	short fb,
	short chan,
	short xi,
	short yi,
	short xf,
	short yf
)
{
	if(!_param_scheda.scheda_aperta)
		return SEAV_ERR_GRABBER_INIT;
	if(_param_scheda.tipo_tv == TV_RGB) {
		short ret=SEAV_OK_ALL;

/* controllo dei parametri */

		if(fb < _param_scheda.fb_min_col || fb > _param_scheda.fb_max_col)
			return SEAV_ERR_PARAMETER;
		if(chan < _param_scheda.chan_min_col || chan > _param_scheda.chan_max_col)
			return SEAV_ERR_PARAMETER;
		if(xi < _param_scheda.x_min || xi > _param_scheda.x_max)
			return SEAV_ERR_PARAMETER;
		if(yi < _param_scheda.y_min || yi > _param_scheda.y_max)
			return SEAV_ERR_PARAMETER;
		if(xf < _param_scheda.x_min || xf > _param_scheda.x_max)
			return SEAV_ERR_PARAMETER;
		if(yf < _param_scheda.y_min || yf > _param_scheda.y_max)
			return SEAV_ERR_PARAMETER;
#ifdef MET
		ret = sea2_start_grab_col(fb, chan, xi, yi, xf, yf);
		if(ret != SEAV_OK_ALL)
			return ret;
		sea2_wait_v_blank();
		sea2_stop_grab();
#endif
#ifdef MET_MIL
		MdigGrab(
			_mil_camera[_param_scheda.numero_scheda],
			_mil_image_col[fb]
		);
		MdigGrabWait(
			_mil_camera[_param_scheda.numero_scheda], M_GRAB_END
		);
#endif
#ifdef ORION
		sea2_chan_col(chan);
		MdigGrab(
			_mil_camera[_param_scheda.numero_scheda],
			_mil_image_col[fb]
		);
		MdigGrabWait(
			_mil_camera[_param_scheda.numero_scheda], M_GRAB_END
		);
#endif
#ifdef METEOR2
		sea2_chan_col(chan);
		MdigGrab(
			_mil_camera[_param_scheda.numero_scheda],
			_mil_image_col[fb]
		);
		MdigGrabWait(
			_mil_camera[_param_scheda.numero_scheda], M_GRAB_END
		);
#endif
	} else {
		return SEAV_ERR_OP_NOT_ALLOWED;
	}

	return SEAV_OK_ALL;
}


/***********************************************************************

;  Nome:    
	sea2_acq_dma
;
;  Funzione:
     acquisisce una serie di immagini da telecamera e le trasferisce su PC
;
;  Formato:
	short sea2_acq_dma(fb0, fb1, fb_dma0, fb_dma1, chan0, chan1, xi, yi, xf, yf)
;
;  Argomenti:    
    short fb0          numero del primo frame buffer sulla scheda
    short fb1          numero del secondo frame buffer sulla scheda
    short fb_dma0      numero del primo frame buffer in memoria PC
    short fb_dma1      numero del secondo frame buffer in memoria PC
    short chan0        numero del primo ingresso video
    short chan1        numero del secondo ingresso video
	short xi           ascissa iniziale della finestra di acquisizione
	short yi           ordinata iniziale della finestra di acquisizione
	short xf           ascissa finale della finestra di acquisizione
	short yf           ordinata finale della finestra di acquisizione
	short n_acq        numero di acquisizioni da effettuare

;
;  Valore di ritorno: 
	 SEAV_OK_ALL          nessun errore
     SEAV_ERR_*           codice di errore
;
;  Descrizione:
     {\bf sea2_acq_dma}() acquisisce una serie di immagini
	 alternativamente sui frame buffer fb0 e fb1; subito dopo ogni acquisizione
	 trasferisce l'immagine in dma sul frame buffer corrispondente della memoria
	 del PC; al termine di ogni trasferimento chiama la routine call-back
	 (*sea2_acq_dma_call_back)() alla quale passa il numero del frame buffer sul
	 quale e' disponibile l'immagine per l'elaborazione (fb_dma0 oppure
	 fb_dma1) ed i parametri della finestra, nonche' il numero d'ordine
	 del frame acquisito;
	 la routine (*sea2_acq_dma_call_back)() e'qui definita come
	 extern void (*sea2_acq_dma_call_back)(
		short fb_dma, short xi, short yi, short xf, short yf, short indice_frame
	 );
	 pertanto deve essere cura dell'applicazione definire un puntatore a
	 funzione con la seguente dichiarazione:
	 void (*sea2_acq_dma_call_back)(
		short fb_dma, short xi, short yi, short xf, short yf, short indice_frame
	 );
	 ed assegnare al puntatore l'indirizzo della funzione che si vuole usare;
	 gli ingressi chan0 e chan1 possono coincidere; se non coincidono le
	 due sorgenti video devono essere sincronizzate;
;
;**********************************************************************/

short
sea2_acq_dma(
	short fb0,
	short fb1,
	short fb_dma0,
	short fb_dma1,
	short chan0,
	short chan1,
	short xi,
	short yi,
	short xf,
	short yf,
	short n_acq
)
{
	if(!_param_scheda.scheda_aperta)
		return SEAV_ERR_GRABBER_INIT;

#ifdef MET
	if(_param_scheda.tipo_tv == TV_MONO) {

		short i;

/* controllo dei parametri */

		if(fb0 < _param_scheda.fb_min || fb0 > _param_scheda.fb_max)
			return SEAV_ERR_PARAMETER;
		if(fb1 < _param_scheda.fb_min || fb1 > _param_scheda.fb_max)
			return SEAV_ERR_PARAMETER;
		if(fb_dma0 < _param_scheda.fb_min || fb_dma0 > _param_scheda.fb_max)
			return SEAV_ERR_PARAMETER;
		if(fb_dma1 < _param_scheda.fb_min || fb_dma1 > _param_scheda.fb_max)
			return SEAV_ERR_PARAMETER;
		if(chan0 < _param_scheda.chan_min || chan0 > _param_scheda.chan_max)
			return SEAV_ERR_PARAMETER;
		if(chan1 < _param_scheda.chan_min || chan1 > _param_scheda.chan_max)
			return SEAV_ERR_PARAMETER;
		if(xi < _param_scheda.x_min || xi > _param_scheda.x_max)
			return SEAV_ERR_PARAMETER;
		if(yi < _param_scheda.y_min || yi > _param_scheda.y_max)
			return SEAV_ERR_PARAMETER;
		if(xf < _param_scheda.x_min || xf > _param_scheda.x_max)
			return SEAV_ERR_PARAMETER;
		if(yf < _param_scheda.y_min || yf > _param_scheda.y_max)
			return SEAV_ERR_PARAMETER;

		util_set_8254_one_shot();
		if(_param_scheda.standard_tv == STANDARD_AM) /* americano */
			util_set_time(0.025);
		else /* europeo */
			util_set_time(0.030);
		for(i=0; i<n_acq; i++) {
			switch(i) {
				case 0: /* primo frame */
					sea2_start_grab(fb_dma0, chan0, xi, yi, xf, yf);
					sea2_wait_v_blank();
					sea2_wait_v_blank();
					break;
				default: /* tutti gli altri */
					util_start_shot();				
					if(i & 1) {
						sea2_start_grab1(fb_dma1, chan1, xi, yi, xf, yf);
						(*sea2_acq_dma_call_back)(
							fb_dma0, xi, yi, xf, yf, (short) (i-1)
						);
					} else {
						sea2_start_grab1(fb_dma0, chan0, xi, yi, xf, yf);
						(*sea2_acq_dma_call_back)(
							fb_dma1, xi, yi, xf, yf, (short) (i-1)
						);
					}
					while(util_check_shot())
						;
					sea2_wait_v_blank();
					if(i == n_acq-1) /* all'ultimo frame fermo l'acquisizione */
						sea2_stop_grab1();
					break;
			}
		}

		/* ho terminato le acquisizioni, ma ho un delay di 1 frame per completare
		   il lavoro */
		if(i & 1)
			(*sea2_acq_dma_call_back)(
				fb_dma0, xi, yi, xf, yf, (short) (i-1)
			);
		else
			(*sea2_acq_dma_call_back)(
				fb_dma1, xi, yi, xf, yf, (short) (i-1)
			);
	} else {
		return SEAV_ERR_OP_NOT_ALLOWED;
	}
#endif

	return SEAV_OK_ALL;
}


