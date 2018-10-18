
/*
	sea2_stop_grab
	sea2_stop_grab1
	sea2_grab_disable
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

/****************************************************************************/

/* variabili globali definite in init.c */

#if defined(MET)
	extern MIL_ID _mil_system[N_BOARD_MAX];
#endif
#if defined(MET_MIL) || defined(ORION) || defined(METEOR2)
	extern MIL_ID _mil_camera[N_BOARD_MAX];
#endif

#if defined(METEOR2) || defined(ORION)
	extern short _tempo_definito;
#endif

extern PARAM_SCHEDA _param_scheda;

/**********************************************************************/

void
sea2_stop_grab(
	void
)
{
	if(!_param_scheda.scheda_aperta)
		return;

#ifdef MET
	sea2_wait_v_blank();
	sea2_grab_disable();
#endif

#ifdef MET_MIL
	MdigHalt(_mil_camera[_param_scheda.numero_scheda]);
#endif

#ifdef ORION
	sea2_wait_v_blank();
	if(_tempo_definito)
		MdigGrabWait(_mil_camera[_param_scheda.numero_scheda], M_GRAB_END);
	else
		MdigHalt(_mil_camera[_param_scheda.numero_scheda]);
#endif

#ifdef METEOR2
	sea2_wait_v_blank();
	if(_tempo_definito)
		MdigGrabWait(_mil_camera[_param_scheda.numero_scheda], M_GRAB_END);
	else
		MdigHalt(_mil_camera[_param_scheda.numero_scheda]);
#endif

	return;
}

/**********************************************************************/

/* sea2_stop_grab1 effettua lo stesso lavoro di
   sea2_stop_grab ma non aspetta il vertical blank */

void
sea2_stop_grab1(
	void
)
{
//long pippo;
	if(!_param_scheda.scheda_aperta)
		return;

#ifdef MET
	sea2_grab_disable();
#endif

#ifdef MET_MIL
	MdigHalt(_mil_camera[_param_scheda.numero_scheda]);
#endif

#ifdef ORION
	if(_tempo_definito)
		MdigGrabWait(_mil_camera[_param_scheda.numero_scheda], M_GRAB_END);
	else {
		MdigControl(_mil_camera[_param_scheda.numero_scheda], M_GRAB_HALT_ON_NEXT_FIELD, M_ENABLE);
		MdigHalt(_mil_camera[_param_scheda.numero_scheda]);
	}
#endif

#ifdef METEOR2
	//MdigControl(_mil_camera[_param_scheda.numero_scheda], M_GRAB_HALT_ON_NEXT_FIELD, M_ENABLE);
//	MdigHalt(_mil_camera[_param_scheda.numero_scheda]);
	if(_tempo_definito)
		MdigGrabWait(_mil_camera[_param_scheda.numero_scheda], M_GRAB_END);
	else
		MdigHalt(_mil_camera[_param_scheda.numero_scheda]);
#endif

	return;
}

/******************************************************************/

void
sea2_grab_disable(
	void
)
{
	if(!_param_scheda.scheda_aperta)
		return;

#ifdef MET
	MTdigGrabStopMio(_mil_system[_param_scheda.numero_scheda]);
#endif
#ifdef MET_MIL
	MdigHalt(_mil_camera[_param_scheda.numero_scheda]);
#endif

}


