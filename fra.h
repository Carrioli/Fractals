// fra.h : main header file for the FRA application
//

#if !defined(AFX_FRA_H__5FB638EB_2EF8_4454_977F_5C8597AEF373__INCLUDED_)
#define AFX_FRA_H__5FB638EB_2EF8_4454_977F_5C8597AEF373__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CFraApp:
// See fra.cpp for the implementation of this class
//

#define APPLICATION_DIR _T("\\usr\\work\\fra")

class CFraApp : public CWinApp
{
public:
	CFraApp();
	void TerminaApplicazione();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFraApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CFraApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FRA_H__5FB638EB_2EF8_4454_977F_5C8597AEF373__INCLUDED_)
