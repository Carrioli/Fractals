
// routine per la compressione
// versione attuale:    7.67  8 aprile 2002
// versione precedente: 7.61 14 gennaio 2002

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <memory.h>
#include <io.h>
#include <imutilnt.h>
#include <windows.h>

/* prototipi delle funzioni private */

static void calcola_soglie(void);
static void crea_codice_valori(void);
static void crea_codice_run(void);
static short metti_buchi(void);
static short metti_buchi_col(void);
static void codifica_blocco(short numero_run, short numero_codice_da_usare);
static void codifica_blocco_col(short numero_run, short numero_codice_da_usare);
static void scrivi_fino_a_16_bit_utili(
	unsigned char n_bit, unsigned short valore
);
static short calcola_codice_ottimo(void);
static void crea_codice_ternario(
	short nb1, short nb2, short nb3, short offset, short inf, short sup
);
static void crea_codice_binario(
	short nb1, short nb2, short offset, short inf, short sup
);
static short calcola_egemone(short valore_r, short valore_g, short valore_b);
static short calcola_egemone_e_gli_altri(
	short valore_r, short valore_g, short valore_b,
	short *valore, short *colore1, short *colore2, short indice
);
static void scrivi_colore(short colore1, short colore2);
static void sistema_dim_blo_com(short *dim_blo_com);
static void init_puntatori(void);
static short alloca_memoria(void);
static void libera_memoria(void);
static void leggi_finestra_col(ROI *roi_blocco);
static void leggi_finestra_bw(ROI *roi_blocco);
static void calcola_statistica(void);
static void calcola_statistica_col(void);
static short trasforma_blocco_col(void);
static void trasforma_blocco(void);
static void codifica_blocco_lossless(short numero_codice_da_usare);
static void codifica_blocco_lossless_col(short numero_codice_da_usare);

/* prototipi delle funzioni di haar_s.c */

void imut_haardir_in_place(
	unsigned char **imaus, short *im, short **im_indici, short **im_scr,
	short lb, short colore
);
void imut_haardir_in_place_col(
	unsigned char **imaus, short *imr, short *img, short *imb, short **im_indici,
	short **im_scr, short lb
);
void imut_crea_im_indici_haar_in_place(short **im_indici, short lb);

/*****************************************************************************/

/* variabili e array di uso globale; hanno l'underscore davanti */

static unsigned short *_vet_run_length=NULL;
/* vettore con le lunghezze dei run */

static short _n_byte;
/* indice di _vet_valori: il primo byte in cui si puo' scrivere */

static unsigned char _n_disponibili;
/* il numero di bit ancora disponibili del byte vet_valori[_n_byte] */

static short *_statistica=NULL;
/* vettore per la statistica dei valori residui normalizzati */

static unsigned short *_vet_codice_valori=NULL;
static unsigned char *_vet_numero_bit_codice=NULL;
/* vettori che contengono il codice per i valori: lunghezza e valore */

static unsigned short *_vet_codice_run=NULL;
static unsigned char *_vet_numero_bit_run=NULL;
/* vettori che contengono il codice per le lunghezze dei run */

static char _aus_vet, *_vet_valori=NULL;
/* vettore per la scrittura dei valori sul file */

static short *_im=NULL;                          /* solo BW e lossless */
static short *_imr=NULL, *_img=NULL, *_imb=NULL;           /* solo COL */
static short **_im_indici=NULL, **_im_scr=NULL;
static unsigned char **_imaus=NULL, **_imaus_fb=NULL;
/* array per il calcolo della trasformata e la lettura da disco */

static long _numero_byte;
/* numero di byte totale dell'immagine compressa */

static short _lb;
/* lunghezza del lato del blocco */

static short _lb2;
/* lb * lb */

static short _cflag;
/* vale 1 se l'immagine e' a colori */

static short _n_passi_quan;
/* (numero di passi di quantizzazione - 1) delle componenti cromatiche,
   questa variabile viene settata da calcola_egemone_e_gli_altri() e
   deve essere visibile a scrivi_colore perche' cosi' si puo' scegliere
   il codice di colore da usare (corto, lungo, lunghissimo) */

// spostati qui con 7.61
short *_dim_blo_com=NULL;
char *_out_buf=NULL;
CMPIMAGE *_header[30];  /* max 30 diverse inizializzazioni */
short _init_DIAC_done=0;
static short _th1[NSOGLIE], _th2[NSOGLIE], _th3[NSOGLIE];
static short _byte_per_pix;
static short _fd; /* fattore di degrado */

/*****************************************************************************/

/* 7.61: fa l'allocazione di memoria ed il calcolo dei vari vettori; essa puo'
   essere lanciata una sola volta, mentre l'inizializzazione degli header
   molte volte, cambiando la ROI da un header all'altro; */
short
imut_init_DIAC_memory(
	short fd,
	short flag_col,
	short len_block // larghezza del blocco, potenza di 2
)
{
	short ret;

	_init_DIAC_done = 0;
	_fd = fd;
	if(fd < -1 || fd > 10000)
		return SEAV_ERR_PARAMETER;

	if(flag_col) {
		_cflag = 1;
		_byte_per_pix = 3;
	} else {
		_cflag = 0;
		_byte_per_pix = 1;
	}
	_lb = len_block;
	_lb2 = _lb * _lb;

	/* allocazione */
	if((ret = alloca_memoria()) < 0)
		return ret;

	/* calcolo delle soglie */
	calcola_soglie();

	/* calcolo del vettore con gli indici di im */
	imut_crea_im_indici_haar_in_place(_im_indici, _lb);

	/* calcolo dei vettori dei codici dei valori e del run_length */
	crea_codice_valori();
	crea_codice_run();

	/* azzeramento del vettore della statistica */
	memset(_statistica, 0, (size_t) (sizeof(short) * RANGE_VALORI));

	_init_DIAC_done = 1;
	return SEAV_OK_ALL;
}

/*****************************************************************************/

/* inizializza un puntatore header */
short
imut_init_DIAC_header(
	ROI *roi,
	CMPIMAGE *header
)
{
	short fb=roi->fb, xi=roi->xi, yi=roi->yi, xf=roi->xf, yf=roi->yf;
	short ret, n_b_o, n_b_v;

	PARAM_SCHEDA param;

	/* controlli del fattore di degrado e della ROI da comprimere */
	sea2_init_config(&param);
	ret = imut_check_roi(roi, &param);
	if(ret < 0)
		return ret;

	_header[fb] = header;
	if(_fd < 0) { // lossless
		header->DIACheader[0] = 'l'; /* sta per lossless */
		header->DIACheader[1] = '1'; /* sta per primo tipo */
	} else {
		header->DIACheader[0] = 's';
	}
	header->DIACheader[2] = (char) roi->fb; /* numero della telecamera */
	header->block_len = _lb;
	_lb2 = _lb * _lb;
	header->xleft = xi;
	header->ydown = yi; // in realta' e' y-up
	header->width = xf - xi + 1;
	header->height = yf - yi + 1;
	if(_cflag) {
		header->pixdepth = DIAC_PIX24;
	} else {
		header->pixdepth = DIAC_PIX8;
	}
	header->cmpfactor = _fd;

	// calcolo del numero di blocchi
	n_b_o = header->width / _lb;
	if(header->width % _lb)
		n_b_o++;
	n_b_v = header->height / _lb;
	if(header->height % _lb)
		n_b_v++;
	header->hor_blocks = n_b_o;
	header->ver_blocks = n_b_v;

	/* inizializzo il contatore dei dati compressi: sommo la dimensione dell'header
	   a quella del vettore che contiene le dimensioni dei blocchi */
	header->cpdatasize = (long) (sizeof(CMPIMAGE) + n_b_o * n_b_v * sizeof(short));

	return SEAV_OK_ALL;
}

/*****************************************************************************/

/* 7.61: clean-up della memoria */
void imut_clean_DIAC_comp(void)
{
	libera_memoria();
	_init_DIAC_done = 0;
}

/*****************************************************************************/

/* comprime un blocco; ritorna il codice di errore */

void 
imut_cmpDIACblock(
	short fb,
	unsigned char *ptcdata,/* punta ai dati compressi */
	short ii,              /* indici del blocco */
	short jj,
	short *dim_blo_com
)
{
	ROI roi_blocco;
	short numero_run, numero_codice;
//short i, j;

	/* inizializzazione di _imaus tramite la lettura da fb */
	roi_blocco.fb = fb;
	roi_blocco.xi = _header[fb]->xleft + jj * _header[fb]->block_len;
	roi_blocco.yi = _header[fb]->ydown + ii * _header[fb]->block_len;
	roi_blocco.xf = roi_blocco.xi + _header[fb]->block_len - 1;
	roi_blocco.yf = roi_blocco.yi + _header[fb]->block_len - 1;
	if(_cflag)
		leggi_finestra_col(&roi_blocco);
	else
		leggi_finestra_bw(&roi_blocco);

	/* inizializzazione dei contatori e delle variabili globali */
	_n_byte = 0;
	_n_disponibili = 8;
	_numero_byte = 0L;
	_aus_vet = 0;
	_vet_valori = (char *) ptcdata;

	/* calcolo della trasformata */
	if(_cflag)
		imut_haardir_in_place_col(_imaus_fb, _imr, _img, _imb, _im_indici, _im_scr, _lb);
	else
		imut_haardir_in_place(_imaus_fb, _im, _im_indici, _im_scr, _lb, -1);
#if 0
sea2_seleziona_buffer(0);
if(roi_blocco.yi <= 400)
	for(i=0; i<lb; i++)
		for(j=0; j<lb; j++)
			sea2_scrivi_pixel(roi_blocco.xi+j, roi_blocco.yi+i, (im[i*lb+j] + 510) >> 2);
sea2_copy_bitmap(0);
sea2_refresh(0,0,0,0);
//MessageBox((HWND) NULL, "1", NULL, MB_OK|MB_APPLMODAL);
#endif

	/* inserimento dei buchi con normalizzazione,
	   calcolo del codice ottimale sulla base della
	   statistica dei valori trovati da metti_buchi e memorizzati in
	   statistica, quindi codifica dei valori superstiti */
	if(_cflag) {
		numero_run = metti_buchi_col();
		numero_codice = calcola_codice_ottimo();
		codifica_blocco_col(numero_run, numero_codice);
	} else {
		numero_run = metti_buchi();
		numero_codice = calcola_codice_ottimo();
		codifica_blocco(numero_run, numero_codice);
	}
	sistema_dim_blo_com(dim_blo_com);
	_header[fb]->cpdatasize += (long) *dim_blo_com;
}

/*******************************************************************/

/* comprime un blocco; ritorna il codice di errore */

void 
cmp_lossless_block(
	short fb,
	unsigned char *ptcdata,/* punta ai dati compressi */
	short ii,              /* indici del blocco */
	short jj,
	short *dim_blo_com
)
{
	ROI roi_blocco;
	short numero_codice, ret;

	/* inizializzazione di _imaus tramite la lettura da fb */
	roi_blocco.fb = fb;
	roi_blocco.xi = _header[fb]->xleft + jj * _header[fb]->block_len;
	roi_blocco.yi = _header[fb]->ydown + ii * _header[fb]->block_len;
	roi_blocco.xf = roi_blocco.xi + _header[fb]->block_len - 1;
	roi_blocco.yf = roi_blocco.yi + _header[fb]->block_len - 1;
	if(_cflag)
		leggi_finestra_col(&roi_blocco);
	else
		leggi_finestra_bw(&roi_blocco);

	/* inizializzazione dei contatori e delle variabili globali */
	_n_byte = 0;
	_n_disponibili = 8;
	_numero_byte = 0L;
	_aus_vet = 0;
	_vet_valori = (char *) ptcdata;

	/* calcolo della trasformata (per ora banale) */
	if(_cflag)
		ret = trasforma_blocco_col();
	else
		trasforma_blocco();

	/* calcolo della statistica,
	   calcolo del codice ottimale sulla base della
	   statistica dei valori trovati, quindi codifica dei valori */
	if(_cflag) {
		if(ret) { /* c'e' stato overflow: faccio tutto diverso */
			codifica_blocco_lossless_col(100/* codice speciale */);
		} else {
			calcola_statistica_col();
			numero_codice = calcola_codice_ottimo();
			codifica_blocco_lossless_col(numero_codice);
		}
	} else {
		calcola_statistica();
		numero_codice = calcola_codice_ottimo();
		codifica_blocco_lossless(numero_codice);
	}
	sistema_dim_blo_com(dim_blo_com);
	_header[fb]->cpdatasize += (long) *dim_blo_com;
}

/*******************************************************************/

/* routine per calcolare le soglie per l'inserimento dei buchi */

static void calcola_soglie()
{
	short i, fatt_qual = _fd;

	if(fatt_qual >= 0) {
		for(i=0; i<NSOGLIE; i++) {
			_th1[i] = 0;
			_th2[i] = 0;
			_th3[i] = 0;
		}

		_th1[0] = fatt_qual;
		_th2[0] = fatt_qual;
		_th3[0] = fatt_qual;

		_th1[1] = fatt_qual / 2;
		_th2[1] = fatt_qual / 2;
		_th3[1] = fatt_qual / 2;

		_th1[2] = fatt_qual / 4;
		_th2[2] = fatt_qual / 4;
		_th3[2] = fatt_qual / 4;

		_th1[3] = fatt_qual / 8;
		_th2[3] = fatt_qual / 8;
		_th3[3] = fatt_qual / 8;

		_th1[4] = fatt_qual / 16;
		_th2[4] = fatt_qual / 16;
		_th3[4] = fatt_qual / 16;

		if(_cflag) {
			for(i=0; i<NSOGLIE; i++) {
				_th1[i] *= 3;
				_th2[i] *= 3;
				_th3[i] *= 3;
			}
		}
	} else {
		for(i=0; i<NSOGLIE; i++) {
			_th1[i] = -1;
			_th2[i] = -1;
			_th3[i] = -1;
		}
	}
}


/*******************************************************************/

/* routine per creare il codice del valore
   nei vettori _vet_numero_bit_codice e _vet_codice_valori

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

static void crea_codice_valori(void)
{
	short i, j;
	short offset = RANGE_VALORI / 2;
	short inf = - RANGE_VALORI / 2;
	short sup = RANGE_VALORI / 2 - 1;

/* codice 2[+4[+9]] (0) */

	crea_codice_ternario(2, 4, 9, offset, inf, sup);

/* codice 2[+5[+9]] (1) */

	offset += RANGE_VALORI;
	crea_codice_ternario(2, 5, 9, offset, inf, sup);

/* codice 3[+4[+9]] (2) */

	offset += RANGE_VALORI;
	crea_codice_ternario(3, 4, 9, offset, inf, sup);

/* codice 2[+9] (3) */

	offset += RANGE_VALORI;
	crea_codice_binario(2, 9, offset, inf, sup);

/* codice 3[+9] (4) */

	offset += RANGE_VALORI;
	crea_codice_binario(3, 9, offset, inf, sup);

/* codice 4[+9] (5) */

	offset += RANGE_VALORI;
	crea_codice_binario(4, 9, offset, inf, sup);

/* codice 5[+9] (6) */

	offset += RANGE_VALORI;
	crea_codice_binario(5, 9, offset, inf, sup);

/* codice 6[+9] (7) */

	offset += RANGE_VALORI;
	crea_codice_binario(6, 9, offset, inf, sup);

/* codice 7[+9] (8) */

	offset += RANGE_VALORI;
	crea_codice_binario(7, 9, offset, inf, sup);

/* codice 9 (9) */

	offset += RANGE_VALORI;
	for(i=inf, j=0; i<=sup; i++, j++) {
		_vet_numero_bit_codice[offset + i] = 9;
		_vet_codice_valori[offset + i] = j;
	}

/* codice 3[+3[+9]] (10) */

	offset += RANGE_VALORI;
	crea_codice_ternario(3, 3, 9, offset, inf, sup);

/* codice 4[+4[+9]] (11) */

	offset += RANGE_VALORI;
	crea_codice_ternario(4, 4, 9, offset, inf, sup);

/* codice 4[+5[+9]] (12) */

	offset += RANGE_VALORI;
	crea_codice_ternario(4, 5, 9, offset, inf, sup);

/* codice 5[+5[+9]] (13) */

	offset += RANGE_VALORI;
	crea_codice_ternario(5, 5, 9, offset, inf, sup);

/* codice 5[+6[+9]] (14) */

	offset += RANGE_VALORI;
	crea_codice_ternario(5, 6, 9, offset, inf, sup);

/* codice 6[+6[+9]] (15) */

	offset += RANGE_VALORI;
	crea_codice_ternario(6, 6, 9, offset, inf, sup);

/* codice 3[+5[+9]] (16) */

	offset += RANGE_VALORI;
	crea_codice_ternario(3, 5, 9, offset, inf, sup);

/* codice 3[+6[+9]] (17) */

	offset += RANGE_VALORI;
	crea_codice_ternario(3, 6, 9, offset, inf, sup);

/* codice 3[+7[+9]] (18) */

	offset += RANGE_VALORI;
	crea_codice_ternario(3, 7, 9, offset, inf, sup);

/* codice 4[+6[+9]] (19) */

	offset += RANGE_VALORI;
	crea_codice_ternario(4, 6, 9, offset, inf, sup);

/* codice 4[+7[+9]] (20) */

	offset += RANGE_VALORI;
	crea_codice_ternario(4, 7, 9, offset, inf, sup);

/* codice 5[+7[+9]] (21) */

	offset += RANGE_VALORI;
	crea_codice_ternario(5, 7, 9, offset, inf, sup);

}

/*******************************************************************/

/* routine per creare il codice del run
   1[+2[+4[+8[+12]]]]
   1:            1
   1+2:          2,3,4
   1+2+4:        5,...,19
   1+2+4+8:     20,...,274
   1+2+4+8+12: 275,...,4096
   nei vettori _vet_numero_bit_run e _vet_codice_run;
   lo 0 fa sempre da prefisso per il pezzo di parola successivo;
*/

static void crea_codice_run(void)
{
	short i;

	_vet_numero_bit_run[0] = 1;
	_vet_codice_run[0] = 1;
	for(i=1; i<=3; i++) {
		_vet_numero_bit_run[i] = 3;
		_vet_codice_run[i] = i;
	}
	for(i=4; i<=18; i++) {
		_vet_numero_bit_run[i] = 7;
		_vet_codice_run[i] = i-3;
	}
	for(i=19; i<=273; i++) {
		_vet_numero_bit_run[i] = 15;
		_vet_codice_run[i] = i-18;
	}
	for(i=274; i<_lb2; i++) {
		_vet_numero_bit_run[i] = 27;
		_vet_codice_run[i] = i-273;
	}
}

/*******************************************************************/

/* routine per inserire i buchi a partire dal basso ma senza il
   controllo dei figli; inoltre viene creato in ordine inverso
   il vettore di run length del quale viene ritornata la lunghezza;
   la routine effettua anche la normalizzazione dei valori: il range
   e' riportato a 9 bit in complemento (-255, 255), la continua varia
   da 0 a 255 */

static short metti_buchi(void)
{
	short inf, h, norm, norm2;
	short *ausp;    /* puntatore dei valori */
	short *auspp;   /* puntatore dei padri */
	short padre, indice, diff;
	short run_in_corso, lunghezza_run;
	unsigned short *aus_run = _vet_run_length;
	short *aus_stat = _statistica + RANGE_VALORI / 2;
                                     /* per tener conto dei valori negativi */

	inf = _lb2 >> 2;      /* inizializzazione dei puntatori */
	ausp = _im + _lb2;
	auspp = _im + inf;
	h = 0;
	norm = 1;
	norm2 = 1;

/* inizializzazione del run in corso */

	diff = ((*(auspp-1)) >> 2) - *(ausp-1);
	if(abs(diff) <= _th3[h])
		run_in_corso = 0;              /* vuoto */
	else
		run_in_corso = 1;              /* pieno */
	lunghezza_run = 0;

	while(inf > 3) {
		short th1h = _th1[h]; /* le soglie */
		short th2h = _th2[h];
		short th3h = _th3[h];

/* terzo quadrante */

		for(indice=inf; indice>0; indice-=4) {
			padre = *--auspp >> 2;
			diff = padre - *--ausp;
			if(abs(diff) <= th3h) {
				if(!run_in_corso) {
					lunghezza_run ++;
				} else {
					*aus_run++ = lunghezza_run;
					lunghezza_run = 1;
					run_in_corso = 0;
				}
			} else {
				*ausp = (*ausp + norm2) >> norm;
				(*(aus_stat + *ausp)) ++;
				if(run_in_corso) {
					lunghezza_run ++;
				} else {
					*aus_run++ = lunghezza_run;
					lunghezza_run = 1;
					run_in_corso = 1;
				}
			}
			diff = padre - *--ausp;
			if(abs(diff) <= th3h) {
				if(!run_in_corso) {
					lunghezza_run ++;
				} else {
					*aus_run++ = lunghezza_run;
					lunghezza_run = 1;
					run_in_corso = 0;
				}
			} else {
				*ausp = (*ausp + norm2) >> norm;
				(*(aus_stat + *ausp)) ++;
				if(run_in_corso) {
					lunghezza_run ++;
				} else {
					*aus_run++ = lunghezza_run;
					lunghezza_run = 1;
					run_in_corso = 1;
				}
			}
			diff = padre - *--ausp;
			if(abs(diff) <= th3h) {
				if(!run_in_corso) {
					lunghezza_run ++;
				} else {
					*aus_run++ = lunghezza_run;
					lunghezza_run = 1;
					run_in_corso = 0;
				}
			} else {
				*ausp = (*ausp + norm2) >> norm;
				(*(aus_stat + *ausp)) ++;
				if(run_in_corso) {
					lunghezza_run ++;
				} else {
					*aus_run++ = lunghezza_run;
					lunghezza_run = 1;
					run_in_corso = 1;
				}
			}
			diff = padre - *--ausp;
			if(abs(diff) <= th3h) {
				if(!run_in_corso) {
					lunghezza_run ++;
				} else {
					*aus_run++ = lunghezza_run;
					lunghezza_run = 1;
					run_in_corso = 0;
				}
			} else {
				*ausp = (*ausp + norm2) >> norm;
				(*(aus_stat + *ausp)) ++;
				if(run_in_corso) {
					lunghezza_run ++;
				} else {
					*aus_run++ = lunghezza_run;
					lunghezza_run = 1;
					run_in_corso = 1;
				}
			}
		}

/* secondo quadrante */

		for(indice=inf; indice>0; indice-=4) {
			padre = *--auspp >> 2;
			diff = padre - *--ausp;
			if(abs(diff) <= th2h) {
				if(!run_in_corso) {
					lunghezza_run ++;
				} else {
					*aus_run++ = lunghezza_run;
					lunghezza_run = 1;
					run_in_corso = 0;
				}
			} else {
				*ausp = (*ausp + norm2) >> norm;
				(*(aus_stat + *ausp)) ++;
				if(run_in_corso) {
					lunghezza_run ++;
				} else {
					*aus_run++ = lunghezza_run;
					lunghezza_run = 1;
					run_in_corso = 1;
				}
			}
			diff = padre - *--ausp;
			if(abs(diff) <= th2h) {
				if(!run_in_corso) {
					lunghezza_run ++;
				} else {
					*aus_run++ = lunghezza_run;
					lunghezza_run = 1;
					run_in_corso = 0;
				}
			} else {
				*ausp = (*ausp + norm2) >> norm;
				(*(aus_stat + *ausp)) ++;
				if(run_in_corso) {
					lunghezza_run ++;
				} else {
					*aus_run++ = lunghezza_run;
					lunghezza_run = 1;
					run_in_corso = 1;
				}
			}
			diff = padre - *--ausp;
			if(abs(diff) <= th2h) {
				if(!run_in_corso) {
					lunghezza_run ++;
				} else {
					*aus_run++ = lunghezza_run;
					lunghezza_run = 1;
					run_in_corso = 0;
				}
			} else {
				*ausp = (*ausp + norm2) >> norm;
				(*(aus_stat + *ausp)) ++;
				if(run_in_corso) {
					lunghezza_run ++;
				} else {
					*aus_run++ = lunghezza_run;
					lunghezza_run = 1;
					run_in_corso = 1;
				}
			}
			diff = padre - *--ausp;
			if(abs(diff) <= th2h) {
				if(!run_in_corso) {
					lunghezza_run ++;
				} else {
					*aus_run++ = lunghezza_run;
					lunghezza_run = 1;
					run_in_corso = 0;
				}
			} else {
				*ausp = (*ausp + norm2) >> norm;
				(*(aus_stat + *ausp)) ++;
				if(run_in_corso) {
					lunghezza_run ++;
				} else {
					*aus_run++ = lunghezza_run;
					lunghezza_run = 1;
					run_in_corso = 1;
				}
			}
		}

/* primo quadrante */

		for(indice=inf; indice>0; indice-=4) {
			padre = *--auspp >> 2;
			diff = padre - *--ausp;
			if(abs(diff) <= th1h) {
				if(!run_in_corso) {
					lunghezza_run ++;
				} else {
					*aus_run++ = lunghezza_run;
					lunghezza_run = 1;
					run_in_corso = 0;
				}
			} else {
				*ausp = (*ausp + norm2) >> norm;
				(*(aus_stat + *ausp)) ++;
				if(run_in_corso) {
					lunghezza_run ++;
				} else {
					*aus_run++ = lunghezza_run;
					lunghezza_run = 1;
					run_in_corso = 1;
				}
			}
			diff = padre - *--ausp;
			if(abs(diff) <= th1h) {
				if(!run_in_corso) {
					lunghezza_run ++;
				} else {
					*aus_run++ = lunghezza_run;
					lunghezza_run = 1;
					run_in_corso = 0;
				}
			} else {
				*ausp = (*ausp + norm2) >> norm;
				(*(aus_stat + *ausp)) ++;
				if(run_in_corso) {
					lunghezza_run ++;
				} else {
					*aus_run++ = lunghezza_run;
					lunghezza_run = 1;
					run_in_corso = 1;
				}
			}
			diff = padre - *--ausp;
			if(abs(diff) <= th1h) {
				if(!run_in_corso) {
					lunghezza_run ++;
				} else {
					*aus_run++ = lunghezza_run;
					lunghezza_run = 1;
					run_in_corso = 0;
				}
			} else {
				*ausp = (*ausp + norm2) >> norm;
				(*(aus_stat + *ausp)) ++;
				if(run_in_corso) {
					lunghezza_run ++;
				} else {
					*aus_run++ = lunghezza_run;
					lunghezza_run = 1;
					run_in_corso = 1;
				}
			}
			diff = padre - *--ausp;
			if(abs(diff) <= th1h) {
				if(!run_in_corso) {
					lunghezza_run ++;
				} else {
					*aus_run++ = lunghezza_run;
					lunghezza_run = 1;
					run_in_corso = 0;
				}
			} else {
				*ausp = (*ausp + norm2) >> norm;
				(*(aus_stat + *ausp)) ++;
				if(run_in_corso) {
					lunghezza_run ++;
				} else {
					*aus_run++ = lunghezza_run;
					lunghezza_run = 1;
					run_in_corso = 1;
				}
			}
		}

		inf >>= 2;
		h++;
		norm ++;
		norm2 *= 2;
	}

/* conteggio i 4 valori residui */

	ausp--;
	*ausp = (*ausp + norm2) >> norm;
	(*(aus_stat + *ausp)) ++;
	ausp--;
	*ausp = (*ausp + norm2) >> norm;
	(*(aus_stat + *ausp)) ++;
	ausp--;
	*ausp = (*ausp + norm2) >> norm;
	(*(aus_stat + *ausp)) ++;
	ausp--;
	*ausp = (*ausp + norm2) >> norm;
	(*(aus_stat + *ausp)) ++;

/* chiudo il run in corso */

	if(run_in_corso) {
		*aus_run++ = lunghezza_run + 4;
	} else {
		*aus_run++ = lunghezza_run;
		*aus_run++ = 4;
	}
	return (short) (aus_run - _vet_run_length);
}

/*******************************************************************/

/* come metti_buchi ma per le immagini a colori */

static short metti_buchi_col(void)
{
	short inf, h, norm, norm2;
	short *auspr, *auspg, *auspb;      /* puntatore dei valori */
	short *ausppr, *ausppg, *ausppb;   /* puntatore dei padri */
	short padrer, padreg, padreb, indice, diffr, diffg, diffb;
	short run_in_corso, lunghezza_run;
	unsigned short *aus_run = _vet_run_length;
	short soglia_in;
	short *aus_stat = _statistica + RANGE_VALORI / 2;
                                     /* per tener conto dei valori negativi */

	inf = _lb2 >> 2;      /* inizializzazione dei puntatori */
	auspr = _imr + _lb2;
	auspg = _img + _lb2;
	auspb = _imb + _lb2;
	ausppr = _imr + inf;
	ausppg = _img + inf;
	ausppb = _imb + inf;
	h = 0;
	norm = 1;
	norm2 = 1;

/* inizializzazione del run in corso */

	diffr = ((*(ausppr-1)) >> 2) - *(auspr-1);
	diffg = ((*(ausppg-1)) >> 2) - *(auspg-1);
	diffb = ((*(ausppb-1)) >> 2) - *(auspb-1);
	soglia_in = _th3[0];
#if 0
	if(abs(diffr)<=soglia_in && abs(diffg)<=soglia_in && abs(diffb)<=soglia_in)
#endif
	if(abs(diffr) + abs(diffg) + abs(diffb)<=soglia_in)
		run_in_corso = 0;              /* vuoto */
	else
		run_in_corso = 1;              /* pieno */
	lunghezza_run = 0;

	while(inf > 3) {
		short th1h = _th1[h]; /* le soglie */
		short th2h = _th2[h];
		short th3h = _th3[h];

/* terzo quadrante */

		for(indice=inf; indice>0; indice-=4) {
			padrer = *--ausppr >> 2;
			padreg = *--ausppg >> 2;
			padreb = *--ausppb >> 2;
			diffr = padrer - *--auspr;
			diffg = padreg - *--auspg;
			diffb = padreb - *--auspb;
	#if 0
			if(abs(diffr) <= th3h && abs(diffg) <= th3h && abs(diffb) <= th3h) {
	#endif
			if(abs(diffr) + abs(diffg) + abs(diffb) <= th3h) {
				if(!run_in_corso) {
					lunghezza_run ++;
				} else {
					*aus_run++ = lunghezza_run;
					lunghezza_run = 1;
					run_in_corso = 0;
				}
			} else {
				*auspr = (*auspr + norm2) >> norm;
				*auspg = (*auspg + norm2) >> norm;
				*auspb = (*auspb + norm2) >> norm;
				(*(aus_stat + calcola_egemone(*auspr,*auspg,*auspb)))++;
				if(run_in_corso) {
					lunghezza_run ++;
				} else {
					*aus_run++ = lunghezza_run;
					lunghezza_run = 1;
					run_in_corso = 1;
				}
			}
			diffr = padrer - *--auspr;
			diffg = padreg - *--auspg;
			diffb = padreb - *--auspb;
	#if 0
			if(abs(diffr) <= th3h && abs(diffg) <= th3h && abs(diffb) <= th3h) {
	#endif
			if(abs(diffr) + abs(diffg) + abs(diffb) <= th3h) {
				if(!run_in_corso) {
					lunghezza_run ++;
				} else {
					*aus_run++ = lunghezza_run;
					lunghezza_run = 1;
					run_in_corso = 0;
				}
			} else {
				*auspr = (*auspr + norm2) >> norm;
				*auspg = (*auspg + norm2) >> norm;
				*auspb = (*auspb + norm2) >> norm;
				(*(aus_stat + calcola_egemone(*auspr,*auspg,*auspb)))++;
				if(run_in_corso) {
					lunghezza_run ++;
				} else {
					*aus_run++ = lunghezza_run;
					lunghezza_run = 1;
					run_in_corso = 1;
				}
			}
			diffr = padrer - *--auspr;
			diffg = padreg - *--auspg;
			diffb = padreb - *--auspb;
	#if 0
			if(abs(diffr) <= th3h && abs(diffg) <= th3h && abs(diffb) <= th3h) {
	#endif
			if(abs(diffr) + abs(diffg) + abs(diffb) <= th3h) {
				if(!run_in_corso) {
					lunghezza_run ++;
				} else {
					*aus_run++ = lunghezza_run;
					lunghezza_run = 1;
					run_in_corso = 0;
				}
			} else {
				*auspr = (*auspr + norm2) >> norm;
				*auspg = (*auspg + norm2) >> norm;
				*auspb = (*auspb + norm2) >> norm;
				(*(aus_stat + calcola_egemone(*auspr,*auspg,*auspb)))++;
				if(run_in_corso) {
					lunghezza_run ++;
				} else {
					*aus_run++ = lunghezza_run;
					lunghezza_run = 1;
					run_in_corso = 1;
				}
			}
			diffr = padrer - *--auspr;
			diffg = padreg - *--auspg;
			diffb = padreb - *--auspb;
	#if 0
			if(abs(diffr) <= th3h && abs(diffg) <= th3h && abs(diffb) <= th3h) {
	#endif
			if(abs(diffr) + abs(diffg) + abs(diffb) <= th3h) {
				if(!run_in_corso) {
					lunghezza_run ++;
				} else {
					*aus_run++ = lunghezza_run;
					lunghezza_run = 1;
					run_in_corso = 0;
				}
			} else {
				*auspr = (*auspr + norm2) >> norm;
				*auspg = (*auspg + norm2) >> norm;
				*auspb = (*auspb + norm2) >> norm;
				(*(aus_stat + calcola_egemone(*auspr,*auspg,*auspb)))++;
				if(run_in_corso) {
					lunghezza_run ++;
				} else {
					*aus_run++ = lunghezza_run;
					lunghezza_run = 1;
					run_in_corso = 1;
				}
			}
		}

/* secondo quadrante */

		for(indice=inf; indice>0; indice-=4) {
			padrer = *--ausppr >> 2;
			padreg = *--ausppg >> 2;
			padreb = *--ausppb >> 2;
			diffr = padrer - *--auspr;
			diffg = padreg - *--auspg;
			diffb = padreb - *--auspb;
	#if 0
			if(abs(diffr) <= th2h && abs(diffg) <= th2h && abs(diffb) <= th2h) {
	#endif
			if(abs(diffr) + abs(diffg) + abs(diffb) <= th2h) {
				if(!run_in_corso) {
					lunghezza_run ++;
				} else {
					*aus_run++ = lunghezza_run;
					lunghezza_run = 1;
					run_in_corso = 0;
				}
			} else {
				*auspr = (*auspr + norm2) >> norm;
				*auspg = (*auspg + norm2) >> norm;
				*auspb = (*auspb + norm2) >> norm;
				(*(aus_stat + calcola_egemone(*auspr,*auspg,*auspb)))++;
				if(run_in_corso) {
					lunghezza_run ++;
				} else {
					*aus_run++ = lunghezza_run;
					lunghezza_run = 1;
					run_in_corso = 1;
				}
			}
			diffr = padrer - *--auspr;
			diffg = padreg - *--auspg;
			diffb = padreb - *--auspb;
	#if 0
			if(abs(diffr) <= th2h && abs(diffg) <= th2h && abs(diffb) <= th2h) {
	#endif
			if(abs(diffr) + abs(diffg) + abs(diffb) <= th2h) {
				if(!run_in_corso) {
					lunghezza_run ++;
				} else {
					*aus_run++ = lunghezza_run;
					lunghezza_run = 1;
					run_in_corso = 0;
				}
			} else {
				*auspr = (*auspr + norm2) >> norm;
				*auspg = (*auspg + norm2) >> norm;
				*auspb = (*auspb + norm2) >> norm;
				(*(aus_stat + calcola_egemone(*auspr,*auspg,*auspb)))++;
				if(run_in_corso) {
					lunghezza_run ++;
				} else {
					*aus_run++ = lunghezza_run;
					lunghezza_run = 1;
					run_in_corso = 1;
				}
			}
			diffr = padrer - *--auspr;
			diffg = padreg - *--auspg;
			diffb = padreb - *--auspb;
	#if 0
			if(abs(diffr) <= th2h && abs(diffg) <= th2h && abs(diffb) <= th2h) {
	#endif
			if(abs(diffr) + abs(diffg) + abs(diffb) <= th2h) {
				if(!run_in_corso) {
					lunghezza_run ++;
				} else {
					*aus_run++ = lunghezza_run;
					lunghezza_run = 1;
					run_in_corso = 0;
				}
			} else {
				*auspr = (*auspr + norm2) >> norm;
				*auspg = (*auspg + norm2) >> norm;
				*auspb = (*auspb + norm2) >> norm;
				(*(aus_stat + calcola_egemone(*auspr,*auspg,*auspb)))++;
				if(run_in_corso) {
					lunghezza_run ++;
				} else {
					*aus_run++ = lunghezza_run;
					lunghezza_run = 1;
					run_in_corso = 1;
				}
			}
			diffr = padrer - *--auspr;
			diffg = padreg - *--auspg;
			diffb = padreb - *--auspb;
	#if 0
			if(abs(diffr) <= th2h && abs(diffg) <= th2h && abs(diffb) <= th2h) {
	#endif
			if(abs(diffr) + abs(diffg) + abs(diffb) <= th2h) {
				if(!run_in_corso) {
					lunghezza_run ++;
				} else {
					*aus_run++ = lunghezza_run;
					lunghezza_run = 1;
					run_in_corso = 0;
				}
			} else {
				*auspr = (*auspr + norm2) >> norm;
				*auspg = (*auspg + norm2) >> norm;
				*auspb = (*auspb + norm2) >> norm;
				(*(aus_stat + calcola_egemone(*auspr,*auspg,*auspb)))++;
				if(run_in_corso) {
					lunghezza_run ++;
				} else {
					*aus_run++ = lunghezza_run;
					lunghezza_run = 1;
					run_in_corso = 1;
				}
			}
		}

/* primo quadrante */

		for(indice=inf; indice>0; indice-=4) {
			padrer = *--ausppr >> 2;
			padreg = *--ausppg >> 2;
			padreb = *--ausppb >> 2;
			diffr = padrer - *--auspr;
			diffg = padreg - *--auspg;
			diffb = padreb - *--auspb;
	#if 0
			if(abs(diffr) <= th1h && abs(diffg) <= th1h && abs(diffb) <= th1h) {
	#endif
			if(abs(diffr) + abs(diffg) + abs(diffb) <= th1h) {
				if(!run_in_corso) {
					lunghezza_run ++;
				} else {
					*aus_run++ = lunghezza_run;
					lunghezza_run = 1;
					run_in_corso = 0;
				}
			} else {
				*auspr = (*auspr + norm2) >> norm;
				*auspg = (*auspg + norm2) >> norm;
				*auspb = (*auspb + norm2) >> norm;
				(*(aus_stat + calcola_egemone(*auspr,*auspg,*auspb)))++;
				if(run_in_corso) {
					lunghezza_run ++;
				} else {
					*aus_run++ = lunghezza_run;
					lunghezza_run = 1;
					run_in_corso = 1;
				}
			}
			diffr = padrer - *--auspr;
			diffg = padreg - *--auspg;
			diffb = padreb - *--auspb;
	#if 0
			if(abs(diffr) <= th1h && abs(diffg) <= th1h && abs(diffb) <= th1h) {
	#endif
			if(abs(diffr) + abs(diffg) + abs(diffb) <= th1h) {
				if(!run_in_corso) {
					lunghezza_run ++;
				} else {
					*aus_run++ = lunghezza_run;
					lunghezza_run = 1;
					run_in_corso = 0;
				}
			} else {
				*auspr = (*auspr + norm2) >> norm;
				*auspg = (*auspg + norm2) >> norm;
				*auspb = (*auspb + norm2) >> norm;
				(*(aus_stat + calcola_egemone(*auspr,*auspg,*auspb)))++;
				if(run_in_corso) {
					lunghezza_run ++;
				} else {
					*aus_run++ = lunghezza_run;
					lunghezza_run = 1;
					run_in_corso = 1;
				}
			}
			diffr = padrer - *--auspr;
			diffg = padreg - *--auspg;
			diffb = padreb - *--auspb;
	#if 0
			if(abs(diffr) <= th1h && abs(diffg) <= th1h && abs(diffb) <= th1h) {
	#endif
			if(abs(diffr) + abs(diffg) + abs(diffb) <= th1h) {
				if(!run_in_corso) {
					lunghezza_run ++;
				} else {
					*aus_run++ = lunghezza_run;
					lunghezza_run = 1;
					run_in_corso = 0;
				}
			} else {
				*auspr = (*auspr + norm2) >> norm;
				*auspg = (*auspg + norm2) >> norm;
				*auspb = (*auspb + norm2) >> norm;
				(*(aus_stat + calcola_egemone(*auspr,*auspg,*auspb)))++;
				if(run_in_corso) {
					lunghezza_run ++;
				} else {
					*aus_run++ = lunghezza_run;
					lunghezza_run = 1;
					run_in_corso = 1;
				}
			}
			diffr = padrer - *--auspr;
			diffg = padreg - *--auspg;
			diffb = padreb - *--auspb;
	#if 0
			if(abs(diffr) <= th1h && abs(diffg) <= th1h && abs(diffb) <= th1h) {
	#endif
			if(abs(diffr) + abs(diffg) + abs(diffb) <= th1h) {
				if(!run_in_corso) {
					lunghezza_run ++;
				} else {
					*aus_run++ = lunghezza_run;
					lunghezza_run = 1;
					run_in_corso = 0;
				}
			} else {
				*auspr = (*auspr + norm2) >> norm;
				*auspg = (*auspg + norm2) >> norm;
				*auspb = (*auspb + norm2) >> norm;
				(*(aus_stat + calcola_egemone(*auspr,*auspg,*auspb)))++;
				if(run_in_corso) {
					lunghezza_run ++;
				} else {
					*aus_run++ = lunghezza_run;
					lunghezza_run = 1;
					run_in_corso = 1;
				}
			}
		}

		inf >>= 2;
		h++;
		norm ++;
		norm2 *= 2;
	}

/* conteggio i 4 valori residui */

	auspr--;
	auspg--;
	auspb--;
	*auspr = (*auspr + norm2) >> norm;
	*auspg = (*auspg + norm2) >> norm;
	*auspb = (*auspb + norm2) >> norm;
	(*(aus_stat + calcola_egemone(*auspr,*auspg,*auspb)))++;
	auspr--;
	auspg--;
	auspb--;
	*auspr = (*auspr + norm2) >> norm;
	*auspg = (*auspg + norm2) >> norm;
	*auspb = (*auspb + norm2) >> norm;
	(*(aus_stat + calcola_egemone(*auspr,*auspg,*auspb)))++;
	auspr--;
	auspg--;
	auspb--;
	*auspr = (*auspr + norm2) >> norm;
	*auspg = (*auspg + norm2) >> norm;
	*auspb = (*auspb + norm2) >> norm;
	(*(aus_stat + calcola_egemone(*auspr,*auspg,*auspb)))++;
	auspr--;
	auspg--;
	auspb--;
	*auspr = (*auspr + norm2) >> norm;
	*auspg = (*auspg + norm2) >> norm;
	*auspb = (*auspb + norm2) >> norm;
	(*(aus_stat + calcola_egemone(*auspr,*auspg,*auspb)))++;

/* chiudo il run in corso */

	if(run_in_corso) {
		*aus_run++ = lunghezza_run + 4;
	} else {
		*aus_run++ = lunghezza_run;
		*aus_run++ = 4;
	}
	return (short) (aus_run - _vet_run_length);
}

/*******************************************************************/

/* routine per la codifica dei valori del blocco all'interno del vettore
   _vet_valori */

static void
codifica_blocco(
	short numero_run,      /* trovati all'interno del blocco */
	short numero_codice_da_usare
)
{
	short i, j, lunghezza_run;
	unsigned short *aus_run = _vet_run_length + numero_run;
	short *aus_im = _im;
	short valore;
	short offset = RANGE_VALORI * numero_codice_da_usare + RANGE_VALORI / 2;
	unsigned char *aus_numero_bit_run    = _vet_numero_bit_run    - 1;
	unsigned short *aus_codice_run       = _vet_codice_run        - 1;
	unsigned char *aus_numero_bit_codice = _vet_numero_bit_codice + offset;
	unsigned short *aus_codice_valori    = _vet_codice_valori     + offset;

/* scrivo il numero del codice da usare */

	scrivi_fino_a_16_bit_utili((unsigned char) 8, numero_codice_da_usare);

/* per ogni coppia di run (pieno-vuoto) */

	for(i=numero_run-1; i>0; i-=2) {

/* scrivo il codice della lunghezza del run pieno */

		lunghezza_run = *--aus_run;
		scrivi_fino_a_16_bit_utili(
			*(aus_numero_bit_run + lunghezza_run),
			*(aus_codice_run + lunghezza_run)
		);

/* scrivo lunghezza_run valori */

		for(j=lunghezza_run; j>0; j--) {
			valore = *aus_im++;
			scrivi_fino_a_16_bit_utili(*(aus_numero_bit_codice + valore),
			                           *(aus_codice_valori + valore));
		}

/* scrivo il codice della lunghezza del run vuoto */

		lunghezza_run = *--aus_run;
		scrivi_fino_a_16_bit_utili(*(aus_numero_bit_run + lunghezza_run),
		                           *(aus_codice_run + lunghezza_run));

/* salto lunghezza_run valori */

		aus_im += lunghezza_run;
	}
	if(!i) { /* l'ultimo run (con indice 0) e' pieno */

/* scrivo il codice della lunghezza del run pieno */

		lunghezza_run = *--aus_run;
		scrivi_fino_a_16_bit_utili(*(aus_numero_bit_run + lunghezza_run),
		                           *(aus_codice_run + lunghezza_run));

/* scrivo lunghezza_run valori */

		for(j=lunghezza_run; j>0; j--) {
			valore = *aus_im++;
			scrivi_fino_a_16_bit_utili(*(aus_numero_bit_codice + valore),
			                           *(aus_codice_valori + valore));
		}
	}
	return;
}

/*******************************************************************/

/* routine per la codifica dei valori del blocco all'interno del vettore
   _vet_valori */

static void
codifica_blocco_col(
	short numero_run,      /* trovati all'interno del blocco */
	short numero_codice_da_usare
)
{
	short i, j, lunghezza_run, k=0;
	unsigned short *aus_run = _vet_run_length + numero_run;
	short *aus_imr = _imr;
	short *aus_img = _img;
	short *aus_imb = _imb;
	short colore1, colore2, valore, egemone;
	short offset = RANGE_VALORI * numero_codice_da_usare + RANGE_VALORI / 2;
	unsigned char *aus_numero_bit_run    = _vet_numero_bit_run    - 1;
	unsigned short *aus_codice_run       = _vet_codice_run        - 1;
	unsigned char *aus_numero_bit_codice = _vet_numero_bit_codice + offset;
	unsigned short *aus_codice_valori    = _vet_codice_valori     + offset;

/* scrivo il numero del codice da usare */

	scrivi_fino_a_16_bit_utili((unsigned char) 8, numero_codice_da_usare);

/* per ogni coppia di run (pieno-vuoto) */

	for(i=numero_run-1; i>0; i-=2) {

/* scrivo il codice della lunghezza del run pieno */

		lunghezza_run = *--aus_run;
		scrivi_fino_a_16_bit_utili(
			*(aus_numero_bit_run + lunghezza_run),
			*(aus_codice_run + lunghezza_run)
		);

/* scrivo lunghezza_run valori a colori */

		for(j=lunghezza_run; j>0; j--, k++) {

/* calcolo il valore egemone e le componenti di colore gia' normalizzate */

			egemone = calcola_egemone_e_gli_altri(
				*aus_imr++, *aus_img++, *aus_imb++,
				&valore, &colore1, &colore2, k
			);

/* scrivo il valore egemone */

			scrivi_fino_a_16_bit_utili(
				*(aus_numero_bit_codice + valore),
				*(aus_codice_valori + valore)
			);

/* scrivo il codice del valore egemone (0,1,2,3) */

			scrivi_fino_a_16_bit_utili((unsigned char) 2, egemone);

/* scrivo le due eventuali componenti di colore */

			if(egemone != 3)
				scrivi_colore(colore1, colore2);
		}

/* scrivo il codice della lunghezza del run vuoto */

		lunghezza_run = *--aus_run;
		scrivi_fino_a_16_bit_utili(
			*(aus_numero_bit_run + lunghezza_run),
			*(aus_codice_run + lunghezza_run)
		);

/* salto lunghezza_run valori */

		aus_imr += lunghezza_run;
		aus_img += lunghezza_run;
		aus_imb += lunghezza_run;
		k += lunghezza_run;
	}
	if(!i) { /* l'ultimo run (con indice 0) e' pieno */

/* scrivo il codice della lunghezza del run pieno */

		lunghezza_run = *--aus_run;
		scrivi_fino_a_16_bit_utili(
			*(aus_numero_bit_run + lunghezza_run),
			*(aus_codice_run + lunghezza_run)
		);

/* scrivo lunghezza_run valori a colori */

		for(j=lunghezza_run; j>0; j--, k++) {

/* calcolo il valore egemone e le componenti di colore gia' normalizzate */

			egemone = calcola_egemone_e_gli_altri(
				*aus_imr++, *aus_img++, *aus_imb++,
				&valore, &colore1, &colore2, k
			);

/* scrivo il valore egemone */

			scrivi_fino_a_16_bit_utili(
				*(aus_numero_bit_codice + valore),
				*(aus_codice_valori + valore)
			);

/* scrivo il codice del valore egemone (0,1,2,3) */

			scrivi_fino_a_16_bit_utili((unsigned char) 2, egemone);

/* scrivo le due eventuali componenti di colore */

			if(egemone != 3)
				scrivi_colore(colore1, colore2);
		}
	}
	return;
}

/*******************************************************************/

/* routine per scrivere fino a 16 bit utili nel vettore _vet_valori;
   se n_bit e' maggiore di 16, i bit che mancano (i piu' significativi)
   sono posti a 0; i bit che sono prelevati da valore sono i meno
   significativi; i bit che non devono essere prelevati da valore
   (i piu' significativi) devono essere a 0 */

static void
scrivi_fino_a_16_bit_utili(
	unsigned char n_bit,      /* numero di bit da scrivere */
	unsigned short valore     /* valore da scrivere */
)
{

	if(n_bit > 16) {
		scrivi_fino_a_16_bit_utili((unsigned char) (n_bit - 16), (short) 0);
		n_bit = 16;
	}
	while(n_bit > _n_disponibili) {
		*_vet_valori++ = _aus_vet |
		                    (char) (valore >> (n_bit -= _n_disponibili));
		_aus_vet = (char) 0;
		_n_disponibili = (char) 8;
		_n_byte++;
	}
	_aus_vet |= (valore << (_n_disponibili -= n_bit));
	if(!_n_disponibili) {
		*_vet_valori++ = _aus_vet;
		_aus_vet = (char) 0;
		_n_disponibili = (char) 8;
		_n_byte++;
	}
	return;
}

/*******************************************************************/

/* routine per scegliere uno dei codici predefiniti in base alla
   _statistica dei valori superstiti calcolata da metti_buchi */

static short calcola_codice_ottimo(void)
{
	unsigned long n_parole[N_CODICI]
                          [22]; /* lunghezza massima di parola + 1 */
	unsigned long min;
	short i_min;
	short i;
	short *aus_stat = _statistica + RANGE_VALORI, n_valori;
	unsigned char *aus_numero_bit_codice = _vet_numero_bit_codice +
                                                	RANGE_VALORI;
	short off0 = 0; short off1 = off0 + RANGE_VALORI;
	short off2 = off1 + RANGE_VALORI; short off3 = off2 + RANGE_VALORI;
	short off4 = off3 + RANGE_VALORI; short off5 = off4 + RANGE_VALORI;
	short off6 = off5 + RANGE_VALORI; short off7 = off6 + RANGE_VALORI;
	short off8 = off7 + RANGE_VALORI; short off9 = off8 + RANGE_VALORI;
	short off10 = off9 + RANGE_VALORI; short off11 = off10 + RANGE_VALORI;
	short off12 = off11 + RANGE_VALORI; short off13 = off12 + RANGE_VALORI;
	short off14 = off13 + RANGE_VALORI; short off15 = off14 + RANGE_VALORI;
	short off16 = off15 + RANGE_VALORI; short off17 = off16 + RANGE_VALORI;
	short off18 = off17 + RANGE_VALORI; short off19 = off18 + RANGE_VALORI;
	short off20 = off19 + RANGE_VALORI; short off21 = off20 + RANGE_VALORI;

/* azzero solo le lunghezze possibili */

	n_parole[0][2]  = n_parole[0][6]   = n_parole[0][15]  = 0;
	n_parole[1][2]  = n_parole[1][7]   = n_parole[1][16]  = 0;
	n_parole[2][3]  = n_parole[2][7]   = n_parole[2][16]  = 0;
	n_parole[3][2]  = n_parole[3][11]  = 0;
	n_parole[4][3]  = n_parole[4][12]  = 0;
	n_parole[5][4]  = n_parole[5][13]  = 0;
	n_parole[6][5]  = n_parole[6][14]  = 0;
	n_parole[7][6]  = n_parole[7][15]  = 0;
	n_parole[8][7]  = n_parole[8][16]  = 0;
	n_parole[9][9]  = 0;
	n_parole[10][3] = n_parole[10][6]  = n_parole[10][15] = 0;
	n_parole[11][4] = n_parole[11][8]  = n_parole[11][17] = 0;
	n_parole[12][4] = n_parole[12][9]  = n_parole[12][18] = 0;
	n_parole[13][5] = n_parole[13][10] = n_parole[13][19] = 0;
	n_parole[14][5] = n_parole[14][11] = n_parole[14][20] = 0;
	n_parole[15][6] = n_parole[15][12] = n_parole[15][21] = 0;
	n_parole[16][3] = n_parole[16][8]  = n_parole[16][17] = 0;
	n_parole[17][3] = n_parole[17][9]  = n_parole[17][18] = 0;
	n_parole[18][3] = n_parole[18][10] = n_parole[18][19] = 0;
	n_parole[19][4] = n_parole[19][10] = n_parole[19][19] = 0;
	n_parole[20][4] = n_parole[20][11] = n_parole[20][20] = 0;
	n_parole[21][5] = n_parole[21][12] = n_parole[21][21] = 0;

/* scandisco il vettore della _statistica ed incremento n_parole */

	for(i=RANGE_VALORI; i>0; i--) {
		aus_numero_bit_codice --;
		if(n_valori = *--aus_stat) {
			*(*(n_parole  )  + *(aus_numero_bit_codice       ))  += n_valori;
			*(*(n_parole+1)  + *(aus_numero_bit_codice + off1))  += n_valori;
			*(*(n_parole+2)  + *(aus_numero_bit_codice + off2))  += n_valori;
			*(*(n_parole+3)  + *(aus_numero_bit_codice + off3))  += n_valori;
			*(*(n_parole+4)  + *(aus_numero_bit_codice + off4))  += n_valori;
			*(*(n_parole+5)  + *(aus_numero_bit_codice + off5))  += n_valori;
			*(*(n_parole+6)  + *(aus_numero_bit_codice + off6))  += n_valori;
			*(*(n_parole+7)  + *(aus_numero_bit_codice + off7))  += n_valori;
			*(*(n_parole+8)  + *(aus_numero_bit_codice + off8))  += n_valori;
			*(*(n_parole+9)  + *(aus_numero_bit_codice + off9))  += n_valori;
			*(*(n_parole+10) + *(aus_numero_bit_codice + off10)) += n_valori;
			*(*(n_parole+11) + *(aus_numero_bit_codice + off11)) += n_valori;
			*(*(n_parole+12) + *(aus_numero_bit_codice + off12)) += n_valori;
			*(*(n_parole+13) + *(aus_numero_bit_codice + off13)) += n_valori;
			*(*(n_parole+14) + *(aus_numero_bit_codice + off14)) += n_valori;
			*(*(n_parole+15) + *(aus_numero_bit_codice + off15)) += n_valori;
			*(*(n_parole+16) + *(aus_numero_bit_codice + off16)) += n_valori;
			*(*(n_parole+17) + *(aus_numero_bit_codice + off17)) += n_valori;
			*(*(n_parole+18) + *(aus_numero_bit_codice + off18)) += n_valori;
			*(*(n_parole+19) + *(aus_numero_bit_codice + off19)) += n_valori;
			*(*(n_parole+20) + *(aus_numero_bit_codice + off20)) += n_valori;
			*(*(n_parole+21) + *(aus_numero_bit_codice + off21)) += n_valori;
			*aus_stat = 0; /* azzero la _statistica */
		}
	}

/* calcolo il numero di bit totali per ciascun codice e metto il risultato
   in n_parole[][0] */

	n_parole[0][0] = 2*n_parole[0][2] + 6*n_parole[0][6] + 15*n_parole[0][15];
	n_parole[1][0] = 2*n_parole[1][2] + 7*n_parole[1][7] + 16*n_parole[1][16];
	n_parole[2][0] = 3*n_parole[2][3] + 7*n_parole[2][7] + 16*n_parole[2][16];
	n_parole[3][0] = 2*n_parole[3][2] + 11*n_parole[3][11];
	n_parole[4][0] = 3*n_parole[4][3] + 12*n_parole[4][12];
	n_parole[5][0] = 4*n_parole[5][4] + 13*n_parole[5][13];
	n_parole[6][0] = 5*n_parole[6][5] + 14*n_parole[6][14];
	n_parole[7][0] = 6*n_parole[7][6] + 15*n_parole[7][15];
	n_parole[8][0] = 7*n_parole[8][7] + 16*n_parole[8][16];
	n_parole[9][0] = 9*n_parole[9][9];
	n_parole[10][0] = 3*n_parole[10][3]+6*n_parole[10][6] + 15*n_parole[10][15];
	n_parole[11][0] = 4*n_parole[11][4]+8*n_parole[11][8] + 17*n_parole[11][17];
	n_parole[12][0] = 4*n_parole[12][4]+9*n_parole[12][9] + 18*n_parole[12][18];
	n_parole[13][0] = 5*n_parole[13][5]+10*n_parole[13][10]+19*n_parole[13][19];
	n_parole[14][0] = 5*n_parole[14][5]+11*n_parole[14][11]+20*n_parole[14][20];
	n_parole[15][0] = 6*n_parole[15][6]+12*n_parole[15][12]+21*n_parole[15][21];
	n_parole[16][0] = 3*n_parole[16][3]+8*n_parole[16][8]  +17*n_parole[16][17];
	n_parole[17][0] = 3*n_parole[17][3]+9*n_parole[17][9]  +18*n_parole[17][18];
	n_parole[18][0] = 3*n_parole[18][3]+10*n_parole[18][10]+19*n_parole[18][19];
	n_parole[19][0] = 4*n_parole[19][4]+10*n_parole[19][10]+19*n_parole[19][19];
	n_parole[20][0] = 4*n_parole[20][4]+11*n_parole[20][11]+20*n_parole[20][20];
	n_parole[21][0] = 5*n_parole[21][5]+12*n_parole[21][12]+21*n_parole[21][21];

/* scelgo il valore minore */

	min = n_parole[0][0];
	i_min = 0;
	for(i=1; i<N_CODICI; i++) {
		if(n_parole[i][0] < min) {
			min = n_parole[i][0];
			i_min = i;
		}
	}

	return i_min;
}

/*******************************************************************/

/* routine per generare un codice del tipo x[+y[+z]] */

static void
crea_codice_ternario(
	short nb1,
	short nb2,
	short nb3,
	short offset,    /* dell'indice del vettore dei codici */
	short inf,       /* valori estremi */
	short sup
)
{
	short NB1 = nb1, NB2 = nb1 + nb2, NB3 = nb1 + nb2 + nb3;
	short sup1, sup2;
	short i, j;
    /* i: valore di ingresso, j: codice associato: il prefisso e' sempre lo 0 */

	sup1 = el(nb1 - 1) - 1;
	sup2 = sup1 + el(nb2 - 1) - 1;

	for(i=-sup1, j=1; i<=sup1; i++, j++) {
		_vet_numero_bit_codice[offset + i] = (unsigned char) NB1;
		_vet_codice_valori[offset + i] = j;
	}

	for(i=-sup2, j=1; i<-sup1; i++, j++) {
		_vet_numero_bit_codice[offset + i] = (unsigned char) NB2;
		_vet_codice_valori[offset + i] = j;
	}
	for(i=sup1+1; i<=(sup2+1); i++, j++) {
		_vet_numero_bit_codice[offset + i] = (unsigned char) NB2;
		_vet_codice_valori[offset + i] = j;
	}

	for(i=inf, j=1; i<-sup2; i++, j++) {
		_vet_numero_bit_codice[offset + i] = (unsigned char) NB3;
		_vet_codice_valori[offset + i] = j;
	}
	for(i=sup2+2; i<=sup; i++, j++) {
		_vet_numero_bit_codice[offset + i] = (unsigned char) NB3;
		_vet_codice_valori[offset + i] = j;
	}
	return;
}

/*******************************************************************/

/* routine per generare un codice del tipo x[+y] */

static void
crea_codice_binario(
	short nb1,
	short nb2,
	short offset,    /* dell'indice del vettore dei codici */
	short inf,       /* valori estremi */
	short sup
)
{
	short NB1 = nb1, NB2 = nb1 + nb2;
	short sup1;
	short i, j;
    /* i: valore di ingresso, j: codice associato: il prefisso e' sempre lo 0 */

	sup1 = el(nb1 - 1) - 1;

	for(i=-sup1, j=1; i<=sup1; i++, j++) {
		_vet_numero_bit_codice[offset + i] = (unsigned char) NB1;
		_vet_codice_valori[offset + i] = j;
	}

	for(i=inf, j=1; i<-sup1; i++, j++) {
		_vet_numero_bit_codice[offset + i] = (unsigned char) NB2;
		_vet_codice_valori[offset + i] = j;
	}
	for(i=sup1+1; i<=sup; i++, j++) {
		_vet_numero_bit_codice[offset + i] = (unsigned char) NB2;
		_vet_codice_valori[offset + i] = j;
	}
	return;
}

/*******************************************************************/

/* routine che restituisce il valore che ha il modulo massimo; */

static short
calcola_egemone(
	short valore_r,
	short valore_g,
	short valore_b
)
{
	short valore_r_a = abs(valore_r);
	short valore_g_a = abs(valore_g);
	short valore_b_a = abs(valore_b);

	if(valore_r_a > valore_g_a) {
		if(valore_r_a > valore_b_a) {
			return valore_r;
		} else {
			return valore_b;
		}
	} else {
		if(valore_g_a > valore_b_a) {
			return valore_g;
		} else {
			return valore_b;
		}
	}
}

/*******************************************************************/

/* routine che mette in ordine i valori dei colori in base all'egemone;
   calcola le componenti di colore normalizzate C1 e C2, setta il valore
   di _n_passi_quan che serve per scegliere il codice di colore da usare 
   e restituisce il codice dell'egemone (0,1,2,3) */

static short
calcola_egemone_e_gli_altri(
	short valore_r,
	short valore_g,
	short valore_b,
	short *valore,
	short *colore1,
	short *colore2,
	short indice
)
{
	short valore_r_a = abs(valore_r);
	short valore_g_a = abs(valore_g);
	short valore_b_a = abs(valore_b);
	short val, val_a, col1, col2, egemone;

	if(valore_r_a > valore_g_a) {
		if(valore_r_a > valore_b_a) {
			egemone = 0;
			val = valore_r;
			val_a = valore_r_a;
			col1 = valore_g;
			col2 = valore_b;
		} else {
			egemone = 2;
			val = valore_b;
			val_a = valore_b_a;
			col1 = valore_r;
			col2 = valore_g;
		}
	} else {
		if(valore_g_a > valore_b_a) {
			egemone = 1;
			val = valore_g;
			val_a = valore_g_a;
			col1 = valore_r;
			col2 = valore_b;
		} else {
			egemone = 2;
			val = valore_b;
			val_a = valore_b_a;
			col1 = valore_r;
			col2 = valore_g;
		}
	}
	if(val) {
		if(indice >= N_VALORI_LUNGHINI) {
			if(val_a <= SC)
				_n_passi_quan = N_PA_CORTO;
			else if(val_a <= SB)
				_n_passi_quan = N_PA_LUNGHINO;
			else if(val_a <= SA)
				_n_passi_quan = N_PA_LUNGO;
			else
				_n_passi_quan = N_PA_LUNGHISSIMO;
		} else {
			if(val_a > SA || indice < N_VALORI_LUNGHISSIMI)
				_n_passi_quan = N_PA_LUNGHISSIMO;
			else if(val_a > SB || indice < N_VALORI_LUNGHI)
				_n_passi_quan = N_PA_LUNGO;
			else
				_n_passi_quan = N_PA_LUNGHINO;
		}
		if(col1 < 0)
			col1 = (short) (((long)_n_passi_quan * col1 - val_a)/(val << 1));
		else
			col1 = (short) (((long)_n_passi_quan * col1 + val_a)/(val << 1));
		if(col2 < 0)
			col2 = (short) (((long)_n_passi_quan * col2 - val_a)/(val << 1));
		else
			col2 = (short) (((long)_n_passi_quan * col2 + val_a)/(val << 1));
		if((col1 + col2) == _n_passi_quan) {
			egemone = 3;
		} else {
			*colore1 = col1;
			*colore2 = col2;
		}
	} else {
		egemone = 3;
	}
	*valore = val;
	return egemone;
}

/*******************************************************************/

/* routine per scrivere le due componenti di colore nei diversi formati
   (corto, lungo, lunghissimo) */

static void
scrivi_colore(
	short colore1,
	short colore2
)
{
	switch(_n_passi_quan) {
		case N_PA_CORTO:
			switch(colore1) {
				case 0:
					switch(colore2) {
						case 0: /* 0 */
							scrivi_fino_a_16_bit_utili(
								(unsigned char) 2, (unsigned short) 0);
							break;
						case 1: /* 1 */
							scrivi_fino_a_16_bit_utili(
								(unsigned char) 2, (unsigned short) 1);
							break;
						case -1: /* 3+2 */
							scrivi_fino_a_16_bit_utili(
								(unsigned char) 4, (unsigned short) 14);
							break;
					}
					break;
				case 1:
					switch(colore2) {
						case 0: /* 2 */
							scrivi_fino_a_16_bit_utili(
								(unsigned char) 2, (unsigned short) 2);
							break;
						case -1: /* 3+3+1 */
							scrivi_fino_a_16_bit_utili(
								(unsigned char) 5, (unsigned short) 31);
							break;
					}
					break;
				case -1:
					switch(colore2) {
						case 0: /* 3+1*/
							scrivi_fino_a_16_bit_utili(
								(unsigned char) 4, (unsigned short) 13);
							break;
						case 1: /* 3+3+0 */
							scrivi_fino_a_16_bit_utili(
								(unsigned char) 5, (unsigned short) 30);
							break;
						case -1: /* 3+0 */
							scrivi_fino_a_16_bit_utili(
								(unsigned char) 4, (unsigned short) 12);
							break;
					}
					break;
			}
			break;
		case N_PA_LUNGHINO:
			if(colore1 > 0)
				scrivi_fino_a_16_bit_utili((unsigned char) 2, colore1);
			else
				scrivi_fino_a_16_bit_utili((unsigned char) 4, (unsigned short) (colore1 & 3));
			if(colore2 > 0)
				scrivi_fino_a_16_bit_utili((unsigned char) 2, colore2);
			else
				scrivi_fino_a_16_bit_utili((unsigned char) 4, (unsigned short) (colore2 & 3));
			break;
		case N_PA_LUNGO:
			if(colore1 > 0)
				scrivi_fino_a_16_bit_utili((unsigned char) 3, colore1);
			else
				scrivi_fino_a_16_bit_utili((unsigned char) 6, (unsigned short) (colore1 & 7));
			if(colore2 > 0)
				scrivi_fino_a_16_bit_utili((unsigned char) 3, colore2);
			else
				scrivi_fino_a_16_bit_utili((unsigned char) 6, (unsigned short) (colore2 & 7));
			break;
		case N_PA_LUNGHISSIMO:
			scrivi_fino_a_16_bit_utili((unsigned char) 8, (unsigned short) (colore1 & 255));
			scrivi_fino_a_16_bit_utili((unsigned char) 8, (unsigned short) (colore2 & 255));
			break;
	}
	return;
}

/*******************************************************************/

/* scrive il valore appropriato nel vettore dim_blo_com e
   mette a 8 _n_disponibili, aggiustando il valore di _n_byte */

static void sistema_dim_blo_com(short *dim_blo_com)
{
	/* riallineo _n_byte */
	if(_n_disponibili != (char) 8) {
		*_vet_valori++ = _aus_vet;
		_aus_vet = (char) 0;
		_n_disponibili = (char) 8;
		_n_byte++;
	}
	*dim_blo_com = _n_byte;
}

/*******************************************************************/

static void init_puntatori(void)
{
	_im_indici = NULL;
	_imaus = NULL;
	_im_scr = NULL;
	_imr = NULL;
	_img = NULL;
	_imb = NULL;
	_im = NULL;
	_vet_run_length = NULL;
	_vet_codice_run = NULL;
	_vet_numero_bit_run = NULL;
	_statistica = NULL;
	_vet_codice_valori = NULL;
	_vet_numero_bit_codice = NULL;
	_dim_blo_com = NULL;
	_out_buf = NULL;
}

/*******************************************************************/

/* gli array _im, _im_scr, _im_indici vengono allocati all'interno di
   uno stesso item in modo che si trovino tutti e tre all'interno dello
   stesso segmento; nel caso del colore al posto di _im si allocano,
   sempre nello stesso modo, _imr, _img, _imb */

static short alloca_memoria()
{
	short nbpp, n_blocchi_max = (1000 / _lb) * (1000 / _lb);

	libera_memoria();
	init_puntatori();

	if(_byte_per_pix == 1)
		nbpp = 1;
	else
		nbpp = 4;
	_im_indici = (short **) util_alloca_matrice(_lb, _lb, sizeof(short));
	_imaus = (unsigned char **) util_alloca_matrice(
		_lb, _lb * nbpp, sizeof(unsigned char)
	);
	_im_scr = (short **) util_alloca_matrice(
		_lb, _lb * _byte_per_pix, sizeof(short)
	);

	_imaus_fb = (unsigned char **) malloc(_lb * sizeof(unsigned char *));
	_vet_run_length = (unsigned short *) malloc(_lb2 * sizeof(unsigned short));
	_vet_codice_run = (unsigned short *) malloc(_lb2 * sizeof(unsigned short));
	_vet_numero_bit_run = (unsigned char *) malloc(_lb2 * sizeof(unsigned char));
	_statistica = (short *) malloc((size_t) RANGE_VALORI * sizeof(short));
	_vet_codice_valori = (unsigned short *) malloc(
		(size_t) LUNG_CODICI * sizeof(unsigned short)
	);
	_vet_numero_bit_codice = (unsigned char *) malloc(
		LUNG_CODICI * sizeof(unsigned char)
	);
	_dim_blo_com = (short *) malloc(n_blocchi_max * sizeof(short));
	_out_buf = (char *) malloc((size_t) DIAC_BUF_SIZE * sizeof(char));
	_im = (short *) malloc(_lb2 * 3 * sizeof(short));

	if(!_im_indici || !_imaus || !_im_scr || !_vet_run_length || !_vet_codice_run ||
	   !_vet_numero_bit_run || !_statistica || !_vet_codice_valori || !_im ||
	   !_vet_numero_bit_codice || !_dim_blo_com || !_out_buf || !_imaus_fb) {
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

/*****************************************************************************/

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
	if(_vet_run_length) {
		free(_vet_run_length);
		_vet_run_length = NULL;
	}
	if(_vet_codice_run) {
		free(_vet_codice_run);
		_vet_codice_run = NULL;
	}
	if(_vet_numero_bit_run) {
		free(_vet_numero_bit_run);
		_vet_numero_bit_run = NULL;
	}
	if(_statistica) {
		free(_statistica);
		_statistica = NULL;
	}
	if(_vet_codice_valori) {
		free(_vet_codice_valori);
		_vet_codice_valori = NULL;
	}
	if(_vet_numero_bit_codice) {
		free(_vet_numero_bit_codice);
		_vet_numero_bit_codice = NULL;
	}
	if(_dim_blo_com) {
		free(_dim_blo_com);
		_dim_blo_com = NULL;
	}
	if(_out_buf) {
		free(_out_buf);
		_out_buf = NULL;
	}
	if(_im) {
		free(_im);
		_im = NULL;
	}
	if(_imaus_fb) {
		free(_imaus_fb);
		_imaus_fb = NULL;
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

/* legge una finestra in _imaus mettendo a 0 tutto cio' che esce fuori dalla roi */

static void
leggi_finestra_col(
	ROI *roi_blocco /* zona da leggere */
)
{
	short fb=roi_blocco->fb;
	short xi=roi_blocco->xi, yi=roi_blocco->yi;
	short xf=roi_blocco->xf, yf=roi_blocco->yf;
	short x_min=_header[fb]->xleft, y_min=_header[fb]->ydown;
	short x_max=x_min+_header[fb]->width-1, y_max=y_min+_header[fb]->height-1;

	sea2_seleziona_buffer(roi_blocco->fb);
	if(xi < x_min || xf > x_max ||
	   yi < y_min || yf > y_max) { // caso di bordo
		short i, indice, xl=(short)(xf-xi+1), yl=(short)(yf-yi+1);

		for(i=0; i<yl; i++) {
			_imaus_fb[i] = _imaus[i];
			memset(_imaus[i], 0, xl*4);
		}
		if(xi < x_min) {
			for(i=yi, indice=0; i<=yf; i++, indice++) {
				if(i >= y_min && i <= y_max) {
					sea2_leggi_riga_col(
						x_min, i, (short) (xf-x_min+1), _imaus[indice]+(x_min-xi)*3
					);
				}
			}
		} else if(xf > x_max) {
			for(i=yi, indice=0; i<=yf; i++, indice++) {
				if(i >= y_min && i <= y_max) {
					sea2_leggi_riga_col(xi, i, (short) (x_max-xi+1), _imaus[indice]);
				}
			}
		} else {
			for(i=yi, indice=0; i<=yf; i++, indice++) {
				if(i >= y_min && i <= y_max) {
					sea2_leggi_riga_col(xi, i, xl, _imaus[indice]);
				}
			}
		}
	} else { // caso regolare
		short i, xl=(short)(xf-xi+1), indice;
		for(i=yi, indice=0; i<=yf; i++, indice++) {
			_imaus_fb[indice] = sea2_indirizzo_memoria_col(xi, i);
//			sea2_leggi_riga_col24(xi, i, xl, _imaus[indice]);
		}
	}
}

/************************************************************************/

/* legge una finestra in _imaus mettendo a 0 tutto cio' che esce fuori dalla roi */

static void
leggi_finestra_bw(
	ROI *roi_blocco /* zona da leggere */
)
{
	short fb=roi_blocco->fb;
	short xi=roi_blocco->xi, yi=roi_blocco->yi;
	short xf=roi_blocco->xf, yf=roi_blocco->yf;
	short x_min=_header[fb]->xleft, y_min=_header[fb]->ydown;
	short x_max=x_min+_header[fb]->width-1, y_max=y_min+_header[fb]->height-1;

	sea2_seleziona_buffer(roi_blocco->fb);
	if(xi < x_min || xf > x_max ||
	   yi < y_min || yf > y_max) { // caso di bordo
		short i, indice, xl=(short)(xf-xi+1), yl=(short)(yf-yi+1);

		for(i=0; i<yl; i++) {
			_imaus_fb[i] = _imaus[i];
			memset(_imaus[i], 0, xl);
		}
		if(xi < x_min) {
			for(i=yi, indice=0; i<=yf; i++, indice++) {
				if(i >= y_min && i <= y_max) {
					sea2_leggi_riga(
						x_min, i, (short) (xf-x_min+1), _imaus[indice]+x_min-xi
					);
				}
			}
		} else if(xf > x_max) {
			for(i=yi, indice=0; i<=yf; i++, indice++) {
				if(i >= y_min && i <= y_max) {
					sea2_leggi_riga(xi, i, (short) (x_max-xi+1), _imaus[indice]);
				}
			}
		} else {
			for(i=yi, indice=0; i<=yf; i++, indice++) {
				if(i >= y_min && i <= y_max) {
					sea2_leggi_riga(xi, i, xl, _imaus[indice]);
				}
			}
		}
	} else { // caso regolare
		short i, indice, xl=(short)(xf-xi+1);
		for(i=yi, indice=0; i<=yf; i++, indice++) {
			// modifica 7.67 per gestire l'acq in parallelo da sorgenti
			// monocromatiche: non si possono usare gli indirizzi perche'
			// i pixel non sono contigui: bisogna usare la leggi_riga
//			_imaus_fb[indice] = sea2_indirizzo_memoria(xi, i);
			_imaus_fb[indice] = _imaus[indice];
			sea2_leggi_riga(xi, i, xl, _imaus[indice]);
		}
	}
}

/************************************************************************/

/* da _imaus a im */

static void trasforma_blocco(void)
{
	short i, j, k=0, prec = 0;

	for(i=0; i<_lb; i++) {
		for(j=0; j<_lb; j++) {
			_im[k++] = (short) ((int) _imaus_fb[i][j] - (int) prec);
			prec = (short) _imaus_fb[i][j];
		}
	}
}

/************************************************************************/

/* da _imaus a imr, img, imb; ritorna 1 se c'e' stato overflow dal RANGE_VALORI
   (512), 0 altrimenti */

static short trasforma_blocco_col(void)
{
	short i, j, k=0, prec = 0, ret = 0, lb4 = _lb2 << 1;

	for(i=0; i<_lb; i++) {
		for(j=0; j<_lb*4; j+=4) {
			_im[k++] = (short) ((int) _imaus_fb[i][j] - (int) prec);
			prec = (short) _imaus_fb[i][j];
		}
	}
	for(i=0; i<_lb; i++) {
		for(j=1; j<_lb*4; j+=4) {
			_im[k++] = (short) ((int) _imaus_fb[i][j] - (int) prec);
			prec = (short) _imaus_fb[i][j];
		}
	}
	for(i=0; i<_lb; i++) {
		for(j=2; j<_lb*4; j+=4) {
			_im[k++] = (short) ((int) _imaus_fb[i][j] - (int) prec);
			prec = (short) _imaus_fb[i][j];
		}
	}

	for(i=0; i<_lb2; i++) {
		_im[i+_lb2] -= _im[i];
		if(_im[i+_lb2] < -255 || _im[i+_lb2] > 255)
			ret = 1; 
		_im[i+lb4] -= _im[i];
		if(_im[i+lb4] < -255 || _im[i+lb4] > 255)
			ret = 1; 
	}
	return ret;
}

/************************************************************************/

/* a partire da _im */

static void calcola_statistica(void)
{
	short i, *aus_stat = _statistica + RANGE_VALORI / 2;
                                     /* per tener conto dei valori negativi */
	for(i=0; i<_lb2; i++)
		(*(aus_stat + _im[i])) ++;
}

/************************************************************************/

/* a partire da _imr, _img, _imb */

static void calcola_statistica_col(void)
{
	short i, *aus_stat = _statistica + RANGE_VALORI / 2;
                                     /* per tener conto dei valori negativi */
	for(i=0; i<_lb2*3; i++)
		(*(aus_stat + _im[i])) ++;
}

/*******************************************************************/

/* routine per la codifica dei valori del blocco all'interno del vettore
   _vet_valori */

static void
codifica_blocco_lossless(
	short numero_codice_da_usare
)
{
	short j;
	short *aus_im = _im;
	short valore;
	short offset = RANGE_VALORI * numero_codice_da_usare + RANGE_VALORI / 2;
	unsigned char *aus_numero_bit_codice = _vet_numero_bit_codice + offset;
	unsigned short *aus_codice_valori    = _vet_codice_valori     + offset;

/* scrivo il numero del codice da usare */

	scrivi_fino_a_16_bit_utili((unsigned char) 8, numero_codice_da_usare);

/* scrivo _lb2 valori */

	for(j=_lb2; j>0; j--) {
		valore = *aus_im++;
		scrivi_fino_a_16_bit_utili(*(aus_numero_bit_codice + valore),
		                           *(aus_codice_valori + valore));
	}
}

/*******************************************************************/

/* routine per la codifica dei valori del blocco all'interno del vettore
   _vet_valori */

static void
codifica_blocco_lossless_col(
	short numero_codice_da_usare /* se vale 100 devo gestire gli overflow */
)
{
	short j, k=0;
	short *aus_im = _im;
	short valore;
	short offset = RANGE_VALORI * numero_codice_da_usare + RANGE_VALORI / 2;
	unsigned char *aus_numero_bit_codice = _vet_numero_bit_codice + offset;
	unsigned short *aus_codice_valori    = _vet_codice_valori     + offset;

/* scrivo il numero del codice da usare */

	scrivi_fino_a_16_bit_utili((unsigned char) 8, numero_codice_da_usare);

/* scrivo 3*_lb2 valori */

	if(numero_codice_da_usare == 100) {
		for(j=3*_lb2; j>0; j--) {
			valore = *aus_im++;
			if(valore >= -7 && valore <= 7) {
				scrivi_fino_a_16_bit_utili(4, (unsigned short) (valore+8));
			} else {
				scrivi_fino_a_16_bit_utili(4, 0);
				scrivi_fino_a_16_bit_utili(10, (unsigned short) (valore+512));
			}
		}
	} else {
		for(j=3*_lb2; j>0; j--) {
			valore = *aus_im++;
			scrivi_fino_a_16_bit_utili(*(aus_numero_bit_codice + valore),
			                           *(aus_codice_valori + valore));
		}
	}
}

