// FileAddDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "CustomDic.h"
#include "FileAddDlg.h"
#include "afxdialogex.h"
#include "SubFunc.h"
#include "resource.h"
#include "Util.h"

// CFileAddDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CFileAddDlg, CDialogEx)

CFileAddDlg::CFileAddDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFileAddDlg::IDD, pParent)
	, m_nLoadType(0)
	, m_nDicType(1)
	, m_bDragFile(false)
	, m_strPath(_T(""))
{

}

CFileAddDlg::~CFileAddDlg()
{
}

void CFileAddDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_NEW, m_nLoadType);
	DDX_Radio(pDX, IDC_RADIO_PRE, m_nDicType);
	DDX_Control(pDX, IDC_FILE_PATH, m_ctrPath);
	DDX_Text(pDX, IDC_FILE_PATH, m_strPath);
	DDX_Control(pDX, IDC_BTN_FIND, m_ctrFind);
}


BEGIN_MESSAGE_MAP(CFileAddDlg, CDialogEx)
	ON_BN_CLICKED(IDC_RADIO_OPEN, &CFileAddDlg::OnBnClickedRadioOpen)
	ON_BN_CLICKED(IDC_RADIO_NEW, &CFileAddDlg::OnBnClickedRadioNew)
	ON_BN_CLICKED(IDC_BTN_FIND, &CFileAddDlg::OnBnClickedBtnFind)
	ON_WM_DROPFILES()
END_MESSAGE_MAP()


// CFileAddDlg 메시지 처리기입니다.


void CFileAddDlg::OnBnClickedRadioOpen()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	
	OnBnClickedBtnFind();
}


void CFileAddDlg::OnBnClickedRadioNew()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	OnBnClickedBtnFind();
}


BOOL CFileAddDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_ctrPath.SetReadOnly(1);
	if(m_bDragFile){
		m_nLoadType=1;
		SetFileType(m_strPath);
	}

	m_ctrFind.SetIcon((HICON)LoadImage(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDI_ICON_FOLDER),IMAGE_ICON,16,16,LR_SHARED));

	UpdateData(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CFileAddDlg::SetFileType(CString strPath){
	//파일의 첫줄을 읽어서 사전 타입을 읽어오자

	//가령
	//Header{DicType:Pre}
	//Header{DicType:Body}
	//Header{DicType:Post}
	//세종류가 있을 수 있겠다!
	//다른 값도 얼마든지 넣을순 있다만 그 경우에는 Body로 인식한다.

	map<wstring,wstring> header = parseHeader(m_strPath.GetString(),true);

	//Header를 참조해서 기본 선택을 해주자
	wstring& strDicType = header[L"DicType"];
	if(strDicType==L"Pre"){
		m_nDicType=0;
	}
	else if(strDicType==L"Post"){
		m_nDicType=2;
	}
	else{
		m_nDicType=1;
	}

	UpdateData(0);
}

void CFileAddDlg::OnBnClickedBtnFind()
{
	UpdateData();

	wchar_t szPathBuff[MAX_PATH];
	swprintf_s(szPathBuff,MAX_PATH,L"%s\\ATData",GetGameDirectory());
	MyCreateDirectory(szPathBuff);

	CFileDialog* pDlg=NULL;
	if(m_nLoadType==0){
		//새파일
		CFileDialog cDlg(FALSE,L"*.txt",L"CustomDic.txt",
			OFN_CREATEPROMPT|OFN_OVERWRITEPROMPT|OFN_NONETWORKBUTTON|OFN_HIDEREADONLY,
			L"텍스트 파일(*.txt)|*.txt|모든 파일(*.*)|*.*||");
		cDlg.m_ofn.Flags |= OFN_NOCHANGEDIR;
		cDlg.m_ofn.lpstrInitialDir=szPathBuff;
		if(cDlg.DoModal()==IDOK){
			m_strPath = cDlg.GetPathName();
		}
		else{
			return;
		}
	}
	else{
		//파일 열기
		CFileDialog cDlg(TRUE,L"*.txt",L"CustomDic.txt",
			OFN_FILEMUSTEXIST|OFN_NONETWORKBUTTON|OFN_HIDEREADONLY,
			L"텍스트 파일(*.txt)|*.txt|모든 파일(*.*)|*.*||");
		cDlg.m_ofn.Flags |= OFN_NOCHANGEDIR;
		cDlg.m_ofn.lpstrInitialDir=szPathBuff;
		if(cDlg.DoModal()==IDOK){
			m_strPath = cDlg.GetPathName();
		}
		else{
			return;
		}
	}

	if(m_nLoadType==1){
		SetFileType(m_strPath);
	}

	UpdateData(0);
}


void CFileAddDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	DWORD dwBufSize;
	CString strFileName;

	dwBufSize = DragQueryFile(hDropInfo,0,NULL,0)+1;
	LPWSTR pszFileName = strFileName.GetBufferSetLength(dwBufSize);
	DragQueryFile(hDropInfo,0,pszFileName,dwBufSize);
	strFileName.ReleaseBuffer();

	m_ctrFind.EnableWindow(1);
	m_nLoadType=1;

	m_strPath = strFileName;

	SetFileType(m_strPath);

	UpdateData(0);
	CDialogEx::OnDropFiles(hDropInfo);
}
