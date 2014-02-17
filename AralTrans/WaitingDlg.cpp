// WaitingDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "WaitingDlg.h"
#include <MMSystem.h>

#pragma comment( lib, "winmm.lib" )


// CWaitingDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CWaitingDlg, CDialog)

CWaitingDlg::CWaitingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWaitingDlg::IDD, pParent)
	, m_dwEndTime(0)
	, m_dwWaitingTime(0)
{

}


CWaitingDlg::~CWaitingDlg()
{
}


void CWaitingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_TIME, m_lblWaitingTime);
	DDX_Control(pDX, IDC_STATIC_PIC, m_picWaiting);
}


BEGIN_MESSAGE_MAP(CWaitingDlg, CDialog)
	ON_WM_TIMER()
	ON_WM_NCDESTROY()
END_MESSAGE_MAP()


// CWaitingDlg 메시지 처리기입니다.
void CWaitingDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	DWORD dwCurTime = timeGetTime();
	if(m_dwEndTime > dwCurTime)
	{
		ShowRemainedTime(m_dwEndTime - dwCurTime);
	}
	else
	{
		ShowRemainedTime(0);
		KillTimer(11);
		PostMessage(WM_CLOSE);
	}
	
	CDialog::OnTimer(nIDEvent);
}


BOOL CWaitingDlg::OnInitDialog()
{
	BOOL bRetVal = CDialog::OnInitDialog();
	
	if(m_dwWaitingTime)
	{
		m_dwEndTime = timeGetTime() + m_dwWaitingTime;
		ShowRemainedTime(m_dwWaitingTime);
		SetTimer(11, 30, NULL);		
	}

	// 그림
	//if (m_picWaiting.Load(_T("D:\\AralTrans02Master\\AralTrans\\res\\d0013354_481f257857fd1.gif")))
	if (m_picWaiting.Load(MAKEINTRESOURCE(IDR_GIF1), _T("GIFTYPE")))
	{
		m_picWaiting.Draw();
		//BOOL bIsGIF = m_picWaiting.IsGIF();
		//BOOL bIsAnimatedGIF = m_picWaiting.IsAnimatedGIF();
		//BOOL bIsPlaying = m_picWaiting.IsPlaying();
		//int  nFrameCount = m_picWaiting.GetFrameCount();
	}

	return bRetVal;
}


void CWaitingDlg::ShowRemainedTime( DWORD dwTime )
{
	float fRemain = (float)dwTime / 1000.0F;

	CString strText;
	if(fRemain > 10.0F) strText.Format(_T("%.0f초 남았습니다."), fRemain);
	else strText.Format(_T("%.1f초 남았습니다."), fRemain);
	m_lblWaitingTime.SetWindowText(strText);
}

void CWaitingDlg::OnNcDestroy()
{
	CDialog::OnNcDestroy();
	
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_picWaiting.UnLoad();
}
