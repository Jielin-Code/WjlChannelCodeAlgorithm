// MainDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Demo.h"
#include "MainDlg.h"
#include "afxdialogex.h"
#include "math.h"
#include "io.h"
#include "time.h"
#include "windows.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// 每个字节中各位置的比特值
unsigned char bitOfByteTable[256][8]=
{
	{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,1},{0,0,0,0,0,0,1,0},{0,0,0,0,0,0,1,1},{0,0,0,0,0,1,0,0},{0,0,0,0,0,1,0,1},{0,0,0,0,0,1,1,0},{0,0,0,0,0,1,1,1},	//0~7
	{0,0,0,0,1,0,0,0},{0,0,0,0,1,0,0,1},{0,0,0,0,1,0,1,0},{0,0,0,0,1,0,1,1},{0,0,0,0,1,1,0,0},{0,0,0,0,1,1,0,1},{0,0,0,0,1,1,1,0},{0,0,0,0,1,1,1,1},	//8~15	
	{0,0,0,1,0,0,0,0},{0,0,0,1,0,0,0,1},{0,0,0,1,0,0,1,0},{0,0,0,1,0,0,1,1},{0,0,0,1,0,1,0,0},{0,0,0,1,0,1,0,1},{0,0,0,1,0,1,1,0},{0,0,0,1,0,1,1,1},	//16~23
	{0,0,0,1,1,0,0,0},{0,0,0,1,1,0,0,1},{0,0,0,1,1,0,1,0},{0,0,0,1,1,0,1,1},{0,0,0,1,1,1,0,0},{0,0,0,1,1,1,0,1},{0,0,0,1,1,1,1,0},{0,0,0,1,1,1,1,1},	//24~31
	{0,0,1,0,0,0,0,0},{0,0,1,0,0,0,0,1},{0,0,1,0,0,0,1,0},{0,0,1,0,0,0,1,1},{0,0,1,0,0,1,0,0},{0,0,1,0,0,1,0,1},{0,0,1,0,0,1,1,0},{0,0,1,0,0,1,1,1},	//32~39
	{0,0,1,0,1,0,0,0},{0,0,1,0,1,0,0,1},{0,0,1,0,1,0,1,0},{0,0,1,0,1,0,1,1},{0,0,1,0,1,1,0,0},{0,0,1,0,1,1,0,1},{0,0,1,0,1,1,1,0},{0,0,1,0,1,1,1,1},	//40~47
	{0,0,1,1,0,0,0,0},{0,0,1,1,0,0,0,1},{0,0,1,1,0,0,1,0},{0,0,1,1,0,0,1,1},{0,0,1,1,0,1,0,0},{0,0,1,1,0,1,0,1},{0,0,1,1,0,1,1,0},{0,0,1,1,0,1,1,1},	//48~55
	{0,0,1,1,1,0,0,0},{0,0,1,1,1,0,0,1},{0,0,1,1,1,0,1,0},{0,0,1,1,1,0,1,1},{0,0,1,1,1,1,0,0},{0,0,1,1,1,1,0,1},{0,0,1,1,1,1,1,0},{0,0,1,1,1,1,1,1},	//56~63
	{0,1,0,0,0,0,0,0},{0,1,0,0,0,0,0,1},{0,1,0,0,0,0,1,0},{0,1,0,0,0,0,1,1},{0,1,0,0,0,1,0,0},{0,1,0,0,0,1,0,1},{0,1,0,0,0,1,1,0},{0,1,0,0,0,1,1,1},	//64~71
	{0,1,0,0,1,0,0,0},{0,1,0,0,1,0,0,1},{0,1,0,0,1,0,1,0},{0,1,0,0,1,0,1,1},{0,1,0,0,1,1,0,0},{0,1,0,0,1,1,0,1},{0,1,0,0,1,1,1,0},{0,1,0,0,1,1,1,1},	//72~79
	{0,1,0,1,0,0,0,0},{0,1,0,1,0,0,0,1},{0,1,0,1,0,0,1,0},{0,1,0,1,0,0,1,1},{0,1,0,1,0,1,0,0},{0,1,0,1,0,1,0,1},{0,1,0,1,0,1,1,0},{0,1,0,1,0,1,1,1},	//80~87
	{0,1,0,1,1,0,0,0},{0,1,0,1,1,0,0,1},{0,1,0,1,1,0,1,0},{0,1,0,1,1,0,1,1},{0,1,0,1,1,1,0,0},{0,1,0,1,1,1,0,1},{0,1,0,1,1,1,1,0},{0,1,0,1,1,1,1,1},	//88~95
	{0,1,1,0,0,0,0,0},{0,1,1,0,0,0,0,1},{0,1,1,0,0,0,1,0},{0,1,1,0,0,0,1,1},{0,1,1,0,0,1,0,0},{0,1,1,0,0,1,0,1},{0,1,1,0,0,1,1,0},{0,1,1,0,0,1,1,1},	//96~103
	{0,1,1,0,1,0,0,0},{0,1,1,0,1,0,0,1},{0,1,1,0,1,0,1,0},{0,1,1,0,1,0,1,1},{0,1,1,0,1,1,0,0},{0,1,1,0,1,1,0,1},{0,1,1,0,1,1,1,0},{0,1,1,0,1,1,1,1},	//104~111
	{0,1,1,1,0,0,0,0},{0,1,1,1,0,0,0,1},{0,1,1,1,0,0,1,0},{0,1,1,1,0,0,1,1},{0,1,1,1,0,1,0,0},{0,1,1,1,0,1,0,1},{0,1,1,1,0,1,1,0},{0,1,1,1,0,1,1,1},	//112~119
	{0,1,1,1,1,0,0,0},{0,1,1,1,1,0,0,1},{0,1,1,1,1,0,1,0},{0,1,1,1,1,0,1,1},{0,1,1,1,1,1,0,0},{0,1,1,1,1,1,0,1},{0,1,1,1,1,1,1,0},{0,1,1,1,1,1,1,1},	//120~127
	{1,0,0,0,0,0,0,0},{1,0,0,0,0,0,0,1},{1,0,0,0,0,0,1,0},{1,0,0,0,0,0,1,1},{1,0,0,0,0,1,0,0},{1,0,0,0,0,1,0,1},{1,0,0,0,0,1,1,0},{1,0,0,0,0,1,1,1},	//128~135
	{1,0,0,0,1,0,0,0},{1,0,0,0,1,0,0,1},{1,0,0,0,1,0,1,0},{1,0,0,0,1,0,1,1},{1,0,0,0,1,1,0,0},{1,0,0,0,1,1,0,1},{1,0,0,0,1,1,1,0},{1,0,0,0,1,1,1,1},	//136~143
	{1,0,0,1,0,0,0,0},{1,0,0,1,0,0,0,1},{1,0,0,1,0,0,1,0},{1,0,0,1,0,0,1,1},{1,0,0,1,0,1,0,0},{1,0,0,1,0,1,0,1},{1,0,0,1,0,1,1,0},{1,0,0,1,0,1,1,1},	//144~151
	{1,0,0,1,1,0,0,0},{1,0,0,1,1,0,0,1},{1,0,0,1,1,0,1,0},{1,0,0,1,1,0,1,1},{1,0,0,1,1,1,0,0},{1,0,0,1,1,1,0,1},{1,0,0,1,1,1,1,0},{1,0,0,1,1,1,1,1},	//152~159
	{1,0,1,0,0,0,0,0},{1,0,1,0,0,0,0,1},{1,0,1,0,0,0,1,0},{1,0,1,0,0,0,1,1},{1,0,1,0,0,1,0,0},{1,0,1,0,0,1,0,1},{1,0,1,0,0,1,1,0},{1,0,1,0,0,1,1,1},	//160~167
	{1,0,1,0,1,0,0,0},{1,0,1,0,1,0,0,1},{1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,1},{1,0,1,0,1,1,0,0},{1,0,1,0,1,1,0,1},{1,0,1,0,1,1,1,0},{1,0,1,0,1,1,1,1},	//168~175
	{1,0,1,1,0,0,0,0},{1,0,1,1,0,0,0,1},{1,0,1,1,0,0,1,0},{1,0,1,1,0,0,1,1},{1,0,1,1,0,1,0,0},{1,0,1,1,0,1,0,1},{1,0,1,1,0,1,1,0},{1,0,1,1,0,1,1,1},	//176~183
	{1,0,1,1,1,0,0,0},{1,0,1,1,1,0,0,1},{1,0,1,1,1,0,1,0},{1,0,1,1,1,0,1,1},{1,0,1,1,1,1,0,0},{1,0,1,1,1,1,0,1},{1,0,1,1,1,1,1,0},{1,0,1,1,1,1,1,1},	//184~191
	{1,1,0,0,0,0,0,0},{1,1,0,0,0,0,0,1},{1,1,0,0,0,0,1,0},{1,1,0,0,0,0,1,1},{1,1,0,0,0,1,0,0},{1,1,0,0,0,1,0,1},{1,1,0,0,0,1,1,0},{1,1,0,0,0,1,1,1},	//192~199
	{1,1,0,0,1,0,0,0},{1,1,0,0,1,0,0,1},{1,1,0,0,1,0,1,0},{1,1,0,0,1,0,1,1},{1,1,0,0,1,1,0,0},{1,1,0,0,1,1,0,1},{1,1,0,0,1,1,1,0},{1,1,0,0,1,1,1,1},	//200~207
	{1,1,0,1,0,0,0,0},{1,1,0,1,0,0,0,1},{1,1,0,1,0,0,1,0},{1,1,0,1,0,0,1,1},{1,1,0,1,0,1,0,0},{1,1,0,1,0,1,0,1},{1,1,0,1,0,1,1,0},{1,1,0,1,0,1,1,1},	//208~215
	{1,1,0,1,1,0,0,0},{1,1,0,1,1,0,0,1},{1,1,0,1,1,0,1,0},{1,1,0,1,1,0,1,1},{1,1,0,1,1,1,0,0},{1,1,0,1,1,1,0,1},{1,1,0,1,1,1,1,0},{1,1,0,1,1,1,1,1},	//216~223
	{1,1,1,0,0,0,0,0},{1,1,1,0,0,0,0,1},{1,1,1,0,0,0,1,0},{1,1,1,0,0,0,1,1},{1,1,1,0,0,1,0,0},{1,1,1,0,0,1,0,1},{1,1,1,0,0,1,1,0},{1,1,1,0,0,1,1,1},	//224~231
	{1,1,1,0,1,0,0,0},{1,1,1,0,1,0,0,1},{1,1,1,0,1,0,1,0},{1,1,1,0,1,0,1,1},{1,1,1,0,1,1,0,0},{1,1,1,0,1,1,0,1},{1,1,1,0,1,1,1,0},{1,1,1,0,1,1,1,1},	//232~239
	{1,1,1,1,0,0,0,0},{1,1,1,1,0,0,0,1},{1,1,1,1,0,0,1,0},{1,1,1,1,0,0,1,1},{1,1,1,1,0,1,0,0},{1,1,1,1,0,1,0,1},{1,1,1,1,0,1,1,0},{1,1,1,1,0,1,1,1},	//240~247
	{1,1,1,1,1,0,0,0},{1,1,1,1,1,0,0,1},{1,1,1,1,1,0,1,0},{1,1,1,1,1,0,1,1},{1,1,1,1,1,1,0,0},{1,1,1,1,1,1,0,1},{1,1,1,1,1,1,1,0},{1,1,1,1,1,1,1,1}		//248~255
};

unsigned int powoftwo[32] = 
{
	0x00000001,0x00000002,0x00000004,0x00000008,0x00000010,0x00000020,0x00000040,0x00000080,
	0x00000100,0x00000200,0x00000400,0x00000800,0x00001000,0x00002000,0x00004000,0x00008000,
	0x00010000,0x00020000,0x00040000,0x00080000,0x00100000,0x00200000,0x00400000,0x00800000,
	0x01000000,0x02000000,0x04000000,0x08000000,0x10000000,0x20000000,0x40000000,0x80000000,
};

// CMainDlg 对话框
IMPLEMENT_DYNAMIC(CMainDlg, CDialog)

CMainDlg::CMainDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMainDlg::IDD, pParent)
	, m_strErrbitRate(_T("0.0"))
{
// 	unsigned char *ucA,*ucB,uc1,uc2;
// 	unsigned int unA,unB;
// 
// 	ucA = WJL_FileToMemory("C:\\Users\\Administrator\\Desktop\\ceshitu\\无误码.wjlr",&unA);
// 	ucB = WJL_FileToMemory("C:\\Users\\Administrator\\Desktop\\ceshitu\\已纠正数据",&unB);
// 
// 	for (int i=0;i<unA;++i)
// 	{
// 		uc1 = ucA[i];
// 		uc2 = ucB[i];
// 
// 		if (uc1 != uc2)
// 		{
// 			uc1 = uc1;
// 		}
// 	}
}

CMainDlg::~CMainDlg()
{
}

void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_ED_INPUTPATH,m_edInputPath);
	DDX_Text(pDX, IDC_ED_INPUTPATH, m_strInputPath);

	DDX_Control(pDX,IDC_ED_OUTPATH,m_edOutputPath);
	DDX_Text(pDX, IDC_ED_OUTPATH, m_strOutputPath);

	DDX_Control(pDX,IDC_ED_ZIPPATH,m_edZipDataPath);
	DDX_Text(pDX, IDC_ED_ZIPPATH, m_strZipdataPath);

	DDX_Control(pDX,IDC_ED_UNZIPOUTPATH,m_edUnzipOutPath);
	DDX_Text(pDX, IDC_ED_UNZIPOUTPATH, m_strUnzipDataPath);

	DDX_Control(pDX, IDC_BTN_OPEN, m_btnOpen);
	DDX_Control(pDX, IDC_BTN_GO, m_btnOpenGo);
	DDX_Control(pDX, IDC_BTN_COMPRESS, m_btnCompress);	
	DDX_Control(pDX, IDC_BTN_UNZIP, m_btnUnzip);

	DDX_Control(pDX, IDC_BTN_OPENZIP, m_btnOpenZipdata);	
	DDX_Control(pDX, IDC_BTN_OPENUNZIP, m_btnOpenUnzipdata);
	DDX_Control(pDX, IDC_PROGRESS_COMPRESS, m_proCompress);
	DDX_Control(pDX, IDC_PROGRESS_UNZIP, m_proUnzip);
	DDX_Control(pDX, IDC_UNZIPED_OPEN, m_btnChkUnzipedOpen);
	DDX_Control(pDX, IDC_ED_ERRRATE, m_edErrbitRate);
	DDX_Control(pDX, IDC_COM_CODERATE, m_comCodeRate);
}

BEGIN_MESSAGE_MAP(CMainDlg, CDialog)
	ON_EN_KILLFOCUS(IDC_ED_INPUTPATH, &CMainDlg::OnEnKillfocusEdInputpath)
	ON_BN_CLICKED(IDC_BTN_COMPRESS, &CMainDlg::OnBnClickedBtnCompress)
	ON_BN_CLICKED(IDC_BTN_GO, &CMainDlg::OnBnClickedBtnGo)
	ON_EN_CHANGE(IDC_ED_INPUTPATH, &CMainDlg::OnEnChangeEdInputpath)
	ON_BN_CLICKED(IDC_BTN_OPEN, &CMainDlg::OnBnClickedBtnOpen)
	ON_BN_CLICKED(IDC_BTN_UNZIP, &CMainDlg::OnBnClickedBtnUnzip)
	ON_EN_CHANGE(IDC_ED_OUTPATH, &CMainDlg::OnEnChangeEdOutpath)
	ON_BN_CLICKED(IDC_BTN_OPENZIP, &CMainDlg::OnBnClickedBtnOpenzip)
	ON_BN_CLICKED(IDC_BTN_OPENUNZIP, &CMainDlg::OnBnClickedBtnOpenunzip)
	ON_EN_CHANGE(IDC_ED_ZIPPATH, &CMainDlg::OnEnChangeEdZippath)
	ON_EN_KILLFOCUS(IDC_ED_ZIPPATH, &CMainDlg::OnEnKillfocusEdZippath)
	ON_WM_CLOSE()
	ON_WM_DROPFILES()
	ON_EN_CHANGE(IDC_ED_ERRRATE, &CMainDlg::OnEnChangeEdErrrate)
END_MESSAGE_MAP()

/*系数计算*/
double CMainDlg::Coeff(char n)
{
	double w=1;
	double dw=0.000001;
	double LeftValue;
	double RightValue;
	unsigned int i;

	RightValue = pow(2.0,n+2.0);
	for (i=0;i<1000000;i++)
	{
		w=w+dw;
		LeftValue = RightValue*w - pow(w,n+2.0);
		if (LeftValue>RightValue)
		{
			break;
		}
	}
	w = w-dw;
	return w;
}

//-------------------------------------------------------------------------------
unsigned long CMainDlg::GetMicroTimeStamp(void)
{
	LARGE_INTEGER frequence, privious;
	DWORD dwRet;
	if(!QueryPerformanceFrequency( &frequence))//取高精度运行计数器的频率
	{
		return GetTickCount();
	}

	if (!QueryPerformanceCounter( &privious ))
	{
		return GetTickCount();
	}

	dwRet = (DWORD)(1000000 * privious.QuadPart / frequence.QuadPart ); //换算到微秒数

	return dwRet;//微秒
}

//---------------------------------------------------------------------------------------
CString CMainDlg::GetTime()
{
	time_t t;
	t=time(&t);
	struct tm* pTimeStruct = localtime(const_cast<time_t *>(&t));

	char szBuffer[128];
	if(pTimeStruct)
	{	
		// 99 or 02, not 1999 or 2002, Year 2050 problem...
		int nYear = pTimeStruct->tm_year;
		sprintf_s(szBuffer,"%04d%02d%02d%02d%02d%02d",(nYear<50)?(nYear+2000):(nYear+1900),pTimeStruct->tm_mon + 1,pTimeStruct->tm_mday,pTimeStruct->tm_hour,pTimeStruct->tm_min,pTimeStruct->tm_sec);
	}

	return CString(szBuffer);
}

void CMainDlg::InitRandom(void)
{
	srand((unsigned)time(NULL));
}

//---------------------------------------------------------------------------------------
//获得一个随机数
unsigned int CMainDlg::Random()
{ 
	return (((unsigned int)rand()<<24)&0xFF000000)|(((unsigned int)rand()<<12)&0x00FFF000)|(((unsigned int)rand())&0x00000FFF);
}
// 基于BPSK的BSC信道误比特率计算的
double Erf(double x)
{
	double result = 0;
	int index = 0;
	do
	{
		index++;
	} while (x / pow((double)10, index) > 1e-3);//设置计算精度
	int maxIndex =(int) pow((double)10, index);
	double deltaX = x / maxIndex;
	for (int i = 0; i <=maxIndex; i++)
	{
		if (i > 0 && i<maxIndex)
		{
			result += 2 * exp(-pow(deltaX * i, 2));
			continue;
		}
		else if (i == maxIndex)
		{
			result += exp(-pow(deltaX * i, 2));
			continue;
		}
		else if(i==0){
			result += exp(-pow(deltaX * i, 2));
			continue;
		}
	}
	return result*deltaX/pow(3.1415926,0.5);
}

// erfc函数
double erfc(double Valuex){
	return 1 - Erf(Valuex);
}

//--------------------------------------------------------------------------------------------------------------------------
void CMainDlg::ErrCode(unsigned char *ucInBuffer,const unsigned int *unInbufferLen,const double *dErrbitRate)
{
	unsigned int i,j,unRmax/*,dex=0,errCount=0,errPos*/;
	unsigned char symbol;
	double RateBSC = 0, RateBAWGN = 0, SNR = (*dErrbitRate);
	if(*dErrbitRate <= -1){
		return;
	}
	
	// BSC根据不同的信噪比，得出的不同的误比特率，注释下面这句会得到纯粹的比特值
	RateBSC = 0.5 * erfc(sqrt(0.63092975357 * pow(10,SNR/10)));//0.63092975357
	// 如果是BAWGN信道的
	RateBAWGN = 0.5 * erfc(sqrt(pow(10,SNR/10)));
	if (*unInbufferLen*8*(*dErrbitRate) == 0)
	{
		return;
	}
	// 下面的根据RateBSC或RateBAWGN进行误比特处理
	unRmax = (*dErrbitRate) * 0xffffffff - 1;

	InitRandom();
	for (i=0;i<*unInbufferLen;++i)
	{
//		dex++;
		for (j=0;j<8;j++)
		{
			symbol = bitOfByteTable[ucInBuffer[i]][j];
			if (Random() <= unRmax)
			{
// 				if (errCount >= 2)
// 				{
// 					if (dex - errPos < 14)
// 					{
// 						continue;
// 					}
// 
// 					errCount = 0;
// 				}
// 
// 				errCount++;
// 				if (errCount == 2)
// 				{
// 					errPos = dex;
// 				}

				if (0x01 == symbol)
				{// 使位1变位0
					ucInBuffer[i] -= powoftwo[7-j];
				}
				else
				{// 使位0变位1
					ucInBuffer[i] += powoftwo[7-j];
				}
			}
		}
	}
}

//---------------------------------------------------------------------------------------
void CMainDlg::DisplayFileInfo(CString strPath,UINT stcControlID)
{
	WIN32_FIND_DATA fileInfo;  
	HANDLE hFind = FindFirstFile(strPath,&fileInfo);  
	DWORD fileSize = fileInfo.nFileSizeLow;  
	FindClose(hFind); 

	TCHAR drive[_MAX_DRIVE];			// 盘符名称
	TCHAR dir[_MAX_DIR];				// 目录
	TCHAR fname[_MAX_FNAME];			// 进程名字
	TCHAR ext[_MAX_EXT];				// 后缀，一般为exe或者是dll
	_tsplitpath_s( strPath, drive, dir, fname, ext);//分割该路径，得到盘符，目录，文件名，后缀名

	CString strTip;
	if (stcControlID == IDC_STC_TIP)
	{
		strTip = _T("Source File Infomation:\n\nForm: ");
	}
	else
	{
		strTip = _T("File Infomation:\n\nForm: ");
	}
	for (int i=0;i<_MAX_EXT;++i)
	{
		if (ext[i] == '.')
		{
			continue;
		}

		if (ext[i] != 0x00)
		{
			strTip += ext[i];
		}
		else
		{
			break;
		}
	}
	
	strTip += _T("\n\nSize:");
	char tmp[100];
	if (fileSize >= 1024*1024)
	{// 字节显示
		sprintf(tmp,"%6.2fMB  ",fileSize/(1024*1024*1.0));
	}
	else if (fileSize >= 1024)
	{// k字节显示
		sprintf(tmp,"%6.2fKB  ",fileSize/(1024*1.0));
	}
	else
	{
		sprintf(tmp,"%dB ",fileSize);
	}

	strTip += CA2W(tmp);
	sprintf(tmp,"%ld Bytes ",fileSize);
	strTip += CA2W(tmp);

	GetDlgItem(stcControlID)->SetWindowText(strTip);
}

//---------------------------------------------------------------------------------------
BOOL CMainDlg::OpenPath(CString strFilePath)
{
	char szParam[_MAX_PATH+64]={0};
	strcpy(szParam,"/e,/select, ");
	strcat(szParam,CW2A(strFilePath));
	ShellExecute(NULL,_T("open"),_T("explorer"),CA2W(szParam),NULL,SW_SHOW);
	return TRUE;
}

//---------------------------------------------------------------------------------------
BOOL CMainDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_edErrbitRate.SetWindowTextW(m_strErrbitRate);
	m_btnOpenGo.SetWindowText(_T("Browse"));
	m_btnOpenUnzipdata.SetWindowText(_T("Browse"));
	m_comCodeRate.InsertString(0,_T("1/1.58496250072115618"));
	//m_comCodeRate.InsertString(1,_T("1/2"));
	m_comCodeRate.SetCurSel(0);
//	m_btnChkUnzipedOpen.SetCheck(1);
	return TRUE;
}

//---------------------------------------------------------------------------------------
void CMainDlg::OnEnKillfocusEdInputpath()
{
	m_edInputPath.GetWindowText(m_strInputPath);
}

//---------------------------------------------------------------------------------------
void CMainDlg::OnEnChangeEdInputpath()
{
	m_edInputPath.GetWindowText(m_strInputPath);
}

//---------------------------------------------------------------------------------------
void CMainDlg::OnBnClickedBtnOpen()
{
	OPENFILENAME ofn;
	TCHAR filePath[MAX_PATH] = {0};/*#define MAX_FILE_FOR_SEL 100，允许浏览的最大文件数*/
	RtlSecureZeroMemory(&ofn, sizeof (ofn) );
	ofn.hwndOwner		= GetSafeHwnd();
	ofn.Flags			= OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_EXPLORER | OFN_FORCESHOWHIDDEN;
	ofn.lpstrFile		= filePath;
	ofn.nMaxFile		= MAX_PATH;
	ofn.lStructSize		= sizeof(OPENFILENAME);
	ofn.lpstrFilter		= TEXT("all Files(*.*)\0*.*\0");//所有文件类型
	ofn.lpstrTitle		= _T("Please select the file to encode");

	if (GetOpenFileName(&ofn))
	{
		m_strInputPath = filePath;
		m_edInputPath.SetWindowText(filePath);

		DisplayFileInfo(m_strInputPath,IDC_STC_TIP);
	}
}

//---------------------------------------------------------------------------------------
void CMainDlg::OnEnChangeEdOutpath()
{

}

//---------------------------------------------------------------------------------------
void CMainDlg::OnBnClickedBtnGo()
{
	TCHAR szBuffer[MAX_PATH] = {0};  
	BROWSEINFO bi;  
	ZeroMemory(&bi,sizeof(BROWSEINFO));  
	bi.hwndOwner = NULL;  
	bi.pszDisplayName = szBuffer;  
	bi.lpszTitle = _T("Select the folder directory from below:");  
	bi.ulFlags = BIF_RETURNFSANCESTORS;  
	LPITEMIDLIST idl = SHBrowseForFolder(&bi);  
	if (NULL == idl)  
	{  
		return;  
	}  
	SHGetPathFromIDList(idl,szBuffer);
	m_edOutputPath.SetWindowText(szBuffer);
	m_strOutputPath = szBuffer;
	m_edUnzipOutPath.SetWindowText(szBuffer);
	m_strUnzipDataPath = szBuffer;
}

//---------------------------------------------------------------------------------------
void CMainDlg::OnEnChangeEdErrrate()
{
	CString strErrbitRate;
	m_edErrbitRate.GetWindowTextW(strErrbitRate);
	if (atoi(CW2A(strErrbitRate)) > 100)
	{
		m_edErrbitRate.SetWindowTextW(m_strErrbitRate);
	}
	else
	{
		m_strErrbitRate = strErrbitRate;
	}
}

//---------------------------------------------------------------------------------------
void CMainDlg::OnBnClickedBtnCompress()
{
	if (m_strInputPath.IsEmpty())
	{
		GetDlgItem(IDC_STC_TIP)->SetWindowText(_T("Please enter the file path to encode!"));
		m_edInputPath.SetFocus();
		return;
	}

	SHFILEINFO psi1;
	SHGetFileInfo(m_strInputPath,NULL,&psi1,sizeof(SHFILEINFO),SHGFI_DISPLAYNAME);
	CString strFileName = psi1.szDisplayName;
	if (strFileName.IsEmpty())
	{
		m_strInputPath = _T("");
		m_edInputPath.SetWindowText(_T(""));
		GetDlgItem(IDC_STC_TIP)->SetWindowText(_T("Please enter the file path to encode!"));
		m_edInputPath.SetFocus();
		return;
	}

	if (m_strOutputPath.IsEmpty())
	{
		GetDlgItem(IDC_STC_TIP)->SetWindowText(_T("Please enter the encoded file storage path!"));
		m_edOutputPath.SetFocus();
		return;
	}

	GetDlgItem(IDC_STC_TIP)->SetWindowText(_T("File is encoding,please wait! "));

	unsigned int unFileLen=0;
	unsigned char *ucFileBuffer = WJL_FileToMemory(CW2A(m_strInputPath),&unFileLen);

	m_proCompress.SetPos(10);

	double dErrbitRate = atof(CW2A(m_strErrbitRate));
	unsigned int unFinalLen = 0;
	unsigned char *ucFinal = NULL;
	unsigned int unSrcCurPos = 0;
	unsigned int unDstCurPos = 0;
	while (unSrcCurPos < unFileLen)
	{// 文件切段
		unsigned char *ucZipedBuffer = NULL;
		unsigned int unRestLen = unFileLen - unSrcCurPos;
		unsigned int unZipLen = 0;
		if (unRestLen < SUB_BLOCK_LEN)
		{
			ucZipedBuffer = WJL_ErrRecovery_Encode_Buff(ucFileBuffer+unSrcCurPos,&unRestLen,&unZipLen);
			unSrcCurPos += unRestLen;
		}
		else
		{
			unsigned int unTmp = SUB_BLOCK_LEN;
			ucZipedBuffer = WJL_ErrRecovery_Encode_Buff(ucFileBuffer+unSrcCurPos,&unTmp,&unZipLen);
			unSrcCurPos += SUB_BLOCK_LEN;
		}

		// 误码预处理
		if (dErrbitRate != 0.0)
		{
			ErrCode(ucZipedBuffer,&unZipLen,&dErrbitRate);
		}		

		unFinalLen += sizeof(unsigned int);
		unFinalLen += unZipLen;

		ucFinal = (unsigned char*)realloc(ucFinal,unFinalLen);
		memcpy(ucFinal+unDstCurPos,&unZipLen,sizeof(unsigned int));
		memcpy(ucFinal+unDstCurPos+sizeof(unsigned int),ucZipedBuffer,unZipLen);
		free(ucZipedBuffer);
		unDstCurPos += (sizeof(unsigned int) + unZipLen);
	}	
	free(ucFileBuffer);

	m_proCompress.SetPos(90);

	// 编码后文件写磁盘
	CString strOutFilePath = m_strOutputPath;
	strOutFilePath += _T("\\编码数据_");
	strOutFilePath += GetTime();
	strOutFilePath += _T(".wjlr");
	WJL_MemoryToFile(CW2A(strOutFilePath),ucFinal,&unFinalLen);
	free(ucFinal);

	m_proCompress.SetPos(100);
	GetDlgItem(IDC_STC_TIP)->SetWindowText(_T("File encoding success!"));
	m_edOutputPath.SetWindowTextW(strOutFilePath);
	m_edZipDataPath.SetWindowTextW(strOutFilePath);
	DisplayFileInfo(strOutFilePath,IDC_STC_TIP2);
}

//---------------------------------------------------------------------------------------
void CMainDlg::OnBnClickedBtnUnzip()
{
	if (m_strZipdataPath.IsEmpty())
	{
		GetDlgItem(IDC_STC_TIP)->SetWindowText(_T("Browse the path of the encoded file!"));
		m_edZipDataPath.SetFocus();
		return;
	}

	SHFILEINFO psi1;
	SHGetFileInfo(m_strZipdataPath,NULL,&psi1,sizeof(SHFILEINFO),SHGFI_DISPLAYNAME);
	CString strTmp = psi1.szDisplayName;
	if (strTmp.IsEmpty())
	{
		m_strZipdataPath = _T("");
		m_edZipDataPath.SetWindowText(_T(""));
		GetDlgItem(IDC_STC_TIP)->SetWindowText(_T("Browse the path of the encoded file!"));
		m_edZipDataPath.SetFocus();
		return;
	}

	if (m_strUnzipDataPath.IsEmpty())
	{
		GetDlgItem(IDC_STC_TIP)->SetWindowText(_T("Please browse the path where the decoded file is stored!"));
		m_btnOpenZipdata.SetFocus();
		return;
	}

	m_edUnzipOutPath.SetWindowTextW(_T(""));
	m_proUnzip.SetPos(50);

	// 从文件中读出数据
	unsigned int unInbufferLen=0;
	unsigned char *ucZipedBuffer = WJL_FileToMemory(CW2A(m_strZipdataPath),&unInbufferLen);	
	if (NULL == ucZipedBuffer)
	{
		GetDlgItem(IDC_STC_TIP)->SetWindowText(_T("File open failed!"));
		m_edUnzipOutPath.SetWindowText(m_strUnzipDataPath);
		m_proUnzip.SetPos(0);
		return;
	}

	GetDlgItem(IDC_STC_TIP)->SetWindowText(_T("Decoding,please wait..."));

	// 编辑输出文件名
	CString strOutputFilePath = m_strUnzipDataPath;
	strOutputFilePath  += _T("\\还原数据_");
	strOutputFilePath += GetTime();

	// 编辑纠正文件名
	CString strOutputCorrectedFilePath = m_strUnzipDataPath;
	strOutputCorrectedFilePath += _T("\\纠回数据_");
	strOutputCorrectedFilePath += GetTime();

	FILE* pOutFile = NULL;
	FILE* pOutCorrectedFile = NULL;

	unsigned int unCurPos = 0;
	while (unCurPos < unInbufferLen)
	{
		unsigned int unOutbufferLen;
		unsigned int unPosErr = 0;
		unsigned int unSubBuffLen = 0;
		memcpy(&unSubBuffLen,ucZipedBuffer+unCurPos,sizeof(unsigned int));
		unCurPos += sizeof(unsigned int);
		unsigned char *ucUnzipedBuffer = WJL_ErrRecovery_Decode_Buff(ucZipedBuffer+unCurPos,&unSubBuffLen,&unOutbufferLen,&unPosErr);

		unCurPos += unSubBuffLen;

		pOutFile = fopen(CW2A(strOutputFilePath),"ab");
		pOutCorrectedFile = fopen(CW2A(strOutputCorrectedFilePath),"ab");

		if (ucUnzipedBuffer)
		{
			fwrite(ucUnzipedBuffer,unOutbufferLen,1,pOutFile);
		}
		else
		{
			unsigned char ucErr[SUB_BLOCK_LEN] = {0xff};
			fwrite(ucErr,SUB_BLOCK_LEN,1,pOutFile);
		}
		// 输出已纠正数据
		fwrite(ucZipedBuffer+unCurPos-unSubBuffLen-sizeof(unsigned int),sizeof(unsigned int)+unSubBuffLen,1,pOutCorrectedFile);
		free(ucUnzipedBuffer);

		fclose(pOutFile);
		fclose(pOutCorrectedFile);
	}

	DisplayFileInfo(strOutputFilePath,IDC_STC_TIP2);
	m_proUnzip.SetPos(100);

	GetDlgItem(IDC_STC_TIP)->SetWindowText(_T("File decoded successfully! "));

	if (m_btnChkUnzipedOpen.GetCheck())
	{
		OpenPath(strOutputFilePath);
	}

	m_edUnzipOutPath.SetWindowText(strOutputFilePath);
}

//---------------------------------------------------------------------------------------
void CMainDlg::OnBnClickedBtnOpenzip()
{
	OPENFILENAME ofn;
	TCHAR filePath[MAX_PATH] = {0};
	RtlSecureZeroMemory(&ofn, sizeof (ofn) );
	ofn.hwndOwner		= GetSafeHwnd();
	ofn.Flags			= OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_EXPLORER | OFN_FORCESHOWHIDDEN;
	ofn.lpstrFile		= filePath;
	ofn.nMaxFile		= MAX_PATH;
	ofn.lStructSize		= sizeof(OPENFILENAME);
	ofn.lpstrFilter		= TEXT("wjlr Files(*.wjlr)\0*.wjlr\0");//所有文件类型
	ofn.lpstrTitle		= _T("Please browse the files to decode");

	if (GetOpenFileName(&ofn))
	{
		m_strZipdataPath = filePath;
		m_edZipDataPath.SetWindowText(filePath);
	}	
}

//---------------------------------------------------------------------------------------
void CMainDlg::OnBnClickedBtnOpenunzip()
{
	CString strText;
	m_btnOpenUnzipdata.GetWindowText(strText);
	if (strText == _T("Browse"))
	{
		TCHAR szBuffer[MAX_PATH] = {0};  
		BROWSEINFO bi;  
		ZeroMemory(&bi,sizeof(BROWSEINFO));  
		bi.hwndOwner = NULL;  
		bi.pszDisplayName = szBuffer;  
		bi.lpszTitle = _T("Select the folder directory from below:");  
		bi.ulFlags = BIF_RETURNFSANCESTORS;  
		LPITEMIDLIST idl = SHBrowseForFolder(&bi);  
		if (NULL == idl)  
		{  
			return;  
		}  
		SHGetPathFromIDList(idl,szBuffer);
		m_edUnzipOutPath.SetWindowText(szBuffer);
		m_strUnzipDataPath = szBuffer;
	}
}

//---------------------------------------------------------------------------------------
void CMainDlg::OnEnChangeEdZippath()
{
	m_edZipDataPath.GetWindowText(m_strZipdataPath);
}

//---------------------------------------------------------------------------------------
void CMainDlg::OnEnKillfocusEdZippath()
{
	m_edZipDataPath.GetWindowText(m_strZipdataPath);
}

//---------------------------------------------------------------------------------------
void CMainDlg::OnClose()
{
	CDialog::OnClose();
}

//---------------------------------------------------------------------------------------
void CMainDlg::OnDropFiles(HDROP hDropInfo)
{
	int DropCount=DragQueryFile(hDropInfo,-1,NULL,0);//取得被拖动文件的数目  
	for(int i=0;i< DropCount;i++)  
	{
		WCHAR wcStr[MAX_PATH];  
		DragQueryFile(hDropInfo,i,wcStr,MAX_PATH);//获得拖曳的第i个文件的文件名
		TCHAR drive[_MAX_DRIVE];			// 盘符名称，比如说C盘啊，D盘啊
		TCHAR dir[_MAX_DIR];				// 目录
		TCHAR fname[_MAX_FNAME];			// 进程名字
		TCHAR ext[_MAX_EXT];				// 后缀，一般为exe或者是dll
		_tsplitpath_s( wcStr, drive, dir, fname, ext);//分割该路径，得到盘符，目录，文件名，后缀名

		if (CString(ext).CompareNoCase(_T(".wjlr")) == 0)
		{
			m_strZipdataPath = wcStr;
			m_edZipDataPath.SetWindowText(wcStr);
			break;
		}

		m_strInputPath = wcStr;
		m_edInputPath.SetWindowText(wcStr);

		DisplayFileInfo(wcStr,IDC_STC_TIP);
		break;
	}
	DragFinish(hDropInfo);  //拖放结束后,释放内存  

	CDialog::OnDropFiles(hDropInfo);
}
