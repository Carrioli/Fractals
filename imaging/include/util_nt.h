#ifndef __UTIL_H__
#define __UTIL_H__

#include "my_err.h"

/************************************************************************/

/* definizioni di strutture */

typedef struct params {      /* struttura per mparse */
	char *match;
	char *val;
} PARAMS;

/************************************************************************/

/* definizioni varie */

#define SIZE_ARRAY2 50000L /* dimensione dei blocchi di memoria contigui
                              allocati da array2_dos */

/************************************************************************/

/* costanti dei contatori INTEL 8254 */

#define	TIMER_PB		0x61 /* porta di abilitazione e lettura outclk */
#define	TIMER_CTRL		0x43 /* porta di controllo integrato 8254-2    */
#define	TIMER_CONT		0x42 /* contatore 2 di 8254-2                  */
#define	TIMER_F_MAX		1193180.0 /* frequenza del clock interno (Hz)  */
#define	TIMER_CONT_SYS	0x40 /* contatore 0 di  sistema                */
#define	TIMER_N_CLOCK_PER_MS 2386L /* numero di count per millisecondo:
                                      in modo 3 il contatore si decrementa di 2
                                      ad ogni clock */


/************************************************************************/

/* prototipi */

short util_cross_corr(
	long *vet1, long *vet2, long *cross_corr, short lung1, short lung2,
	long max, short tipo
);
short util_smooth_riga(
	unsigned char *vettore, long *risultato, short dim_vettore, short dim_kernel
);
short util_sub_sampling(
	void *vet1, void *vet2, short dim_vettore, short step, short tipo
);
long util_calcola_periodo(long *cross_corr, short n_punti_segmento);
long util_calcola_massimo(void *vet, short n_punti, short tipo);
long util_calcola_massimo2(void *vet, short n_punti, short tipo, short *pos);
long util_calcola_minimo(void *vet, short n_punti, short tipo);
long util_calcola_minimo2(void *vet, short n_punti, short tipo, short *pos);
void util_normalizza(
	long *vet_in, long *vet_out, short n_punti, long max, long new_max,
	short flag_inversione
);
short util_trova_segmenti(
	short *buf, short len, short soglia, short lung_min, short dist_min,
	short *inizio, short *fine, short *lunghezza
);
short util_flip_short(short valore);
long util_flip_long(long valore);
char **util_array2(long n_righe, long n_colonne, long nbytes_per_pix);
void util_free2(char **ptr);
char **util_array2_dos(long nr, long nc, long nbytes_per_pix, short *n_blocchi);
void util_free2_dos(char **ptr, long n_righe, short n_blocchi);
int mparse(int argc, char **argv, struct params par[]);
void util_spot_detection(
	short *vet_in, short *vet_out, short n_punti, short semi_larg
);
short util_median_riga(
	void *vettore, void *risultato, short dim_vettore,
	short dim_kernel, short tipo
);
void util_ordinamento(
	void *vettore, short dim_vettore, short tipo, short flag_ordine
);
short util_gram_schmidt(
	double **A, short n_equ, short n_var, double *B, double *X
);
void util_disalloca_matrice(char **ptr, long n_righe);
char **util_alloca_matrice(long n_righe, long n_colonne, long n_bytes_per_pix);
char ***util_alloca_array_di_matrici(
	long n_matrici, long n_righe, long n_colonne, long n_bytes_per_pix
);
void util_disalloca_array_di_matrici(char ***ptr, long n_matrici, long n_righe);

short util_portio_init(void);
void util_portio_close(void);
void util_portio_outp(long address, long val);
long util_portio_inp(long address);
short util_portio_is_device_open(void);

void util_set_8254(void);
void util_set_freq(double freq);
void util_start_clock(void);
void util_start_beep(void);
void util_stop_clock(void);
void util_stop_beep(void);
void util_wait_clock(void);
void util_wait_0(void);
void util_wait_1(void);
void util_set_8254_one_shot(void);
void util_set_time(double time);
void util_start_shot(void);
short util_check_shot(void);
void util_set_divider(long value);
unsigned short util_read_counter(void);
unsigned short util_read_system_counter(void);
void util_reset_internal_counter(void);
unsigned long util_query_internal_counter(void);
short util_start_pulnix(
	long start_type, long HD_installed, long address_out,
	unsigned char mask_out, long address_in, unsigned char mask_in,
	long exposure_time
);
short util_wait_h_blank(long address_in, unsigned char mask_in);
#endif /* __UTIL_H__ */


