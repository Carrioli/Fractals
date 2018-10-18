


/***********************************************************************

;  Nome:    
     imut_rpi
;
;  Funzione:
     memorizza su file una immagine con header
;
;  Formato:
     short imut_rpi(roi, file_name)
;
;  Argomenti:    
     ROI *roi         puntatore alla struttura che descrive l'area di interesse
     char *file_name  nome del file in cui memorizzare l'immagine

;
;  Valore di ritorno: 
     SEAV_OK_ALL              nessun errore
	 SEAV_ERR_*               codice di errore

;
;  Descrizione:
     {\bf imut_rpi}() memorizza nel file specificato la regione di interesse
	 letta dalla memoria video; i valori dell'immagine sono memorizzati
	 per righe all'interno del file e sono preceduti da un header di
	 lunghezza fissa (1024 byte); la prima parte dell'header contiene
	 una struttura PVS_HDR con i parametri significativi dell'immagine
	 (vedi im_util.h); la seconda parte dell'header e' vuota a disposizione
	 dell'utente per la memorizzazione di informazioni aggiuntive;

;
;**********************************************************************/

#include <io.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <imutilnt.h>

#ifndef O_BINARY
	#define O_BINARY 0
#endif

#ifdef __WATCOMC__
	#define _write write
#endif

static short internal_rpi(ROI *roi, int fout);

/**************************************************************************/

static short internal_rpi(ROI *roi, int fout)
{
	PVS_HDR hout;
	short yi, yf, i, xl, yl, n_righe;
	short n_scritture, ultime;
	unsigned char *buffer;
	PARAM_SCHEDA param;

	/* controllo della ROI */
	sea2_init_config(&param);
	if(imut_check_roi(roi, &param) != SEAV_OK_ALL)
		return SEAV_ERR_PARAMETER;
	xl = roi->xf - roi->xi + 1;
	yl = roi->yf - roi->yi + 1;
	n_righe = (short) (DIM_MAX_BUF / (long) xl);

	/* allocazione di memoria per il buffer */
	buffer = (unsigned char *) malloc((size_t) n_righe * xl * sizeof(unsigned char));
	if(buffer == NULL)
		return SEAV_ERR_MEMORY;

	/* scrittura dell'header */
	hout.hdr = HDR_MAGIC;
	hout.xsize = (long) xl;
	hout.ysize = (long) yl;
	hout.zsize = 8L;
	hout.fmt = 1L;
	time((time_t *) hout.crdate);
	hout.nchan = 1L;
	hout.title[0] = 0;
	_write(fout, (char *) &hout, 744);
	_write(fout, "\000", 1);
	_write(fout, (char *) &hout, 279);
	_lseek(fout, 1024L, SEEK_SET);

	/* selezione del buffer */
	sea2_seleziona_buffer(roi->fb);

	/* memorizzazione */
	n_scritture = yl / n_righe;
	ultime = yl % n_righe;
	for(i=0, yi=roi->yi; i<n_scritture; i++, yi+=n_righe) {
		yf = yi + n_righe - 1;
		sea2_leggi_finestra(roi->xi, yi, roi->xf, yf, buffer);
		(void) _write(fout, (char *) buffer, (size_t) xl * (size_t) n_righe);
	}
	if(ultime) {
		yf = yi + ultime - 1;
		sea2_leggi_finestra(roi->xi, yi, roi->xf, yf, buffer);
		(void) _write(fout, (char *) buffer, (size_t) xl * (size_t) ultime);
	}

	/* uscita */
	_flushall();
	_commit(fout);
	free(buffer);

	return SEAV_OK_ALL;
}

/**************************************************************************/

/* versione che accetta una stringa unicode */

short
imut_rpi(
	ROI *roi,
	wchar_t *file_name
)
{
	int fout;
	short ret;

	/* apertura del file d'uscita */
	if((fout = _wopen(
		file_name, O_BINARY|O_CREAT|O_TRUNC|O_WRONLY,S_IREAD|S_IWRITE)) < 0
	)
		return SEAV_ERR_IMG_FILE_SAVE;

	/* scrittura del file */
	ret = internal_rpi(roi, fout);

	/* uscita */
	_close(fout);
	return ret;
}

/**************************************************************************/

/* versione che accetta una stringa di char */

short
imut_rpi_char(
	ROI *roi,
	char *file_name
)
{
	int fout;
	short ret;

	/* apertura del file d'uscita */
	if((fout = _open(
		file_name, O_BINARY|O_CREAT|O_TRUNC|O_WRONLY,S_IREAD|S_IWRITE)) < 0
	)
		return SEAV_ERR_IMG_FILE_SAVE;

	/* scrittura del file */
	ret = internal_rpi(roi, fout);

	/* uscita */
	_close(fout);
	return ret;
}

/**************************************************************************/

short
imut_rpi_col(
	ROI *roi,
	wchar_t *file_name
)
{
	int fout;
	PVS_HDR hout;
	short yi, yf, i, xl, yl, n_righe;
	short n_scritture, ultime;
	unsigned char *buffer;
	PARAM_SCHEDA param;

/* controllo della ROI */

	sea2_init_config(&param);
	if(imut_check_roi(roi, &param) != SEAV_OK_ALL)
		return SEAV_ERR_PARAMETER;
	xl = roi->xf - roi->xi + 1;
	yl = roi->yf - roi->yi + 1;
	n_righe = (short) (DIM_MAX_BUF / (long) (xl*3));

/* allocazione di memoria per il buffer */

	buffer = (unsigned char *) malloc
		((size_t) n_righe * (xl*3) * sizeof(unsigned char)
	);
	if(buffer == NULL)
		return SEAV_ERR_MEMORY;

/* apertura del file d'uscita e scrittura dell'header */

	if((fout = _wopen(
		file_name, O_BINARY|O_CREAT|O_TRUNC|O_WRONLY,S_IREAD|S_IWRITE)) < 0
	) {
		free(buffer);
		return SEAV_ERR_IMG_FILE_SAVE;
	}
	hout.hdr = HDR_MAGIC;
	hout.xsize = (long) xl;
	hout.ysize = (long) yl;
	hout.zsize = 8L;
	hout.fmt = 1L;
	time((time_t *) hout.crdate);
	hout.nchan = 3L;
	hout.title[0] = 0;
	_write(fout, (char *) &hout, 744);
	_write(fout, "\000", 1);
	_write(fout, (char *) &hout, 279);
	_lseek(fout, 1024L, SEEK_SET);

/* selezione del buffer */

	sea2_seleziona_buffer(roi->fb);

/* memorizzazione */

	n_scritture = yl / n_righe;
	ultime = yl % n_righe;
	for(i=0, yi=roi->yi; i<n_scritture; i++, yi+=n_righe) {
		yf = yi + n_righe - 1;
		sea2_leggi_finestra_col24(roi->xi, yi, roi->xf, yf, buffer);
		(void) _write(fout, (char *) buffer, (size_t) (xl*3) * (size_t) n_righe);
	}
	if(ultime) {
		yf = yi + ultime - 1;
		sea2_leggi_finestra_col24(roi->xi, yi, roi->xf, yf, buffer);
		(void) _write(fout, (char *) buffer, (size_t) (xl*3) * (size_t) ultime);
	}

/* uscita */

	_flushall();
	_commit(fout);
	_close(fout);
	free(buffer);
	return SEAV_OK_ALL;
}
