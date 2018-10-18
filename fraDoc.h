// fraDoc.h : interface of the CFraDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_FRADOC_H__A8388C3C_5E5B_442A_AEB6_FAEE40EA01E3__INCLUDED_)
#define AFX_FRADOC_H__A8388C3C_5E5B_442A_AEB6_FAEE40EA01E3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define DEFAULT_NAME _T("Untitled")
#define ESTENSIONE_FILE_INFO_IMAGE _T(".iii")
#define ESTENSIONE_FILE_BITMAP _T(".bmp")

class CFraDoc : public CDocument
{
protected: // create from serialization only
	CFraDoc();
	DECLARE_DYNCREATE(CFraDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFraDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	protected:
	virtual BOOL SaveModified();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFraDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	void FileNew(int tipo);

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CFraDoc)
	afx_msg void OnFileSaveAs();
	afx_msg void OnUpdateFileSaveAs(CCmdUI* pCmdUI);
	afx_msg void OnFileSave();
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	afx_msg void OnFileNew1();
	afx_msg void OnUpdateFileNew1(CCmdUI* pCmdUI);
	afx_msg void OnFileNew2();
	afx_msg void OnUpdateFileNew2(CCmdUI* pCmdUI);
	afx_msg void OnFileNew3();
	afx_msg void OnUpdateFileNew3(CCmdUI* pCmdUI);
	afx_msg void OnFileOpen();
	afx_msg void OnUpdateFileOpen(CCmdUI* pCmdUI);
	afx_msg void OnFileNewPiu();
	afx_msg void OnUpdateFileNewPiu(CCmdUI* pCmdUI);
	afx_msg void OnFileNewMax();
	afx_msg void OnUpdateFileNewMax(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FRADOC_H__A8388C3C_5E5B_442A_AEB6_FAEE40EA01E3__INCLUDED_)
