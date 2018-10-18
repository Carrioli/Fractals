
// routine per la decompressione
// versione attuale: 7.61 14 gennaio 2002

#include <math.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <memory.h>
#include <io.h>
#include <malloc.h>
#include <imutilnt.h>

/*****************************************************************************/

/* prototipi delle funzioni private */

static unsigned char leggi_fino_a_8_bit(unsigned char n_bit);
static short leggi_9_bit(void);
static short leggi_10_bit(void);
static void decodifica_blocco(void);
static void decodifica_blocco_col(void);
static void decodifica_blocco_lossless(void);
static void decodifica_blocco_lossless_col(void);
static short leggi_lunghezza_run(void);
static short leggi_codice_ternario(void);
static short leggi_codice_binario(void);
static short leggi_codice_9(void);
static void crea_codice_inverso_valori(void);
static void crea_codice_ternario_inverso(
	short nb1, short nb2, short offset, short inf, short sup
);
static void crea_codice_binario_inverso(
	short nb1, short offset, short inf, short sup
);
static void inizializza_valori_per_codici(short numero_codice_da_usare);
static void leggi_colore_e_calcola_rgb(
	short valore, short egemone, short *aus_imr, short *aus_img,
	short *aus_imb, short indice
);
static void init_puntatori(void);
static short alloca_memoria(void);
static void libera_memoria(void);
static void scrivi_finestra_col(
	ROI *roi_blocco, short flag_origine_x, short flag_origine_y
);
static void scrivi_finestra_bw(
	ROI *roi_blocco, short flag_origine_x, short flag_origine_y
);
static void trasforma_blocco_inverso(void);
static void trasforma_blocco_inverso_col(void);

/* prototipi delle funzioni di haar_s.c */

void imut_haarinv_in_place(
	unsigned char **imaus, short *im_in, short **im_indici, short **im_scr,
	short lb, short colore
);
void imut_haarinv_in_place_col(
	unsigned char **imaus, short *imr, short *img, short *imb, short **im_indici,
	short **im_scr, short lb
);
void imut_crea_im_indici_haar_in_place(short **im_indici, short lb);

/*****************************************************************************/

/* variabili e array di uso globale */

static short _n_byte;
/* indice di _vet_valori: il primo byte da cui si puo' leggere */

static char _n_disponibili;
/* il numero di bit ancora disponibili per la lettura del byte
   _vet_valori[_n_byte] */

static unsigned char *_vet_valori;
/* vettore per la lettura dei valori dell'immagine compressa */

static short *_vet_codice_inverso_valori;
/* vettore che contiene il codice inverso per i valori */

static short *_im;                          /* solo BW */
static short *_imr, *_img, *_imb;             /* solo COL */
static unsigned char **_imaus;
static short **_im_scr, **_im_indici;
/* array per il calcolo della trasformata e la lettura da disco */

static short _lb;
/* lunghezza del lato del blocco */

static short _lb2;
/* _lb * _lb */

static short *_p_codice0, *_p_codice1, *_p_codice2;
/* puntatori al vettore del codice inverso: sono usati delle leggi_codice */

static unsigned char _n_bit1, _n_bit2;
/* numero di bit da leggere: sono usati dalle leggi_codice */

static unsigned char _maschera[] = {0, 1, 3, 7, 15, 31, 63, 127, 255};
/* _maschera per leggi_fino_a_8_bit() */

static short _tab_lunghini[] = {0, -3, -2, -1};
static short _tab_lunghi[] = {0, -7, -6, -5, -4, -3, -2, -1};
/* servono alla routine leggi_colore_e_calcola_rgb() */

static short _cflag;
/* vale 1 se l'immagine e' a colori */

// spostati qui con 7.61
char *_in_buf;
long *_dim_letture;
short *_dim_blo_com;
static CMPIMAGE *_header;
static short _byte_per_pix;

/*****************************************************************************/

/* 7.61: inizializza tutto quello che serve per la decompressione */
short init_DIAC_uncomp(CMPIMAGE *header)
{
	short ret;

	_header = header; // gia' letto da file

	if(_header->pixdepth == DIAC_PIX24) {
		_cflag = 1;
		_byte_per_pix = 3;
	} else if(_header->pixdepth == DIAC_PIX8) {
		_cflag = 0;
		_byte_per_pix = 1;
	} else
		return SEAV_ERR_NOT_FOUND;
	_lb = _header->block_len;
	_lb2 = _lb * _lb;

	/* allocazione di memoria */
	if((ret = alloca_memoria()) < 0)
		return ret;

	/* calcolo del vettore con gli indici di _im */
	imut_crea_im_indici_haar_in_place(_im_indici, _lb);

	/* calcolo del vettore dei codici inversi */
	crea_codice_inverso_valori();

	return SEAV_OK_ALL;
}

/* 7.61: clean-up della memoria */
void clean_DIAC_uncomp(void)
{
	libera_memoria();
}

/*****************************************************************************/

/* decodifica un blocco e lo scrive nella locazione di memoria assegnata */

void 
expandDIACblock(
	short fb,
	unsigned char *ptcdata,/* punta ai dati compressi */
	short ii,              /* indici del blocco */
	short jj,
	short offset_x,        /* offset di posizionamento dell'immagine decompressa
	                          rispetto a xi,yi memorizzati nell'header */
	short offset_y
)
{
	ROI roi_blocco;
	short flag_origine_x, flag_origine_y;

	/* inizializzazione di _n_byte, _n_disponibili e _vet_valori */
	_n_byte = 0;
	_n_disponibili = 8;
	_vet_valori = ptcdata;

	/* decodifica con riempimento dei buchi */
	if(_cflag)
		decodifica_blocco_col();
	else
		decodifica_blocco();

	/* calcolo della trasformata inversa e trasferimento
	   a imaus con clipping dei valori tra 0 e 255 */
	if(_cflag)
		imut_haarinv_in_place_col(_imaus, _imr, _img, _imb, _im_indici, _im_scr, _lb);
	else
		imut_haarinv_in_place(_imaus, _im, _im_indici, _im_scr, _lb, -1);

	/* scrittura da _imaus a frame buffer */
	roi_blocco.fb = fb;
	if(offset_x < 0) {
		flag_origine_x = 1;
		roi_blocco.xi = jj * _header->block_len;
		roi_blocco.xf = roi_blocco.xi + _header->block_len - 1;
	} else {
		flag_origine_x = 0;
		roi_blocco.xi = _header->xleft + jj * _header->block_len + offset_x;
		roi_blocco.xf = roi_blocco.xi + _header->block_len - 1 + offset_x;
	}
	if(offset_y < 0) {
		flag_origine_y = 1;
		roi_blocco.yi = ii * _header->block_len;
		roi_blocco.yf = roi_blocco.yi + _header->block_len - 1;
	} else {
		flag_origine_y = 0;
		roi_blocco.yi = _header->ydown + ii * _header->block_len + offset_y;
		roi_blocco.yf = roi_blocco.yi + _header->block_len - 1 + offset_y;
	}
	if(_cflag)
		scrivi_finestra_col(&roi_blocco, flag_origine_x, flag_origine_y);
	else
		scrivi_finestra_bw(&roi_blocco, flag_origine_x, flag_origine_y);
}

/*****************************************************************************/

/* decodifica un blocco e lo scrive nella locazione di memoria assegnata */

void 
expand_lossless_block(
	short fb,
	unsigned char *ptcdata,/* punta ai dati compressi */
	short ii,              /* indici del blocco */
	short jj,
	short offset_x,        /* offset di posizionamento dell'immagine decompressa
	                          rispetto a xi,yi memorizzati nell'header */
	short offset_y
)
{
	ROI roi_blocco;
	short flag_origine_x, flag_origine_y;

	/* inizializzazione di _n_byte, _n_disponibili e _vet_valori */
	_n_byte = 0;
	_n_disponibili = 8;
	_vet_valori = ptcdata;

	/* decodifica */
	if(_cflag)
		decodifica_blocco_lossless_col();
	else
		decodifica_blocco_lossless();

	/* calcolo della trasformata inversa e trasferimento a _imaus */
	if(_cflag)
		trasforma_blocco_inverso_col();
	else
		trasforma_blocco_inverso();

	/* scrittura da _imaus a frame buffer */
	roi_blocco.fb = fb;
	if(offset_x < 0) {
		flag_origine_x = 1;
		roi_blocco.xi = jj * _header->block_len;
		roi_blocco.xf = roi_blocco.xi + _header->block_len - 1;
	} else {
		flag_origine_x = 0;
		roi_blocco.xi = _header->xleft + jj * _header->block_len + offset_x;
		roi_blocco.xf = roi_blocco.xi + _header->block_len - 1 + offset_x;
	}
	if(offset_y < 0) {
		flag_origine_y = 1;
		roi_blocco.yi = ii * _header->block_len;
		roi_blocco.yf = roi_blocco.yi + _header->block_len - 1;
	} else {
		flag_origine_y = 0;
		roi_blocco.yi = _header->ydown + ii * _header->block_len + offset_y;
		roi_blocco.yf = roi_blocco.yi + _header->block_len - 1 + offset_y;
	}
	if(_cflag)
		scrivi_finestra_col(&roi_blocco, flag_origine_x, flag_origine_y);
	else
		scrivi_finestra_bw(&roi_blocco, flag_origine_x, flag_origine_y);
}

/*******************************************************************/

/* routine per leggere al piu' 8 bit dal vettore _vet_valori e metterli
   nella parte meno significativa del valore di ritorno (allineati a destra);
   gli altri bit del valore restituito sono posti a 0; */

static unsigned char leggi_fino_a_8_bit(unsigned char n_bit)
{
	if((_n_disponibili -= n_bit) > 0) {
		return (unsigned char) ((*(_vet_valori + _n_byte) >> _n_disponibili) &
		       *(_maschera + n_bit));
	} else {
		if(!_n_disponibili) {
			_n_disponibili = 8;
			return(*(_vet_valori + _n_byte++) & *(_maschera + n_bit));
		} else {
			unsigned char auss = (unsigned char) (*(_vet_valori +_n_byte)
			                              << (-_n_disponibili));
			auss |= (*(_vet_valori + ++_n_byte) >> (_n_disponibili += 8));
			return(auss & *(_maschera + n_bit));
		}
	}
}

/*******************************************************************/

/* routine per leggere esattamente 9 bit dal vettore _vet_valori (gli altri bit
   del valore restituito sono posti a 0) */

static short leggi_9_bit(void)
{
	short aus;

	aus = ((short) leggi_fino_a_8_bit((char) 4)) << 5;
	return aus | (short) leggi_fino_a_8_bit((char) 5);
}

/*******************************************************************/

/* routine per leggere esattamente 10 bit dal vettore _vet_valori (gli altri bit
   del valore restituito sono posti a 0) */

static short leggi_10_bit(void)
{
	short aus;

	aus = ((short) leggi_fino_a_8_bit((char) 5)) << 5;
	return aus | (short) leggi_fino_a_8_bit((char) 5);
}

/*******************************************************************/

/* routine per la decodifica dei valori del blocco dal vettore _vet_valori
   e il riempimento dei buchi */

static void decodifica_blocco(void)
{
	short numero_codice_da_usare;
	short i=0, j;
	short *aus_im = _im;
	short (*leggi_valore)(void);

/* leggo il numero del codice da usare */

	numero_codice_da_usare = (short) leggi_fino_a_8_bit((unsigned char) 8);

/* assegno la funzione giusta a leggi_codice, inizializzo _p_codice0, _p_codice1
   e _p_codice2 con lo spiazzamento relativo al codice da usare e inizializzo
   i valori di _n_bit1 (e _n_bit2 per i codici ternari soltanto) */

	if(numero_codice_da_usare < 3 || numero_codice_da_usare > 9)
		leggi_valore = leggi_codice_ternario;
	else if(numero_codice_da_usare != 9)
		leggi_valore = leggi_codice_binario;
	else
		leggi_valore = leggi_codice_9;

	inizializza_valori_per_codici(numero_codice_da_usare);

/* fino a che l'immagine non e' stata riempita leggo i run */

	while(i < _lb2) {

/* leggo lunghezza_run valori e li metto in _im */

		for(i = i + (j=leggi_lunghezza_run()); j>7; j-=8) {
			*aus_im++ = (*leggi_valore)();
			*aus_im++ = (*leggi_valore)();
			*aus_im++ = (*leggi_valore)();
			*aus_im++ = (*leggi_valore)();
			*aus_im++ = (*leggi_valore)();
			*aus_im++ = (*leggi_valore)();
			*aus_im++ = (*leggi_valore)();
			*aus_im++ = (*leggi_valore)();
		}
		for(; j>0; j--) {
			*aus_im++ = (*leggi_valore)();
		}

/* se non ho esaurito l'immagine */

		if(i < _lb2) {

/* leggo la lunghezza del run vuoto e riempio lunghezza_run buchi */

			j = leggi_lunghezza_run();
			for(; j>7; j-=8) {
				*aus_im++ = *(_im + (i++>>2)) >> 1;
				*aus_im++ = *(_im + (i++>>2)) >> 1;
				*aus_im++ = *(_im + (i++>>2)) >> 1;
				*aus_im++ = *(_im + (i++>>2)) >> 1;
				*aus_im++ = *(_im + (i++>>2)) >> 1;
				*aus_im++ = *(_im + (i++>>2)) >> 1;
				*aus_im++ = *(_im + (i++>>2)) >> 1;
				*aus_im++ = *(_im + (i++>>2)) >> 1;
			}
			for(; j>0; j--) {
				*aus_im++ = *(_im + (i++>>2)) >> 1;
			}
		}
	}
}

/*******************************************************************/

/* routine per la decodifica dei valori del blocco dal vettore _vet_valori */

static void decodifica_blocco_lossless(void)
{
	short numero_codice_da_usare;
	short i=0, j;
	short *aus_im = _im;
	short (*leggi_valore)(void);

/* leggo il numero del codice da usare */

	numero_codice_da_usare = (short) leggi_fino_a_8_bit((unsigned char) 8);

/* assegno la funzione giusta a leggi_codice, inizializzo _p_codice0, _p_codice1
   e _p_codice2 con lo spiazzamento relativo al codice da usare e inizializzo
   i valori di _n_bit1 (e _n_bit2 per i codici ternari soltanto) */

	if(numero_codice_da_usare < 3 || numero_codice_da_usare > 9)
		leggi_valore = leggi_codice_ternario;
	else if(numero_codice_da_usare != 9)
		leggi_valore = leggi_codice_binario;
	else
		leggi_valore = leggi_codice_9;

	inizializza_valori_per_codici(numero_codice_da_usare);

/* leggo _lb2 valori e li metto in _im */

	for(j=_lb2; j>0; j--) {
		*aus_im++ = (*leggi_valore)();
	}
}

/*******************************************************************/

static void decodifica_blocco_col(void)
{
	short numero_codice_da_usare;
	short i=0, j, valore, egemone;
	short *aus_imr = _imr;
	short *aus_img = _img;
	short *aus_imb = _imb;
	short (*leggi_valore)(void);

/* leggo il numero del codice da usare */

	numero_codice_da_usare = (short) leggi_fino_a_8_bit((unsigned char) 8);

/* assegno la funzione giusta a leggi_codice e
   inizializzo _p_codice0, _p_codice1 e _p_codice2 con lo spiazzamento
   relativo al codice da usare e inizializzo i valori di _n_bit1
   (e _n_bit2 per i codici ternari soltanto) */

	if(numero_codice_da_usare < 3 || numero_codice_da_usare > 9)
		leggi_valore = leggi_codice_ternario;
	else if(numero_codice_da_usare != 9)
		leggi_valore = leggi_codice_binario;
	else
		leggi_valore = leggi_codice_9;

	inizializza_valori_per_codici(numero_codice_da_usare);

/* fino a che l'immagine non e' stata riempita leggo i run */

	while(i < _lb2) {

/* leggo lunghezza_run valori a colori e li metto in _imr _img _imb antinorm. */

		for(j = leggi_lunghezza_run(); j>0; j--, i++) {

/* leggo il valore egemone */

			valore = (*leggi_valore)();

/* leggo il codice del valore egemone ed eventualmente il colore */

			if((egemone = (short) leggi_fino_a_8_bit((unsigned char) 2)) != 3) {
				leggi_colore_e_calcola_rgb(
					valore, egemone, aus_imr++, aus_img++, aus_imb++, i
				);
			} else {
				*aus_imr = valore;
				*aus_img++ = *aus_imr;
				*aus_imb++ = *aus_imr++;
			}
		}

/* se non ho esaurito l'immagine */

		if(i < _lb2) {

/* leggo la lunghezza del run vuoto e riempio lunghezza_run buchi */

			j = leggi_lunghezza_run();
			for(; j>0; j--) {
				*aus_imr++ = *(_imr + (i>>2)) >> 1;
				*aus_img++ = *(_img + (i>>2)) >> 1;
				*aus_imb++ = *(_imb + (i++>>2)) >> 1;
			}
		}
	}
}

/*******************************************************************/

static void decodifica_blocco_lossless_col(void)
{
	short numero_codice_da_usare;
	short i=0, j;
	short *aus_im = _im;
	short (*leggi_valore)(void);

/* leggo il numero del codice da usare */

	numero_codice_da_usare = (short) leggi_fino_a_8_bit((unsigned char) 8);

/* se il codice da usare vale 100 devo gestire gli overflow: uso un codice
   speciale */

	if(numero_codice_da_usare == 100) {
		short valore;
		for(j=_lb2*3; j>0; j--) {
			valore = (short) leggi_fino_a_8_bit(4);
			if(!valore)
				valore = (short) leggi_10_bit() - 512;
			else
				valore -= 8;
			*aus_im++ = valore;
		}
	} else {

/* assegno la funzione giusta a leggi_codice e
   inizializzo _p_codice0, _p_codice1 e _p_codice2 con lo spiazzamento
   relativo al codice da usare e inizializzo i valori di _n_bit1
   (e _n_bit2 per i codici ternari soltanto) */

		if(numero_codice_da_usare < 3 || numero_codice_da_usare > 9)
			leggi_valore = leggi_codice_ternario;
		else if(numero_codice_da_usare != 9)
			leggi_valore = leggi_codice_binario;
		else
			leggi_valore = leggi_codice_9;

		inizializza_valori_per_codici(numero_codice_da_usare);

/* leggo _lb2*3 valori e li metto in _im */

		for(j=_lb2*3; j>0; j--) {
			*aus_im++ = (*leggi_valore)();
		}
	}

	return;
}

/*******************************************************************/

static short leggi_lunghezza_run(void)
{
	short lunghezza_run;

	if(!(lunghezza_run = (short) leggi_fino_a_8_bit((unsigned char) 1))) {
		if(!(lunghezza_run = (short) leggi_fino_a_8_bit((unsigned char) 2))) {
			if(!(lunghezza_run = (short)leggi_fino_a_8_bit((unsigned char)4))) {
				if(!(lunghezza_run = (short) leggi_fino_a_8_bit
				                             ((unsigned char)8))) {
					lunghezza_run =
					       (short) leggi_fino_a_8_bit((unsigned char) 4) << 8;
					lunghezza_run |=
					       (short) leggi_fino_a_8_bit((unsigned char) 8);
					return(lunghezza_run + 274);
				} else {
					return(lunghezza_run + 19);
				}
			} else {
				return(lunghezza_run + 4);
			}
		} else {
			return(lunghezza_run + 1);
		}
	} else {
		return(lunghezza_run);
	}
}

/*******************************************************************/

/* routine per leggere un valore scritto con un codice ternario */

static short leggi_codice_ternario(void)
{
	short letto;

	if(!(letto = (short) leggi_fino_a_8_bit(_n_bit1))) {
		if(!(letto = (short) leggi_fino_a_8_bit(_n_bit2))) {
			letto = leggi_9_bit();
			return(*(_p_codice2 + letto));
		} else {
			return(*(_p_codice1 + letto));
		}
	} else {
		return(*(_p_codice0 + letto));
	}
}

/*******************************************************************/

/* routine per leggere un valore scritto con un codice binario */

static short leggi_codice_binario(void)
{
	short letto;

	if(!(letto = (short) leggi_fino_a_8_bit(_n_bit1))) {
		letto = leggi_9_bit();
		return(*(_p_codice1 + letto));
	} else {
		return(*(_p_codice0 + letto));
	}
}

/*******************************************************************/

/* routine per leggere un valore scritto col codice 9 */

static short leggi_codice_9(void)
{
	short letto;

	letto = leggi_9_bit();
	return(*(_p_codice0 + letto + 1));
}

/*******************************************************************/

/* routine per creare il codice inverso per i valori
   nel vettore _vet_codice_inverso_valori

   codice n. 0     2[+4[+9]]:
              2      -> -1...1
              2+4    -> -8...-2, 2...9
			  2+4+9  -> altrove
   codice n. 1     2[+5[+9]]:
              2      -> -1...1
              2+5    -> -16...-2, 2...17
			  2+5+9  -> altrove
   codice n. 2     3[+4[+9]]:
              3      -> -3...3
              3+4    -> -10...-4, 4...11
			  3+4+9  -> altrove
   codice n. 3     2[+9]:
              2      -> -1...1
			  2+9    -> altrove
   codice n. 4     3[+9]:
              3      -> -3...3
			  3+9    -> altrove
   codice n. 5     4[+9]:
              4      -> -7...7
			  4+9    -> altrove
   codice n. 6     5[+9]:
              5      -> -15...15
			  5+9    -> altrove
   codice n. 7     6[+9]:
              6      -> -31...31
			  6+9    -> altrove
   codice n. 8     7[+9]:
              7      -> -63...63
			  7+9    -> altrove
   codice n. 9     9:
              9      -> dappertutto
   codice n. 10     3[+3[+9]]:
              3      -> -3...3
              3+3    -> -6...-4, 4...7
			  3+3+9  -> altrove
   codice n. 11     4[+4[+9]]:
              4      -> -7...7
              4+4    -> -14...-8, 8...15
			  4+4+9  -> altrove
   codice n. 12     4[+5[+9]]:
              4      -> -7...7
              4+5    -> -22...-8, 8...23
			  4+5+9  -> altrove
   codice n. 13     5[+5[+9]]:
              5      -> -15...15
              5+5    -> -30...-16, 16...31
			  5+5+9  -> altrove
   codice n. 14     5[+6[+9]]:
              5      -> -15...15
              5+6    -> -46...-16, 16...47
			  5+6+9  -> altrove
   codice n. 15     6[+6[+9]]:
              6      -> -31...31
              6+6    -> -62...-32, 32...63
			  6+6+9  -> altrove
   codice n. 16     3[+5[+9]]:
              3      -> -3...3
              3+5    -> -18...-4, 4...19
			  3+5+9  -> altrove
   codice n. 17     3[+6[+9]]:
              3      -> -3...3
              3+6    -> -34...-4, 4...35
			  3+6+9  -> altrove
   codice n. 18     3[+7[+9]]:
              3      -> -3...3
              3+7    -> -66...-4, 4...67
			  3+7+9  -> altrove
   codice n. 19     4[+6[+9]]:
              4      -> -7...7
              4+6    -> -38...-8, 8...39
			  4+6+9  -> altrove
   codice n. 20     4[+7[+9]]:
              4      -> -7...7
              4+7    -> -70...-8, 8...71
			  4+7+9  -> altrove
   codice n. 21     5[+7[+9]]:
              5      -> -15...15
              5+7    -> -78...-16, 16...79
			  5+7+9  -> altrove
*/

static void crea_codice_inverso_valori(void)
{
	short i, j, offset = 0;
	short inf = - RANGE_VALORI / 2;
	short sup = RANGE_VALORI / 2 - 1;

/* codice 2[+4[+9]] (0) */

	crea_codice_ternario_inverso(2, 4, offset, inf, sup);

/* codice 2[+5[+9]] (1) */

	offset += RANGE_VALORI;
	crea_codice_ternario_inverso(2, 5, offset, inf, sup);

/* codice 3[+4[+9]] (2) */

	offset += RANGE_VALORI;
	crea_codice_ternario_inverso(3, 4, offset, inf, sup);

/* codice 2[+9] (3) */

	offset += RANGE_VALORI;
	crea_codice_binario_inverso(2, offset, inf, sup);

/* codice 3[+9] (4) */

	offset += RANGE_VALORI;
	crea_codice_binario_inverso(3, offset, inf, sup);

/* codice 4[+9] (5) */

	offset += RANGE_VALORI;
	crea_codice_binario_inverso(4, offset, inf, sup);

/* codice 5[+9] (6) */

	offset += RANGE_VALORI;
	crea_codice_binario_inverso(5, offset, inf, sup);

/* codice 6[+9] (7) */

	offset += RANGE_VALORI;
	crea_codice_binario_inverso(6, offset, inf, sup);

/* codice 7[+9] (8) */

	offset += RANGE_VALORI;
	crea_codice_binario_inverso(7, offset, inf, sup);

/* codice 9 (9) */

	offset += RANGE_VALORI;
	for(i=inf, j=0; i<=sup; i++, j++) {
		_vet_codice_inverso_valori[offset + j] = i;
	}

/* codice 3[+3[+9]] (10) */

	offset += RANGE_VALORI;
	crea_codice_ternario_inverso(3, 3, offset, inf, sup);

/* codice 4[+4[+9]] (11) */

	offset += RANGE_VALORI;
	crea_codice_ternario_inverso(4, 4, offset, inf, sup);

/* codice 4[+5[+9]] (12) */

	offset += RANGE_VALORI;
	crea_codice_ternario_inverso(4, 5, offset, inf, sup);

/* codice 5[+5[+9]] (13) */

	offset += RANGE_VALORI;
	crea_codice_ternario_inverso(5, 5, offset, inf, sup);

/* codice 5[+6[+9]] (14) */

	offset += RANGE_VALORI;
	crea_codice_ternario_inverso(5, 6, offset, inf, sup);

/* codice 6[+6[+9]] (15) */

	offset += RANGE_VALORI;
	crea_codice_ternario_inverso(6, 6, offset, inf, sup);

/* codice 3[+5[+9]] (16) */

	offset += RANGE_VALORI;
	crea_codice_ternario_inverso(3, 5, offset, inf, sup);

/* codice 3[+6[+9]] (17) */

	offset += RANGE_VALORI;
	crea_codice_ternario_inverso(3, 6, offset, inf, sup);

/* codice 3[+7[+9]] (18) */

	offset += RANGE_VALORI;
	crea_codice_ternario_inverso(3, 7, offset, inf, sup);

/* codice 4[+6[+9]] (19) */

	offset += RANGE_VALORI;
	crea_codice_ternario_inverso(4, 6, offset, inf, sup);

/* codice 4[+7[+9]] (20) */

	offset += RANGE_VALORI;
	crea_codice_ternario_inverso(4, 7, offset, inf, sup);

/* codice 5[+7[+9]] (21) */

	offset += RANGE_VALORI;
	crea_codice_ternario_inverso(5, 7, offset, inf, sup);
}

/*****************************************************************************/

/* routine per generare un codice inverso del tipo x[+y[+z]] */

static void
crea_codice_ternario_inverso(
	short nb1,      /* x */
	short nb2,      /* y: z non serve perche' i limiti sono dati da inf e sup */
	short offset,   /* dell'indice del vettore dei codici */
	short inf,      /* valori estremi */
	short sup
)
{
	short sup1 = el(nb1 - 1) - 1;
	short sup2 = sup1 + el(nb2 - 1) - 1;
	short i, j;

	for(i=-sup1, j=0; i<=sup1; i++, j++) {
		_vet_codice_inverso_valori[offset + j] = i;
	}

	for(i=-sup2; i<-sup1; i++, j++) {
		_vet_codice_inverso_valori[offset + j] = i;
	}
	for(i=sup1+1; i<=(sup2+1); i++, j++) {
		_vet_codice_inverso_valori[offset + j] = i;
	}

	for(i=inf; i<-sup2; i++, j++) {
		_vet_codice_inverso_valori[offset + j] = i;
	}
	for(i=sup2+2; i<=sup; i++, j++) {
		_vet_codice_inverso_valori[offset + j] = i;
	}
	return;
}

/*******************************************************************/

/* routine per generare un codice inverso del tipo x[+y] */

static void
crea_codice_binario_inverso(
	short nb1,      /* x: y non serve perche' i limiti sono dati da inf e sup */
	short offset,   /* dell'indice del vettore dei codici */
	short inf,      /* valori estremi */
	short sup
)
{
	short sup1 = el(nb1 - 1) - 1;
	short i, j;

	for(i=-sup1, j=0; i<=sup1; i++, j++) {
		_vet_codice_inverso_valori[offset + j] = i;
	}

	for(i=inf; i<-sup1; i++, j++) {
		_vet_codice_inverso_valori[offset + j] = i;
	}
	for(i=sup1+1; i<=sup; i++, j++) {
		_vet_codice_inverso_valori[offset + j] = i;
	}
	return;
}

/*******************************************************************/

/* funzione per inizializzare _p_codice0, _p_codice1 e _p_codice2 con lo
   spiazzamento relativo al codice da usare e inizializzo i valori di _n_bit1
   (e _n_bit2 per i codici ternari soltanto) */

static void
inizializza_valori_per_codici(
	short numero_codice_da_usare
)
{
	switch(numero_codice_da_usare) {
		case 0:   /* 2[+4[+9]] */
			_n_bit1 = (unsigned char) 2; _n_bit2 = (unsigned char) 4;
			break;
		case 1:   /* 2[+5[+9]] */
			_n_bit1 = (unsigned char) 2; _n_bit2 = (unsigned char) 5;
			break;
		case 2:   /* 3[+4[+9]] */
			_n_bit1 = (unsigned char) 3; _n_bit2 = (unsigned char) 4;
			break;
		case 3:   /* 2[+9] */
			_n_bit1 = (unsigned char) 2;
			break;
		case 4:   /* 3[+9] */
			_n_bit1 = (unsigned char) 3;
			break;
		case 5:   /* 4[+9] */
			_n_bit1 = (unsigned char) 4;
			break;
		case 6:   /* 5[+9] */
			_n_bit1 = (unsigned char) 5;
			break;
		case 7:   /* 6[+9] */
			_n_bit1 = (unsigned char) 6;
			break;
		case 8:   /* 7[+9] */
			_n_bit1 = (unsigned char) 7;
			break;
		case 9:   /* 9 */
			break;
		case 10:  /* 3[+3[+9]] */
			_n_bit1 = (unsigned char) 3; _n_bit2 = (unsigned char) 3;
			break;
		case 11:  /* 4[+4[+9]] */
			_n_bit1 = (unsigned char) 4; _n_bit2 = (unsigned char) 4;
			break;
		case 12:  /* 4[+5[+9]] */
			_n_bit1 = (unsigned char) 4; _n_bit2 = (unsigned char) 5;
			break;
		case 13:  /* 5[+5[+9]] */
			_n_bit1 = (unsigned char) 5; _n_bit2 = (unsigned char) 5;
			break;
		case 14:  /* 5[+6[+9]] */
			_n_bit1 = (unsigned char) 5; _n_bit2 = (unsigned char) 6;
			break;
		case 15:  /* 6[+6[+9]] */
			_n_bit1 = (unsigned char) 6; _n_bit2 = (unsigned char) 6;
			break;
		case 16:  /* 3[+5[+9]] */
			_n_bit1 = (unsigned char) 3; _n_bit2 = (unsigned char) 5;
			break;
		case 17:  /* 3[+6[+9]] */
			_n_bit1 = (unsigned char) 3; _n_bit2 = (unsigned char) 6;
			break;
		case 18:  /* 3[+7[+9]] */
			_n_bit1 = (unsigned char) 3; _n_bit2 = (unsigned char) 7;
			break;
		case 19:  /* 4[+6[+9]] */
			_n_bit1 = (unsigned char) 4; _n_bit2 = (unsigned char) 6;
			break;
		case 20:  /* 4[+7[+9]] */
			_n_bit1 = (unsigned char) 4; _n_bit2 = (unsigned char) 7;
			break;
		case 21:  /* 5[+7[+9]] */
			_n_bit1 = (unsigned char) 5; _n_bit2 = (unsigned char) 7;
			break;
	}
	_p_codice0 = _vet_codice_inverso_valori +
	            (numero_codice_da_usare * RANGE_VALORI) - 1;
	if(numero_codice_da_usare != 9)
		_p_codice1 = _p_codice0 + el(_n_bit1) - 1;
	if(numero_codice_da_usare<3 || numero_codice_da_usare>9) /* solo ternari */
		_p_codice2 = _p_codice1 + el(_n_bit2) - 1;
	return;
}

/*******************************************************************/

/* legge le componenti di colore e calcola i valori di r,g,b antinormalizzati */

static void
leggi_colore_e_calcola_rgb(
	short valore,
	short egemone,
	short *aus_imr,
	short *aus_img,
	short *aus_imb,
	short indice
)
{
	short valore_a, colore1, colore2;
	
	valore_a = abs(valore);
	if(valore_a > SA || indice < N_VALORI_LUNGHISSIMI) {
		colore1 = (short) ((char) leggi_fino_a_8_bit((unsigned char) 8));
		colore1 = (short) (((long) colore1 * valore) / N_PA_LUNGHISSIMO_2);
		colore2 = (short) ((char) leggi_fino_a_8_bit((unsigned char) 8));
		colore2 = (short) (((long) colore2 * valore) / N_PA_LUNGHISSIMO_2);
	} else {
		if(valore_a > SB || indice < N_VALORI_LUNGHI) {
			if(!(colore1 = (short) leggi_fino_a_8_bit((unsigned char) 3)))
				colore1 = (valore *
				_tab_lunghi[(short) leggi_fino_a_8_bit((unsigned char) 3)])
				/ N_PA_LUNGO_2;
			else
				colore1 = (colore1 * valore) / N_PA_LUNGO_2;
			if(!(colore2 = (short) leggi_fino_a_8_bit((unsigned char) 3)))
				colore2 = (valore *
				_tab_lunghi[(short) leggi_fino_a_8_bit((unsigned char) 3)])
				/ N_PA_LUNGO_2;
			else
				colore2 = (colore2 * valore) / N_PA_LUNGO_2;
		} else {
			if(valore_a > SC || indice < N_VALORI_LUNGHINI) {
				if(!(colore1 = (short) leggi_fino_a_8_bit((unsigned char) 2)))
					colore1 = (valore *
					_tab_lunghini[(short) leggi_fino_a_8_bit((unsigned char) 2)])
					/ N_PA_LUNGHINO_2;
				else
					colore1 = (colore1 * valore) / N_PA_LUNGHINO_2;
				if(!(colore2 = (short) leggi_fino_a_8_bit((unsigned char) 2)))
					colore2 = (valore *
					_tab_lunghini[(short) leggi_fino_a_8_bit((unsigned char) 2)])
					/ N_PA_LUNGHINO_2;
				else
					colore2 = (colore2 * valore) / N_PA_LUNGHINO_2;
			} else {
				switch((short) leggi_fino_a_8_bit((unsigned char) 2)) {
					case 0:
						colore1 = colore2 = 0;
						break;
					case 1:
						colore1 = 0;
						colore2 = valore;
						break;
					case 2:
						colore1 = valore;
						colore2 = 0;
						break;
					case 3:
						switch((short) leggi_fino_a_8_bit((unsigned char) 2)) {
							case 0:
								colore1 = -valore;
								colore2 = -valore;
								break;
							case 1:
								colore1 = -valore;
								colore2 = 0;
								break;
							case 2:
								colore1 = 0;
								colore2 = -valore;
								break;
							case 3:
								switch(
									(short) leggi_fino_a_8_bit(
									        (unsigned char) 1)) {
									case 0:
										colore1 = -valore;
										colore2 = valore;
										break;
									case 1:
										colore1 = valore;
										colore2 = -valore;
										break;
								}
								break;
						}
						break;
				}
			}
		}
	}
	switch(egemone) {
		case 0:
			*aus_imr = valore;
			*aus_img = colore1;
			*aus_imb = colore2;
			break;
		case 1:
			*aus_imr = colore1;
			*aus_img = valore;
			*aus_imb = colore2;
			break;
		case 2:
			*aus_imr = colore1;
			*aus_img = colore2;
			*aus_imb = valore;
			break;
	}
}

/*******************************************************************/

static void init_puntatori(void)
{
	_im_indici = NULL;
	_imaus = NULL;
	_im_scr = NULL;
	_in_buf = NULL;
	_dim_letture = NULL;
	_dim_blo_com = NULL;
	_vet_codice_inverso_valori = NULL;
	_im = NULL;
	_imr = NULL;
	_img = NULL;
	_imb = NULL;
}

/*******************************************************************/

static short alloca_memoria(void)
{
	short n_blocchi = _header->hor_blocks * _header->ver_blocks;

	libera_memoria();
	init_puntatori();

	_im_indici = (short **) util_alloca_matrice(_lb, _lb, sizeof(short));
	_imaus = (unsigned char **) util_alloca_matrice(
		_lb, _lb * _byte_per_pix, sizeof(unsigned char)
	);
	_im_scr = (short **) util_alloca_matrice(
		_lb, _lb * _byte_per_pix, sizeof(short)
	);

	_in_buf = (char *) malloc((size_t) DIAC_BUF_SIZE * sizeof(char));
	_dim_letture = (long *) malloc((size_t) DIM_DIM_L * sizeof(long));
	_dim_blo_com = (short *) malloc(n_blocchi * sizeof(short));
	_vet_codice_inverso_valori = (short *) malloc(
		(size_t) (LUNG_CODICI * sizeof(short))
	);
	_im = (short *) malloc(_lb2 * 3 * sizeof(short));

	if(!_im_indici || !_imaus || !_im_scr || !_in_buf || !_dim_letture ||
	   !_dim_blo_com || !_vet_codice_inverso_valori || !_im) {
		libera_memoria();
		return SEAV_ERR_MEMORY;
	}

	if(_cflag) {
		_imr = (short *) malloc(_lb2 * _byte_per_pix * sizeof(short));
		_img = (short *) malloc(_lb2 * _byte_per_pix * sizeof(short));
		_imb = (short *) malloc(_lb2 * _byte_per_pix * sizeof(short));
		if(!_imr || !_img || !_img) {
			libera_memoria();
			return SEAV_ERR_MEMORY;
		}
	}

	return SEAV_OK_ALL;
}

/*******************************************************************/

static void libera_memoria(void)
{
	if(_im_indici) {
		util_disalloca_matrice((char **) _im_indici, _lb);
		_im_indici = NULL;
	}
	if(_imaus) {
		util_disalloca_matrice((char **) _imaus, _lb);
		_imaus = NULL;
	}
	if(_im_scr) {
		util_disalloca_matrice((char **) _im_scr, _lb);
		_im_scr = NULL;
	}
	if(_in_buf) {
		free(_in_buf);
		_in_buf = NULL;
	}
	if(_dim_letture) {
		free(_dim_letture);
		_dim_letture = NULL;
	}
	if(_dim_blo_com) {
		free(_dim_blo_com);
		_dim_blo_com = NULL;
	}
	if(_vet_codice_inverso_valori) {
		free(_vet_codice_inverso_valori);
		_vet_codice_inverso_valori = NULL;
	}
	if(_im) {
		free(_im);
		_im = NULL;
	}
	if(_cflag) {
		if(_imr) {
			free(_imr);
			_imr = NULL;
		}
		if(_img) {
			free(_img);
			_img = NULL;
		}
		if(_imb) {
			free(_imb);
			_imb = NULL;
		}
	}
}

/************************************************************************/

/* scrive una finestra da _imaus controllando se esce fuori dalla roi;
   il col si riferisce al fatto che l'immagine da decomprimere e' col */

static void
scrivi_finestra_col(
	ROI *roi_blocco, /* zona da scrivere */
	short flag_origine_x, /* se 1 l'immagine e' traslata in 0 */
	short flag_origine_y  /* se 1 l'immagine e' traslata in 0 */
)
{
	short xi=roi_blocco->xi, yi=roi_blocco->yi;
	short xf=roi_blocco->xf, yf=roi_blocco->yf;
	short x_min, y_min, x_max, y_max;

	if(flag_origine_x) {
		x_min = 0;
	} else {
		x_min = _header->xleft;
	}
	if(flag_origine_y) {
		y_min = 0;
	} else {
		y_min = _header->ydown;
	}
	x_max = x_min + _header->width - 1;
	y_max = y_min + _header->height -1;

	sea2_seleziona_buffer(roi_blocco->fb);
	if(xi < x_min || xf > x_max ||
	   yi < y_min || yf > y_max) {
		short i, indice, xl=(short)(xf-xi+1), yl=(short)(yf-yi+1);

		if(xi < x_min) {
			for(i=yi, indice=0; i<=yf; i++, indice++) {
				if(i >= y_min && i <= y_max) {
					sea2_scrivi_riga_col24(
						x_min, i, (short) (xf-x_min+1), _imaus[indice]+(x_min-xi)*3
					);
				}
			}
		} else if(xf > x_max) {
			for(i=yi, indice=0; i<=yf; i++, indice++) {
				if(i >= y_min && i <= y_max) {
					sea2_scrivi_riga_col24(xi, i, (short) (x_max-xi+1), _imaus[indice]);
				}
			}
		} else {
			for(i=yi, indice=0; i<=yf; i++, indice++) {
				if(i >= y_min && i <= y_max) {
					sea2_scrivi_riga_col24(xi, i, xl, _imaus[indice]);
				}
			}
		}
	} else {
		short i, xl=(short)(xf-xi+1), indice;
		for(i=yi, indice=0; i<=yf; i++, indice++)
			sea2_scrivi_riga_col24(xi, i, xl, _imaus[indice]);
	}
}

/************************************************************************/

/* scrive una finestra da _imaus controllando se esce fuori dalla roi;
   il bw si riferisce al fatto che l'immagine da decomprimere e' bw */

static void
scrivi_finestra_bw(
	ROI *roi_blocco, /* zona da scrivere */
	short flag_origine_x, /* se 1 l'immagine e' traslata in 0 */
	short flag_origine_y  /* se 1 l'immagine e' traslata in 0 */
)
{
	short xi=roi_blocco->xi, yi=roi_blocco->yi;
	short xf=roi_blocco->xf, yf=roi_blocco->yf;
	short x_min, y_min, x_max, y_max;

	if(flag_origine_x) {
		x_min = 0;
	} else {
		x_min = _header->xleft;
	}
	if(flag_origine_y) {
		y_min = 0;
	} else {
		y_min = _header->ydown;
	}
	x_max = x_min + _header->width - 1;
	y_max = y_min + _header->height -1;

	sea2_seleziona_buffer(roi_blocco->fb);
	if(xi < x_min || xf > x_max ||
	   yi < y_min || yf > y_max) {
		short i, indice, xl=(short)(xf-xi+1), yl=(short)(yf-yi+1);

		if(xi < x_min) {
			for(i=yi, indice=0; i<=yf; i++, indice++) {
				if(i >= y_min && i <= y_max) {
					sea2_scrivi_riga(
						x_min, i, (short) (xf-x_min+1), _imaus[indice]+x_min-xi
					);
				}
			}
		} else if(xf > x_max) {
			for(i=yi, indice=0; i<=yf; i++, indice++) {
				if(i >= y_min && i <= y_max) {
					sea2_scrivi_riga(xi, i, (short) (x_max-xi+1), _imaus[indice]);
				}
			}
		} else {
			for(i=yi, indice=0; i<=yf; i++, indice++) {
				if(i >= y_min && i <= y_max) {
					sea2_scrivi_riga(xi, i, xl, _imaus[indice]);
				}
			}
		}
	} else {
		short i, indice, xl=(short)(xf-xi+1);
		for(i=yi, indice=0; i<=yf; i++, indice++) {
			sea2_scrivi_riga(xi, i, xl, _imaus[indice]);
		}
	}
}

/************************************************************************/

/* da _im a _imaus */

static void trasforma_blocco_inverso(void)
{
	short i, j, k=0, prec = 0;

	for(i=0; i<_lb; i++) {
		for(j=0; j<_lb; j++) {
			_imaus[i][j] = (unsigned char) ((int) prec + _im[k++]);
			prec = (short) _imaus[i][j];
		}
	}
}

/************************************************************************/

/* da _imr, _img, _imb a _imaus */

static void trasforma_blocco_inverso_col(void)
{
	short i, j, k=0, prec = 0, lb4 = _lb2 << 1;

	for(i=0; i<_lb2; i++) {
		_im[i+_lb2] += _im[i];
		_im[i+lb4] += _im[i];
	}
	for(i=0; i<_lb; i++) {
		for(j=0; j<_lb*3; j+=3) {
			_imaus[i][j] = (unsigned char) ((int) prec + _im[k++]);
			prec = (short) _imaus[i][j];
		}
	}
	for(i=0; i<_lb; i++) {
		for(j=1; j<_lb*3; j+=3) {
			_imaus[i][j] = (unsigned char) ((int) prec + _im[k++]);
			prec = (short) _imaus[i][j];
		}
	}
	for(i=0; i<_lb; i++) {
		for(j=2; j<_lb*3; j+=3) {
			_imaus[i][j] = (unsigned char) ((int) prec + _im[k++]);
			prec = (short) _imaus[i][j];
		}
	}
}





