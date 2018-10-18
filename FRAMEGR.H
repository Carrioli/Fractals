
//  Framegr.h  CFrameGrab class interface file 
//////////////////////////////////////////////

#ifndef __FRAMEGR_H__
#define __FRAMEGR_H__  
                                                
#define NUM_TV1 0
#define NUM_TV2 1
#define NUM_TV3 2                                               
#define NUM_TV4 3                                               

#define N_TV_MAX 4

#define SINGLE_GRAB 1
#define CONTINUOUS_GRAB 2
#define NO_TRIGGER 0
#define TRIGGER 1

#define DISPLAY_ROSSO   0
#define DISPLAY_VERDE   1
#define DISPLAY_BLU     2
#define DISPLAY_COLORE  3

#define ID_TIMER_GRAB 2
#define REFRESH_GRAB_TIMER  100  // tempo refresh in msec

#define COLORE_NERO                    0
#define COLORE_ROSSO                   1
#define COLORE_VERDE                   2
#define COLORE_GIALLO                  3
#define COLORE_BLU                     4
#define COLORE_MAGENTA                 5
#define COLORE_CIANO                   6
#define COLORE_BIANCO                  7

// colori che corrispondono alla palette di sistema
#define NERO                 0
#define ROSSO_SCURO          1
#define VERDE_SCURO          2
#define VERDE_MEDIO          3
#define BLU_COBALTO          4
#define VIOLA_CHIARO         5
#define CARTA_DA_ZUCCHERO    6
#define GRIGIO_SBIADITO      7
#define VERDE_SBIADITO       8
#define AZZURRO_SBIADITO     9
#define PRIMO_GRIGIO        10   /* della scala lineare */
#define ULTIMO_GRIGIO      245   /* della scala lineare */
#define GRIGIO_CHIARO      246
#define GRIGIO_MEDIO       247
#define GRIGIO_SCURO       248
#define ROSSO              249
#define VERDE              250
#define GIALLO             251
#define BLU                252
#define MAGENTA            253
#define CIANO              254
#define BIANCO             255

#define ULTIMO_GRIGIO_COL  255

#define DOUBLE_HOR 1
#define DOUBLE_VER 2
#define DOUBLE_QUAD 3

extern "C" {
	#include <sea2_nt.h>
	#include <imutilnt.h>
}

class CFrameGrab 
{      

public:
	CFrameGrab(); 
	CFrameGrab::~CFrameGrab();
	BOOL Init(CWnd *wnd, int color_system, int n_tel, int layout_zoom);
	void CancellaSchermo(short colore=COLORE_NERO);
	void Fbv(short tv);
	void ZoomMode(BOOL flag);
	void DisplayPianoColore(int piano_colore);
	bool SaveBitmap(LPCTSTR filename, int *lutr, int *lutg, int *lutb);
	void DoubleImage(int direction);

public:
	BOOL m_grabber_aperto;
	int x_min;
	int x_max;
	int y_min;
	int y_max;  
	int m_PianoColore;  // indica il piano di colore da far vedere
	                    // 0:r, 1:g, 2:b, 3:colore
	int m_Fb;  // indica il numero della telecamera
	int m_Chan;// indica l'ingresso video della scheda: non e' utilizzato
	int n_telecamere;
	int color_system;
	BOOL m_ZoomMode;
	int m_ZoomLayout;
	PARAM_SCHEDA param_scheda;
	unsigned char *m_riga;
	unsigned char *m_colonna;
};


#endif // __FRAMEGR_H__
