// fraView.cpp : implementation of the CFraView class
//

#include "stdafx.h"
#include "fra.h"

#include "fraDoc.h"
#include "fraView.h"
#include "framegr.h"
#include "MainFrm.h"
//#include <winable.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFraView

IMPLEMENT_DYNCREATE(CFraView, CView)

BEGIN_MESSAGE_MAP(CFraView, CView)
	//{{AFX_MSG_MAP(CFraView)
	ON_COMMAND(ID_IMMAGINI_PULISCISCHERMO, OnImmaginiPuliscischermo)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_IMMAGINI_RANDOM_COLORS, OnImmaginiRandomColors)
	ON_COMMAND(ID_IMMAGINI_START_COLOR_RUN, OnImmaginiStartColorRun)
	ON_COMMAND(ID_IMMAGINI_STOP_COLOR_RUN, OnImmaginiStopColorRun)
	ON_WM_TIMER()
	ON_COMMAND(ID_IMMAGINI_LINEAR_COLOR, OnImmaginiLinearColor)
	ON_COMMAND(ID_IMMAGINI_LAST_IMAGE, OnImmaginiLastImage)
	ON_UPDATE_COMMAND_UI(ID_IMMAGINI_LAST_IMAGE, OnUpdateImmaginiLastImage)
	ON_COMMAND(ID_TOOLS_PER_2_H, OnToolsPer2H)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_PER_2_H, OnUpdateToolsPer2H)
	ON_COMMAND(ID_TOOLS_PER_2_V, OnToolsPer2V)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_PER_2_V, OnUpdateToolsPer2V)
	ON_COMMAND(ID_TOOLS_PER_4, OnToolsPer4)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_PER_4, OnUpdateToolsPer4)
	ON_COMMAND(ID_TOOLS_MEDIAN_FILTER, OnToolsMedianFilter)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_MEDIAN_FILTER, OnUpdateToolsMedianFilter)
	ON_COMMAND(ID_TOOLS_CLOSING_FILTER, OnToolsClosingFilter)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_CLOSING_FILTER, OnUpdateToolsClosingFilter)
	ON_COMMAND(ID_TOOLS_SOBEL_FILTER, OnToolsSobelFilter)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_SOBEL_FILTER, OnUpdateToolsSobelFilter)
	ON_COMMAND(ID_TOOLS_CONV_FILTER, OnToolsConvFilter)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_CONV_FILTER, OnUpdateToolsConvFilter)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFraView construction/destruction

CFraView::CFraView()
{
	int i;

	m_PennaDrag1.CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
	m_PennaDrag2.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	m_LeftDrag = FALSE;
	for(i=0; i<N_TV_MAX; i++)
		m_indice_stack[i] = 0;
	m_riga = (unsigned char *) malloc(50000);
	for(i=0; i<256; i++) {
		m_lutr[i] = rand() % 256;
		m_lutg[i] = rand() % 256;
		m_lutb[i] = rand() % 256;
	}
}

CFraView::~CFraView()
{
	free(m_riga);
}

BOOL CFraView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////

void CFraView::OnInitialUpdate()
{
	m_pDoc = (CFraDoc *) GetDocument();
	m_pApp = (CFraApp *) AfxGetApp();
	m_pFrame = (CMainFrame *) GetParentFrame();

	m_pDoc->SetModifiedFlag(FALSE);
	// si inizializza la scheda con gli annessi e connessi
	if(!ApriGrabber()) {
		m_pApp->TerminaApplicazione();
		return;
	}
}
                                            
// ritorna TRUE se tutto e' andato bene, FALSE altrimenti

BOOL CFraView::ApriGrabber()
{
	if(m_Grabber.m_grabber_aperto)
		return TRUE;

	// Calcolo l'origine della Client Area e sposto l'overlay della scheda
	CPoint punto(0,0);
	ClientToScreen(&punto); 

	if(!m_Grabber.Init(
		this, 1/*color_system*/, 1/*n_tel*/, 0/*layout_zoom*/
	))
		return FALSE;
	m_Grabber.m_grabber_aperto = TRUE;

	sea2_pan_display(0);  
	sea2_scroll_display((short) -punto.y);
	m_Grabber.Fbv(0);
	m_Grabber.DisplayPianoColore(DISPLAY_COLORE);

	m_Grabber.CancellaSchermo();	
	m_pDoc->UpdateAllViews(NULL, UPDATE_AGGIORNA_IMMAGINE_DI_SFONDO);

	return TRUE;
}

#if 0
void CFraView::OnImmaginiZoom()
{
	m_Grabber.ZoomMode(TRUE);
	m_pDoc->UpdateAllViews(NULL, UPDATE_AGGIORNA_IMMAGINE_DI_SFONDO);	
}

void CFraView::OnUpdateImmaginiZoom(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_Grabber.m_ZoomMode);
}

void CFraView::OnImmaginiVisualizzatv1()
{
	DisplayFbWindows(0);
}

void CFraView::OnUpdateImmaginiVisualizzatv1(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(
		m_Grabber.m_Fb == NUM_TV1 && m_Grabber.m_ZoomMode == FALSE
	);
}

void CFraView::OnImmaginiVisualizzatv2()
{
	DisplayFbWindows(1);
}

void CFraView::OnUpdateImmaginiVisualizzatv2(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(
		m_Grabber.m_Fb == NUM_TV2 && m_Grabber.m_ZoomMode == FALSE
	);
}

void CFraView::OnImmaginiVisualizzatv3()
{
	DisplayFbWindows(2);
}

void CFraView::OnUpdateImmaginiVisualizzatv3(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(
		m_Grabber.m_Fb == NUM_TV3 && m_Grabber.m_ZoomMode == FALSE
	);
}

void CFraView::OnImmaginiVisualizzatv4()
{
	DisplayFbWindows(3);
}

void CFraView::OnUpdateImmaginiVisualizzatv4(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(
		m_Grabber.m_Fb == NUM_TV4 && m_Grabber.m_ZoomMode == FALSE
	);
}

void CFraView::DisplayFbWindows(short num_tv)
{
	m_Grabber.ZoomMode(FALSE);
	m_Grabber.Fbv(num_tv);
	m_pDoc->UpdateAllViews(NULL, UPDATE_AGGIORNA_IMMAGINE_DI_SFONDO);
}
#endif

void CFraView::OnImmaginiPuliscischermo()
{
	m_Grabber.CancellaSchermo(COLORE_NERO);	
	m_pDoc->UpdateAllViews(NULL, UPDATE_AGGIORNA_IMMAGINE_DI_SFONDO);	
}


/////////////////////////////////////////////////////////////////////////////
// CFraView drawing

void CFraView::OnDraw(CDC* pDC)
{
	pDC = pDC; // per evitare il warning in compilazione

	if(m_Grabber.m_grabber_aperto) {
		// copia dalla bitmap al video:
		if(m_Grabber.color_system)
			sea2_refresh_col(0,0,0,0);
		else
			sea2_refresh(0,0,0,0);
	}
	if(m_LeftDrag)
		DisegnaRettangoloTratteggiato();
}

void CFraView::
OnUpdate(
	CView *pSender,
	LPARAM lHint,
	CObject *pHint
)
{
	pSender = pSender; // per evitare il warning in compilazione
	if(lHint == UPDATE_CANCELLA_BACKGROUND) {
		if(m_Grabber.m_grabber_aperto) {
			// voglio che la bitmap venga riportata a video col refresh
			// non cancello il background perche' lo fa la bitmap stessa
			if(m_Grabber.param_scheda.nome_scheda == SCHEDA_METEOR ||
			   m_Grabber.param_scheda.nome_scheda == SCHEDA_ASSENTE ||
			   m_Grabber.param_scheda.nome_scheda == SCHEDA_ORION ||
			   m_Grabber.param_scheda.nome_scheda == SCHEDA_METEOR2_MC) {
				if(pHint)
					InvalidateRect((CRect *) pHint, FALSE);
				else
					Invalidate(FALSE);
			}
		} else {
			// cancello il background
			if(m_Grabber.param_scheda.nome_scheda == SCHEDA_METEOR ||
			   m_Grabber.param_scheda.nome_scheda == SCHEDA_ASSENTE ||
			   m_Grabber.param_scheda.nome_scheda == SCHEDA_ORION ||
			   m_Grabber.param_scheda.nome_scheda == SCHEDA_METEOR2_MC) {
				if(pHint)
					InvalidateRect((CRect *) pHint, TRUE);
				else
					Invalidate(TRUE);
			}
		}
	} else if(lHint == UPDATE_AGGIORNA_IMMAGINE_DI_SFONDO) {
		// rigenero la bitmap a partire dai frame buffer
		// voglio che la bitmap venga riportata a video col refresh
		// non cancello il background perche' lo fa la bitmap stessa
		if(m_Grabber.param_scheda.nome_scheda == SCHEDA_METEOR ||
		   m_Grabber.param_scheda.nome_scheda == SCHEDA_ASSENTE ||
		   m_Grabber.param_scheda.nome_scheda == SCHEDA_ORION ||
		   m_Grabber.param_scheda.nome_scheda == SCHEDA_METEOR2_MC) {
			if(m_Grabber.color_system)
				sea2_copy_bitmap_col(1);
			else
				sea2_copy_bitmap(1);
			if(pHint)
				InvalidateRect((CRect *) pHint, FALSE);
			else
				Invalidate(FALSE);
		}
	} else {
		if(pHint)
			InvalidateRect((CRect *) pHint, TRUE);
		else
			Invalidate(TRUE);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CFraView printing

BOOL CFraView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CFraView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

void CFraView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

/////////////////////////////////////////////////////////////////////////////
// CFraView diagnostics

#ifdef _DEBUG
void CFraView::AssertValid() const
{
	CView::AssertValid();
}

void CFraView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CFraDoc* CFraView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CFraDoc)));
	return (CFraDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CFraView message handlers

// se area e' NULL si calcola l'area in base al rect passato, altrimenti
// il rect viene ignorato;
// se tipo = 0 si usa la variabile della classe (m_tipo_disegno); altrimenti
// la variabile viene settata con il valore passato
bool CFraView::frattali(CRect rect, ROID *area, bool first_time, int tipo/*=0*/)
{
	int i, j, v_pix, valore;
	double x_size, y_size;
	short fb = m_Grabber.m_Fb;
	ROID roid;
	int l_riga = m_Grabber.x_max - m_Grabber.x_min + 1;
	int l_colonna = m_Grabber.y_max - m_Grabber.y_min + 1;
	int v_max=0;
	double modulo, cr, ci;
	long n_iterazioni;
	PUNTOD p, p0, p1;

	if(!area) {
		int xx = rect.right - rect.left;
		int yy = rect.bottom - rect.top;
		if(!xx || !yy)
			return false;

		if(tipo)
			m_tipo_disegno = tipo;

		// aggiusto il rect in modo da averlo sempre di rapporto 16/10
		double rapporto = (double) xx / (double) yy;
		if(rapporto > 1.6) {
			// diminuisco right
			rect.right = (long) (rect.left + yy * 1.6);
		} else {
			// diminuisco bottom
			rect.bottom = (long) (rect.top + xx * (1./1.6));
		}

		if(first_time) {
			CTime tempo = GetCurrentTime();
			srand((unsigned int) tempo.GetTime());
			int s1 = rand() - 16384;
			int s2 = rand() - 16384;
			m_c[fb].x = 0. + (double) s1 / 16384.;
			m_c[fb].y = 0. + (double) s2 / 16384.;

			roid.xi = -2.0;
			roid.xf = 2.0;
			roid.yi = -1.5;
			roid.yf = 1.5;
			m_indice_stack[fb] = 0;
		} else {
			roid = m_stack[fb][m_indice_stack[fb]-1];
		}
		x_size = roid.xf - roid.xi;
		y_size = roid.yf - roid.yi;

		// calcolo i nuovi valori dell'area di calcolo
		double xxi = ((double) rect.left / l_riga) * x_size + roid.xi;
		double xxf = ((double) rect.right / l_riga) * x_size + roid.xi;
		double yyi = ((double) rect.top / l_colonna) * y_size + roid.yi;
		double yyf = ((double) rect.bottom / l_colonna) * y_size + roid.yi;
		roid.xi = xxi;
		roid.yi = yyi;
		roid.xf = xxf;
		roid.yf = yyf;
	} else {
		// l'area di calcolo e' passata
		roid.xi = area->xi;
		roid.yi = area->yi;
		roid.xf = area->xf;
		roid.yf = area->yf;
	}

	x_size = roid.xf - roid.xi;
	y_size = roid.yf - roid.yi;
	// introduco l'area di calcolo nello stack
	if(m_indice_stack[fb] < 100) {
		m_stack[fb][m_indice_stack[fb]] = roid;
		m_indice_stack[fb] ++;
	}

	sea2_seleziona_buffer((fb << 2) + 3);
	BeginWaitCursor();
	::BlockInput(TRUE);
	cr = m_c[fb].x;
	ci = m_c[fb].y;
	for(i=0; i<l_colonna; i++) {
		p0.y = roid.yi + (y_size * i) / (double) l_colonna;
		for(j=0; j<l_riga; j++) {
			//int pix = sea2_leggi_pixel(j, i);
			double cr1 = cr /*+ l_riga / 640.*/;
			double ci1 = ci /*+ l_colonna / 480.*/;
			p0.x = roid.xi + (x_size * j) / (double) l_riga;
			p = p0;
			n_iterazioni = 0;
			modulo = 1.0;
			while(modulo < 10e300) {
				n_iterazioni ++;
				switch(m_tipo_disegno) {
					case 1: // x = x^2 + c
					case 5: // x = x^2 + c
					case 6: // x = x^2 + c
						p = c_per_c(p, p);
						p.x += cr1;
						p.y += ci1;
						break;
					case 2: // x = x^3 + c
						p1 = c_per_c(p, p);
						p = c_per_c(p, p1);
						p.x += cr1;
						p.y += ci1;
						break;
					case 3: // x = x^4 + c
						p1 = c_per_c(p, p);
						p = c_per_c(p1, p1);
						p.x += cr1;
						p.y += ci1;
						break;
					case 4: // x = x^5 + c
						p1 = c_per_c(p, p);
						p1 = c_per_c(p1, p1);
						p = c_per_c(p, p1);
						p.x += cr1;
						p.y += ci1;
						break;
				}
				if(n_iterazioni > 252)
					break;
				modulo = mod2(p);
			}
			if(n_iterazioni > v_max)
				v_max = n_iterazioni;
			valore = n_iterazioni & 255;
			if(m_tipo_disegno == 5) {
				v_pix = sea2_leggi_pixel(j, i);
				sea2_scrivi_pixel(j, i, (valore + v_pix) & 255);
			} else if(m_tipo_disegno == 6) {
				v_pix = sea2_leggi_pixel(j, i);
				if(v_pix < valore)
					sea2_scrivi_pixel(j, i, valore);
			} else {
				sea2_scrivi_pixel(j, i, valore);
			}
		}
	}
	double normal;
	if(v_max)
		normal = 255.0 / (double) v_max;
	else
		normal = 1.0;
	sea2_seleziona_buffer(fb);
	for(i=0; i<l_colonna; i++) {
		sea2_leggi_riga_col(0, i, l_riga, m_riga);
		for(j=0; j<l_riga; j++) {
			int n_it = m_riga[(j<<2)+3];
			m_riga[(j<<2)+3] = (unsigned char) (n_it * normal);
		}
		sea2_scrivi_riga_col(0, i, l_riga, m_riga);
	}
	VisualizzaColori(fb);
	::BlockInput(FALSE);
	EndWaitCursor();
	m_pDoc->UpdateAllViews(NULL, UPDATE_AGGIORNA_IMMAGINE_DI_SFONDO);
	return true;
}

__inline PUNTOD CFraView::c_per_c(PUNTOD p1, PUNTOD p2)
{
	PUNTOD p;
	p.x = p1.x * p2.x - p1.y * p2.y;
	p.y = p1.y * p2.x + p1.x * p2.y;
	return p;
}

__inline double CFraView::mod2(PUNTOD p)
{
	return p.x * p.x + p.y * p.y;
}

void CFraView::VisualizzaColori(short fb/*se -1 aggiorna tutti i fb*/)
{
	int i, j, ii, primo, ultimo;
	int l_riga = m_Grabber.x_max - m_Grabber.x_min + 1;
	int l_colonna = m_Grabber.y_max - m_Grabber.y_min + 1;
	if(fb == -1) {
		primo = 0;
		ultimo = N_TV_MAX - 1;
	} else {
		primo = fb;
		ultimo = fb;
	}
	for(ii=primo; ii<=ultimo; ii++) {
		sea2_seleziona_buffer(ii);
		for(i=0; i<l_colonna; i++) {
			sea2_leggi_riga_col(0, i, l_riga, m_riga);
			for(j=0; j<l_riga; j++) {
				int n_it = m_riga[(j<<2)+3];
				m_riga[j<<2] = m_lutb[n_it];
				m_riga[(j<<2)+1] = m_lutg[n_it];
				m_riga[(j<<2)+2] = m_lutr[n_it];
			}
			sea2_scrivi_riga_col(0, i, l_riga, m_riga);
		}
	}
}

void CFraView::OnImmaginiRandomColors() 
{
	CTime tempo = GetCurrentTime();
	srand((unsigned int) tempo.GetTime());
	for(int i=0; i<256; i++) {
		m_lutr[i] = rand() % 256;
		m_lutg[i] = rand() % 256;
		m_lutb[i] = rand() % 256;
	}
	if(m_Grabber.m_ZoomMode)
		VisualizzaColori(-1);
	else
		VisualizzaColori(m_Grabber.m_Fb);
	m_pDoc->UpdateAllViews(NULL, UPDATE_AGGIORNA_IMMAGINE_DI_SFONDO);
}

void CFraView::OnImmaginiLinearColor() 
{
#if 0
	for(int i=0; i<85; i++) {
		m_lutr[i] = i*3;
		m_lutg[i] = 0;
		m_lutb[i] = 0;
	}
	for(; i<170; i++) {
		m_lutr[i] = m_lutr[84];
		m_lutg[i] = (i-84)*3;
		m_lutb[i] = 0;
	}
	for(; i<256; i++) {
		m_lutr[i] = m_lutr[84];
		m_lutg[i] = m_lutg[169];
		m_lutb[i] = (i-170)*3;
	}
#else
	int i, j;
	for(i=0,j=0; i<32; i++,j++) {
		m_lutr[i] = j*8;
		m_lutg[i] = 0;
		m_lutb[i] = 0;
	}
	for(j=0; i<64; i++,j++) {
		m_lutr[i] = 255;
		m_lutg[i] = j*8;
		m_lutb[i] = 0;
	}
	for(j=0; i<96; i++,j++) {
		m_lutr[i] = 255;
		m_lutg[i] = 255;
		m_lutb[i] = j*8;
	}
	for(j=0; i<128; i++,j++) {
		m_lutr[i] = 255;
		m_lutg[i] = 255-j*8;
		m_lutb[i] = 255;
	}
	for(j=0; i<160; i++,j++) {
		m_lutr[i] = 255-j*8;
		m_lutg[i] = 0;
		m_lutb[i] = 255;
	}
	for(j=0; i<192; i++,j++) {
		m_lutr[i] = 0;
		m_lutg[i] = j*8;
		m_lutb[i] = 255;
	}
	for(j=0; i<224; i++,j++) {
		m_lutr[i] = j*8;
		m_lutg[i] = 255;
		m_lutb[i] = 255;
	}
	for(j=0; i<256; i++,j++) {
		m_lutr[i] = 255-j*8;
		m_lutg[i] = 255-j*8;
		m_lutb[i] = 255-j*8;
	}
#endif
	if(m_Grabber.m_ZoomMode)
		VisualizzaColori(-1);
	else
		VisualizzaColori(m_Grabber.m_Fb);
	m_pDoc->UpdateAllViews(NULL, UPDATE_AGGIORNA_IMMAGINE_DI_SFONDO);
}

void CFraView::ShowStatusBar(BOOL flag)
{
	static BOOL status_bar_visible = TRUE;

	if(!flag) {
		// elimino la status bar se visibile
		if(m_pFrame->m_wndStatusBar.IsWindowVisible()) {
			status_bar_visible = TRUE;
			m_pFrame->m_wndStatusBar.ShowWindow(SW_HIDE);
			m_pFrame->RecalcLayout();
			Esegui_wm_paint(TRUE);
		} else {
			status_bar_visible = FALSE;
		}
	} else {
		// ripristino la status bar solo se era visibile prima dell'eliminazione
		if(status_bar_visible) {
			m_pFrame->m_wndStatusBar.ShowWindow(SW_SHOW);
			m_pFrame->RecalcLayout();
			Esegui_wm_paint(TRUE);
		}
	}
}

/////////////////////////////////////////////////////////////////////////

// esegue i WM_PAINT presenti in coda; se flag=TRUE elimina tutti i messaggi
// dalla coda
void CFraView::Esegui_wm_paint(BOOL flag)
{
	MSG msg;
	if(flag) {
		while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			if(msg.message == WM_PAINT)
				::DispatchMessage(&msg);
	} else {
		while(PeekMessage(&msg, NULL, WM_PAINT, WM_PAINT, PM_NOREMOVE))
			if(msg.message == WM_PAINT)
				::DispatchMessage(&msg);
	}
}  

/////////////////////////////////////////////////////////////////////////////

void CFraView::DisegnaRettangoloTratteggiato(void)
{
	int i;
	CClientDC dc(this);
	int xi = m_RectDrag.left;
	int yi = m_RectDrag.top;
	int xf = m_RectDrag.right;
	int yf = m_RectDrag.bottom;
	dc.MoveTo(xi, yi);
	for(i=xi; i<=xf; i+=10) {
		dc.SelectObject(&m_PennaDrag1);
		dc.LineTo(min(i+5,xf), yi);
		dc.SelectObject(&m_PennaDrag2);
		dc.LineTo(min(i+10,xf), yi);
	}               
	dc.MoveTo(xi, yf);
	for(i=xi; i<=xf; i+=10) {
		dc.SelectObject(&m_PennaDrag1);
		dc.LineTo(min(i+5,xf), yf);
		dc.SelectObject(&m_PennaDrag2);
		dc.LineTo(min(i+10,xf), yf);
	}               
	dc.MoveTo(xi, yi);
	for(i=yi; i<=yf; i+=10) {
		dc.SelectObject(&m_PennaDrag1);
		dc.LineTo(xi, min(i+5,yf));
		dc.SelectObject(&m_PennaDrag2);
		dc.LineTo(xi, min(i+10,yf));
	}               
	dc.MoveTo(xf, yi);
	for(i=yi; i<=yf; i+=10) {
		dc.SelectObject(&m_PennaDrag1);
		dc.LineTo(xf, min(i+5,yf));
		dc.SelectObject(&m_PennaDrag2);
		dc.LineTo(xf, min(i+10,yf));
	}               
}

void CFraView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// Clippo le coordinate del mouse entro l'intersezione 
	// tra la client area ed il frame buffer
	if(point.x >= m_Grabber.x_min &&
	   point.x <= m_Grabber.x_max &&
	   point.y >= m_Grabber.y_min &&
	   point.y <= m_Grabber.y_max && !m_Grabber.m_ZoomMode &&
	   m_indice_stack[m_Grabber.m_Fb] > 0) {

		// Memorizzo il punto di partenza
		m_OldPoint = point;
		m_LeftDrag = TRUE;
		SetCapture();
	}
	
	CView::OnLButtonDown(nFlags, point);
}

void CFraView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// controllo se sono in modifica finestre generale
	if(m_LeftDrag) {
		CRect rect;
		if(point.x < m_OldPoint.x) {
			rect.left = point.x;
			rect.right = m_OldPoint.x;
		} else {
			rect.left = m_OldPoint.x;
			rect.right = point.x;
		}
		if(point.y < m_OldPoint.y) {
			rect.top = point.y;
			rect.bottom = m_OldPoint.y;
		} else {
			rect.top = m_OldPoint.y;
			rect.bottom = point.y;
		}
		m_LeftDrag = FALSE;

		if(frattali(rect, NULL, false))
			m_pDoc->SetModifiedFlag();
		if(GetCapture() == this)
			::ReleaseCapture();

		m_pDoc->UpdateAllViews(NULL, UPDATE_CANCELLA_BACKGROUND);
	}
	
	CView::OnLButtonUp(nFlags, point);
}

void CFraView::OnMouseMove(UINT nFlags, CPoint point) 
{
	if(m_LeftDrag) {
		// Clippo le coordinate del mouse entro l'intersezione
		// tra la client area ed il frame buffer
		if(point.x < m_Grabber.x_min)
			point.x = m_Grabber.x_min;
		if(point.x > m_Grabber.x_max)
			point.x = m_Grabber.x_max; 
		if(point.y < m_Grabber.y_min)
			point.y = m_Grabber.y_min;
		if(point.y > m_Grabber.y_max)
			point.y = m_Grabber.y_max;		

		if(point.x < m_OldPoint.x) {
			m_RectDrag.left = point.x;
			m_RectDrag.right = m_OldPoint.x;
		} else {
			m_RectDrag.left = m_OldPoint.x;
			m_RectDrag.right = point.x;
		}
		if(point.y < m_OldPoint.y) {
			m_RectDrag.top = point.y;
			m_RectDrag.bottom = m_OldPoint.y;
		} else {
			m_RectDrag.top = m_OldPoint.y;
			m_RectDrag.bottom = point.y;
		}

		// ridisegno le modifiche a video
		m_pDoc->UpdateAllViews(NULL, UPDATE_CANCELLA_BACKGROUND);
	}
	
	CView::OnMouseMove(nFlags, point);
}

void CFraView::OnImmaginiStartColorRun() 
{
	if(!SetTimer((UINT) ID_TIMER_COLOR, (UINT) REFRESH_COLOR_TIME, NULL))
    	AfxMessageBox("Timer error");
}

void CFraView::OnImmaginiStopColorRun() 
{
	KillTimer((UINT) ID_TIMER_COLOR);
	m_pDoc->UpdateAllViews(NULL, UPDATE_AGGIORNA_IMMAGINE_DI_SFONDO);
}

void CFraView::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent == ID_TIMER_COLOR) {
		int i;
		int ausr, ausg, ausb;

		ausr = m_lutr[255];
		ausg = m_lutg[255];
		ausb = m_lutb[255];
		for(i=254; i>=0; i--) {
			m_lutr[i+1] = m_lutr[i];
			m_lutg[i+1] = m_lutg[i];
			m_lutb[i+1] = m_lutb[i];
		}
		m_lutr[0] = ausr;
		m_lutg[0] = ausg;
		m_lutb[0] = ausb;
		if(m_Grabber.m_ZoomMode)
			VisualizzaColori(-1);
		else
			VisualizzaColori(m_Grabber.m_Fb);
		m_pDoc->UpdateAllViews(NULL, UPDATE_AGGIORNA_IMMAGINE_DI_SFONDO);
	}
	
	CView::OnTimer(nIDEvent);
}


void CFraView::OnImmaginiLastImage() 
{
	if(m_indice_stack[m_Grabber.m_Fb] >= 2) {
		CRect rect(0,0,0,0);
		m_indice_stack[m_Grabber.m_Fb] -= 2;
		if(frattali(
			rect, m_stack[m_Grabber.m_Fb] + m_indice_stack[m_Grabber.m_Fb], false
		))
			m_pDoc->SetModifiedFlag();
	}
}

void CFraView::ActualImage() 
{
	if(m_indice_stack[m_Grabber.m_Fb] >= 1) {
		CRect rect(0,0,0,0);
		m_indice_stack[m_Grabber.m_Fb] -= 1;
		if(frattali(
			rect, m_stack[m_Grabber.m_Fb] + m_indice_stack[m_Grabber.m_Fb], false
		))
			m_pDoc->SetModifiedFlag(FALSE);
	}
}

void CFraView::OnUpdateImmaginiLastImage(CCmdUI* pCmdUI) 
{
	if(m_indice_stack[m_Grabber.m_Fb] < 2)
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(TRUE);
}

// scrive su file:
// c,
// m_tipo_disegno,
// m_indice_stack,
// m_stack (m_indice_stack valori)
bool CFraView::SaveInfoImage(LPCTSTR filename)
{
	CStdioFile file;
	CFileException fe;
	char auss[200]; 

	if(file.Open(filename, CFile::modeCreate | CFile::modeReadWrite, &fe)) {
		sprintf(auss, "%.10lf %.10lf %d\n\n", m_c[0].x, m_c[0].y, m_tipo_disegno);
		file.WriteString(auss);
		sprintf(auss, "%d\n", m_indice_stack[0]);
		file.WriteString(auss);
		for(int i=0; i<m_indice_stack[0]; i++) {
			sprintf(
				auss, "%.10lf %.10lf %.10lf %.10lf\n",
				m_stack[0][i].xi, m_stack[0][i].yi,
				m_stack[0][i].xf, m_stack[0][i].yf
			);
			file.WriteString(auss);
		}
		file.Close();
	} else
		return false;
	return true;
}

bool CFraView::ReadInfoImage(LPCTSTR filename)
{
	CStdioFile file;
	CFileException fe;
	CString ausstr;

	if(file.Open(
		filename, CFile::modeRead, &fe)
	) {
		do
			file.ReadString(ausstr);
		while(!ausstr.GetLength());
		sscanf(ausstr, "%lf %lf %d", &m_c[0].x, &m_c[0].y, &m_tipo_disegno);
		do
			file.ReadString(ausstr);
		while(!ausstr.GetLength());
		sscanf(ausstr, "%d", &m_indice_stack[0]);
		for(int i=0; i<m_indice_stack[0]; i++) {
			do
				file.ReadString(ausstr);
			while(!ausstr.GetLength());
			sscanf(ausstr, "%lf %lf %lf %lf",
				&m_stack[0][i].xi, &m_stack[0][i].yi,
				&m_stack[0][i].xf, &m_stack[0][i].yf
			);
		}
		file.Close();
	} else
		return false;
	return true;
}


void CFraView::OnToolsPer2H()
{
	m_Grabber.DoubleImage(DOUBLE_HOR);
	VisualizzaColori(m_Grabber.m_Fb);
	m_pDoc->UpdateAllViews(NULL, UPDATE_AGGIORNA_IMMAGINE_DI_SFONDO);
}

void CFraView::OnUpdateToolsPer2H(CCmdUI* pCmdUI) 
{
}

void CFraView::OnToolsPer2V() 
{
	m_Grabber.DoubleImage(DOUBLE_VER);
	VisualizzaColori(m_Grabber.m_Fb);
	m_pDoc->UpdateAllViews(NULL, UPDATE_AGGIORNA_IMMAGINE_DI_SFONDO);
}

void CFraView::OnUpdateToolsPer2V(CCmdUI* pCmdUI) 
{
}

void CFraView::OnToolsPer4() 
{
	m_Grabber.DoubleImage(DOUBLE_QUAD);
	VisualizzaColori(m_Grabber.m_Fb);
	m_pDoc->UpdateAllViews(NULL, UPDATE_AGGIORNA_IMMAGINE_DI_SFONDO);
}

void CFraView::OnUpdateToolsPer4(CCmdUI* pCmdUI) 
{
}

void CFraView::OnToolsMedianFilter() 
{
	short fb = m_Grabber.m_Fb;
	ROI roi_i, roi_o;
	roi_i.fb = (fb << 2) + 3;
	roi_i.xi = m_Grabber.x_min;
	roi_i.yi = m_Grabber.y_min;
	roi_i.xf = m_Grabber.x_max;
	roi_i.yf = m_Grabber.y_max;
	roi_o = roi_i;
	BeginWaitCursor();
	imut_median_filter(&roi_i, &roi_o, 5, 5);
	EndWaitCursor();
	VisualizzaColori(m_Grabber.m_Fb);
	m_pDoc->UpdateAllViews(NULL, UPDATE_AGGIORNA_IMMAGINE_DI_SFONDO);
}

void CFraView::OnUpdateToolsMedianFilter(CCmdUI* pCmdUI) 
{
}

void CFraView::OnToolsClosingFilter() 
{
	short fb = m_Grabber.m_Fb;
	ROI roi_i, roi_o;
	roi_i.fb = (fb << 2) + 3;
	roi_i.xi = m_Grabber.x_min;
	roi_i.yi = m_Grabber.y_min;
	roi_i.xf = m_Grabber.x_max;
	roi_i.yf = m_Grabber.y_max;
	roi_o = roi_i;
	BeginWaitCursor();
	imut_morf(&roi_i, &roi_o, 15, 15, OP_DILATION);
	imut_morf(&roi_i, &roi_o, 15, 15, OP_EROSION);
	EndWaitCursor();
	VisualizzaColori(m_Grabber.m_Fb);
	m_pDoc->UpdateAllViews(NULL, UPDATE_AGGIORNA_IMMAGINE_DI_SFONDO);
}

void CFraView::OnUpdateToolsClosingFilter(CCmdUI* pCmdUI) 
{
}

void CFraView::OnToolsSobelFilter() 
{
	short fb = m_Grabber.m_Fb;
	ROI roi_i, roi_o;
	roi_i.fb = (fb << 2) + 3;
	roi_i.xi = m_Grabber.x_min;
	roi_i.yi = m_Grabber.y_min;
	roi_i.xf = m_Grabber.x_max;
	roi_i.yf = m_Grabber.y_max;
	roi_o = roi_i;
	BeginWaitCursor();
	imut_sobel(&roi_i, &roi_o, 5/*tipo*/, 1/*n_bit_shift*/);
	EndWaitCursor();
	VisualizzaColori(m_Grabber.m_Fb);
	m_pDoc->UpdateAllViews(NULL, UPDATE_AGGIORNA_IMMAGINE_DI_SFONDO);
}

void CFraView::OnUpdateToolsSobelFilter(CCmdUI* pCmdUI) 
{
}

void CFraView::OnToolsConvFilter() 
{
	short i, fb = m_Grabber.m_Fb;
	short kernel[49], nr=7, nc=7, norm=5, flag_shift=1, flag_abs=1;
	for(i=0; i<49; i++)
		kernel[i] = 1;
	for(i=8; i<13; i++)
		kernel[i] = 0;
	for(i=36; i<41; i++)
		kernel[i] = 0;
	kernel[15] = 0;
	kernel[22] = 0;
	kernel[29] = 0;

	kernel[19] = 0;
	kernel[26] = 0;
	kernel[34] = 0;

	kernel[24] = 0;
	
	ROI roi_i, roi_o;
	roi_i.fb = (fb << 2) + 3;
	roi_i.xi = m_Grabber.x_min;
	roi_i.yi = m_Grabber.y_min;
	roi_i.xf = m_Grabber.x_max;
	roi_i.yf = m_Grabber.y_max;
	roi_o = roi_i;
	BeginWaitCursor();
	imut_conv(&roi_i, &roi_o, kernel, nr, nc, norm, flag_shift, flag_abs);
	EndWaitCursor();
	VisualizzaColori(m_Grabber.m_Fb);
	m_pDoc->UpdateAllViews(NULL, UPDATE_AGGIORNA_IMMAGINE_DI_SFONDO);
}

void CFraView::OnUpdateToolsConvFilter(CCmdUI* pCmdUI) 
{
}
