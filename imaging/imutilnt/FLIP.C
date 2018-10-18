


/***********************************************************************

;  Nome:    
     imut_flip_sea_header
;
;  Funzione:
     scambia i byte dei campi dell'header SEA
;
;  Formato:
     void imut_flip_sea_header(header_in, header_out)
;
;  Argomenti:    
	 PVS_HDR *header_in         header da trasformare
	 PVS_HDR *header_out        header trasformato

;
;  Descrizione:
     {\bf imut_flip_sea_header}() trasforma i campi interi dell'header
	 delle immagini SEA scambiando l'ordine dei byte; cio' permette
	 l'utilizzo delle stesse immagini sia su macchine INTEL che su
	 macchine MOTOROLA, SUN, etc.;
;
;**********************************************************************/

#include <imutilnt.h>

void
imut_flip_sea_header(
	PVS_HDR *header_in,
	PVS_HDR *header_out
)
{
	short i;

	header_out->hdr = util_flip_long(header_in->hdr);
	header_out->fmt = util_flip_long(header_in->fmt);
	header_out->xsize = util_flip_long(header_in->xsize);
	header_out->ysize = util_flip_long(header_in->ysize);
	header_out->zsize = util_flip_long(header_in->zsize);
	header_out->nchan = util_flip_long(header_in->nchan);
	header_out->uchan = util_flip_long(header_in->uchan);
	header_out->crdate[0] = util_flip_long(header_in->crdate[0]);
	header_out->crdate[1] = util_flip_long(header_in->crdate[1]);
	header_out->uid = util_flip_long(header_in->uid);
	header_out->modified = util_flip_long(header_in->modified);
	for(i=0; i<40; i++)
		header_out->title[i] = header_in->title[i];
	for(i=0; i<128; i++)
		header_out->descrip[i] = header_in->descrip[i];
	header_out->extdesc = util_flip_long(header_in->extdesc);
	for(i=0; i<32; i++)
		header_out->_filler_[i] = util_flip_long(header_in->_filler_[i]);
	for(i=0; i<100; i++)
		header_out->extra[i] = util_flip_long(header_in->extra[i]);
	return;
}



