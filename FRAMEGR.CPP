
// framegr.cpp : implementation of the CFrameGrab class


#include "stdafx.h" 
#include "fra.h" 
#include "mainfrm.h"
#include "fradoc.h" 
#include "framegr.h"
#include "fraview.h" 
#include "lzexpand.h" 

unsigned char rosso[] = {0, 255, 0, 255,   0, 255,   0, 255};
unsigned char verde[] = {0, 0, 255, 255,   0,   0, 255, 255};
unsigned char blu[]   = {0, 0,   0,   0, 255, 255, 255, 255};
unsigned char livello_bw[] = {NERO, ROSSO, VERDE, GIALLO, BLU, MAGENTA, CIANO, BIANCO};

CFrameGrab::CFrameGrab()
{   
	x_min = 0;
	x_max = 0;
	y_min = 0;
	y_max = 0;
	m_PianoColore = DISPLAY_COLORE;
	m_Fb = 0;
	m_ZoomMode = FALSE;
	m_grabber_aperto = FALSE;
	m_riga = (unsigned char *) malloc(10000);
	m_colonna = (unsigned char *) malloc(10000);
}

CFrameGrab::~CFrameGrab()
{
	if(!m_grabber_aperto)
		return;
	sea2_close();
	free(m_riga);
	free(m_colonna);
}

// ritorna TRUE se tutto ok
BOOL CFrameGrab::
Init(
	CWnd *wnd,
	int color_sys,
	int n_tel,
	int layout_zoom // 0:horizontal, 1:vertical
)
{
	short ret;
	long colore_sfondo=0L;

	ret = sea2_init(STANDARD_4000, TV_RGB, NULL);
	sea2_init_config(&param_scheda);
	if(ret != SEAV_OK_ALL) {
		AfxMessageBox("display_init1 failed");
		return FALSE;
	}

	ret = sea2_set_dma(n_tel);
	sea2_init_config(&param_scheda);
	if(ret != SEAV_OK_ALL) {
		AfxMessageBox("display_init2 failed");
		return FALSE;
	}

	/* inizializzazione del display su vga;
	   IMPORTANTE: tra le operazioni di init (init_scheda) e di inizializzazione
	   della vga (sea2_set_window_handle) non si possono fare operazioni di
	   copy_bitmap perche' alcune variabili della libreria non sono inizializzate */
	ret = sea2_set_window_handle((void near *) wnd->m_hWnd);
	sea2_init_config(&param_scheda);
	if(ret != SEAV_OK_ALL) {
		AfxMessageBox("display_init3 failed");
		return FALSE;
	}

	x_min = param_scheda.x_min;
	x_max = param_scheda.x_max;
	y_min = param_scheda.y_min;
	y_max = param_scheda.y_max;

	color_system = color_sys;
	n_telecamere = n_tel;
	m_ZoomLayout = layout_zoom;

	colore_sfondo = RGB(0, 0, 0);

	if(wnd) {
		static CBrush brush(colore_sfondo);
		::SetClassLong(
			wnd->GetSafeHwnd(), GCL_HBRBACKGROUND,
			(LONG) brush.GetSafeHandle()
		);
	}

	return TRUE;
}

void CFrameGrab::CancellaSchermo(short colore/*COLORE_NERO*/)
{
	if(!m_grabber_aperto)
		return;
	short i, j, ii;
	unsigned char *buffer;

	if(color_system) {
		buffer = (unsigned char *) malloc(param_scheda.lung_riga << 2);
		if(!buffer)
			return;
		for(i=0, j=0; i<param_scheda.lung_riga; i++, j+=4) {
			buffer[j] = blu[colore];
			buffer[j+1] = verde[colore];
			buffer[j+2] = rosso[colore];
		}
		for(j=0; j<n_telecamere; j++) {
			sea2_seleziona_buffer(j);
			for(ii=param_scheda.y_min; ii<=param_scheda.y_max; ii++)
				sea2_scrivi_riga_col(param_scheda.x_min, ii, param_scheda.lung_riga, buffer);
			sea2_reset_overlay(j);
		}
		free(buffer);
	} else {
		ROI roi;
		roi.xi = param_scheda.x_min; roi.yi = param_scheda.y_min;
		roi.xf = param_scheda.x_max; roi.yf = param_scheda.y_max;
		for(j=0; j<n_telecamere; j++) {
			roi.fb = j;  /* numero del frame buffer in memoria */
			(void) imut_disegna_rettangolo(&roi, livello_bw[colore], 1);
			sea2_reset_overlay(j);
		}
	}
}

void CFrameGrab::Fbv(short tv)
{
	// se banco_memoria_display=-1 si lascia visualizzato il 
	// banco_memoria_display precedente
	if(!m_grabber_aperto)
		return;
	sea2_display(tv);
	m_Fb = tv;
}

void CFrameGrab::ZoomMode(BOOL flag)
{
	if(!m_grabber_aperto)
		return;

	if(flag) {
		short i, fb_zoom[4];
		
		for(i=0; i<n_telecamere; i++)
			fb_zoom[i] = i;
		for(; i<4; i++)
			fb_zoom[i] = -1;
		if(m_ZoomLayout)
			sea2_display_zoom(fb_zoom[0], fb_zoom[2], fb_zoom[1], fb_zoom[3]);
		else
			sea2_display_zoom(fb_zoom[0], fb_zoom[1], fb_zoom[2], fb_zoom[3]);
	}
	m_ZoomMode = flag;
	sea2_set_zoom_mode((short) flag);
}

void CFrameGrab::DisplayPianoColore(int piano_colore)
{
	if(!m_grabber_aperto)
		return;
	if(color_system) {
		sea2_display_piano_colore(piano_colore);
		m_PianoColore = piano_colore;
	}
}

// scrive su file la bitmap a 8 bit
bool CFrameGrab::SaveBitmap(LPCTSTR filename, int *lutr, int *lutg, int *lutb)
{
	int i;

	CFraApp *pApp = (CFraApp *) AfxGetApp();
	pApp->BeginWaitCursor();
	BITMAPFILEHEADER file_header;
	BITMAPINFOHEADER bmp_info_header;

	file_header.bfType = 0x4d42;
	file_header.bfReserved1 = 0;
	file_header.bfReserved2 = 0;
	file_header.bfOffBits =
		sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD);
	file_header.bfSize =
		file_header.bfOffBits + param_scheda.lung_riga * param_scheda.lung_colonna;

	bmp_info_header.biSize = sizeof(BITMAPINFOHEADER);
	bmp_info_header.biWidth = param_scheda.lung_riga;
	bmp_info_header.biHeight = param_scheda.lung_colonna;
	bmp_info_header.biPlanes = 1;
	bmp_info_header.biBitCount = 8;
	bmp_info_header.biCompression = BI_RGB;
	bmp_info_header.biSizeImage = 0;
	bmp_info_header.biXPelsPerMeter = 3937;
	bmp_info_header.biYPelsPerMeter = 3937;
	bmp_info_header.biClrUsed = 256;
	bmp_info_header.biClrImportant = 0;

	RGBQUAD palette[256];
	for(i=0; i<256; i++) {
		palette[i].rgbRed = lutr[i];
		palette[i].rgbGreen = lutg[i];
		palette[i].rgbBlue = lutb[i];
		palette[i].rgbReserved = 0;
	}

	// apro il file
	CFile bitmap_file;
	CFileException e;
	if(!bitmap_file.Open(filename, CFile::modeCreate | CFile::modeWrite, &e)) {
		pApp->EndWaitCursor();
		return false;
	}
	bitmap_file.Write(&file_header, sizeof file_header);
	bitmap_file.Write(&bmp_info_header, sizeof bmp_info_header);
	bitmap_file.Write(palette, 256 * sizeof RGBQUAD);

	sea2_seleziona_buffer((m_Fb << 2) + 3);

	for(i=param_scheda.lung_colonna-1; i>=0; i--) {
		sea2_leggi_riga(0, i, param_scheda.lung_riga, m_riga);
		bitmap_file.Write(m_riga, param_scheda.lung_riga);
	}
	
	bitmap_file.Flush();
	bitmap_file.Close();
	pApp->EndWaitCursor();
	return true;
}

void CFrameGrab::DoubleImage(int direction)
{
	int i, j;

	sea2_seleziona_buffer((m_Fb << 2) + 3);
	if(direction == DOUBLE_HOR || direction == DOUBLE_QUAD) {
		for(i=param_scheda.lung_colonna-1; i>=0; i--) {
			sea2_leggi_riga(0, i, param_scheda.lung_riga, m_riga);
			for(j=0; j<param_scheda.lung_riga>>1; j++)
				m_riga[j] = m_riga[j<<1];
			for(; j<param_scheda.lung_riga; j++)
				m_riga[j] = m_riga[j - (param_scheda.lung_riga>>1)];
			sea2_scrivi_riga(0, i, param_scheda.lung_riga, m_riga);
		}
	}
	if(direction == DOUBLE_VER || direction == DOUBLE_QUAD) {
		for(i=param_scheda.lung_riga-1; i>=0; i--) {
			sea2_leggi_colonna(i, 0, param_scheda.lung_colonna, m_colonna);
			for(j=0; j<param_scheda.lung_colonna>>1; j++)
				m_colonna[j] = m_colonna[j<<1];
			for(; j<param_scheda.lung_colonna; j++)
				m_colonna[j] = m_colonna[j - (param_scheda.lung_colonna>>1)];
			sea2_scrivi_colonna(i, 0, param_scheda.lung_colonna, m_colonna);
		}
	}
}
