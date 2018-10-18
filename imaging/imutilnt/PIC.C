

/***********************************************************************

;  Nome:    
     imut_pic
;
;  Funzione:
     presenta su memoria video una immagine con header letta da file
;
;  Formato:
     short imut_pic(file_name, fb, xi, yi)
;
;  Argomenti:    
     char *file_name  nome del file in cui e' memorizzata l'immagine
	 short fb         frame buffer
	 short xi         ascissa del pixel a partire dal quale caricare l'immagine
	 short yi         ordinata del pixel a partire dal quale caricare l'immagine

;
;  Valore di ritorno: 
     SEAV_OK_ALL              nessun errore
	 SEAV_ERR_*               codice di errore

;
;  Descrizione:
     {\bf imut_pic}() legge dal file specificato una immagine con header
	 e la carica nel frame buffer "fb" della memoria video a partire dalle
	 coordinate ("xi", "yi");

;
;**********************************************************************/

#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <imutilnt.h>

#ifndef O_BINARY
	#define O_BINARY 0
#endif

#ifdef __WATCOMC__
	#define _read read
#endif

static short internal_pic(int fin, short fb, short xi, short yi);

/**********************************************************************/
/* versione che accetta una stringa unicode */

short
imut_pic(
	wchar_t *file_name,
	short fb,
	short xi,
	short yi
)
{
	int fin;
	short ret;

	/* apertura del file di ingresso */
	if((fin = _wopen(file_name, O_BINARY | O_RDONLY)) < 0)
		return SEAV_ERR_IMG_FILE_LOAD;

	/* lettura del file */
	ret = internal_pic(fin, fb, xi, yi);

	/* uscita */
	_close(fin);
	return ret;
}

/**********************************************************************/
/* versione che accetta una stringa di char */

short
imut_pic_char(
	char *file_name,
	short fb,
	short xi,
	short yi
)
{
	int fin;
	short ret;

	/* apertura del file di ingresso */
	if((fin = _open(file_name, O_BINARY | O_RDONLY)) < 0)
		return SEAV_ERR_IMG_FILE_LOAD;

	/* lettura del file */
	ret = internal_pic(fin, fb, xi, yi);

	/* uscita */
	_close(fin);
	return ret;
}

/**********************************************************************/

static short
internal_pic(
	int fin,
	short fb,
	short xi,
	short yi
)
{
	PVS_HDR hin;
	short i, xl, yl, xf, yf, n_righe, ii, n_letture, ultime;
	unsigned char *buffer;
	PARAM_SCHEDA param;
	long offl;

	_read(fin, (char *) &hin, sizeof(PVS_HDR));
	if(hin.hdr != HDR_MAGIC || hin.nchan != 1L) {
		_close(fin);
		return SEAV_ERR_IMG_FILE_LOAD;
	}
	_lseek(fin, 1024L, 0);
	xl = (short) hin.xsize;
	yl = (short) hin.ysize;
	xf = xi + xl - 1;

/* allocazione di memoria per il buffer */

	n_righe = (short) (DIM_MAX_BUF / (long) xl);
	buffer = (unsigned char *) malloc
		((size_t) n_righe * xl * sizeof(unsigned char)
	);
	if(buffer == NULL) {
		_close(fin);
		return SEAV_ERR_MEMORY;
	}

/* selezione del buffer */

	sea2_seleziona_buffer(fb);

/* lettura del file e scaricamento su video */

	sea2_init_config(&param);
	n_letture = yl / n_righe;
	ultime = yl % n_righe;
	for(i=0; i<n_letture; i++, yi+=n_righe) {
		yf = yi + n_righe - 1;
		(void) _read(fin, (char *) buffer, (size_t) xl * (size_t) n_righe);
		for(ii=yi, offl=0L; ii<=yf; ii++, offl+=(long)xl) {
			if(ii>=param.y_min && ii<=param.y_max) {
				if(xi >= param.x_min) {
					if(xf <= param.x_max) {
						sea2_scrivi_riga(xi, ii, xl, buffer+offl);
					} else {
						sea2_scrivi_riga(xi, ii, (short) (xl-xf+param.x_max), buffer+offl);
					}
				} else {
					if(xf <= param.x_max) {
						sea2_scrivi_riga(
							param.x_min, ii, (short) (xl+xi-param.x_min),
							buffer+offl-xi+param.x_min
						);
					} else {
						sea2_scrivi_riga(
							param.x_min, ii, (short) (xl+xi-param.x_min-xf+param.x_max),
							buffer+offl-xi+param.x_min
						);
					}
				}
			}
		}
	}
	if(ultime) {
		yf = yi + ultime - 1;
		(void) _read(fin, (char *) buffer, (size_t) xl * (size_t) ultime);
		for(ii=yi, offl=0L; ii<=yf; ii++, offl+=(long)xl) {
			if(ii>=param.y_min && ii<=param.y_max) {
				if(xi >= param.x_min) {
					if(xf <= param.x_max) {
						sea2_scrivi_riga(xi, ii, xl, buffer+offl);
					} else {
						sea2_scrivi_riga(xi, ii, (short) (xl-xf+param.x_max), buffer+offl);
					}
				} else {
					if(xf <= param.x_max) {
						sea2_scrivi_riga(
							param.x_min, ii, (short) (xl+xi-param.x_min),
							buffer+offl-xi+param.x_min
						);
					} else {
						sea2_scrivi_riga(
							param.x_min, ii, (short) (xl+xi-param.x_min-xf+param.x_max),
							buffer+offl-xi+param.x_min
						);
					}
				}
			}
		}
	}
	free(buffer);
	return SEAV_OK_ALL;
}

/**********************************************************************/

short
imut_pic_col(
	wchar_t *file_name,
	short fb,
	short xi,
	short yi
)
{
	int fin;
	PVS_HDR hin;
	short i, xl, yl, xf, yf, n_righe, ii, n_letture, ultime, x_size;
	unsigned char *buffer;
	PARAM_SCHEDA param;
	long offl;

/* apertura del file di ingresso */

	if((fin = _wopen(file_name, O_BINARY | O_RDONLY)) < 0)
		return SEAV_ERR_IMG_FILE_LOAD;
	_read(fin, (char *) &hin, sizeof(PVS_HDR));
	if(hin.hdr != HDR_MAGIC || hin.nchan != 3L) {
		_close(fin);
		return SEAV_ERR_IMG_FILE_LOAD;
	}
	_lseek(fin, 1024L, 0);
	xl = (short) hin.xsize;
	yl = (short) hin.ysize;
	xf = xi + xl - 1;
	x_size = (short) (xl * hin.nchan);

/* allocazione di memoria per il buffer */

	n_righe = (short) (DIM_MAX_BUF / (long) x_size);
	buffer = (unsigned char *) malloc
		((size_t) n_righe * x_size * sizeof(unsigned char)
	);
	if(buffer == NULL) {
		_close(fin);
		return SEAV_ERR_MEMORY;
	}

/* selezione del buffer */

	sea2_seleziona_buffer(fb);

/* lettura del file e scaricamento su video */

	sea2_init_config(&param);
	n_letture = yl / n_righe;
	ultime = yl % n_righe;
	for(i=0; i<n_letture; i++, yi+=n_righe) {
		yf = yi + n_righe - 1;
		(void) _read(fin, (char *) buffer, (size_t) x_size * (size_t) n_righe);
		for(ii=yi, offl=0L; ii<=yf; ii++, offl+=(long)x_size) {
			if(ii>=param.y_min && ii<=param.y_max) {
				if(xi >= param.x_min) {
					if(xf <= param.x_max) {
						sea2_scrivi_riga_col24(xi, ii, xl, buffer+offl);
					} else {
						sea2_scrivi_riga_col24(xi, ii, (short) (xl-xf+param.x_max), buffer+offl);
					}
				} else {
					if(xf <= param.x_max) {
						sea2_scrivi_riga_col24(
							param.x_min, ii, (short) (xl+xi-param.x_min),
							buffer+offl-xi+param.x_min
						);
					} else {
						sea2_scrivi_riga_col24(
							param.x_min, ii,
							(short) (xl+xi-param.x_min-xf+param.x_max),
							buffer+offl-xi+param.x_min
						);
					}
				}
			}
		}
	}
	if(ultime) {
		yf = yi + ultime - 1;
		(void) _read(fin, (char *) buffer, (size_t) x_size * (size_t) ultime);
		for(ii=yi, offl=0L; ii<=yf; ii++, offl+=(long)x_size) {
			if(ii>=param.y_min && ii<=param.y_max) {
				if(xi >= param.x_min) {
					if(xf <= param.x_max) {
						sea2_scrivi_riga_col24(xi, ii, xl, buffer+offl);
					} else {
						sea2_scrivi_riga_col24(
							xi, ii, (short) (xl-xf+param.x_max), buffer+offl
						);
					}
				} else {
					if(xf <= param.x_max) {
						sea2_scrivi_riga_col24(
							param.x_min, ii, (short) (xl+xi-param.x_min),
							buffer+offl-xi+param.x_min
						);
					} else {
						sea2_scrivi_riga_col24(
							param.x_min, ii,
							(short) (xl+xi-param.x_min-xf+param.x_max),
							buffer+offl-xi+param.x_min
						);
					}
				}
			}
		}
	}

/* uscita */

	_close(fin);
	free(buffer);
	return SEAV_OK_ALL;
}

/**********************************************************************/

// harle 7.64: file bmp a 8 bit
// modifica 7.74 per leggere anche le righe di lunghezza non multipla di 4
// che normalmente sono paddate a 32 bit

short
imut_pic_bmp(
	wchar_t *file_name,
	short fb,
	short xi,
	short yi
)
{
	int i, xl, yl, xf, yf, n_byte_riga;
	unsigned char *buffer;
	PARAM_SCHEDA param;
	FILE *fp;
	BITMAPFILEHEADER file_header;
	BITMAPINFOHEADER bmp_info_header;
	wchar_t aus[4];

	// apertura del file di ingresso
	aus[0] = 'r';
	aus[1] = 'b';
	aus[2] = 0;
	fp = _wfopen(file_name, aus);
	if(fp == NULL)
		return SEAV_ERR_IMG_FILE_LOAD;

	// lettura degli header
	fread(&file_header, 1, sizeof(BITMAPFILEHEADER), fp);
	fread(&bmp_info_header, 1, sizeof(BITMAPINFOHEADER), fp);
	if(bmp_info_header.biBitCount != 8)
		return SEAV_ERR_IMG_FILE_LOAD;
	if(bmp_info_header.biClrUsed != 256 && bmp_info_header.biClrUsed != 0) // 7.70-3
		return SEAV_ERR_IMG_FILE_LOAD;

	// seek al posto giusto: ignoro la palette
	fseek(fp, file_header.bfOffBits, SEEK_SET);
	xl = bmp_info_header.biWidth;
	yl = bmp_info_header.biHeight;
	xf = xi + xl - 1;
	yf = yi + yl - 1;
	n_byte_riga = (file_header.bfSize - file_header.bfOffBits) / bmp_info_header.biHeight;

	// allocazione di memoria per il buffer */
	buffer = (unsigned char *) malloc((size_t) n_byte_riga * sizeof(unsigned char));
	if(buffer == NULL) {
		fclose(fp);
		return SEAV_ERR_MEMORY;
	}

	// selezione del buffer
	sea2_seleziona_buffer(fb);

	// lettura del file e scaricamento su video una riga per volta;
	// la bitmap e' memorizzata bottom up
	sea2_init_config(&param);
	for(i=yf; i>=yi; i--) {
		fread(buffer, 1, n_byte_riga, fp);
		if(i>=param.y_min && i<=param.y_max) {
			if(xi >= param.x_min) {
				if(xf <= param.x_max) {
					sea2_scrivi_riga(xi, (short) i, (short) xl, buffer);
				} else {
					sea2_scrivi_riga(xi, (short) i, (short) (xl-xf+param.x_max), buffer);
				}
			} else {
				if(xf <= param.x_max) {
					sea2_scrivi_riga(
						param.x_min, (short) i, (short) (xl+xi-param.x_min),
						buffer-xi+param.x_min
					);
				} else {
					sea2_scrivi_riga(
						param.x_min, (short) i, (short) (xl+xi-param.x_min-xf+param.x_max),
						buffer-xi+param.x_min
					);
				}
			}
		}
	}

	/* uscita */
	free(buffer);
	fclose(fp);
	return SEAV_OK_ALL;
}

/**********************************************************************/

// harle 7.64: file bmp a 8 bit
// modifica 7.74 per leggere anche le righe di lunghezza non multipla di 4
// che normalmente sono paddate a 32 bit

short
imut_pic_col_bmp(
	wchar_t *file_name,
	short fb,
	short xi,
	short yi
)
{
	int i, xl, yl, xf, yf, n_byte_riga;
	unsigned char *buffer;
	PARAM_SCHEDA param;
	FILE *fp;
	BITMAPFILEHEADER file_header;
	BITMAPINFOHEADER bmp_info_header;
	wchar_t aus[4];

	// apertura del file di ingresso
	aus[0] = 'r';
	aus[1] = 'b';
	aus[2] = 0;
	fp = _wfopen(file_name, aus);
	if(fp == NULL)
		return SEAV_ERR_IMG_FILE_LOAD;

	// lettura degli header
	fread(&file_header, 1, sizeof(BITMAPFILEHEADER), fp);
	fread(&bmp_info_header, 1, sizeof(BITMAPINFOHEADER), fp);
	if(bmp_info_header.biBitCount != 24)
		return SEAV_ERR_IMG_FILE_LOAD;
	if(bmp_info_header.biClrUsed != 0)
		return SEAV_ERR_IMG_FILE_LOAD;

	// seek al posto giusto
	fseek(fp, file_header.bfOffBits, SEEK_SET);
	xl = bmp_info_header.biWidth;
	yl = bmp_info_header.biHeight;
	xf = xi + xl - 1;
	yf = yi + yl - 1;
	n_byte_riga = (file_header.bfSize - file_header.bfOffBits) / bmp_info_header.biHeight;

	// allocazione di memoria per il buffer */
	buffer = (unsigned char *) malloc((size_t) n_byte_riga * sizeof(unsigned char));
	if(buffer == NULL) {
		fclose(fp);
		return SEAV_ERR_MEMORY;
	}

	// selezione del buffer
	sea2_seleziona_buffer(fb);

	// lettura del file e scaricamento su video una riga per volta;
	// la bitmap e' memorizzata bottom up
	sea2_init_config(&param);
	for(i=yf; i>=yi; i--) {
		fread(buffer, 1, n_byte_riga, fp);
		if(i>=param.y_min && i<=param.y_max) {
			if(xi >= param.x_min) {
				if(xf <= param.x_max) {
					sea2_scrivi_riga_col24(xi, (short) i, (short) xl, buffer);
				} else {
					sea2_scrivi_riga_col24(xi, (short) i, (short) (xl-xf+param.x_max), buffer);
				}
			} else {
				if(xf <= param.x_max) {
					sea2_scrivi_riga_col24(
						param.x_min, (short) i, (short) (xl+xi-param.x_min),
						buffer-xi+param.x_min
					);
				} else {
					sea2_scrivi_riga_col24(
						param.x_min, (short) i, (short) (xl+xi-param.x_min-xf+param.x_max),
						buffer-xi+param.x_min
					);
				}
			}
		}
	}

	/* uscita */
	free(buffer);
	fclose(fp);
	return SEAV_OK_ALL;
}
