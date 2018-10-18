

#include <conio.h>
#include <util_nt.h>

static unsigned long internal_counter_high = 0L;
static unsigned long internal_counter_low = 0L;

                                                                  
/***********************************************************************

;  Nome:    
     util_set_8254
;
;  Funzione:
     programma il chip INTEL 8254
;
;  Formato:
     void util_set_8254()
;
;  Descrizione:
     {\bf util_set_8254}() programma il chip INTEL 8254 per utilizzare
	 il contatore n.2 in modo binario, ricevere, in fase di programmazione,
	 prima il byte meno significativo ed emettere un'onda quadra (modo 3);
;
;**********************************************************************/

void
util_set_8254(
	void
)
{
	util_portio_outp(TIMER_CTRL, 0xb6);
}


/***********************************************************************

;  Nome:    
     util_set_8254_one_shot
;
;  Funzione:
     programma il chip INTEL 8254 per generare un impulso singolo
;
;  Formato:
     void util_set_8254_one_shot()
;
;  Descrizione:
     {\bf util_set_8254_one_shot}() programma il chip INTEL 8254 per utilizzare
	 il contatore n.2 in modo binario, ricevere, in fase di programmazione,
	 prima il byte meno significativo ed emettere un impulso singolo
	 la cui durata e' pari al tempo che impiega il contatore ad arrivare
	 a 0 (modo 1);
;
;**********************************************************************/

void
util_set_8254_one_shot(
	void
)
{
	util_portio_outp(TIMER_CTRL, 0xb2);
	util_portio_outp(TIMER_PB, util_portio_inp(TIMER_PB) & 0xfe);  /* abbasso il GATE che fa partire lo shot */
}


/***********************************************************************

;  Nome:    
     util_set_freq
;
;  Funzione:
     programma la frequenza di emissione del contatore
;
;  Formato:
     void util_set_freq(freq)
;
;  Argomenti:    
     double freq      frequenza dell'onda quadra emessa dal contatore (Hz)
;
;  Descrizione:
     {\bf set_freq}() imposta la frequenza dell'onda quadra emessa
	 dal contatore; cio' viene fatto
	 scrivendo nel contatore a decremento a 16 bit (n.2), che riceve
	 in ingresso l'onda quadra a frequenza TIMER_F_MAX, il valore
	 TIMER_F_MAX/freq: pertanto la frequenza massima generabile e'
	 pari a TIMER_F_MAX (1.19318 MHz, 838.0965 ns), mentre la minima 
	 TIMER_F_MAX / 65536 (18.20681 Hz, 54.9245 ms);
;
;**********************************************************************/

void
util_set_freq(
	double freq
)
{
	int divider;

	divider = (int) (TIMER_F_MAX / freq);
	util_portio_outp(TIMER_CONT, divider & 255);
	util_portio_outp(TIMER_CONT, (divider>>8) & 255);
}


/***********************************************************************

;  Nome:    
     util_set_time
;
;  Funzione:
     programma la durata dello shot im modo one-shot
;
;  Formato:
     void util_set_time(time)
;
;  Argomenti:    
     double time      durata dello shot in secondi
;
;  Descrizione:
     {\bf util_set_time}() imposta la durata dell'impulso emesso;
	 cio' viene fatto scrivendo nel contatore a decremento a 16 bit (n.2),
	 che riceve in ingresso l'onda quadra a frequenza TIMER_F_MAX, il valore
	 time*TIMER_F_MAX: pertanto la durata massima generabile e'
	 pari a 54.9 ms, mentre la minima e' 0.838 us (1/TIMER_F_MAX);
;
;**********************************************************************/

void
util_set_time(
	double time
)
{
	long divider;

	if(time > 0.0549)
		divider = 65535L;
	else if(time < 0.0000009)
		divider = 1L;
	else
		divider = (long) (time * TIMER_F_MAX);
	util_portio_outp(TIMER_CONT, (int) (divider & 255L));
	util_portio_outp(TIMER_CONT, (int) ((divider>>8) & 255L));
}


/***********************************************************************

;  Nome:    
     util_set_divider
;
;  Funzione:
     programma direttamente il divisore della frequenza TIMER_F_MAX
;
;  Formato:
     void util_set_divider(value)
;
;  Argomenti:    
     unsigned int value      valore del divisore di frequenza
;
;  Descrizione:
     {\bf util_set_divider}() imposta il valore del divisore;
	 cio' viene fatto scrivendo nel contatore a decremento a 16 bit (n.2),
	 che riceve in ingresso l'onda quadra a frequenza TIMER_F_MAX, il valore
	 value; il modo di lavoro deve essere gia' stato programmato in
	 precedenza;
;
;**********************************************************************/

void
util_set_divider(
	long value
)
{
	util_portio_outp(TIMER_CONT, value & 255L);
	util_portio_outp(TIMER_CONT, (value>>8) & 255L);
}


/***********************************************************************

;  Nome:    
     util_start_clock
;
;  Funzione:
     abilita il conteggio da parte del contatore
;
;  Formato:
     void util_start_clock()
;
;  Descrizione:
     {\bf util_start_clock}() abilita il conteggio da parte del contatore
	 che deve essere stato precedentemente programmato con le funzioni
	 {\bf util_set_8254}() e {\bf util_set_freq}();
;
;**********************************************************************/

void
util_start_clock(
	void
)
{
	util_portio_outp(TIMER_PB, util_portio_inp(TIMER_PB) | 0x01); /* si abilita il conteggio */
}


/***********************************************************************

;  Nome:    
     util_start_shot
;
;  Funzione:
     abilita il conteggio da parte del contatore
;
;  Formato:
     void util_start_shot()
;
;  Descrizione:
     {\bf util_start_shot}() abilita il conteggio da parte del contatore
	 che deve essere stato precedentemente programmato con le funzioni
	 {\bf util_set_8254_one_shot}() e {\bf util_set_time}(); l'uscita va bassa
	 sul fronte di salita dell'impulso di start e ritorna alta al termine
	 del tempo programmato;
;
;**********************************************************************/

void
util_start_shot(
	void
)
{
	short i;

	/* alzo il gate per far partire lo shot */
	util_portio_outp(TIMER_PB, util_portio_inp(TIMER_PB) | 0x01);

	/* controllo che lo shot sia partito */
	for(i=0; i<1000; i++) {
		if(!(util_portio_inp(TIMER_PB) & 0x20)) { /* lo shot e' partito */
			/* abbasso il gate */
			util_portio_outp(TIMER_PB, util_portio_inp(TIMER_PB) & 0xfe);
			return;
		}
	}
}


/***********************************************************************

;  Nome:    
     util_start_beep
;
;  Funzione:
     abilita l'uscita del contatore verso l'altoparlante
;
;  Formato:
     void util_start_beep()
;
;  Descrizione:
     {\bf util_start_beep}() abilita l'uscita del contatore verso l'altoparlante
	 del PC; cio' permette di controllare "acusticamente" il lavoro del
	 contatore;
;
;**********************************************************************/

void
util_start_beep(
	void
)
{
	util_portio_outp(TIMER_PB, util_portio_inp(TIMER_PB) | 0x02);
}


/***********************************************************************

;  Nome:    
     util_stop_clock
;
;  Funzione:
     disabilita il conteggio da parte del contatore
;
;  Formato:
     void util_stop_clock()
;
;  Descrizione:
     {\bf util_stop_clock}() disabilita il conteggio da parte del contatore;
;
;**********************************************************************/

void
util_stop_clock(
	void
)
{
	util_portio_outp(TIMER_PB, util_portio_inp(TIMER_PB) & 0xfe);
}


/***********************************************************************

;  Nome:    
     util_stop_beep
;
;  Funzione:
     disabilita l'uscita del contatore verso l'altoparlante
;
;  Formato:
     void util_stop_beep()
;
;  Descrizione:
     {\bf util_stop_beep}() disabilita l'uscita del contatore verso l'altoparlante
	 del PC;
;
;**********************************************************************/

void
util_stop_beep(
	void
)
{
	util_portio_outp(TIMER_PB, util_portio_inp(TIMER_PB) & 0xfd);
}


/***********************************************************************

;  Nome:    
     util_wait_clock
;
;  Funzione:
     attende fino ad un periodo dell'onda quadra di uscita del contatore
;
;  Formato:
     void util_wait_clock()
;
;  Descrizione:
     {\bf util_wait_clock}() attende fino ad un periodo dell'onda quadra
	 di uscita del contatore; l'attesa viene realizzata tramite
	 un busy-wait durante il quale si legge il bit di uscita che
	 porta l'onda quadra; dapprima si attende che tale bit vada
	 basso, quindi si attende che torni alto; l'attesa minima
	 e' pertanto di mezzo periodo;
;
;**********************************************************************/

void
util_wait_clock(
	void
)
{

/* si attende che il bit 5 (OUT) vada basso */

	while(util_portio_inp(TIMER_PB) & 0x20)
		;

/* si attende che il bit 5 (OUT) torni alto */

	while(!(util_portio_inp(TIMER_PB) & 0x20))
		;

	return;
}


/***********************************************************************

;  Nome:    
     util_check_shot
;
;  Funzione:
     ritorna 1 se lo shot e' in corso, 0 se e' terminato
;
;  Formato:
     short util_check_shot()
;
;  Descrizione:
     {\bf check_shot}() ritorna 1 se lo shot e' in corso, 0 se e' terminato;
;
;**********************************************************************/

short
util_check_shot(
	void
)
{
	if(util_portio_inp(TIMER_PB) & 0x20)
		return 0;
	else
		return 1;
}


/***********************************************************************

;  Nome:    
     util_wait_0
;
;  Funzione:
     attende fino a mezzo periodo dell'onda quadra di uscita del contatore
;
;  Formato:
     void util_wait_0()
;
;  Descrizione:
     {\bf util_wait_0}() attende fino a mezzo periodo dell'onda quadra
	 di uscita del contatore; la routine attende che il bit di uscita vada
	 basso, pertanto se tale bit e' gia' basso ritorna immediatamente;
;
;**********************************************************************/

void
util_wait_0(
	void
)
{
	while(util_portio_inp(TIMER_PB) & 0x20)
		;
	return;
}


/***********************************************************************

;  Nome:    
     util_wait_1
;
;  Funzione:
     attende fino a mezzo periodo dell'onda quadra di uscita del contatore
;
;  Formato:
     void util_wait_1()
;
;  Descrizione:
     {\bf util_wait_1}() attende fino a mezzo periodo dell'onda quadra
	 di uscita del contatore; la routine attende che il bit di uscita vada
	 alto, pertanto se tale bit e' gia' alto ritorna immediatamente;
;
;**********************************************************************/

void
util_wait_1(
	void
)
{
	while(!(util_portio_inp(TIMER_PB) & 0x20))
		;
	return;
}

/**********************************************************************/

/* latcha e legge il counter #2 */

unsigned short
util_read_counter(
	void
)
{
	unsigned short byte_basso;

	util_portio_outp(TIMER_CTRL, 0xd8); /* latch tramite readback command */
	byte_basso = (unsigned short) util_portio_inp(TIMER_CONT);

	return (unsigned short) ((util_portio_inp(TIMER_CONT) << 8) | byte_basso);
}

/**********************************************************************/

/* latcha e legge il counter #0, quello di sistema;
   poiche' questo contatore e' letto anche da win.com, se tale lettura avviene
   dopo che il contatore e' stato latchato la routine ritorna un valore
   non corretto; */

unsigned short
util_read_system_counter(
	void
)
{

#if 1
	unsigned short byte_basso;
	util_portio_outp(TIMER_CTRL, 0xd2); /* latch tramite readback command */
	byte_basso = (unsigned short) util_portio_inp(TIMER_CONT_SYS);
	return (unsigned short) ((util_portio_inp(TIMER_CONT_SYS) << 8) | byte_basso);
#else
	__asm {
		MOV AX,0xd2
		OUT	0x43,AX
		IN	AL,0x40
		MOV	BL,AL
		IN	AL,0x40
		SHL AX,0x08
		MOV	AL,BL
	}
#endif
}

/**********************************************************************/

/* imposta il contatore #2 in modo standard (3) con periodo massimo e 
   fa partire il conteggio; inoltre azzera la variabile interna che fornisce
   il valore del nostro contatore interno; tale contatore e' a 32 bit e pertanto
   si riazzera dopo circa un'ora; */

void
util_reset_internal_counter(
	void
)
{
	util_set_8254();
	util_set_divider(65535L);
	util_start_clock();
	internal_counter_high = 0L;
	internal_counter_low = 65535L - (unsigned long) util_read_counter();
}

/**********************************************************************/

/* ritorna il valore del contatore interno dopo aver chiamto read_counter()
   per aggiornarne il valore; se tale routine non viene chiamata almeno ogni
   54 ms viene perso un incremento della parte alta del contatore; */

unsigned long
util_query_internal_counter(
	void
)
{
	unsigned long actual_value = 65535L - (unsigned long) util_read_counter();
	if(actual_value < internal_counter_low)
		internal_counter_high++;
	internal_counter_low = actual_value;
	return (internal_counter_high << 16) | internal_counter_low;
}

