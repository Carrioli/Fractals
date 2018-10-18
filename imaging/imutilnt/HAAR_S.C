
/* routine per la trasformata di haar */

#include <math.h>
#include <stdio.h>
#include <fcntl.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <memory.h>
#include <imutilnt.h>

static void imut_crea_indici_haar_in_place(
	short **im_indici, short lb, short numero, short offset, short xi,
	short yi
);

/*****************************************************************************/

/* calcolo della trasformata diretta in place: il numero di passi e' pari al
   logaritmo in base due del lato dell'immagine; il primo passo e'
   compiuto al livello 2**n * 2**n, l'ultimo passo al livello 2 * 2;
   la complessita' del calcolo e' lineare: il numero di volte che
   viene applicato l'operatore h4 e' pari a n_pixel/3;
   il primo passo viene fatto da imaus a im_scr, i successivi in place
   su im_scr; il vettore im ruba 3 valori su 4 dopo ogni h4;
   nel caso di immagine di ingresso a colori cambia soltano il primo
   passo della trasformata */

void
imut_haardir_in_place(
	unsigned char **imaus,     /* array d'ingresso */
	short *im,                 /* vettore d'uscita */
	short **im_indici,         /* matrici ausiliarie */
	short **im_scr,
	short lb,                  /* dimensione del blocco */
	short colore               /* se -1 imaus contiene una immagine in B/W */
)
{
	register short ab, ba, cd, dc;
	short i, j, offj, passo, passo2, n_passi, passo_col, passo2_col;
	register short *im_scr_alto_sin;
	register short *im_scr_alto_des;
	register short *im_scr_basso_sin;
	register short *im_scr_basso_des;
	register short *im_indici_alto_des;
	register short *im_indici_basso_sin;
	register short *im_indici_basso_des;
	register unsigned char *imaus_alto_sin;
	register unsigned char *imaus_alto_des;
	register unsigned char *imaus_basso_sin;
	register unsigned char *imaus_basso_des;
	unsigned char zero = 0;

	passo = 1;      /* passo di lettura scrittura */
	passo2 = 2;     /* passo di saltellamento */
	n_passi = lb >> 1;
	if(colore < 0) {      /* B/W */
		offj = 0;
		passo_col = passo;
		passo2_col = passo2;
	} else {              /* COL */
		offj = colore;
		passo_col = 3 * passo;
		passo2_col = 3 * passo2;
	}

/* primo passo da imaus a scr */

	{
		register short **im_indici_alto      = im_indici;
		register short **im_indici_basso     = im_indici + 1;
		register unsigned char **imaus_alto  = imaus;
		register unsigned char **imaus_basso = imaus + 1;
		register short **im_scr_alto         = im_scr;
		for(i=n_passi; i>0; i--) {
			im_scr_alto_sin     = *im_scr_alto;
			im_indici_alto_des  = *im_indici_alto + passo;
			im_indici_basso_sin = *im_indici_basso;
			im_indici_basso_des = *im_indici_basso + passo;
			imaus_alto_sin  = *imaus_alto              + offj;
			imaus_alto_des  = *imaus_alto  + passo_col + offj;
			imaus_basso_sin = *imaus_basso             + offj;
			imaus_basso_des = *imaus_basso + passo_col + offj;
			for(j=n_passi; j>0; j--) {
				ab = (short) *imaus_alto_sin  + (short) *imaus_alto_des;
				ba = (short) *imaus_alto_sin  - (short) *imaus_alto_des;
				cd = (short) *imaus_basso_sin + (short) *imaus_basso_des;
				dc = (short) *imaus_basso_sin - (short) *imaus_basso_des;
/* per evitare di usare i long si elimina un bit dalla continua */
				*im_scr_alto_sin             = (ab + cd) >> 1;
				*(im + *im_indici_alto_des)  = ba + dc;
				*(im + *im_indici_basso_sin) = ab - cd;
				*(im + *im_indici_basso_des) = ba - dc;
				im_scr_alto_sin     += passo2;
				im_indici_alto_des  += passo2;
				im_indici_basso_sin += passo2;
				im_indici_basso_des += passo2;
				imaus_alto_sin   += passo2_col;
				imaus_alto_des   += passo2_col;
				imaus_basso_sin  += passo2_col;
				imaus_basso_des  += passo2_col;
			}
			im_scr_alto     += passo2;
			im_indici_alto  += passo2;
			im_indici_basso += passo2;
			imaus_alto   += passo2;
			imaus_basso  += passo2;
		}
	}

/* passi successivi: in place su scr */

	for(n_passi>>=1, passo=2;
	    n_passi>0;
		n_passi>>=1, passo<<=1) {

		register short **im_scr_alto     = im_scr;
		register short **im_scr_basso    = im_scr + passo;
		register short **im_indici_alto  = im_indici;
		register short **im_indici_basso = im_indici + passo;
		passo2 = passo << 1;
		for(i=n_passi; i>0; i--) {
			im_scr_alto_sin     = *im_scr_alto;
			im_scr_alto_des     = *im_scr_alto + passo;
			im_scr_basso_sin    = *im_scr_basso;
			im_scr_basso_des    = *im_scr_basso + passo;
			im_indici_alto_des  = *im_indici_alto + passo;
			im_indici_basso_sin = *im_indici_basso;
			im_indici_basso_des = *im_indici_basso + passo;
			for(j=n_passi; j>0; j--) {
				ab = *im_scr_alto_sin  + *im_scr_alto_des;
				ba = *im_scr_alto_sin  - *im_scr_alto_des;
				cd = *im_scr_basso_sin + *im_scr_basso_des;
				dc = *im_scr_basso_sin - *im_scr_basso_des;
/* per evitare di usare i long si elimina un bit dalla continua */
				*im_scr_alto_sin             = (ab + cd) >> 1;
				*(im + *im_indici_alto_des)  = ba + dc;
				*(im + *im_indici_basso_sin) = ab - cd;
				*(im + *im_indici_basso_des) = ba - dc;
				im_scr_alto_sin     += passo2;
				im_scr_alto_des     += passo2;
				im_scr_basso_sin    += passo2;
				im_scr_basso_des    += passo2;
				im_indici_alto_des  += passo2;
				im_indici_basso_sin += passo2;
				im_indici_basso_des += passo2;
			}
			im_scr_alto     += passo2;
			im_scr_basso    += passo2;
			im_indici_alto  += passo2;
			im_indici_basso += passo2;
		}
	}

/* trasferimento della continua */

	*im = **im_scr;
	return;
}

/*****************************************************************************/

/* come haardir_in_place ma per immagini a colori */

void
imut_haardir_in_place_col(
	unsigned char **imaus,     /* array d'ingresso */
	short *imr,                /* vettori d'uscita */
	short *img,
	short *imb,
	short **im_indici,         /* matrici ausiliarie */
	short **im_scr,
	short lb                   /* dimensione dell'immagine */
)
{
	register short ab, ba, cd, dc;
	short i, j, passo, passo2, n_passi, passo_col, passo2_col, passo3_col, passo4_col;
	short *im_scr_alto_sin;
	short *im_scr_alto_des;
	short *im_scr_basso_sin;
	short *im_scr_basso_des;
	short *im_indici_alto_des;
	short *im_indici_basso_sin;
	short *im_indici_basso_des;
	register unsigned char *imaus_alto_sin;
	register unsigned char *imaus_alto_des;
	register unsigned char *imaus_basso_sin;
	register unsigned char *imaus_basso_des;
	unsigned char zero = 0;

	passo = 1;      /* passo di lettura scrittura */
	passo2 = 2;     /* passo di saltellamento */
	passo_col = 4 * passo;
	passo3_col = 3 * passo;
	passo4_col = passo_col + 1;
	n_passi = lb >> 1;

/* primo passo da imaus a scr */

	{
		short **im_indici_alto      = im_indici;
		short **im_indici_basso     = im_indici + 1;
		unsigned char **imaus_alto  = imaus;
		unsigned char **imaus_basso = imaus + 1;
		short **im_scr_alto         = im_scr;
		for(i=n_passi; i>0; i--) {
			im_scr_alto_sin     = *im_scr_alto;
			im_indici_alto_des  = *im_indici_alto + passo;
			im_indici_basso_sin = *im_indici_basso;
			im_indici_basso_des = *im_indici_basso + passo;
			imaus_alto_sin  = *imaus_alto;
			imaus_alto_des  = *imaus_alto  + passo_col;
			imaus_basso_sin = *imaus_basso;
			imaus_basso_des = *imaus_basso + passo_col;
			for(j=n_passi; j>0; j--) {
				short aus_indici_alto_des;
				short aus_indici_basso_sin;
				short aus_indici_basso_des;
/* rosso */
				ab = (short) *imaus_alto_sin    + (short) *imaus_alto_des;
				ba = (short) *imaus_alto_sin++  - (short) *imaus_alto_des++;
				cd = (short) *imaus_basso_sin   + (short) *imaus_basso_des;
				dc = (short) *imaus_basso_sin++ - (short) *imaus_basso_des++;
				*im_scr_alto_sin++            = (ab + cd) >> 1;
				*(imr + (aus_indici_alto_des = *im_indici_alto_des))  = ba + dc;
				*(imr + (aus_indici_basso_sin= *im_indici_basso_sin)) = ab - cd;
				*(imr + (aus_indici_basso_des= *im_indici_basso_des)) = ba - dc;
/* verde */
				ab = (short) *imaus_alto_sin    + (short) *imaus_alto_des;
				ba = (short) *imaus_alto_sin++  - (short) *imaus_alto_des++;
				cd = (short) *imaus_basso_sin   + (short) *imaus_basso_des;
				dc = (short) *imaus_basso_sin++ - (short) *imaus_basso_des++;
				*im_scr_alto_sin++            = (ab + cd) >> 1;
				*(img + aus_indici_alto_des)  = ba + dc;
				*(img + aus_indici_basso_sin) = ab - cd;
				*(img + aus_indici_basso_des) = ba - dc;
/* blue */
				ab = (short) *imaus_alto_sin    + (short) *imaus_alto_des;
				ba = (short) *imaus_alto_sin++  - (short) *imaus_alto_des++;
				cd = (short) *imaus_basso_sin   + (short) *imaus_basso_des;
				dc = (short) *imaus_basso_sin++ - (short) *imaus_basso_des++;
				*im_scr_alto_sin++            = (ab + cd) >> 1;
				*(imb + aus_indici_alto_des)  = ba + dc;
				*(imb + aus_indici_basso_sin) = ab - cd;
				*(imb + aus_indici_basso_des) = ba - dc;

				im_scr_alto_sin     += passo3_col;
				im_indici_alto_des  += passo2;
				im_indici_basso_sin += passo2;
				im_indici_basso_des += passo2;
				imaus_alto_sin   += passo4_col;
				imaus_alto_des   += passo4_col;
				imaus_basso_sin  += passo4_col;
				imaus_basso_des  += passo4_col;
			}
			im_scr_alto     += passo2;
			im_indici_alto  += passo2;
			im_indici_basso += passo2;
			imaus_alto   += passo2;
			imaus_basso  += passo2;
		}
	}

/* passi successivi: in place su scrr, scrg, scrb */

	for(n_passi>>=1, passo=2;
	    n_passi>0;
		n_passi>>=1, passo<<=1) {

		short **im_scr_alto     = im_scr;
		short **im_scr_basso    = im_scr + passo;
		short **im_indici_alto  = im_indici;
		short **im_indici_basso = im_indici + passo;
		passo2 = passo << 1;
		passo_col = 3 * passo;
		passo2_col = 3 * passo2 - 3;
		for(i=n_passi; i>0; i--) {
			im_scr_alto_sin     = *im_scr_alto;
			im_scr_alto_des     = *im_scr_alto + passo_col;
			im_scr_basso_sin    = *im_scr_basso;
			im_scr_basso_des    = *im_scr_basso + passo_col;
			im_indici_alto_des  = *im_indici_alto + passo;
			im_indici_basso_sin = *im_indici_basso;
			im_indici_basso_des = *im_indici_basso + passo;
			for(j=n_passi; j>0; j--) {
/* rosso */
				ab = *im_scr_alto_sin    + *im_scr_alto_des;
				ba = *im_scr_alto_sin    - *im_scr_alto_des++;
				cd = *im_scr_basso_sin   + *im_scr_basso_des;
				dc = *im_scr_basso_sin++ - *im_scr_basso_des++;
				*im_scr_alto_sin++            = (ab + cd) >> 1;
				*(imr + *im_indici_alto_des)  = ba + dc;
				*(imr + *im_indici_basso_sin) = ab - cd;
				*(imr + *im_indici_basso_des) = ba - dc;
/* verde */
				ab = *im_scr_alto_sin    + *im_scr_alto_des;
				ba = *im_scr_alto_sin    - *im_scr_alto_des++;
				cd = *im_scr_basso_sin   + *im_scr_basso_des;
				dc = *im_scr_basso_sin++ - *im_scr_basso_des++;
				*im_scr_alto_sin++            = (ab + cd) >> 1;
				*(img + *im_indici_alto_des)  = ba + dc;
				*(img + *im_indici_basso_sin) = ab - cd;
				*(img + *im_indici_basso_des) = ba - dc;
/* blue */
				ab = *im_scr_alto_sin    + *im_scr_alto_des;
				ba = *im_scr_alto_sin    - *im_scr_alto_des++;
				cd = *im_scr_basso_sin   + *im_scr_basso_des;
				dc = *im_scr_basso_sin++ - *im_scr_basso_des++;
				*im_scr_alto_sin++            = (ab + cd) >> 1;
				*(imb + *im_indici_alto_des)  = ba + dc;
				*(imb + *im_indici_basso_sin) = ab - cd;
				*(imb + *im_indici_basso_des) = ba - dc;

				im_scr_alto_sin     += passo2_col;
				im_scr_alto_des     += passo2_col;
				im_scr_basso_sin    += passo2_col;
				im_scr_basso_des    += passo2_col;
				im_indici_alto_des  += passo2;
				im_indici_basso_sin += passo2;
				im_indici_basso_des += passo2;
			}
			im_scr_alto     += passo2;
			im_scr_basso    += passo2;
			im_indici_alto  += passo2;
			im_indici_basso += passo2;
		}
	}

/* trasferimento delle continue */

	*imr = **im_scr;
	*img = *(*im_scr + 1);
	*imb = *(*im_scr + 2);
	return;
}

/*******************************************************************/

/* calcolo della trasformata inversa in place */

void
imut_haarinv_in_place(
	unsigned char **imaus,     /* array di uscita */
	short *im_in,              /* vettore di ingresso */
	short **im_indici,         /* vettori ausiliari */
	short **im_scr,
	short lb,                  /* dimensione dell'immagine */
	short colore               /* se -1 imaus contiene una immagine in B/W */
)
{
	short i, j, n_passi_max, passo, passo2, n_passi, passo_col, passo2_col;
	short ab, ba, cd, dc, ausl1;
	short offj;
	short *im_scr_alto_sin;
	short *im_scr_alto_des;
	short *im_scr_basso_sin;
	short *im_scr_basso_des;
	short *im_indici_alto_des;
	short *im_indici_basso_sin;
	short *im_indici_basso_des;
	unsigned char *imaus_alto_sin;
	unsigned char *imaus_alto_des;
	unsigned char *imaus_basso_sin;
	unsigned char *imaus_basso_des;
	register short mask256 = -256, ausl;

	if(colore < 0) {      /* B/W */
		offj = 0;
		passo_col = 1;
		passo2_col = 2;
	} else {              /* COL */
		offj = colore;
		passo_col = 3;
		passo2_col = 6;
	}

/* trasferimento della continua che ha un bit in meno */

	**im_scr = *im_in << 1;

/* primi passi da im_in a im_scr: vengono letti 3 valori da im_in e 1
   da im_scr, vengono scritti 4 valori in im_scr */

	for(n_passi=1, n_passi_max=passo=lb>>1;
	    n_passi<n_passi_max;
		n_passi<<=1, passo>>=1) {

		short **im_scr_alto     = im_scr;
		short **im_scr_basso    = im_scr + passo;
		short **im_indici_alto  = im_indici;
		short **im_indici_basso = im_indici + passo;
		passo2 = passo << 1;
		for(i=n_passi; i>0; i--) {
			im_scr_alto_sin     = *im_scr_alto;
			im_scr_alto_des     = *im_scr_alto + passo;
			im_scr_basso_sin    = *im_scr_basso;
			im_scr_basso_des    = *im_scr_basso + passo;
			im_indici_alto_des  = *im_indici_alto + passo;
			im_indici_basso_sin = *im_indici_basso;
			im_indici_basso_des = *im_indici_basso + passo;
			for(j=n_passi; j>0; j--) {
				ab = *im_scr_alto_sin + (ausl = *(im_in + *im_indici_alto_des));
				ba = *im_scr_alto_sin - ausl;
				cd = (ausl1 = *(im_in + *im_indici_basso_sin)) +
				     (ausl = *(im_in + *im_indici_basso_des));
				dc = ausl1 - ausl;
				*im_scr_alto_sin  = ab + cd;
				*im_scr_alto_des  = ba + dc;
				*im_scr_basso_sin = ab - cd;
				*im_scr_basso_des = ba - dc;
				im_scr_alto_sin     += passo2;
				im_scr_alto_des     += passo2;
				im_scr_basso_sin    += passo2;
				im_scr_basso_des    += passo2;
				im_indici_alto_des  += passo2;
				im_indici_basso_sin += passo2;
				im_indici_basso_des += passo2;
			}
			im_scr_alto     += passo2;
			im_scr_basso    += passo2;
			im_indici_alto  += passo2;
			im_indici_basso += passo2;
		}
	}

/* ultimo passo: anziche' in im_scr si scrive in imaus previo clipping
   tra 0 e 255 */

	{
		short **im_indici_alto       = im_indici;
		short **im_indici_basso      = im_indici + passo;
		unsigned char **imaus_alto   = imaus;
		unsigned char **imaus_basso  = imaus + passo;
		short **im_scr_alto          = im_scr;
		passo2 = passo << 1;
		for(i=n_passi_max; i>0; i--) {
			im_scr_alto_sin     = *im_scr_alto;
			im_indici_alto_des  = *im_indici_alto + passo;
			im_indici_basso_sin = *im_indici_basso;
			im_indici_basso_des = *im_indici_basso + passo;
			imaus_alto_sin  = *imaus_alto           +offj;
			imaus_alto_des  = *imaus_alto+passo_col +offj;
			imaus_basso_sin = *imaus_basso          +offj;
			imaus_basso_des = *imaus_basso+passo_col+offj;
			for(j=n_passi_max; j>0; j--) {
				ab = *im_scr_alto_sin + (ausl = *(im_in + *im_indici_alto_des));
				ba = *im_scr_alto_sin - ausl;
				cd = (ausl1 = *(im_in + *im_indici_basso_sin)) +
				     (ausl = *(im_in + *im_indici_basso_des));
				dc = ausl1 - ausl;

				if((ausl = (ab+cd) >> 1) & mask256) {
					if(ausl > 0)
						*imaus_alto_sin = (unsigned char) 255;
					else
						*imaus_alto_sin = (unsigned char) 0;
				} else {
					*imaus_alto_sin = (unsigned char) ausl;
				}
				if((ausl = (ba+dc) >> 1) & mask256) {
					if(ausl > 0)
						*imaus_alto_des = (unsigned char) 255;
					else
						*imaus_alto_des = (unsigned char) 0;
				} else {
					*imaus_alto_des = (unsigned char) ausl;
				}
				if((ausl = (ab-cd) >> 1) & mask256) {
					if(ausl > 0)
						*imaus_basso_sin = (unsigned char) 255;
					else
						*imaus_basso_sin = (unsigned char) 0;
				} else {
					*imaus_basso_sin = (unsigned char) ausl;
				}
				if((ausl = (ba-dc) >> 1) & mask256) {
					if(ausl > 0)
						*imaus_basso_des = (unsigned char) 255;
					else
						*imaus_basso_des = (unsigned char) 0;
				} else {
					*imaus_basso_des = (unsigned char) ausl;
				}
				im_scr_alto_sin     += passo2;
				im_indici_alto_des  += passo2;
				im_indici_basso_sin += passo2;
				im_indici_basso_des += passo2;
				imaus_alto_sin   += passo2_col;
				imaus_alto_des   += passo2_col;
				imaus_basso_sin  += passo2_col;
				imaus_basso_des  += passo2_col;
			}
			im_scr_alto     += passo2;
			im_indici_alto  += passo2;
			im_indici_basso += passo2;
			imaus_alto   += passo2;
			imaus_basso  += passo2;
		}
	}
	return;
}

/*******************************************************************/

/* come haarinv_in_place, ma solo per immagini a colori */

void
imut_haarinv_in_place_col(
	unsigned char **imaus,     /* array di uscita */
	short *imr,                /* vettori di ingresso */
	short *img,
	short *imb,
	short **im_indici,         /* vettori ausiliari */
	short **im_scr,
	short lb                   /* dimensione dell'immagine */
)
{
	short i, j, n_passi_max, n_passi, passo_col, passo2_col;
	short ab, ba, cd, dc, ausl1;
	short *im_scr_alto_sin;
	short *im_scr_alto_des;
	short *im_scr_basso_sin;
	short *im_scr_basso_des;
	short *im_indici_alto_des;
	short *im_indici_basso_sin;
	short *im_indici_basso_des;
	unsigned char *imaus_alto_sin;
	unsigned char *imaus_alto_des;
	unsigned char *imaus_basso_sin;
	unsigned char *imaus_basso_des;
	short passo;   /* di lettura scrittura */
	short passo2;  /* di saltellamento */
	register short mask256 = -256, ausl;

/* trasferimento delle continue che hanno un bit in meno */

	**im_scr = *imr << 1;
	*(*im_scr + 1) = *img << 1;
	*(*im_scr + 2) = *imb << 1;

/* primi passi da imr,img,imb a im_scr: vengono letti 3 valori da imr,img,imb
   e 1 da im_scr, vengono scritti 4 valori in im_scr */

	for(n_passi=1, n_passi_max=passo=lb>>1;
	    n_passi<n_passi_max;
		n_passi<<=1, passo>>=1) {

		short **im_scr_alto     = im_scr;
		short **im_scr_basso    = im_scr + passo;
		short **im_indici_alto  = im_indici;
		short **im_indici_basso = im_indici + passo;
		passo2 = passo << 1;
		passo_col = 3 * passo;
		passo2_col = 3 * passo2 - 3;
		for(i=n_passi; i>0; i--) {
			im_scr_alto_sin     = *im_scr_alto;
			im_scr_alto_des     = *im_scr_alto + passo_col;
			im_scr_basso_sin    = *im_scr_basso;
			im_scr_basso_des    = *im_scr_basso + passo_col;
			im_indici_alto_des  = *im_indici_alto + passo;
			im_indici_basso_sin = *im_indici_basso;
			im_indici_basso_des = *im_indici_basso + passo;
			for(j=n_passi; j>0; j--) {
				short aus_indici_alto_des;
				short aus_indici_basso_sin;
				short aus_indici_basso_des;
/* rosso */
				ab = *im_scr_alto_sin +
				     (ausl = *(imr+(aus_indici_alto_des =*im_indici_alto_des)));
				ba = *im_scr_alto_sin - ausl;
				cd = (ausl1 = *(imr + (aus_indici_basso_sin =
				     *im_indici_basso_sin))) + (ausl = *(imr +
					 (aus_indici_basso_des = *im_indici_basso_des)));
				dc = ausl1 - ausl;
				*im_scr_alto_sin++  = ab + cd;
				*im_scr_alto_des++  = ba + dc;
				*im_scr_basso_sin++ = ab - cd;
				*im_scr_basso_des++ = ba - dc;
/* verde */
				ab = *im_scr_alto_sin + (ausl = *(img + aus_indici_alto_des));
				ba = *im_scr_alto_sin - ausl;
				cd = (ausl1 = *(img + aus_indici_basso_sin)) +
				     (ausl = *(img + aus_indici_basso_des));
				dc = ausl1 - ausl;
				*im_scr_alto_sin++  = ab + cd;
				*im_scr_alto_des++  = ba + dc;
				*im_scr_basso_sin++ = ab - cd;
				*im_scr_basso_des++ = ba - dc;
/* blue */
				ab = *im_scr_alto_sin + (ausl = *(imb + aus_indici_alto_des));
				ba = *im_scr_alto_sin - ausl;
				cd = (ausl1 = *(imb + aus_indici_basso_sin)) +
				     (ausl = *(imb + aus_indici_basso_des));
				dc = ausl1 - ausl;
				*im_scr_alto_sin++  = ab + cd;
				*im_scr_alto_des++  = ba + dc;
				*im_scr_basso_sin++ = ab - cd;
				*im_scr_basso_des++ = ba - dc;

				im_scr_alto_sin     += passo2_col;
				im_scr_alto_des     += passo2_col;
				im_scr_basso_sin    += passo2_col;
				im_scr_basso_des    += passo2_col;
				im_indici_alto_des  += passo2;
				im_indici_basso_sin += passo2;
				im_indici_basso_des += passo2;
			}
			im_scr_alto     += passo2;
			im_scr_basso    += passo2;
			im_indici_alto  += passo2;
			im_indici_basso += passo2;
		}
	}

/* ultimo passo: anziche' in im_scr si scrive in imaus previo clipping
   tra 0 e 255 */

	{
		short **im_indici_alto      = im_indici;
		short **im_indici_basso     = im_indici + passo;
		unsigned char **imaus_alto  = imaus;
		unsigned char **imaus_basso = imaus + passo;
		short **im_scr_alto         = im_scr;
		passo2 = passo << 1;
		passo_col = 3;
		passo2_col = 6;
		for(i=n_passi_max; i>0; i--) {
			im_scr_alto_sin     = *im_scr_alto;
			im_indici_alto_des  = *im_indici_alto + passo;
			im_indici_basso_sin = *im_indici_basso;
			im_indici_basso_des = *im_indici_basso + passo;
			imaus_alto_sin  = *imaus_alto;
			imaus_alto_des  = *imaus_alto+passo_col;
			imaus_basso_sin = *imaus_basso;
			imaus_basso_des = *imaus_basso+passo_col;
			for(j=n_passi_max; j>0; j--) {
				short aus_indici_alto_des;
				short aus_indici_basso_sin;
				short aus_indici_basso_des;
/* rosso */
				ab = *im_scr_alto_sin +
				     (ausl = *(imr+(aus_indici_alto_des =*im_indici_alto_des)));
				ba = *im_scr_alto_sin++ - ausl;
				cd = (ausl1 = *(imr + (aus_indici_basso_sin =
				     *im_indici_basso_sin))) + (ausl = *(imr +
					 (aus_indici_basso_des = *im_indici_basso_des)));
				dc = ausl1 - ausl;
				if((ausl = (ab+cd) >> 1) & mask256) {
					if(ausl > 0)
						*imaus_alto_sin++ = (unsigned char) 255;
					else
						*imaus_alto_sin++ = (unsigned char) 0;
				} else {
					*imaus_alto_sin++ = (unsigned char) ausl;
				}
				if((ausl = (ba+dc) >> 1) & mask256) {
					if(ausl > 0)
						*imaus_alto_des++ = (unsigned char) 255;
					else
						*imaus_alto_des++ = (unsigned char) 0;
				} else {
					*imaus_alto_des++ = (unsigned char) ausl;
				}
				if((ausl = (ab-cd) >> 1) & mask256) {
					if(ausl > 0)
						*imaus_basso_sin++ = (unsigned char) 255;
					else
						*imaus_basso_sin++ = (unsigned char) 0;
				} else {
					*imaus_basso_sin++ = (unsigned char) ausl;
				}
				if((ausl = (ba-dc) >> 1) & mask256) {
					if(ausl > 0)
						*imaus_basso_des++ = (unsigned char) 255;
					else
						*imaus_basso_des++ = (unsigned char) 0;
				} else {
					*imaus_basso_des++ = (unsigned char) ausl;
				}
/* verde */
				ab = *im_scr_alto_sin + (ausl = *(img + aus_indici_alto_des));
				ba = *im_scr_alto_sin++ - ausl;
				cd = (ausl1 = *(img + aus_indici_basso_sin)) +
				     (ausl = *(img + aus_indici_basso_des));
				dc = ausl1 - ausl;
				if((ausl = (ab+cd) >> 1) & mask256) {
					if(ausl > 0)
						*imaus_alto_sin++ = (unsigned char) 255;
					else
						*imaus_alto_sin++ = (unsigned char) 0;
				} else {
					*imaus_alto_sin++ = (unsigned char) ausl;
				}
				if((ausl = (ba+dc) >> 1) & mask256) {
					if(ausl > 0)
						*imaus_alto_des++ = (unsigned char) 255;
					else
						*imaus_alto_des++ = (unsigned char) 0;
				} else {
					*imaus_alto_des++ = (unsigned char) ausl;
				}
				if((ausl = (ab-cd) >> 1) & mask256) {
					if(ausl > 0)
						*imaus_basso_sin++ = (unsigned char) 255;
					else
						*imaus_basso_sin++ = (unsigned char) 0;
				} else {
					*imaus_basso_sin++ = (unsigned char) ausl;
				}
				if((ausl = (ba-dc) >> 1) & mask256) {
					if(ausl > 0)
						*imaus_basso_des++ = (unsigned char) 255;
					else
						*imaus_basso_des++ = (unsigned char) 0;
				} else {
					*imaus_basso_des++ = (unsigned char) ausl;
				}
/* blue */
				ab = *im_scr_alto_sin + (ausl = *(imb + aus_indici_alto_des));
				ba = *im_scr_alto_sin++ - ausl;
				cd = (ausl1 = *(imb + aus_indici_basso_sin)) +
				     (ausl = *(imb + aus_indici_basso_des));
				dc = ausl1 - ausl;
				if((ausl = (ab+cd) >> 1) & mask256) {
					if(ausl > 0)
						*imaus_alto_sin++ = (unsigned char) 255;
					else
						*imaus_alto_sin++ = (unsigned char) 0;
				} else {
					*imaus_alto_sin++ = (unsigned char) ausl;
				}
				if((ausl = (ba+dc) >> 1) & mask256) {
					if(ausl > 0)
						*imaus_alto_des++ = (unsigned char) 255;
					else
						*imaus_alto_des++ = (unsigned char) 0;
				} else {
					*imaus_alto_des++ = (unsigned char) ausl;
				}
				if((ausl = (ab-cd) >> 1) & mask256) {
					if(ausl > 0)
						*imaus_basso_sin++ = (unsigned char) 255;
					else
						*imaus_basso_sin++ = (unsigned char) 0;
				} else {
					*imaus_basso_sin++ = (unsigned char) ausl;
				}
				if((ausl = (ba-dc) >> 1) & mask256) {
					if(ausl > 0)
						*imaus_basso_des++ = (unsigned char) 255;
					else
						*imaus_basso_des++ = (unsigned char) 0;
				} else {
					*imaus_basso_des++ = (unsigned char) ausl;
				}

				im_scr_alto_sin     += passo_col;
				im_indici_alto_des  += passo2;
				im_indici_basso_sin += passo2;
				im_indici_basso_des += passo2;
				imaus_alto_sin   += passo_col;
				imaus_alto_des   += passo_col;
				imaus_basso_sin  += passo_col;
				imaus_basso_des  += passo_col;
			}
			im_scr_alto     += passo2;
			im_indici_alto  += passo2;
			im_indici_basso += passo2;
			imaus_alto   += passo2;
			imaus_basso  += passo2;
		}
	}
	return;
}

/*****************************************************************************/

/* routine che scrive nella matrice im_indici gli indici del vettore
   im che contiene la trasformata in place */

void
imut_crea_im_indici_haar_in_place(
	short **im_indici,
	short lb
)
{
	**im_indici = 0;
	imut_crea_indici_haar_in_place(im_indici, lb, 1, 0, 0, 0);
}

/***********************************************************************/

/* routine per scrivere nella matrice im i corrispondenti indici del
   vettore degli indici: vale solo per la haar in place */

static void
imut_crea_indici_haar_in_place(
	short **im_indici,
	short lb,
	short numero,
	short offset,
	short xi,
	short yi
)
{
	short lb2 = lb >> 1;
	short numero4, offset4;

	*(*(im_indici+yi)      +xi + lb2) = numero + offset;
	*(*(im_indici+yi + lb2)+xi)       = 2*numero + offset;
	*(*(im_indici+yi + lb2)+xi + lb2) = 3*numero + offset;
	if(lb > 2) {
		numero4 = numero << 2;
		offset4 = offset << 2;
		imut_crea_indici_haar_in_place(
			im_indici,lb2, numero4, offset4, xi, yi
		);
		imut_crea_indici_haar_in_place(
			im_indici,lb2, numero4, (short) (1 + offset4), (short) (xi+lb2), yi
		);
		imut_crea_indici_haar_in_place(
			im_indici,lb2, numero4, (short) (2 + offset4), xi, (short) (yi+lb2)
		);
		imut_crea_indici_haar_in_place(
			im_indici,lb2, numero4, (short) (3 + offset4),
			(short) (xi+lb2), (short) (yi+lb2)
		);
	}
}




