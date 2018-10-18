
/*
	sea2_start_grab
	sea2_start_grab1
	sea2_start_grab2
	sea2_start_grab3
	sea2_start_grab4
	sea2_start_grab_col
	sea2_start_grab_col1
	sea2_start_grab_col2
	sea2_start_grab_col3
	sea2_start_grab_col4

	sea2_grab_enable
	sea2_input_signal_present
	sea2_chan
	sea2_chan_col
	sea2_sync_chan
	sea2_is_grab_in_progress
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

#include <windows.h>

/****************************************************************************/

/* variabili globali definite in init.c */

#if defined(MET) || defined(MET_MIL) || defined(ORION) || defined(METEOR2)
	extern MIL_ID _mil_camera[N_BOARD_MAX], _mil_system[N_BOARD_MAX];
	extern MIL_ID _mil_image[N_FB_MAX];
	extern ROI _roi_fb[N_FB_MAX];
	extern short _roi_enable_acq[N_FB_MAX];
	// per il colore RGB
	extern MIL_ID _mil_image_col[N_FB_MAX_COL];
	extern ROI _roi_fb_col[N_FB_MAX_COL];
	extern short _roi_enable_acq_col[N_FB_MAX_COL];
	// fine colore RGB
#endif
	extern PARAM_SCHEDA _param_scheda;

#if defined(METEOR2) || defined(ORION)
	extern short _tempo_definito;
#endif

static short start_grab_interno(
	short fb, short chan, short xi, short yi, short xf, short yf,
	short inverti_field, short flag_col, short flag_wait, short tempo_definito,
	short flag_chan
);

/**********************************************************************/

short
sea2_start_grab(
	short fb,
	short chan,
	short xi,
	short yi,
	short xf,
	short yf
)
{
	return start_grab_interno(
		fb, chan, xi, yi, xf, yf, 0, /* inverti_field, non inverte */
		0 /* flag_col */, 1 /* flag_wait */, 0 /* tempo_definito */,
		1 /* flag_chan */
	);
}

/**********************************************************************/

/* sea2_start_grab1 effettua lo stesso lavoro di
   sea2_start_grab ma non aspetta il vertical blank e esce subito */

short
sea2_start_grab1(
	short fb,
	short chan,
	short xi,
	short yi,
	short xf,
	short yf
)
{
	return start_grab_interno(
		fb, chan, xi, yi, xf, yf, 0, /* inverti_field, non inverte */
		0 /* flag_col */, 0 /* flag_wait */, 0 /* tempo_definito */,
		1 /* flag_chan */
	);
}

/**************************************************************************/

/* sea2_start_grab2 effettua lo stesso lavoro di
   sea2_start_grab1 con l'aggiunta di inverti_field */

short
sea2_start_grab2(
	short fb,
	short chan,
	short xi,
	short yi,
	short xf,
	short yf,
	short inverti_field
)
{
	return start_grab_interno(
		fb, chan, xi, yi, xf, yf, inverti_field,
		0 /* flag_col */, 0 /* flag_wait */, 0 /* tempo_definito */,
		1 /* flag_chan */
	);
}

/**************************************************************************/

/* sea2_start_grab3 effettua lo stesso lavoro di
   sea2_start_grab1 con la possibilita' di definire se il grab va fatto
   all'infinito o a tempo definito */

short
sea2_start_grab3(
	short fb,
	short chan,
	short xi,
	short yi,
	short xf,
	short yf,
	short tempo_definito // solo un frame
)
{
	return start_grab_interno(
		fb, chan, xi, yi, xf, yf, 0 /* inverti_field */,
		0 /* flag_col */, 0 /* flag_wait */, tempo_definito,
		1 /* flag_chan */
	);
}

/**************************************************************************/

/* sea2_start_grab3 effettua lo stesso lavoro di
   sea2_start_grab1 con la possibilita' di definire se il grab va fatto
   all'infinito o a tempo definito, ma non cambia il canale video */

short
sea2_start_grab4(
	short fb,
	short xi,
	short yi,
	short xf,
	short yf,
	short tempo_definito // solo un frame
)
{
	return start_grab_interno(
		fb, 0, xi, yi, xf, yf, 0 /* inverti_field */,
		0 /* flag_col */, 0 /* flag_wait */, tempo_definito,
		0 /* flag_chan */
	);
}

/**********************************************************************/

short
sea2_start_grab_col(
	short fb,
	short chan,
	short xi,
	short yi,
	short xf,
	short yf
)
{
	return start_grab_interno(
		fb, chan, xi, yi, xf, yf, 0, /* inverti_field, non inverte */
		1 /* flag_col */, 1 /* flag_wait */, 0 /* tempo_definito */,
		1 /* flag_chan */
	);
}

/**********************************************************************/

/* sea2_start_grab_col1 effettua lo stesso lavoro di
   sea2_start_grab_col ma non aspetta il vertical blank e esce subito */

short
sea2_start_grab_col1(
	short fb,
	short chan,
	short xi,
	short yi,
	short xf,
	short yf
)
{
	return start_grab_interno(
		fb, chan, xi, yi, xf, yf, 0, /* inverti_field, non inverte */
		1 /* flag_col */, 0 /* flag_wait */, 0 /* tempo_definito */,
		1 /* flag_chan */
	);
}

/**************************************************************************/

/* sea2_start_grab_col2 effettua lo stesso lavoro di
   sea2_start_grab_col1 con l'aggiunta di inverti_field */

short
sea2_start_grab_col2(
	short fb,
	short chan,
	short xi,
	short yi,
	short xf,
	short yf,
	short inverti_field
)
{
	return start_grab_interno(
		fb, chan, xi, yi, xf, yf, inverti_field,
		1 /* flag_col */, 0 /* flag_wait */, 0 /* tempo_definito */,
		1 /* flag_chan */
	);
}

/**************************************************************************/

/* sea2_start_grab_col3 effettua lo stesso lavoro di
   sea2_start_grab_col1 con la possibilita' di definire se il grab va fatto
   all'infinito o a tempo definito */

short
sea2_start_grab_col3(
	short fb,
	short chan,
	short xi,
	short yi,
	short xf,
	short yf,
	short tempo_definito // solo un frame
)
{
	return start_grab_interno(
		fb, chan, xi, yi, xf, yf, 0 /* inverti_field */,
		1 /* flag_col */, 0 /* flag_wait */, tempo_definito,
		1 /* flag_chan */
	);
}

/**************************************************************************/

/* sea2_start_grab_col4 effettua lo stesso lavoro di
   sea2_start_grab_col1 con la possibilita' di definire se il grab va fatto
   all'infinito o a tempo definito, ma non cambia il canale video */

short
sea2_start_grab_col4(
	short fb,
	short xi,
	short yi,
	short xf,
	short yf,
	short tempo_definito // solo un frame
)
{
	return start_grab_interno(
		fb, 0, xi, yi, xf, yf, 0 /* inverti_field */,
		1 /* flag_col */, 0 /* flag_wait */, tempo_definito,
		0 /* flag_chan */
	);
}

/******************************************************************/

void
sea2_grab_enable(
	void
)
{
#ifdef MET
	if(!_param_scheda.scheda_aperta)
		return;
	MTdigGrabContinuousMio(_mil_system[_param_scheda.numero_scheda]);
#endif
}

/******************************************************************/

static short
start_grab_interno(
	short fb,
	short chan,
	short xi,
	short yi,
	short xf,
	short yf,
	short inverti_field,  /* 0:non inverte, 1:inverte */
	short flag_col, /* se != 0 si fa l'acquisizione a colori */
	short flag_wait, /* se != 0 si aspetta un v_blank prima dello start */
	short tempo_definito, // se != 0 si lancia una acquisizione che dura solo un frame
	short flag_chan // se != 0 si cambia il canale
)
{
//static long pippo=0;
	if(!_param_scheda.scheda_aperta)
		return SEAV_ERR_GRABBER_INIT;

/* controllo dei parametri */

	if(flag_col) {
		if(fb < _param_scheda.fb_min_col || fb > _param_scheda.fb_max_col)
			return SEAV_ERR_PARAMETER;
		if(chan < _param_scheda.chan_min_col || chan > _param_scheda.chan_max_col)
			return SEAV_ERR_PARAMETER;
		if(_param_scheda.tipo_tv == TV_MONO)
			return SEAV_ERR_OP_NOT_ALLOWED;
	} else {
		if(fb < _param_scheda.fb_min || fb > _param_scheda.fb_max)
			return SEAV_ERR_PARAMETER;
		if(chan < _param_scheda.chan_min || chan > _param_scheda.chan_max)
			return SEAV_ERR_PARAMETER;
		if(_param_scheda.tipo_tv == TV_RGB)
			return SEAV_ERR_OP_NOT_ALLOWED;
	}
	if(xi < _param_scheda.x_min || xi > _param_scheda.x_max)
		return SEAV_ERR_PARAMETER;
	if(yi < _param_scheda.y_min || yi > _param_scheda.y_max)
		return SEAV_ERR_PARAMETER;
	if(xf < _param_scheda.x_min || xf > _param_scheda.x_max)
		return SEAV_ERR_PARAMETER;
	if(yf < _param_scheda.y_min || yf > _param_scheda.y_max)
		return SEAV_ERR_PARAMETER;

	if(flag_col) {

#ifdef MET
		if(flag_wait)
			sea2_wait_v_blank();
		sea2_grab_enable();
		MTdigSelectGrabBufferMio(
			_mil_camera[_param_scheda.numero_scheda],
			_mil_image_col[fb], inverti_field,
			_roi_fb_col[fb].xi, _roi_fb_col[fb].yi,
			_roi_fb_col[fb].xf, _roi_fb_col[fb].yf,
			_roi_enable_acq_col[fb]
		);
#endif

#ifdef MET_MIL
		MdigGrabContinuous(
			_mil_camera[_param_scheda.numero_scheda], _mil_image_col[fb]
		);
#endif

#ifdef ORION // si usa la tecnica della meteor2
		_tempo_definito = tempo_definito;
		if(flag_chan)
			sea2_chan_col(chan);
		if(flag_wait)
			sea2_wait_v_blank();
		if(tempo_definito) {
			// ovviamente il M_GRAB_MODE deve essere M_ASYNCHRONOUS, altrimenti la
			// MdigGrab aspetta di aver finito prima di uscire
			MdigGrab(
				_mil_camera[_param_scheda.numero_scheda], _mil_image_col[fb]
			);
		} else {
			MdigControl(_mil_camera[_param_scheda.numero_scheda], M_GRAB_START_MODE, M_FIELD_START);
			MdigGrabContinuous(
				_mil_camera[_param_scheda.numero_scheda], _mil_image_col[fb]
			);
		}
#endif

#ifdef METEOR2
		_tempo_definito = tempo_definito;
		if(flag_chan)
			sea2_chan_col(chan);
		if(flag_wait)
			sea2_wait_v_blank();
		if(tempo_definito) {
			// ovviamente il M_GRAB_MODE deve essere M_ASYNCHRONOUS, altrimenti la
			// MdigGrab aspetta di aver finito prima di uscire
			MdigGrab(
				_mil_camera[_param_scheda.numero_scheda], _mil_image_col[fb]
			);
		} else {
			MdigControl(_mil_camera[_param_scheda.numero_scheda], M_GRAB_START_MODE, M_FIELD_START);
			MdigGrabContinuous(
				_mil_camera[_param_scheda.numero_scheda], _mil_image_col[fb]
			);
		}
#endif

	} else { // flag_col

#ifdef MET
		if(flag_wait)
			sea2_wait_v_blank();
		sea2_grab_enable();
		MTdigSelectGrabBufferMio(
			_mil_camera[_param_scheda.numero_scheda], _mil_image[fb],
			inverti_field, _roi_fb[fb].xi, _roi_fb[fb].yi, _roi_fb[fb].xf,
			_roi_fb[fb].yf, _roi_enable_acq[fb]
		);
		MTdigChannelMio(_mil_system[_param_scheda.numero_scheda], (long) chan);
#endif

#ifdef MET_MIL
		if(flag_chan)
			sea2_chan(chan);
		MdigGrabContinuous(
			_mil_camera[_param_scheda.numero_scheda], _mil_image[fb]
		);
#endif

#ifdef ORION
		_tempo_definito = tempo_definito;
		if(flag_chan)
			sea2_chan(chan);
		if(flag_wait)
			sea2_wait_v_blank();
		if(tempo_definito) {
			// ovviamente il M_GRAB_MODE deve essere M_ASYNCHRONOUS, altrimenti la
			// MdigGrab aspetta di aver finito prima di uscire
			MdigGrab(
				_mil_camera[_param_scheda.numero_scheda], _mil_image[fb]
			);
		} else {
			MdigControl(_mil_camera[_param_scheda.numero_scheda], M_GRAB_START_MODE, M_FIELD_START);
			MdigGrabContinuous(
				_mil_camera[_param_scheda.numero_scheda], _mil_image[fb]
			);
		}
#endif

#ifdef METEOR2
		_tempo_definito = tempo_definito;
		if(flag_chan)
			sea2_chan(chan);
		if(flag_wait)
			sea2_wait_v_blank();
		if(tempo_definito) {
			// ovviamente il M_GRAB_MODE deve essere M_ASYNCHRONOUS, altrimenti la
			// MdigGrab aspetta di aver finito prima di uscire
			MdigGrab(
				_mil_camera[_param_scheda.numero_scheda], _mil_image[fb]
			);
		} else {
			MdigControl(_mil_camera[_param_scheda.numero_scheda], M_GRAB_START_MODE, M_FIELD_START);
			MdigGrabContinuous(
				_mil_camera[_param_scheda.numero_scheda], _mil_image[fb]
			);
		}
//pippo = MdigInquire(_mil_camera[0], M_GRAB_START_MODE, M_NULL);
//		MdigControl(_mil_camera[_param_scheda.numero_scheda], M_GRAB_MODE, M_ASYNCHRONOUS);
//		MdigControl(_mil_camera[_param_scheda.numero_scheda], M_GRAB_HALT_ON_NEXT_FIELD, M_ENABLE);
#endif

	}

	return SEAV_OK_ALL;
}

/**********************************************************************/

// restituisce 1 se il segnale e' presente sul canale selezionato,
// 0 altrimenti

short
sea2_input_signal_present(
	void
)
{
#ifdef MET
	if(MTdigInquire(
		_mil_camera[_param_scheda.numero_scheda], M_DIG_INPUT_SIGNAL_PRESENT, M_NULL
	) == 1)
		return 1;
	else
		return 0;
#endif

#ifdef MET_MIL
	if(MdigInquire(
		_mil_camera[_param_scheda.numero_scheda], M_INPUT_SIGNAL_PRESENT, M_NULL
	) == M_YES)
		return 1;
	else
		return 0;
#endif

#ifdef ORION
	if(MdigInquire(
		_mil_camera[_param_scheda.numero_scheda], M_INPUT_SIGNAL_PRESENT, M_NULL
	) == M_YES)
		return 1;
	else
		return 0;
#endif

#ifdef METEOR2 // non funziona sulla multichannel
	if(MdigInquire(
		_mil_camera[_param_scheda.numero_scheda], M_INPUT_SIGNAL_PRESENT, M_NULL
	) == M_YES)
		return 1;
	else
		return 0;
#endif

	return 1;
}

/**************************************************************************/

void
sea2_chan(
	short chan
)
{
	if(!_param_scheda.scheda_aperta)
		return;
#ifdef MET
	MTdigChannelMio(_mil_system[_param_scheda.numero_scheda], (long) chan);
#endif
#ifdef MET_MIL
	switch(chan) {
		case 0:
			MdigChannel(_mil_camera[_param_scheda.numero_scheda], M_CH0);
			break;
		case 1:
			MdigChannel(_mil_camera[_param_scheda.numero_scheda], M_CH1);
			break;
		case 2:
			MdigChannel(_mil_camera[_param_scheda.numero_scheda], M_CH2);
			break;
		case 3:
			MdigChannel(_mil_camera[_param_scheda.numero_scheda], M_CH3);
			break;
		case 4:
			MdigChannel(_mil_camera[_param_scheda.numero_scheda], M_RGB);
			break;
	}
#endif
#ifdef ORION
	switch(chan) {
		case 0:
			MdigChannel(_mil_camera[_param_scheda.numero_scheda], M_CH0);
			break;
		case 1:
			MdigChannel(_mil_camera[_param_scheda.numero_scheda], M_CH1);
			break;
		case 2:
			MdigChannel(_mil_camera[_param_scheda.numero_scheda], M_CH2);
			break;
		case 3:
			MdigChannel(_mil_camera[_param_scheda.numero_scheda], M_CH3);
			break;
		case 4:
			MdigChannel(_mil_camera[_param_scheda.numero_scheda], M_CH4);
			break;
		case 5:
			MdigChannel(_mil_camera[_param_scheda.numero_scheda], M_CH5);
			break;
		case 6:
			MdigChannel(_mil_camera[_param_scheda.numero_scheda], M_CH6);
			break;
		case 7:
			MdigChannel(_mil_camera[_param_scheda.numero_scheda], M_CH7);
			break;
		case 8:
//			MdigChannel(_mil_camera[_param_scheda.numero_scheda], M_RGB0);
			break;
		case 9:
//			MdigChannel(_mil_camera[_param_scheda.numero_scheda], M_RGB1);
			break;
	}
#endif
#ifdef METEOR2
	switch(chan) {
		case 0:
			MdigChannel(_mil_camera[_param_scheda.numero_scheda], M_CH0);
			break;
		case 1:
			MdigChannel(_mil_camera[_param_scheda.numero_scheda], M_CH1);
			break;
		case 2:
			MdigChannel(_mil_camera[_param_scheda.numero_scheda], M_CH2);
			break;
		case 3: // e' il sincronismo: non puo' essere usato come sorgente video
			break;
		case 4:
			MdigChannel(_mil_camera[_param_scheda.numero_scheda], M_CH4);
			break;
		case 5:
			MdigChannel(_mil_camera[_param_scheda.numero_scheda], M_CH5);
			break;
		case 6:
			MdigChannel(_mil_camera[_param_scheda.numero_scheda], M_CH6);
			break;
		case 7: // e' il sincronismo: non puo' essere usato come sorgente video
			break;
	}
#endif
}

/**************************************************************************/

// serve solo alla meteor2 e alla orion

void
sea2_chan_col(
	short chan
)
{
	if(!_param_scheda.scheda_aperta)
		return;

#ifdef ORION
	switch(chan) {
		case 0:
			MdigChannel(_mil_camera[_param_scheda.numero_scheda], M_CH0 + M_SIGNAL);
			sea2_sync_chan(3);
			break;
		case 1:
			MdigChannel(_mil_camera[_param_scheda.numero_scheda], M_CH1 + M_SIGNAL);
			sea2_sync_chan(7);
			break;
	}
#endif

#ifdef METEOR2
	switch(chan) {
		case 0:
			MdigChannel(_mil_camera[_param_scheda.numero_scheda], M_CH0);
			break;
		case 1:
			MdigChannel(_mil_camera[_param_scheda.numero_scheda], M_CH1);
			break;
	}
#endif
}

/*****************************************************************/

/* seleziona il canale di sincronismo; per la meteor il canale 4, che non esiste, e'
   l'RGB */

short
sea2_sync_chan(
	short chan
)
{
	if(!_param_scheda.scheda_aperta)
		return SEAV_ERR_GRABBER_INIT;

#ifdef MET
	if(chan < 0 || chan > 4)
		return SEAV_ERR_PARAMETER;
	switch(chan) {
		case 0:
			MTdigChannel(_mil_camera[_param_scheda.numero_scheda], M_CH0);
			break;
		case 1:
			MTdigChannel(_mil_camera[_param_scheda.numero_scheda], M_CH1);
			break;
		case 2:
			MTdigChannel(_mil_camera[_param_scheda.numero_scheda], M_CH2);
			break;
		case 3:
			MTdigChannel(_mil_camera[_param_scheda.numero_scheda], M_CH3);
			break;
		case 4:
			MTdigChannel(_mil_camera[_param_scheda.numero_scheda], M_RGB);
			break;
	}
#endif

#ifdef MET_MIL
	if(chan < 0 || chan > 4)
		return SEAV_ERR_PARAMETER;
	switch(chan) {
		case 0:
			MdigChannel(_mil_camera[_param_scheda.numero_scheda], M_CH0 + M_SYNC);
			break;
		case 1:
			MdigChannel(_mil_camera[_param_scheda.numero_scheda], M_CH1 + M_SYNC);
			break;
		case 2:
			MdigChannel(_mil_camera[_param_scheda.numero_scheda], M_CH2 + M_SYNC);
			break;
		case 3:
			MdigChannel(_mil_camera[_param_scheda.numero_scheda], M_CH3 + M_SYNC);
			break;
		case 4:
			MdigChannel(_mil_camera[_param_scheda.numero_scheda], M_RGB);
			break;
	}
#endif

#ifdef ORION
	// la MIL non sopporta il + M_SYNC
	if(chan < 0 || chan > 7)
		return SEAV_ERR_PARAMETER;
	if(_param_scheda.tipo_tv == TV_RGB) {
		switch(chan) {
			case 0:
			case 3: // si usano questi valori per analogia
				MdigChannel(_mil_camera[_param_scheda.numero_scheda], M_CH3 + M_SYNC);
				break;
			case 1:
			case 7:
				MdigChannel(_mil_camera[_param_scheda.numero_scheda], M_CH7 + M_SYNC);
				break;
			case 2: // non considero gli altri valori
			case 4:
			case 5:
			case 6:
				break;
		}
	} else {
		switch(chan) {
			case 0:
				MdigChannel(_mil_camera[_param_scheda.numero_scheda], M_CH0 + M_SYNC);
				break;
			case 1:
				MdigChannel(_mil_camera[_param_scheda.numero_scheda], M_CH1 + M_SYNC);
				break;
			case 2:
				MdigChannel(_mil_camera[_param_scheda.numero_scheda], M_CH2 + M_SYNC);
				break;
			case 3:
				MdigChannel(_mil_camera[_param_scheda.numero_scheda], M_CH3 + M_SYNC);
				break;
			case 4:
				MdigChannel(_mil_camera[_param_scheda.numero_scheda], M_CH4 + M_SYNC);
				break;
			case 5:
				MdigChannel(_mil_camera[_param_scheda.numero_scheda], M_CH5 + M_SYNC);
				break;
			case 6:
				MdigChannel(_mil_camera[_param_scheda.numero_scheda], M_CH6 + M_SYNC);
				break;
			case 7:
				MdigChannel(_mil_camera[_param_scheda.numero_scheda], M_CH7 + M_SYNC);
				break;
		}
	}
#endif

#ifdef METEOR2
	if(chan < 0 || chan > 7)
		return SEAV_ERR_PARAMETER;
	switch(chan) {
		case 0: // r1
			MdigChannel(_mil_camera[_param_scheda.numero_scheda], M_CH0 + M_SYNC);
			break;
		case 1: // g1
			MdigChannel(_mil_camera[_param_scheda.numero_scheda], M_CH1 + M_SYNC);
			break;
		case 2: // b1
			MdigChannel(_mil_camera[_param_scheda.numero_scheda], M_CH2 + M_SYNC);
			break;
		case 3: // sync
			MdigChannel(_mil_camera[_param_scheda.numero_scheda], M_CH3 + M_SYNC);
			break;
		case 4: // r2
			MdigChannel(_mil_camera[_param_scheda.numero_scheda], M_CH4 + M_SYNC);
			break;
		case 5: // g2
			MdigChannel(_mil_camera[_param_scheda.numero_scheda], M_CH5 + M_SYNC);
			break;
		case 6: // b2
			MdigChannel(_mil_camera[_param_scheda.numero_scheda], M_CH6 + M_SYNC);
			break;
		case 7: // sync
			MdigChannel(_mil_camera[_param_scheda.numero_scheda], M_CH7 + M_SYNC);
			break;
	}
#endif

	return SEAV_OK_ALL;
}

/**************************************************************************/

// per orion e meteor2

short sea2_is_grab_in_progress()
{
	if(!_param_scheda.scheda_aperta)
		return 0;

#ifdef ORION
	if(MdigInquire(
		_mil_camera[_param_scheda.numero_scheda], M_GRAB_IN_PROGRESS, M_NULL
	) == M_YES)
		return 1;
	else
		return 0;
#endif
	
#ifdef METEOR2
	if(MdigInquire(
		_mil_camera[_param_scheda.numero_scheda], M_GRAB_IN_PROGRESS, M_NULL
	) == M_YES)
		return 1;
	else
		return 0;
#endif
	
	return 0;
}




