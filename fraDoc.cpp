// fraDoc.cpp : implementation of the CFraDoc class
//

#include "stdafx.h"
#include "fra.h"

#include "fraDoc.h"
#include "fraview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFraDoc

IMPLEMENT_DYNCREATE(CFraDoc, CDocument)

BEGIN_MESSAGE_MAP(CFraDoc, CDocument)
	//{{AFX_MSG_MAP(CFraDoc)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, OnUpdateFileSaveAs)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	ON_COMMAND(ID_FILE_NEW, OnFileNew1)
	ON_UPDATE_COMMAND_UI(ID_FILE_NEW, OnUpdateFileNew1)
	ON_COMMAND(ID_FILE_NEW_2, OnFileNew2)
	ON_UPDATE_COMMAND_UI(ID_FILE_NEW_2, OnUpdateFileNew2)
	ON_COMMAND(ID_FILE_NEW_3, OnFileNew3)
	ON_UPDATE_COMMAND_UI(ID_FILE_NEW_3, OnUpdateFileNew3)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN, OnUpdateFileOpen)
	ON_COMMAND(ID_FILE_NEW_PIU, OnFileNewPiu)
	ON_UPDATE_COMMAND_UI(ID_FILE_NEW_PIU, OnUpdateFileNewPiu)
	ON_COMMAND(ID_FILE_NEW_MAX, OnFileNewMax)
	ON_UPDATE_COMMAND_UI(ID_FILE_NEW_MAX, OnUpdateFileNewMax)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFraDoc construction/destruction

CFraDoc::CFraDoc()
{
}

CFraDoc::~CFraDoc()
{
}

BOOL CFraDoc::OnNewDocument()
{
	if(!CDocument::OnNewDocument())
		return FALSE;

	DeleteContents();
	SetModifiedFlag();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CFraDoc serialization

void CFraDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
	}
	else
	{
	}
}

/////////////////////////////////////////////////////////////////////////////
// CFraDoc diagnostics

#ifdef _DEBUG
void CFraDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CFraDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CFraDoc commands


void CFraDoc::FileNew(int tipo)
{
#if 0
	if(IsModified()) {
		int ret = AfxMessageBox(IDS_SAVE_DOC, MB_YESNOCANCEL);
		if(ret == IDYES)
			OnFileSave();
		else if(ret == IDCANCEL)
			return;
	}
#endif
	if(OnNewDocument()) {
		CRect rect;

		POSITION pos = GetFirstViewPosition();
		if(pos == NULL)
			return;
		CFraView *pView = (CFraView *) GetNextView(pos);
		rect.left = pView->m_Grabber.param_scheda.x_min;
		rect.top = pView->m_Grabber.param_scheda.y_min;
		rect.right = pView->m_Grabber.param_scheda.x_max;
		rect.bottom = pView->m_Grabber.param_scheda.y_max;
		pView->frattali(rect, NULL, true, tipo);
		SetTitle(DEFAULT_NAME);
	}
}

void CFraDoc::OnFileNew1() 
{
	FileNew(1);
}

void CFraDoc::OnUpdateFileNew1(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}

void CFraDoc::OnFileNew2() 
{
	FileNew(2);
}

void CFraDoc::OnUpdateFileNew2(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}

void CFraDoc::OnFileNew3() 
{
	FileNew(3);
}

void CFraDoc::OnUpdateFileNew3(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}

void CFraDoc::OnFileNewPiu() 
{
	FileNew(5);
}

void CFraDoc::OnUpdateFileNewPiu(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}

void CFraDoc::OnFileNewMax() 
{
	FileNew(6);
}

void CFraDoc::OnUpdateFileNewMax(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}

void CFraDoc::OnFileSaveAs() 
{
	POSITION pos = GetFirstViewPosition();
	if(pos == NULL)
		return;
	CFraView *pView = (CFraView *) GetNextView(pos);

	// apro la dlg per chiedere il nome del file su disco
	CFileDialog dlg(
		FALSE, _T("iif"),_T("*.iif"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("Image Files (*.iif)|*.iif|All Files (*.*)|*.*||"), pView
	);
	dlg.m_ofn.lpstrInitialDir = APPLICATION_DIR;
	if(dlg.DoModal() == IDOK) {
		CString filename = dlg.GetPathName();
		pView->SaveInfoImage(filename);
		CString filename1 = filename.Left(filename.GetLength() - 4);
		filename1 += ESTENSIONE_FILE_BITMAP;
		pView->m_Grabber.SaveBitmap(
			filename1, pView->m_lutr, pView->m_lutg, pView->m_lutb
		);
		SetModifiedFlag(FALSE);
		// cambio la caption
		SetTitle(filename);
	}
}

void CFraDoc::OnUpdateFileSaveAs(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}

BOOL CFraDoc::SaveModified() 
{
	if(IsModified()) {
		int ret = AfxMessageBox(IDS_SAVE_DOC, MB_YESNOCANCEL);
		if(ret == IDYES)
			OnFileSave();
		else if(ret == IDNO)
			return TRUE;
		else
			return FALSE;
	}
	return TRUE;
}


void CFraDoc::OnFileSave() 
{
	if(GetTitle() == DEFAULT_NAME)
		OnFileSaveAs();
	else {
		POSITION pos = GetFirstViewPosition();
		if(pos == NULL)
			return;
		CFraView *pView = (CFraView *) GetNextView(pos);
		CString filename = GetTitle();
		pView->SaveInfoImage(filename);
		CString filename1 = filename.Left(filename.GetLength() - 4);
		filename1 += ESTENSIONE_FILE_BITMAP;
		pView->m_Grabber.SaveBitmap(
			filename1, pView->m_lutr, pView->m_lutg, pView->m_lutb
		);
		SetModifiedFlag(FALSE);
	}
}

void CFraDoc::OnUpdateFileSave(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(IsModified());
}

void CFraDoc::OnFileOpen() 
{
	POSITION pos = GetFirstViewPosition();
	if(pos == NULL)
		return;
	CFraView *pView = (CFraView *) GetNextView(pos);

	// apro la dlg per chiedere il nome del file su disco
	CFileDialog dlg(
		TRUE, _T("iif"),_T("*.iif"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("Image Files (*.iif)|*.iif|All Files (*.*)|*.*||"), pView
	);
	dlg.m_ofn.lpstrInitialDir = APPLICATION_DIR;
	if(dlg.DoModal() == IDOK) {
		CString filename = dlg.GetPathName();
		if(pView->ReadInfoImage(filename)) {
			// cambio la caption
			SetTitle(filename);
			// faccio il disegno
			pView->ActualImage();
		}
	}
}

void CFraDoc::OnUpdateFileOpen(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}

