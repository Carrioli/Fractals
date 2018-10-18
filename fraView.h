// fraView.h : interface of the CFraView class
//

#include "framegr.h"

/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_FRAVIEW_H__85A0737D_102A_48FF_B926_8CE08181F9AE__INCLUDED_)
#define AFX_FRAVIEW_H__85A0737D_102A_48FF_B926_8CE08181F9AE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define UPDATE_CANCELLA_BACKGROUND         1001
#define UPDATE_AGGIORNA_IMMAGINE_DI_SFONDO 1002
#define UPDATE_DISEGNA_FINESTRE            1003

#define ID_TIMER_COLOR 1
#define REFRESH_COLOR_TIME 150

typedef struct {
    double xi;
    double yi;
    double xf;
    double yf;
} ROID;

class CMainFrame;

class CFraView : public CView
{
protected: // create from serialization only
	CFraView();
	DECLARE_DYNCREATE(CFraView)
	virtual void OnUpdate(CView *pSender, LPARAM lHint, CObject *pHint);

// Attributes
public:
	CFraDoc* GetDocument();

// Operations
public:
	BOOL ApriGrabber();
//	void DisplayFbWindows(short num_tv);
	bool frattali(CRect rect, ROID *area, bool first_time, int tipo=0);
	void ShowStatusBar(BOOL flag);
	void Esegui_wm_paint(BOOL flag /*=TRUE*/);
	void VisualizzaColori(short fb);
	bool SaveInfoImage(LPCTSTR filename);
	bool ReadInfoImage(LPCTSTR filename);
	void ActualImage();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFraView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFraView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void OnInitialUpdate(); // called first time after construct

// Generated message map functions
protected:
	//{{AFX_MSG(CFraView)
	afx_msg void OnImmaginiPuliscischermo();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnImmaginiRandomColors();
	afx_msg void OnImmaginiStartColorRun();
	afx_msg void OnImmaginiStopColorRun();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnImmaginiLinearColor();
	afx_msg void OnImmaginiLastImage();
	afx_msg void OnUpdateImmaginiLastImage(CCmdUI* pCmdUI);
	afx_msg void OnToolsPer2H();
	afx_msg void OnUpdateToolsPer2H(CCmdUI* pCmdUI);
	afx_msg void OnToolsPer2V();
	afx_msg void OnUpdateToolsPer2V(CCmdUI* pCmdUI);
	afx_msg void OnToolsPer4();
	afx_msg void OnUpdateToolsPer4(CCmdUI* pCmdUI);
	afx_msg void OnToolsMedianFilter();
	afx_msg void OnUpdateToolsMedianFilter(CCmdUI* pCmdUI);
	afx_msg void OnToolsClosingFilter();
	afx_msg void OnUpdateToolsClosingFilter(CCmdUI* pCmdUI);
	afx_msg void OnToolsSobelFilter();
	afx_msg void OnUpdateToolsSobelFilter(CCmdUI* pCmdUI);
	afx_msg void OnToolsConvFilter();
	afx_msg void OnUpdateToolsConvFilter(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


public:
	CFraDoc *m_pDoc;
	CFraApp *m_pApp;
	CMainFrame *m_pFrame;
	int m_lutr[256], m_lutg[256], m_lutb[256];
	CFrameGrab m_Grabber;

private:
	BOOL m_LeftDrag;
	void DisegnaRettangoloTratteggiato(void);
	PUNTOD c_per_c(PUNTOD p1, PUNTOD p2);
	double mod2(PUNTOD p);
	CPoint m_OldPoint;
	CRect m_RectDrag;
	CPen m_PennaDrag1, m_PennaDrag2;
	unsigned char *m_riga;
	ROID m_stack[N_TV_MAX][100];
	int m_indice_stack[N_TV_MAX]; // primo libero
	int m_tipo_disegno; // 1, 2, 3
	PUNTOD m_c[N_TV_MAX];
};

#ifndef _DEBUG  // debug version in fraView.cpp
inline CFraDoc* CFraView::GetDocument()
   { return (CFraDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FRAVIEW_H__85A0737D_102A_48FF_B926_8CE08181F9AE__INCLUDED_)
