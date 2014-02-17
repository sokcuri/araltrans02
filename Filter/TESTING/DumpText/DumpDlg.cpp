// DumpDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DumpText.h"
#include "DumpDlg.h"


// CDumpDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDumpDlg, CDialog)

CDumpDlg::CDumpDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDumpDlg::IDD, pParent)
{

}

CDumpDlg::~CDumpDlg()
{
}

BOOL CDumpDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRect WndRect;
	CDumpDlg::GetClientRect(WndRect);
	DumpEdit.MoveWindow(WndRect,TRUE);
	DumpEdit.SetLimitText(-1);

	g_csThread.Lock();
	g_bContinue = true;
	g_csThread.Unlock();

	m_PrintThread = AfxBeginThread(PrintThreadFunc,1324, THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED,NULL);
	if(NULL == m_PrintThread)
	{
		//에러났뜸! 큰일임!
	}
}

void CDumpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, DumpEdit);
}


BEGIN_MESSAGE_MAP(CDumpDlg, CDialog)
END_MESSAGE_MAP()


// CDumpDlg 메시지 처리기입니다.

BOOL CDumpDlg::PrintText(CString Text)
{
	if(m_PrintThread)
	{
		g_csThread.Lock();
		//텍스트를 집어넣도록하자!
		m_PrintQueue.push(Text);

		g_csThread.Unlock();

		return TRUE;
	}
	else
	{
		//쓰레드가 생성되지 않았으므로
		return FALSE;
	}
}

UINT CDumpDlg::PrintThreadFunc(LPVOID lpParam)
{
	CString Text;
	bool inText=false;
	while(true)
		{
		Sleep(1);
		inText=false;

		g_csThread.Lock();
		if(FALSE == g_bContinue)
		{
			g_csThread.Unlock();
			break;
		}
		else
		{
			if(!m_PrintQueue.empty())
			{
				Text = m_PrintQueue.front();
				m_PrintQueue.pop();
				inText=true;
			}
			g_csThread.Unlock();

			if(inText)
			{

			}
		}

	}
	return 0L;
}