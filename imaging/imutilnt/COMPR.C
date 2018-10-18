
/* routine pubbliche per la compressione/decompressione:

	imut_rpi_DIAC
	imut_pic_DIAC
	imut_rpi_lossless
	imut_pic_lossless

	versione attuale: 7.62 28 febbraio 2002
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <imutilnt.h>

/* prototipi di funzioni private */
static short calcola_letture_ottime(short n_blocchi, long *dim_letture);

/* prototipi di funzioni di comp_s.c e uncomp_s.c */
void cmp_lossless_block(
	short fb, unsigned char *ptcdata,
	short ii, short jj, short *dim_blo_com
);
void expandDIACblock(
	short fb, unsigned char *ptcdata, short ii, short jj,
	short offset_x, short offset_y
);
short expand_lossless_block(
	short fb, unsigned char *ptcdata, short ii, short jj,
	short offset_x, short offset_y
);
short init_DIAC_uncomp(CMPIMAGE *header);
void clean_DIAC_uncomp(void);

extern short *_dim_blo_com;
extern char *_out_buf;
extern char *_in_buf;
extern long *_dim_letture;
extern CMPIMAGE *_header[]; /* max 30 diverse inizializzazioni */
extern short _init_DIAC_done;

/**************************************************************************/

/* ritorna il puntatore all'header relativo al fb; se la imut_init_DIAC_memory
   non e' ancora stata fatta ritorna NULL */

CMPIMAGE *imut_get_DIAC_header(short fb) // numero della tv che corrisponde all'header
{
	if(!_init_DIAC_done)
		return NULL;
	return _header[fb];
}

/**************************************************************************/

/* ritorna il numero di byte messi nel buffer oppure 0 se qualcosa va storto
   o se si eccede la dimensione massima passata */
unsigned long
imut_compr_buf_DIAC(
	short fb, // numero della tv che corrisponde all'header
	char *buf_out,
	unsigned long max_buf_out_len
)
{
	short i, ii, jj;
	char *aus_buf_out;
	unsigned long n_byte_compressi;
	short n_blocchi;

	if(!_init_DIAC_done)
		return 0L;

	/* scrittura della struttura e delle lunghezze dei blocchi compressi */
	n_blocchi = _header[fb]->hor_blocks * _header[fb]->ver_blocks;

	/* compressione */
	aus_buf_out = buf_out + n_blocchi * sizeof(short);
	for(ii=0, i=0; ii<_header[fb]->ver_blocks; ii++) {
		for(jj=0; jj<_header[fb]->hor_blocks; jj++, i++) {
			imut_cmpDIACblock(fb, aus_buf_out, ii, jj, _dim_blo_com+i);
			aus_buf_out += _dim_blo_com[i];
			n_byte_compressi = aus_buf_out - buf_out;
			if(n_byte_compressi > max_buf_out_len - 1000) /* di sicurezza */
				return 0L;
		}
	}

	/* copio il vettore delle lunghezze dei blocchi compressi */
	memcpy(buf_out, _dim_blo_com, (size_t) (n_blocchi * sizeof(short)));

	return n_byte_compressi;
}

/**************************************************************************/

short
imut_rpi_DIAC(
	ROI *roi,
	wchar_t *file_name,
	short fd, /* fattore di degrado */
	short flag_col,
	double *comp_ratio,
	double *bit_per_pixel,
	unsigned long *n_byte_file
)
{
//#define TESTV
	short i, ret, lb=64, ii, jj, n_blocchi, fb=roi->fb;
	int fout;
	CMPIMAGE header;
	char *aus_buf_out;
	long n_byte_disponibili;

	/* inizializzazione: allocazione */
	if((ret = imut_init_DIAC_memory(fd, flag_col, lb)) < 0)
		return ret;

	/* inizializzazione dell'header */
	if((ret = imut_init_DIAC_header(roi, &header)) < 0)
		return ret;

#ifndef TESTV
	/* apertura del file */
	fout = _wopen(
		file_name, O_BINARY | O_WRONLY | O_CREAT | O_TRUNC, S_IREAD | S_IWRITE
	);
	if(fout < 0)
		return SEAV_ERR_FILE;

	/* scrittura della struttura e delle lunghezze dei blocchi compressi */
	n_blocchi = header.hor_blocks * header.ver_blocks;
	_write(fout, (char *) &header, sizeof(CMPIMAGE));
	_write(fout, (char *) _dim_blo_com, (size_t) (n_blocchi * sizeof(short)));
#endif

	/* compressione */
	n_byte_disponibili = DIAC_BUF_SIZE;
	aus_buf_out = _out_buf;
	for(ii=0, i=0; ii<header.ver_blocks; ii++) {
		for(jj=0; jj<header.hor_blocks; jj++, i++) {
			imut_cmpDIACblock(fb, aus_buf_out, ii, jj, _dim_blo_com+i);
			n_byte_disponibili -= (long) _dim_blo_com[i];
			aus_buf_out += _dim_blo_com[i];
			if(n_byte_disponibili < DIAC_BUF_SIZE>>1) {
				/* eventualmente scrivo su disco; ovviamente out_buf deve essere
				   sufficientemente grande per contenere almeno un blocco */
#ifndef TESTV
				_write(fout, (char *) _out_buf, (size_t) (DIAC_BUF_SIZE - n_byte_disponibili));
#endif
				n_byte_disponibili = DIAC_BUF_SIZE;
				aus_buf_out = _out_buf;
			}
		}
	}

	/* flush su file */
#ifndef TESTV
	_write(fout, (char *) _out_buf, (size_t)(DIAC_BUF_SIZE-n_byte_disponibili));

	/* nuova scrittura della struttura e delle lunghezze dei blocchi compressi */
	_lseek(fout, 0L, SEEK_SET);
	_write(fout, (char *) &header, sizeof(CMPIMAGE));
	_write(fout, (char *) _dim_blo_com, (size_t) (n_blocchi * sizeof(short)));
	_flushall();
	_commit(fout);
	_close(fout);
#endif

	/* calcolo di alcune informazioni */
	if(comp_ratio)
		*comp_ratio = ((double) header.width * (double) header.height *
		              (double) header.pixdepth) / (double) header.cpdatasize;
	if(bit_per_pixel)
		*bit_per_pixel = ((double) header.cpdatasize * 8.0)/
						((double) header.width * (double) header.height);
	if(n_byte_file)
		*n_byte_file = header.cpdatasize;
#if 0
	printf("\nn. of bytes of file %s: %ld  (%.2lf %%, %.2lf bits per pixel)\n",
	        file_name, header.cpdatasize, 100./(*comp_ratio), *bit_per_pixel);
	printf("compression ratio: %.2lf\n\n", *comp_ratio);
#endif

	/* clean-up */
	imut_clean_DIAC_comp();
	return SEAV_OK_ALL;
}

/***************************************************************************/

short
imut_pic_DIAC(
	wchar_t *file_name,
	short fb,
	short offset_x, /* se vale -1 l'immagine viene scritta a partire dalla ascissa 0 */
	short offset_y /* se vale -1 l'immagine viene scritta a partire dalla ordinata 0 */
)
{
	short n_letture, i, ii, jj, n_blocchi, ret;
	int fin;
	CMPIMAGE header;
	char *aus_buf_in;
	long n_disponibili;

	/* apertura del file */
	fin = _wopen(file_name, O_BINARY | O_RDONLY);
	if(fin < 0)
		return SEAV_ERR_FILE;

	/* lettura della struttura di header */
	_read(fin, (char *) &header, sizeof(CMPIMAGE));
	n_blocchi = header.hor_blocks * header.ver_blocks;

	// inizializzazione
	ret = init_DIAC_uncomp(&header);
	if(ret != SEAV_OK_ALL)
		return ret;

	/* lettura delle lunghezze dei blocchi compressi */
	_read(fin, (char *) _dim_blo_com, (size_t) (n_blocchi * sizeof(short)));

	/* calcolo il numero ottimo di letture da fare dal file nel vettore _in_buf */
	n_letture = calcola_letture_ottime(n_blocchi, _dim_letture);

	/* decompressione */
	n_letture = 0;
	n_disponibili = 0L;
	for(ii=0, i=0; ii<header.ver_blocks; ii++) {
		for(jj=0; jj<header.hor_blocks; jj++, i++) {
			if(!n_disponibili) {
				n_disponibili = _dim_letture[n_letture++];
				_read(fin, (char *) _in_buf, (size_t) n_disponibili);
				aus_buf_in = _in_buf;
			}
			expandDIACblock(fb, aus_buf_in, ii, jj, offset_x, offset_y);
			n_disponibili -= (long) _dim_blo_com[i];
			aus_buf_in += _dim_blo_com[i];
		}
	}
	_close(fin);

	/* clean-up */
	clean_DIAC_uncomp();
	return SEAV_OK_ALL;
}

/*******************************************************************/

/* calcolo il numero ottimo di letture da fare dal file nel vettore vet_valori,
   la dimensione di ciascuna di esse e ritorna il numero di letture */

static short
calcola_letture_ottime(
	short n_blocchi,
	long *dim_letture
)
{
	long acc = 0L;
	short n_letture = 0, i;

	for(i=0; i<n_blocchi; i++) {
		acc += (long) _dim_blo_com[i];
		if(acc > DIAC_BUF_SIZE) {
			dim_letture[n_letture++] = acc - (long) _dim_blo_com[i];
			acc = (long) _dim_blo_com[i];
			if(n_letture >= DIM_DIM_L)
				return SEAV_ERR_MEMORY;
		}
	}
	dim_letture[n_letture++] = acc;

	return n_letture;
}

/************************************************************************/

short
imut_rpi_lossless(
	ROI *roi,
	wchar_t *file_name,
	short flag_col,
	double *comp_ratio,
	double *bit_per_pixel,
	unsigned long *n_byte_file
)
{
	short i, ret, lb=64, ii, jj, n_blocchi, fb=roi->fb;
	int fout;
	CMPIMAGE header;
	char *aus_buf_out;
	long n_byte_disponibili;

	/* inizializzazione: allocazione */
	if((ret = imut_init_DIAC_memory(-1/*fd*/, flag_col, lb)) < 0)
		return ret;

	/* inizializzazione dell'header */
	if((ret = imut_init_DIAC_header(roi, &header)) < 0)
		return ret;

	/* apertura del file */
	fout = _wopen(
		file_name, O_BINARY | O_WRONLY | O_CREAT | O_TRUNC, S_IREAD | S_IWRITE
	);
	if(fout < 0)
		return SEAV_ERR_FILE;

	/* scrittura della struttura e delle lunghezze dei blocchi compressi */
	n_blocchi = header.hor_blocks * header.ver_blocks;
	_write(fout, (char *) &header, sizeof(CMPIMAGE));
	_write(fout, (char *) _dim_blo_com, (size_t) (n_blocchi * sizeof(short)));

	/* compressione */
	n_byte_disponibili = DIAC_BUF_SIZE;
	aus_buf_out = _out_buf;
	for(ii=0, i=0; ii<header.ver_blocks; ii++) {
		for(jj=0; jj<header.hor_blocks; jj++, i++) {
			cmp_lossless_block(fb, aus_buf_out, ii, jj, _dim_blo_com+i);
			n_byte_disponibili -= (long) _dim_blo_com[i];
			aus_buf_out += _dim_blo_com[i];
			if(n_byte_disponibili < DIAC_BUF_SIZE>>1) {
				/* eventualmente scrivo su disco; ovviamente out_buf deve essere
				   sufficientemente grande per contenere almeno un blocco */
				_write(fout, (char *) _out_buf, (size_t) (DIAC_BUF_SIZE - n_byte_disponibili));
				n_byte_disponibili = DIAC_BUF_SIZE;
				aus_buf_out = _out_buf;
			}
		}
	}

	/* flush su file */
	_write(fout, (char *) _out_buf, (size_t)(DIAC_BUF_SIZE-n_byte_disponibili));

	/* nuova scrittura della struttura e delle lunghezze dei blocchi compressi */
	_lseek(fout, 0L, SEEK_SET);
	_write(fout, (char *) &header, sizeof(CMPIMAGE));
	_write(fout, (char *) _dim_blo_com, (size_t) (n_blocchi * sizeof(short)));
	_flushall();
	_commit(fout);
	_close(fout);

	/* calcolo di alcune informazioni */
	if(comp_ratio)
		*comp_ratio = ((double) header.width * (double) header.height *
		              (double) header.pixdepth) / (double) header.cpdatasize;
	if(bit_per_pixel)
		*bit_per_pixel = ((double) header.cpdatasize * 8.0)/
						((double) header.width * (double) header.height);
	if(n_byte_file)
		*n_byte_file = header.cpdatasize;
#if 0
	printf("\nn. of bytes of file %s: %ld  (%.2lf %%, %.2lf bits per pixel)\n",
	        file_name, header.cpdatasize, 100./(*comp_ratio), *bit_per_pixel);
	printf("compression ratio: %.2lf\n\n", *comp_ratio);
#endif

	/* clean-up */
	imut_clean_DIAC_comp();
	return SEAV_OK_ALL;
}

/************************************************************************/

short
imut_pic_lossless(
	wchar_t *file_name,
	short fb,
	short offset_x, /* se vale -1 l'immagine viene scritta a partire dalla ascissa 0 */
	short offset_y /* se vale -1 l'immagine viene scritta a partire dalla ordinata 0 */
)
{
	short n_letture, i, ii, jj, n_blocchi, ret;
	int fin;
	CMPIMAGE header;
	char *aus_buf_in;
	long n_disponibili;

	/* apertura del file */
	fin = _wopen(file_name, O_BINARY | O_RDONLY);
	if(fin < 0)
		return SEAV_ERR_FILE;

	/* lettura della struttura di header */
	_read(fin, (char *) &header, sizeof(CMPIMAGE));
	if(header.DIACheader[0] != 'l' || header.DIACheader[1] != '1')
		return SEAV_ERR_FILE;
	n_blocchi = header.hor_blocks * header.ver_blocks;

	// inizializzazione
	ret = init_DIAC_uncomp(&header);
	if(ret != SEAV_OK_ALL)
		return ret;

	/* lettura delle lunghezze dei blocchi compressi */
	_read(fin, (char *) _dim_blo_com, (size_t) (n_blocchi * sizeof(short)));

	/* calcolo il numero ottimo di letture da fare dal file nel vettore _in_buf */
	n_letture = calcola_letture_ottime(n_blocchi, _dim_letture);

	n_letture = 0;
	n_disponibili = 0L;
	for(ii=0, i=0; ii<header.ver_blocks; ii++) {
		for(jj=0; jj<header.hor_blocks; jj++, i++) {
			if(!n_disponibili) {
				n_disponibili = _dim_letture[n_letture++];
				_read(fin, (char *) _in_buf, (size_t) n_disponibili);
				aus_buf_in = _in_buf;
			}
			expand_lossless_block(fb, aus_buf_in, ii, jj, offset_x, offset_y);
			n_disponibili -= (long) _dim_blo_com[i];
			aus_buf_in += _dim_blo_com[i];
		}
	}
	_close(fin);

	return SEAV_OK_ALL;
}

