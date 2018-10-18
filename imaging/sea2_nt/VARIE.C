
/*
	sea2_indirizzo_memoria
	sea2_wait_v_blank
	sea2_v_filter_on_off
	sea2_wait_h_blank
	sea2_leggi_field
	sea2_start_pulse_width
	sea2_start_double_pulse
	sea2_init_serial_port
	sea2_are_data_pending_on_serial_port
	sea2_read_char_from_serial_port
	sea2_write_string_on_serial_port
	sea2_set_pci_latency
	sea2_set_automatic_grab_on_trigger
	sea2_start_pulse_width_automatic_grab
	sea2_set_user_bit_out // 2.31 OCV
*/

#include <stdlib.h>
#include <string.h>
#include <util_nt.h>
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
	extern unsigned char *_mem_address_col[N_FB_MAX_COL];
	extern long *_offset_riga;
	/* contiene, per ogni riga, l'offset del primo pixel */
#endif
#if defined(MET) || defined(MET_MIL) || defined(ORION)
	extern MIL_ID _mil_system[N_BOARD_MAX];
#endif
#if defined(METEOR2)
	extern MIL_ID _mil_system[N_BOARD_MAX], _mil_camera[N_BOARD_MAX];
#endif
	extern PARAM_SCHEDA _param_scheda;


/***********************************************************************

;  Nome:    
	sea2_indirizzo_memoria
;
;  Funzione:
	ritorna l'indirizzo di un pixel nella memoria del PC
;
;  Formato:
	unsigned char *sea2_indirizzo_memoria(xp, yp)
;
;  Argomenti:    
	short xp           ascissa del pixel
	short yp           ordinata del pixel

;
;  Descrizione:
     {\bf sea2_indirizzo_memoria}() ritorna l'indirizzo del pixel
	 nella memoria del PC; il frame buffer e' quello selezionato
	 con la routine {\bf sea2_seleziona_buffer}();
;
;**********************************************************************/

unsigned char *
sea2_indirizzo_memoria(
	short xp,
	short yp
)
{
	if(!_param_scheda.scheda_aperta)
		return NULL;

	if(_param_scheda.tipo_tv == TV_RGB) {
		unsigned char *mem = _mem_address[_param_scheda.fb_lavoro];
		unsigned char *ausmem = mem + (long) (xp<<2) + _offset_riga[yp];

		return ausmem;
	} else {
		unsigned char *mem = _mem_address[_param_scheda.fb_lavoro];
		unsigned char *ausmem = mem + (long) xp + _offset_riga[yp];

		return ausmem;
	}

	return NULL;
}

unsigned char *
sea2_indirizzo_memoria_col(
	short xp,
	short yp
)
{
	if(!_param_scheda.scheda_aperta)
		return NULL;

	if(_param_scheda.tipo_tv == TV_RGB) {
		unsigned char *mem = _mem_address_col[_param_scheda.fb_lavoro];
		unsigned char *ausmem = mem + (long) (xp<<2) + _offset_riga[yp];

		return ausmem;
	}

	return NULL;
}

/***********************************************************************

;  Nome:    
	sea2_wait_v_blank
;
;  Funzione:
     attende il successivo vertical blank
;
;  Formato:
	long sea2_wait_v_blank()

;
;  Valore di ritorno: 
	n_cicli          numero di letture fatte sulla scheda
;
;  Descrizione:
     {\bf sea2_wait_v_blank}() attende il successivo vartical blank;
	 ritorna all'inizio del vertical blank; se la scheda si trova
	 nello stato di vertical blank all'ingresso della routine, si aspetta
	 comunque il vertical blank successivo;

;
;**********************************************************************/

long
sea2_wait_v_blank(
	void
)
{
	long n_colpi = 0L;
#ifdef MET
	long field;
#endif

	if(!_param_scheda.scheda_aperta)
		return n_colpi;

#ifdef MET
	field = MTdigReadField(_mil_system[_param_scheda.numero_scheda]);
	while(field == MTdigReadField(_mil_system[_param_scheda.numero_scheda]) &&
	      n_colpi < 100L)
		n_colpi ++;
#endif

	return n_colpi;
}

/****************************************************************************/

/* con il filtro off si puo' resettare con maggiore velocita' la fase
  verticale del segnale video */

void
sea2_v_filter_on_off(
	short on
)
{
	if(!_param_scheda.scheda_aperta)
		return;
#ifdef MET
	MTfiltro_on_off(_mil_system[_param_scheda.numero_scheda], on);
#endif /* MET */
}


/***********************************************************************

;  Nome:    
	sea2_wait_h_blank
;
;  Funzione:
     attende il successivo horizontal blank
;
;  Formato:
	short sea2_wait_h_blank(punt)

;
;  Argomenti:
	void *punt              indirizzo ai dati necessari

;
;  Descrizione:
     {\bf sea2_wait_h_blank}() attende il cambiamento della riga;
	 ritorna all'inizio della nuova riga oppure se si verifica un timeout;
;
;**********************************************************************/

short
sea2_wait_h_blank(
	void *punt
)
{

#if defined(MET) || defined(ASS) || defined(ORION) || defined(METEOR2)

	short i;

/* punt[0] e' l'indirizzo a cui leggere, punt[1] e' la maschera; il puntatore
   va castato a (int *) */
	int addr = ((int *) punt)[0];
	int mask = ((int *) punt)[1];
	int n_colpi = ((int *) punt)[2];

#if 0
long aus[1000];
for(i=0; i<1000; i++)
	aus[i] = util_portio_inp(addr) & mask;
n_colpi = 0;
#endif

	for(i=n_colpi; i>0; i--) {
		if((util_portio_inp(addr) & mask))
			break;
	}
	for(; i>0; i--) {
		if(!(util_portio_inp(addr) & mask))
			return SEAV_OK_ALL;
	}
	if(!i)
		return SEAV_ERR_HD_TIMEOUT;
#endif

	return SEAV_OK_ALL;
}


/***********************************************************************

;  Nome:    
	sea2_leggi_field
;
;  Funzione:
     legge il numero del field in corso
;
;  Formato:
	short sea2_leggi_field()

;
;  Descrizione:
     {\bf sea2_leggi_field}() ritorna il numero del field in corso (0 o 1);
	 il numero del field e' 0 per le righe pari e 1 per le righe dispari;

;
;**********************************************************************/

short
sea2_leggi_field(
	void
)
{
	if(!_param_scheda.scheda_aperta)
		return 0;

#ifdef MET
	if(MTdigReadField(_mil_system[_param_scheda.numero_scheda]))
		return 1; /* field dispari */
	else
		return 0; /* field pari */
#endif

	return 0; /* field pari */
}

/**********************************************************************/

short
sea2_init_serial_port(
	long speed,  // valore numerico (ad esempio 9600)
	long parity, // 0:none, 1:odd, 2:even
	long bits,   // 7, 8
	long stops   // 1, 2
)
{
#ifdef METEOR2
	long ControlValue;

	switch(parity) {
		case 0: ControlValue = M_DISABLE; break;
		case 1: ControlValue = M_ODD; break;
		case 2: ControlValue = M_EVEN; break;
		default: ControlValue = M_DISABLE; break;
	}
	MdigControl(
		_mil_camera[_param_scheda.numero_scheda], M_UART_PARITY, ControlValue
	);
	MdigControl(
		_mil_camera[_param_scheda.numero_scheda], M_UART_SPEED, speed
	);
	MdigControl(
		_mil_camera[_param_scheda.numero_scheda], M_UART_DATA_LENGTH, bits
	);
	MdigControl(
		_mil_camera[_param_scheda.numero_scheda], M_UART_STOP_BITS, stops
	);
	MdigControl(
		_mil_camera[_param_scheda.numero_scheda], M_UART_TIMEOUT, 1000 // ms
	);
	MdigControl(
		_mil_camera[_param_scheda.numero_scheda],
		M_UART_READ_STRING_MAXIMUM_LENGTH, 10 // leggo max 10 caratteri
	);
#endif

	return SEAV_OK_ALL; // la routine potrebbe in futuro ritornare qualcosa di significativo
}

/**********************************************************************/

// ritorna 1 se c'e' qualcosa sulla porta, 0 altrimenti
short sea2_are_data_pending_on_serial_port()
{
#ifdef METEOR2
	if(MdigInquire(
		_mil_camera[_param_scheda.numero_scheda],
		M_UART_DATA_PENDING, M_NULL
	) == M_TRUE)
		return 1;
	else
		return 0;
#endif
	return 0;
}

/**********************************************************************/

// ritorna il carattere letto
char sea2_read_char_from_serial_port()
{
	char msg[5];
	msg[0] = 0;

#ifdef METEOR2
	// voglio leggere un solo carattere
	MdigControl(
		_mil_camera[_param_scheda.numero_scheda],
		M_UART_READ_STRING_LENGTH, 1
	);
	MdigControl(
		_mil_camera[_param_scheda.numero_scheda],
		M_UART_READ_STRING, (unsigned long) msg
	);
#endif
	return msg[0];
}

/**********************************************************************/

// ritorna SEAV_OK_ALL se scrittura a buon fine
short sea2_write_string_on_serial_port(char *st)
{
#ifdef METEOR2
	MdigControl(
		_mil_camera[_param_scheda.numero_scheda],
		M_UART_WRITE_STRING_LENGTH, strlen(st)
	);
	MdigControl(
		_mil_camera[_param_scheda.numero_scheda],
		M_UART_WRITE_STRING, (unsigned long) st
	);
#endif
	return SEAV_OK_ALL;
}

/**********************************************************************/

void sea2_set_pci_latency(int latency)
{
#ifdef METEOR2
	MsysControl(_mil_system[_param_scheda.numero_scheda], M_PCI_LATENCY, latency);
#endif
}

/**********************************************************************/

// imposta il modo di acquisizione automatico su trigger
short sea2_set_automatic_grab_on_trigger(short flag)
{
#ifdef METEOR2
	if(flag) {
		MdigControl(
			_mil_camera[_param_scheda.numero_scheda], M_GRAB_TRIGGER, M_ENABLE
		);
		MdigControl(
			_mil_camera[_param_scheda.numero_scheda], M_GRAB_TRIGGER_SOURCE, M_TIMER2
		);
		MdigControl( // modifica 2.53.0 OCV da M_EDGE_RISING a M_DEFAULT
			_mil_camera[_param_scheda.numero_scheda], M_GRAB_TRIGGER_MODE, M_DEFAULT
		);
		MdigControl(
			_mil_camera[_param_scheda.numero_scheda], M_GRAB_EXPOSURE_SOURCE+M_TIMER2, M_TIMER1
		);
//		MdigControl(
//			_mil_camera[_param_scheda.numero_scheda], M_GRAB_TIMEOUT, 150
//		);
	} else {
		MdigControl(
			_mil_camera[_param_scheda.numero_scheda], M_GRAB_TRIGGER, M_DISABLE
		);
	}
#endif

	return SEAV_OK_ALL; // la routine potrebbe in futuro ritornare qualcosa di significativo
}

/**********************************************************************/

// 2.53.0 imposta la polarita' del timer1
// se level=0 l'impulso e' negativo, se level=1 l'impulso e' positivo
short sea2_set_timer_level(short timer/*1,2*/, short level/*0,1*/)
{
#ifdef METEOR2
	if(timer == 1) {
		if(level == 0)
			MdigControl(_mil_camera[_param_scheda.numero_scheda], M_GRAB_EXPOSURE_MODE+M_TIMER1, M_LEVEL_LOW);
		else
			MdigControl(_mil_camera[_param_scheda.numero_scheda], M_GRAB_EXPOSURE_MODE+M_TIMER1, M_LEVEL_HIGH);
	} else if(timer == 2) {
		if(level == 0)
			MdigControl(_mil_camera[_param_scheda.numero_scheda], M_GRAB_EXPOSURE_MODE+M_TIMER2, M_LEVEL_LOW);
		else
			MdigControl(_mil_camera[_param_scheda.numero_scheda], M_GRAB_EXPOSURE_MODE+M_TIMER2, M_LEVEL_HIGH);
	}
#endif

	return SEAV_OK_ALL; // la routine potrebbe in futuro ritornare qualcosa di significativo
}

/**********************************************************************/

short sea2_set_exposure_time(long n_ns) // numero di ns
{
#ifdef METEOR2
	// imposto il tempo in nanosecondi
	MdigControl(
		_mil_camera[_param_scheda.numero_scheda],
		M_GRAB_EXPOSURE_TIME+M_TIMER1, n_ns
	);
#endif

	return SEAV_OK_ALL; // la routine potrebbe in futuro ritornare qualcosa di significativo
}

/**********************************************************************/

/* attenzione!!!: dalle prove sulla TM6300 double speed risulta che se si imposta
   tempo in questa routine il VINIT non e' piu' sincrono con l'HD;
   pero', la prima volta che si supera un tempo lungo, il VINIT diventa
   sicrono!!! e mantiene tale caratteristica al variare dei tempi; quando
   poi si re-inizializza la scheda con un altro start del programma si
   perde ancora la sincronizzazione	 */
short sea2_start_pulse_width(long n_ns) // numero di ns
{
#ifdef METEOR2
#if 1
	// imposto il tempo in nanosecondi
	MdigControl(
		_mil_camera[_param_scheda.numero_scheda],
		M_GRAB_EXPOSURE_TIME+M_TIMER1, n_ns
	);
#endif
	// effettuo l'esposizione: la routine ritorna subito,
	// qualunque sia il tempo usato
	MdigControl(
		_mil_camera[_param_scheda.numero_scheda],
		M_GRAB_EXPOSURE+M_TIMER1, M_ACTIVATE
	);
#endif

	return SEAV_OK_ALL; // la routine potrebbe in futuro ritornare qualcosa di significativo
}

/**********************************************************************/

short sea2_start_double_pulse(long n_ns) // numero di ns tra i due fronti
{
#ifdef METEOR2
#if 0
	// imposto il tempo in nanosecondi dei due impulsi (127 us):
	// non serve; sono impostati dal dcf
	MdigControl(
		_mil_camera[_param_scheda.numero_scheda],
		M_GRAB_EXPOSURE_TIME+M_TIMER1, 127000L
	);
	MdigControl(
		_mil_camera[_param_scheda.numero_scheda],
		M_GRAB_EXPOSURE_TIME+M_TIMER2, 127000L
	);
#endif

	// imposto il ritardo del secondo timer in nanosecondi: e' triggerato dal
	// fronte di discesa del primo timer
	MdigControl(
		_mil_camera[_param_scheda.numero_scheda],
		M_GRAB_EXPOSURE_TIME_DELAY+M_TIMER2, n_ns
	);
	// effettuo l'esposizione: la routine ritorna subito,
	// qualunque sia il tempo usato
	MdigControl(
		_mil_camera[_param_scheda.numero_scheda],
		M_GRAB_EXPOSURE+M_TIMER1, M_ACTIVATE
	);

#endif

	return SEAV_OK_ALL; // la routine potrebbe in futuro ritornare qualcosa di significativo
}

/**********************************************************************/

// 2.31 OCV
void sea2_set_user_bit_out(
	short numero_bit, // 0,1
	short valore // 0: pin 24, 1: pin 8 del DSUB44
)
{
#ifdef METEOR2
	MdigControl(
		_mil_camera[_param_scheda.numero_scheda],
		M_USER_BIT+3+numero_bit, valore
	);
#endif
}

